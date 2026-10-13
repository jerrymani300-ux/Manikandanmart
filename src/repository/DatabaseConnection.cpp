#include "repository/DatabaseConnection.hpp"
#include "exception/AppException.hpp"
#include <sqlite3.h>
#include <spdlog/spdlog.h>
#include <algorithm>
#include <cctype>

namespace manimart::repository {

SqlRow::SqlRow(std::vector<std::string> col_names, std::vector<std::string> col_values, std::vector<bool> is_null)
    : col_names_(std::move(col_names)), col_values_(std::move(col_values)), is_null_(std::move(is_null)) {}

int SqlRow::FindColIndex(const std::string& col_name) const {
    for (size_t i = 0; i < col_names_.size(); ++i) {
        if (col_names_[i] == col_name) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

bool SqlRow::IsNull(const std::string& col_name) const {
    int idx = FindColIndex(col_name);
    if (idx < 0 || idx >= static_cast<int>(is_null_.size())) return true;
    return is_null_[idx];
}

std::string SqlRow::GetString(const std::string& col_name, const std::string& default_val) const {
    int idx = FindColIndex(col_name);
    if (idx < 0 || is_null_[idx]) return default_val;
    return col_values_[idx];
}

int SqlRow::GetInt(const std::string& col_name, int default_val) const {
    int idx = FindColIndex(col_name);
    if (idx < 0 || is_null_[idx]) return default_val;
    try {
        return std::stoi(col_values_[idx]);
    } catch (...) {
        return default_val;
    }
}

int64_t SqlRow::GetInt64(const std::string& col_name, int64_t default_val) const {
    int idx = FindColIndex(col_name);
    if (idx < 0 || is_null_[idx]) return default_val;
    try {
        return std::stoll(col_values_[idx]);
    } catch (...) {
        return default_val;
    }
}

double SqlRow::GetDouble(const std::string& col_name, double default_val) const {
    int idx = FindColIndex(col_name);
    if (idx < 0 || is_null_[idx]) return default_val;
    try {
        return std::stod(col_values_[idx]);
    } catch (...) {
        return default_val;
    }
}

void SqlResultSet::AddRow(SqlRow row) {
    rows_.push_back(std::move(row));
}

DatabaseConnection& DatabaseConnection::GetInstance() {
    static DatabaseConnection instance;
    return instance;
}

DatabaseConnection::~DatabaseConnection() {
    Close();
}

void DatabaseConnection::Initialize(const std::string& db_type, const std::string& connection_str) {
    std::lock_guard<std::recursive_mutex> lock(db_mutex_);
    Close();

    db_type_ = db_type;
    connection_str_ = connection_str;

    int rc = sqlite3_open(connection_str_.c_str(), &sqlite_db_);
    if (rc != SQLITE_OK) {
        std::string err = sqlite3_errmsg(sqlite_db_);
        Close();
        spdlog::error("Failed to open database '{}': {}", connection_str_, err);
        throw exception::DatabaseException("Database initialization failed: " + err);
    }

    // Enable foreign keys and WAL mode for SQLite
    sqlite3_exec(sqlite_db_, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);
    sqlite3_exec(sqlite_db_, "PRAGMA journal_mode = WAL;", nullptr, nullptr, nullptr);

    spdlog::info("Database initialized successfully (type: {}, path: {})", db_type_, connection_str_);
}

void DatabaseConnection::Close() {
    std::lock_guard<std::recursive_mutex> lock(db_mutex_);
    if (sqlite_db_ != nullptr) {
        sqlite3_close_v2(sqlite_db_);
        sqlite_db_ = nullptr;
    }
}

bool DatabaseConnection::IsInitialized() const {
    return sqlite_db_ != nullptr;
}

bool DatabaseConnection::IsHealthy() {
    std::lock_guard<std::recursive_mutex> lock(db_mutex_);
    if (!sqlite_db_) return false;

    try {
        auto res = ExecuteQuery("SELECT 1 AS alive;");
        return !res.IsEmpty() && res.Get(0).GetInt("alive") == 1;
    } catch (...) {
        return false;
    }
}

SqlResultSet DatabaseConnection::ExecuteQuery(const std::string& sql, const std::vector<SqlParam>& params) {
    std::lock_guard<std::recursive_mutex> lock(db_mutex_);
    if (!sqlite_db_) {
        throw exception::DatabaseException("Database connection not initialized");
    }

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(sqlite_db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::string err = sqlite3_errmsg(sqlite_db_);
        spdlog::error("SQL prepare error: {} | Query: {}", err, sql);
        throw exception::DatabaseException("SQL prepare failed: " + err);
    }

    // Bind parameters
    for (size_t i = 0; i < params.size(); ++i) {
        int idx = static_cast<int>(i + 1);
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                sqlite3_bind_null(stmt, idx);
            } else if constexpr (std::is_same_v<T, int>) {
                sqlite3_bind_int(stmt, idx, arg);
            } else if constexpr (std::is_same_v<T, int64_t>) {
                sqlite3_bind_int64(stmt, idx, arg);
            } else if constexpr (std::is_same_v<T, double>) {
                sqlite3_bind_double(stmt, idx, arg);
            } else if constexpr (std::is_same_v<T, std::string>) {
                sqlite3_bind_text(stmt, idx, arg.c_str(), static_cast<int>(arg.length()), SQLITE_TRANSIENT);
            }
        }, params[i]);
    }

    SqlResultSet result_set;
    int col_count = sqlite3_column_count(stmt);
    std::vector<std::string> col_names(col_count);
    for (int i = 0; i < col_count; ++i) {
        col_names[i] = sqlite3_column_name(stmt, i);
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        std::vector<std::string> col_values(col_count);
        std::vector<bool> is_null(col_count, false);

        for (int i = 0; i < col_count; ++i) {
            if (sqlite3_column_type(stmt, i) == SQLITE_NULL) {
                is_null[i] = true;
                col_values[i] = "";
            } else {
                const unsigned char* text = sqlite3_column_text(stmt, i);
                col_values[i] = text ? reinterpret_cast<const char*>(text) : "";
            }
        }
        result_set.AddRow(SqlRow(col_names, col_values, is_null));
    }

    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        std::string err = sqlite3_errmsg(sqlite_db_);
        spdlog::error("SQL step error: {} | Query: {}", err, sql);
        throw exception::DatabaseException("SQL execution error: " + err);
    }

    return result_set;
}

int64_t DatabaseConnection::ExecuteUpdate(const std::string& sql, const std::vector<SqlParam>& params) {
    std::lock_guard<std::recursive_mutex> lock(db_mutex_);
    if (!sqlite_db_) {
        throw exception::DatabaseException("Database connection not initialized");
    }

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(sqlite_db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::string err = sqlite3_errmsg(sqlite_db_);
        spdlog::error("SQL update prepare error: {} | Query: {}", err, sql);
        throw exception::DatabaseException("SQL prepare failed: " + err);
    }

    for (size_t i = 0; i < params.size(); ++i) {
        int idx = static_cast<int>(i + 1);
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                sqlite3_bind_null(stmt, idx);
            } else if constexpr (std::is_same_v<T, int>) {
                sqlite3_bind_int(stmt, idx, arg);
            } else if constexpr (std::is_same_v<T, int64_t>) {
                sqlite3_bind_int64(stmt, idx, arg);
            } else if constexpr (std::is_same_v<T, double>) {
                sqlite3_bind_double(stmt, idx, arg);
            } else if constexpr (std::is_same_v<T, std::string>) {
                sqlite3_bind_text(stmt, idx, arg.c_str(), static_cast<int>(arg.length()), SQLITE_TRANSIENT);
            }
        }, params[i]);
    }

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        std::string err = sqlite3_errmsg(sqlite_db_);
        spdlog::error("SQL update execution error: {} | Query: {}", err, sql);
        throw exception::DatabaseException("SQL update error: " + err);
    }

    // If it was an INSERT, return last_insert_rowid, else changes
    int64_t last_id = sqlite3_last_insert_rowid(sqlite_db_);
    if (last_id > 0) {
        return last_id;
    }
    return sqlite3_changes(sqlite_db_);
}

bool DatabaseConnection::ExecuteBatchScript(const std::string& multi_sql) {
    std::lock_guard<std::recursive_mutex> lock(db_mutex_);
    if (!sqlite_db_) {
        throw exception::DatabaseException("Database connection not initialized");
    }

    char* err_msg = nullptr;
    int rc = sqlite3_exec(sqlite_db_, multi_sql.c_str(), nullptr, nullptr, &err_msg);
    if (rc != SQLITE_OK) {
        std::string err = err_msg ? err_msg : "Unknown error";
        sqlite3_free(err_msg);
        spdlog::error("Batch script execution error: {}", err);
        return false;
    }
    return true;
}

bool DatabaseConnection::BeginTransaction() {
    std::lock_guard<std::recursive_mutex> lock(db_mutex_);
    if (is_in_transaction_) return true;
    char* err_msg = nullptr;
    int rc = sqlite3_exec(sqlite_db_, "BEGIN TRANSACTION;", nullptr, nullptr, &err_msg);
    if (rc == SQLITE_OK) {
        is_in_transaction_ = true;
        return true;
    }
    if (err_msg) {
        spdlog::error("Begin transaction error: {}", err_msg);
        sqlite3_free(err_msg);
    }
    return false;
}

bool DatabaseConnection::CommitTransaction() {
    std::lock_guard<std::recursive_mutex> lock(db_mutex_);
    if (!is_in_transaction_) return true;
    char* err_msg = nullptr;
    int rc = sqlite3_exec(sqlite_db_, "COMMIT;", nullptr, nullptr, &err_msg);
    if (rc == SQLITE_OK) {
        is_in_transaction_ = false;
        return true;
    }
    if (err_msg) {
        spdlog::error("Commit transaction error: {}", err_msg);
        sqlite3_free(err_msg);
    }
    return false;
}

bool DatabaseConnection::RollbackTransaction() {
    std::lock_guard<std::recursive_mutex> lock(db_mutex_);
    if (!is_in_transaction_) return true;
    char* err_msg = nullptr;
    int rc = sqlite3_exec(sqlite_db_, "ROLLBACK;", nullptr, nullptr, &err_msg);
    is_in_transaction_ = false;
    if (err_msg) {
        spdlog::warn("Rollback transaction message: {}", err_msg);
        sqlite3_free(err_msg);
    }
    return rc == SQLITE_OK;
}

} // namespace manimart::repository
