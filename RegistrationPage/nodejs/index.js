const crypto = require('crypto');
const express = require('express');
const argon2 = require('argon2');
const sql = require('mssql');
require('dotenv').config();

const config = loadConfig();
const app = express();

app.use(express.urlencoded({ extended: false }));

app.get('/', (_req, res) => {
  res.send(renderPage());
});

app.post('/register', async (req, res) => {
  const username = typeof req.body.username === 'string' ? req.body.username.trim() : '';
  const password = typeof req.body.password === 'string' ? req.body.password.trim() : '';
  const retypePassword = typeof req.body.retypePassword === 'string' ? req.body.retypePassword.trim() : '';

  const validationError = validateRegistration(username, password, retypePassword);
  if (validationError) {
    res.status(400).send(renderPage({ error: validationError, username }));
    return;
  }

  try {
    const result = await createNormalAccount(username, password);
    if (!result.ok) {
      res.status(400).send(renderPage({ error: result.message, username }));
      return;
    }

    res.send(renderPage({ success: result.message }));
  } catch (error) {
    res.status(500).send(renderPage({
      error: `Registration failed: ${error instanceof Error ? error.message : 'unknown error'}`,
      username
    }));
  }
});

app.listen(config.port, () => {
  console.log(`Registration page listening on http://0.0.0.0:${config.port}`);
});

function loadConfig() {
  const port = parseInteger(process.env.PORT, 3000);
  const dbHost = requireString(process.env.DB_HOST, 'DB_HOST');
  const dbPort = parseInteger(process.env.DB_PORT, 1433);
  const dbName = requireString(process.env.DB_NAME, 'DB_NAME');
  const dbUser = requireString(process.env.DB_USER, 'DB_USER');
  const dbPassword = requireString(process.env.DB_PASSWORD, 'DB_PASSWORD');
  const argon2SaltBase64 = requireString(process.env.ARGON2_SALT_BASE64, 'ARGON2_SALT_BASE64');
  const argon2Salt = Buffer.from(argon2SaltBase64, 'base64');

  if (argon2Salt.length < 16) {
    throw new Error('ARGON2_SALT_BASE64 must decode to at least 16 bytes.');
  }

  return {
    port,
    dbHost,
    dbPort,
    dbName,
    dbUser,
    dbPassword,
    argon2Salt,
    aesKey: crypto.createHash('sha256').update(argon2Salt).digest()
  };
}

function requireString(value, name) {
  if (!value || !value.trim()) {
    throw new Error(`${name} is required.`);
  }
  return value.trim();
}

function parseInteger(value, fallbackValue) {
  const parsed = Number.parseInt(String(value ?? ''), 10);
  return Number.isFinite(parsed) ? parsed : fallbackValue;
}

function validateRegistration(username, password, retypePassword) {
  if (!username) {
    return 'Username is required.';
  }

  if (username.startsWith('!')) {
    return "Normal account usernames must not start with '!'.";
  }

  if (username.length > 12) {
    return 'Username must be 12 characters or fewer.';
  }

  if (!isAsciiVisible(username)) {
    return 'Username must use ASCII characters only.';
  }

  if (!password) {
    return 'Password is required.';
  }

  if (password.length > 12) {
    return 'Password must be 12 characters or fewer.';
  }

  if (!isAsciiVisible(password)) {
    return 'Password must use ASCII characters only.';
  }

  if (password !== retypePassword) {
    return 'Password and retype password must match.';
  }

  return null;
}

function isAsciiVisible(value) {
  for (const ch of value) {
    const code = ch.charCodeAt(0);
    if (code < 33 || code > 126) {
      return false;
    }
  }
  return true;
}

async function createNormalAccount(username, password) {
  const idHmac = computeIdHmac(username);
  const idEnc = encryptId(username);
  const passwordHashBase64 = await hashPassword(password);
  const idHmacHex = idHmac.toString('hex');
  const idEncHex = idEnc.toString('hex');
  const now = new Date();

  const pool = new sql.ConnectionPool({
    server: config.dbHost,
    port: config.dbPort,
    database: config.dbName,
    user: config.dbUser,
    password: config.dbPassword,
    options: {
      encrypt: false,
      trustServerCertificate: true
    }
  });

  await pool.connect();
  const transaction = new sql.Transaction(pool);
  let transactionBegun = false;

  try {
    await transaction.begin();
    transactionBegun = true;

    let request = new sql.Request(transaction);
    request.input('id_hmac_hex', sql.VarChar(64), idHmacHex);
    let result = await request.query(
      "SELECT TOP 1 1 AS found FROM dbo.tbl_rfaccount WHERE id_hmac = CONVERT(binary(32), @id_hmac_hex, 2);"
    );
    if (result.recordset.length > 0) {
      await transaction.rollback();
      return { ok: false, message: 'That normal account already exists.' };
    }

    request = new sql.Request(transaction);
    request.input('id_hmac_hex', sql.VarChar(64), idHmacHex);
    result = await request.query(
      "SELECT TOP 1 1 AS found FROM dbo.tbl_UserAccount WHERE id_hmac = CONVERT(binary(32), @id_hmac_hex, 2);"
    );
    if (result.recordset.length > 0) {
      await transaction.rollback();
      return { ok: false, message: 'That normal account already exists.' };
    }

    request = new sql.Request(transaction);
    request.input('id_hmac_hex', sql.VarChar(64), idHmacHex);
    request.input('id_enc_hex', sql.VarChar(sql.MAX), idEncHex);
    request.input('password_hash', sql.VarChar(255), passwordHashBase64);
    await request.query(`
      INSERT INTO dbo.tbl_rfaccount (id_hmac, id_enc, password_hash, accounttype, birthdate)
      VALUES (
        CONVERT(binary(32), @id_hmac_hex, 2),
        CONVERT(varbinary(128), @id_enc_hex, 2),
        @password_hash,
        0,
        NULL
      );
    `);

    request = new sql.Request(transaction);
    request.input('id_hmac_hex', sql.VarChar(64), idHmacHex);
    request.input('id_enc_hex', sql.VarChar(sql.MAX), idEncHex);
    request.input('now', sql.DateTime, now);
    await request.query(`
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
        CONVERT(binary(32), @id_hmac_hex, 2),
        CONVERT(varbinary(128), @id_enc_hex, 2),
        @now,
        '0',
        @now,
        @now,
        0,
        '0',
        @now,
        '0',
        0,
        0,
        @now,
        @now,
        0,
        '',
        0,
        @now,
        0,
        '',
        0
      );
    `);

    request = new sql.Request(transaction);
    request.input('id_hmac_hex', sql.VarChar(64), idHmacHex);
    result = await request.query(
      "SELECT TOP 1 serial FROM dbo.tbl_UserAccount WHERE id_hmac = CONVERT(binary(32), @id_hmac_hex, 2);"
    );
    if (result.recordset.length === 0) {
      throw new Error('Created account row but could not read back serial.');
    }

    const serial = result.recordset[0].serial;
    request = new sql.Request(transaction);
    request.input('serial', sql.Int, serial);
    await request.query(
      'INSERT INTO dbo.tbl_usercurrentstate (serial, state) VALUES (@serial, 0);'
    );

    await transaction.commit();
    return { ok: true, message: `Normal account '${username}' created.` };
  } catch (error) {
    if (transactionBegun) {
      try {
        await transaction.rollback();
      } catch {
      }
    }
    throw error;
  } finally {
    await pool.close();
  }
}

function computeIdHmac(username) {
  return crypto.createHmac('sha256', config.argon2Salt)
    .update(Buffer.from(username, 'utf8'))
    .digest();
}

function encryptId(username) {
  const nonce = crypto.randomBytes(12);
  const cipher = crypto.createCipheriv('aes-256-gcm', config.aesKey, nonce);
  const ciphertext = Buffer.concat([
    cipher.update(Buffer.from(username, 'utf8')),
    cipher.final()
  ]);
  const tag = cipher.getAuthTag();
  return Buffer.concat([nonce, tag, ciphertext]);
}

async function hashPassword(password) {
  const rawHash = await argon2.hash(password, {
    type: argon2.argon2id,
    salt: config.argon2Salt,
    timeCost: 3,
    memoryCost: 65536,
    parallelism: 1,
    hashLength: 32,
    raw: true
  });

  return Buffer.from(rawHash).toString('base64');
}

function renderPage({ error = '', success = '', username = '' } = {}) {
  const escapedError = escapeHtml(error);
  const escapedSuccess = escapeHtml(success);
  const escapedUsername = escapeHtml(username);

  return `<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>RF Online Registration</title>
  <style>
    body {
      margin: 0;
      font-family: Segoe UI, Tahoma, sans-serif;
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
    button:hover {
      background: linear-gradient(180deg, #616161 0%, #3b3b3b 100%);
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
    ${escapedError ? `<div class="notice error">${escapedError}</div>` : ''}
    ${escapedSuccess ? `<div class="notice success">${escapedSuccess}</div>` : ''}
    <form method="post" action="/register" autocomplete="off">
      <label for="username">Username</label>
      <input id="username" name="username" maxlength="12" value="${escapedUsername}" required>

      <label for="password">Password</label>
      <input id="password" name="password" type="password" maxlength="12" required>

      <label for="retypePassword">Retype Password</label>
      <input id="retypePassword" name="retypePassword" type="password" maxlength="12" required>

      <button type="submit">Register</button>
    </form>
    <div class="hint">Username and password must be ASCII and 12 characters or fewer.</div>
  </div>
</body>
</html>`;
}

function escapeHtml(value) {
  return String(value)
    .replaceAll('&', '&amp;')
    .replaceAll('<', '&lt;')
    .replaceAll('>', '&gt;')
    .replaceAll('"', '&quot;')
    .replaceAll("'", '&#39;');
}
