#!/usr/bin/env bash
set -e

echo "=== Applying ManiMart Database Migrations ==="

DB_TYPE=${DB_TYPE:-sqlite}
SQLITE_DB_PATH=${SQLITE_DB_PATH:-manimart.db}

if [ "$DB_TYPE" == "sqlite" ]; then
    for f in $(ls -v db/migrations/*.sql); do
        echo "Applying $f..."
        sqlite3 "${SQLITE_DB_PATH}" < "$f"
    done
    echo "=== Migrations Completed Successfully ==="
else
    echo "Applying migrations to PostgreSQL at ${DB_HOST}:${DB_PORT}/${DB_NAME}..."
    PGPASSWORD=${DB_PASSWORD} psql -h ${DB_HOST} -p ${DB_PORT} -U ${DB_USER} -d ${DB_NAME} -f db/schema.sql
    echo "=== PostgreSQL Schema Setup Complete ==="
fi
