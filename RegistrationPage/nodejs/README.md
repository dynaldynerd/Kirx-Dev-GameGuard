# Node.js Registration Page

Standalone normal-user registration page for the current `RF_User` schema.

Requirements:
- Node.js 20+
- SQL Server account with insert/select access to `tbl_rfaccount`, `tbl_UserAccount`, and `tbl_usercurrentstate`

Setup:
1. Copy `.env.example` to `.env`
2. Put the current AccountServer Argon2 salt into `ARGON2_SALT_BASE64`
3. Run `npm install`
4. Run `npm start`
5. Open `http://localhost:3000`

Notes:
- This page follows the current AccountServer logic for normal accounts
- It stores `id_hmac`, `id_enc`, and `password_hash`
- It does not create GM accounts
