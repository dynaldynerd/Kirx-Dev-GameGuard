<?php
declare(strict_types=1);

loadEnv(__DIR__ . DIRECTORY_SEPARATOR . '.env');

$error = '';
$success = '';
$username = '';

try {
    $config = loadConfig();
} catch (Throwable $ex) {
    $config = null;
    $error = 'Configuration error: ' . $ex->getMessage();
}

if ($_SERVER['REQUEST_METHOD'] === 'POST' && $config !== null) {
    $username = trim((string)($_POST['username'] ?? ''));
    $password = trim((string)($_POST['password'] ?? ''));
    $retypePassword = trim((string)($_POST['retypePassword'] ?? ''));

    $validationError = validateRegistration($username, $password, $retypePassword);
    if ($validationError !== null) {
        $error = $validationError;
    } else {
        try {
            $result = createNormalAccount($config, $username, $password);
            if ($result['ok']) {
                $success = $result['message'];
                $username = '';
            } else {
                $error = $result['message'];
            }
        } catch (Throwable $ex) {
            $error = 'Registration failed: ' . $ex->getMessage();
        }
    }
}

function loadEnv(string $path): void
{
    if (!is_file($path)) {
        return;
    }

    $lines = file($path, FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES);
    if ($lines === false) {
        return;
    }

    foreach ($lines as $line) {
        $trimmed = trim($line);
        if ($trimmed === '' || str_starts_with($trimmed, '#')) {
            continue;
        }

        $separator = strpos($trimmed, '=');
        if ($separator === false) {
            continue;
        }

        $name = trim(substr($trimmed, 0, $separator));
        $value = trim(substr($trimmed, $separator + 1));
        $value = trim($value, "\"'");

        $_ENV[$name] = $value;
        $_SERVER[$name] = $value;
        putenv($name . '=' . $value);
    }
}

function loadConfig(): array
{
    if (!extension_loaded('sqlsrv')) {
        throw new RuntimeException('sqlsrv extension is required.');
    }
    if (!extension_loaded('openssl')) {
        throw new RuntimeException('openssl extension is required.');
    }
    if (!function_exists('sodium_crypto_pwhash')) {
        throw new RuntimeException('sodium extension is required.');
    }

    $dbHost = requireEnv('DB_HOST');
    $dbPort = (int)requireEnv('DB_PORT');
    $dbName = requireEnv('DB_NAME');
    $dbUser = requireEnv('DB_USER');
    $dbPassword = requireEnv('DB_PASSWORD');
    $argon2SaltBase64 = requireEnv('ARGON2_SALT_BASE64');
    $argon2Salt = base64_decode($argon2SaltBase64, true);

    if ($argon2Salt === false || strlen($argon2Salt) < 16) {
        throw new RuntimeException('ARGON2_SALT_BASE64 must decode to at least 16 bytes.');
    }

    return [
        'dbHost' => $dbHost,
        'dbPort' => $dbPort,
        'dbName' => $dbName,
        'dbUser' => $dbUser,
        'dbPassword' => $dbPassword,
        'argon2Salt' => $argon2Salt,
        'aesKey' => hash('sha256', $argon2Salt, true),
    ];
}

function requireEnv(string $name): string
{
    $value = $_ENV[$name] ?? $_SERVER[$name] ?? getenv($name);
    if (!is_string($value) || trim($value) === '') {
        throw new RuntimeException($name . ' is required.');
    }

    return trim($value);
}

function validateRegistration(string $username, string $password, string $retypePassword): ?string
{
    if ($username === '') {
        return 'Username is required.';
    }

    if (str_starts_with($username, '!')) {
        return "Normal account usernames must not start with '!'.";
    }

    if (strlen($username) > 12) {
        return 'Username must be 12 characters or fewer.';
    }

    if (!isAsciiVisible($username)) {
        return 'Username must use ASCII characters only.';
    }

    if ($password === '') {
        return 'Password is required.';
    }

    if (strlen($password) > 12) {
        return 'Password must be 12 characters or fewer.';
    }

    if (!isAsciiVisible($password)) {
        return 'Password must use ASCII characters only.';
    }

    if ($password !== $retypePassword) {
        return 'Password and retype password must match.';
    }

    return null;
}

function isAsciiVisible(string $value): bool
{
    $length = strlen($value);
    for ($i = 0; $i < $length; $i++) {
        $code = ord($value[$i]);
        if ($code < 33 || $code > 126) {
            return false;
        }
    }

    return true;
}

function createNormalAccount(array $config, string $username, string $password): array
{
    $connection = connectSqlServer($config);
    if ($connection === false) {
        throw new RuntimeException(formatSqlsrvErrors());
    }

    $idHmac = computeIdHmac($username, $config['argon2Salt']);
    $idEnc = encryptId($username, $config['aesKey']);
    $passwordHashBase64 = hashPasswordArgon2id($password, $config['argon2Salt']);

    $idHmacHex = bin2hex($idHmac);
    $idEncHex = bin2hex($idEnc);
    $now = date('Y-m-d H:i:s');

    if (!sqlsrv_begin_transaction($connection)) {
        throw new RuntimeException(formatSqlsrvErrors());
    }

    try {
        if (binaryRecordExists($connection, 'dbo.tbl_rfaccount', 'id_hmac', $idHmacHex)) {
            sqlsrv_rollback($connection);
            return ['ok' => false, 'message' => 'That normal account already exists.'];
        }

        if (binaryRecordExists($connection, 'dbo.tbl_UserAccount', 'id_hmac', $idHmacHex)) {
            sqlsrv_rollback($connection);
            return ['ok' => false, 'message' => 'That normal account already exists.'];
        }

        $insertAuthSql = "
            INSERT INTO dbo.tbl_rfaccount (id_hmac, id_enc, password_hash, accounttype, birthdate)
            VALUES (
                CONVERT(binary(32), ?, 2),
                CONVERT(varbinary(128), ?, 2),
                ?,
                0,
                NULL
            );";
        runStatement($connection, $insertAuthSql, [$idHmacHex, $idEncHex, $passwordHashBase64]);

        $insertUserSql = "
            INSERT INTO dbo.tbl_UserAccount (
                id_hmac,
                id_enc,
                createtime,
                createip,
                lastlogintime,
                lastlogofftime,
                totallogmin,
                lastconnectip,
                pushclosetime,
                pusherip,
                JoinCode,
                LoginFailureCnt,
                fire_on,
                fire_warning,
                uilock,
                uilock_pw,
                uilock_failcnt,
                uilock_update,
                uilock_hintindex,
                uilock_hintanswer,
                uilock_find_pass_failcnt
            )
            VALUES (
                CONVERT(binary(32), ?, 2),
                CONVERT(varbinary(128), ?, 2),
                ?,
                '0',
                ?,
                ?,
                0,
                '0',
                ?,
                '0',
                0,
                0,
                ?,
                ?,
                0,
                '',
                0,
                ?,
                0,
                '',
                0
            );";
        runStatement($connection, $insertUserSql, [$idHmacHex, $idEncHex, $now, $now, $now, $now, $now, $now, $now]);

        $serialSql = "SELECT TOP 1 serial FROM dbo.tbl_UserAccount WHERE id_hmac = CONVERT(binary(32), ?, 2);";
        $serialStatement = runStatement($connection, $serialSql, [$idHmacHex]);
        $serialRow = sqlsrv_fetch_array($serialStatement, SQLSRV_FETCH_ASSOC);
        if ($serialRow === null || !isset($serialRow['serial'])) {
            throw new RuntimeException('Created account row but could not read back serial.');
        }

        runStatement($connection, 'INSERT INTO dbo.tbl_usercurrentstate (serial, state) VALUES (?, 0);', [(int)$serialRow['serial']]);

        if (!sqlsrv_commit($connection)) {
            throw new RuntimeException(formatSqlsrvErrors());
        }

        return ['ok' => true, 'message' => "Normal account '{$username}' created."];
    } catch (Throwable $ex) {
        sqlsrv_rollback($connection);
        throw $ex;
    } finally {
        sqlsrv_close($connection);
    }
}

function connectSqlServer(array $config)
{
    $serverName = $config['dbHost'] . ',' . $config['dbPort'];
    $connectionInfo = [
        'Database' => $config['dbName'],
        'UID' => $config['dbUser'],
        'PWD' => $config['dbPassword'],
        'TrustServerCertificate' => true,
        'Encrypt' => false,
        'CharacterSet' => 'UTF-8',
    ];

    return sqlsrv_connect($serverName, $connectionInfo);
}

function binaryRecordExists($connection, string $table, string $column, string $hexValue): bool
{
    $sql = "SELECT TOP 1 1 AS found FROM {$table} WHERE {$column} = CONVERT(binary(32), ?, 2);";
    $statement = runStatement($connection, $sql, [$hexValue]);
    $row = sqlsrv_fetch_array($statement, SQLSRV_FETCH_ASSOC);
    return $row !== null;
}

function runStatement($connection, string $sql, array $params)
{
    $statement = sqlsrv_query($connection, $sql, $params);
    if ($statement === false) {
        throw new RuntimeException(formatSqlsrvErrors());
    }

    return $statement;
}

function formatSqlsrvErrors(): string
{
    $errors = sqlsrv_errors();
    if (!is_array($errors) || $errors === []) {
        return 'Unknown SQL Server error.';
    }

    $parts = [];
    foreach ($errors as $error) {
        if (isset($error['message'])) {
            $parts[] = $error['message'];
        }
    }

    return implode(' | ', $parts);
}

function computeIdHmac(string $username, string $hmacKey): string
{
    return hash_hmac('sha256', $username, $hmacKey, true);
}

function encryptId(string $username, string $aesKey): string
{
    $nonce = random_bytes(12);
    $tag = '';
    $ciphertext = openssl_encrypt(
        $username,
        'aes-256-gcm',
        $aesKey,
        OPENSSL_RAW_DATA,
        $nonce,
        $tag,
        '',
        16
    );

    if ($ciphertext === false) {
        throw new RuntimeException('AES-GCM encryption failed.');
    }

    return $nonce . $tag . $ciphertext;
}

function hashPasswordArgon2id(string $password, string $saltBytes): string
{
    $rawHash = sodium_crypto_pwhash(
        32,
        $password,
        $saltBytes,
        3,
        65536 * 1024,
        SODIUM_CRYPTO_PWHASH_ALG_ARGON2ID13
    );

    return base64_encode($rawHash);
}

function h(string $value): string
{
    return htmlspecialchars($value, ENT_QUOTES | ENT_SUBSTITUTE, 'UTF-8');
}
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>RF Online Registration</title>
    <style>
        body {
            margin: 0;
            font-family: "Segoe UI", Tahoma, sans-serif;
            background: linear-gradient(180deg, #171717 0%, #232323 100%);
            color: #f2f2f2;
        }
        .wrap {
            max-width: 440px;
            margin: 48px auto;
            padding: 28px;
            background: rgba(0, 0, 0, 0.32);
            border: 1px solid #454545;
            box-shadow: 0 16px 38px rgba(0, 0, 0, 0.35);
        }
        h1 {
            margin: 0 0 10px;
            font-size: 28px;
        }
        p {
            margin: 0 0 18px;
            color: #d0d0d0;
            line-height: 1.45;
        }
        label {
            display: block;
            margin: 14px 0 6px;
            font-weight: 600;
        }
        input {
            width: 100%;
            box-sizing: border-box;
            padding: 11px 12px;
            border: 1px solid #666;
            background: #111;
            color: #fff;
            font-size: 15px;
        }
        .notice {
            margin-bottom: 14px;
            padding: 11px 12px;
            border: 1px solid;
        }
        .error {
            background: #341414;
            border-color: #a34343;
            color: #ffd2d2;
        }
        .success {
            background: #15381e;
            border-color: #3f8f58;
            color: #d8ffdf;
        }
        button {
            margin-top: 18px;
            width: 100%;
            padding: 12px;
            border: 1px solid #868686;
            background: linear-gradient(180deg, #4f4f4f 0%, #2f2f2f 100%);
            color: #fff;
            font-size: 15px;
            cursor: pointer;
        }
        .hint {
            margin-top: 16px;
            font-size: 12px;
            color: #bbbbbb;
        }
    </style>
</head>
<body>
    <div class="wrap">
        <h1>Create Account</h1>
        <p>Register a normal account for the current RF_User schema.</p>
        <?php if ($error !== ''): ?>
            <div class="notice error"><?= h($error) ?></div>
        <?php endif; ?>
        <?php if ($success !== ''): ?>
            <div class="notice success"><?= h($success) ?></div>
        <?php endif; ?>
        <form method="post" autocomplete="off">
            <label for="username">Username</label>
            <input id="username" name="username" maxlength="12" value="<?= h($username) ?>" required>

            <label for="password">Password</label>
            <input id="password" name="password" type="password" maxlength="12" required>

            <label for="retypePassword">Retype Password</label>
            <input id="retypePassword" name="retypePassword" type="password" maxlength="12" required>

            <button type="submit">Register</button>
        </form>
        <div class="hint">Username and password must be ASCII and 12 characters or fewer.</div>
    </div>
</body>
</html>
