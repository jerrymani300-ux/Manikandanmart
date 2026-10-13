#!/usr/bin/env bash
set -e

echo "=== Seeding ManiMart Database ==="

DB_TYPE=${DB_TYPE:-sqlite}
SQLITE_DB_PATH=${SQLITE_DB_PATH:-manimart.db}

if [ "$DB_TYPE" == "sqlite" ]; then
    sqlite3 "${SQLITE_DB_PATH}" < "db/seed.sql"
    echo "=== SQLite Seed Data Inserted Successfully ==="
else
    echo "Seeding PostgreSQL at ${DB_HOST}:${DB_PORT}/${DB_NAME}..."
    PGPASSWORD=${DB_PASSWORD} psql -h ${DB_HOST} -p ${DB_PORT} -U ${DB_USER} -d ${DB_NAME} -f db/seed.sql
    echo "=== PostgreSQL Seed Complete ==="
fi
