#pragma once

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <variant>
#include <optional>
#include <functional>
#include <cstdint>

struct sqlite3;
struct sqlite3_stmt;

namespace manimart::repository {

using SqlParam = std::variant<std::nullptr_t, int, int64_t, double, std::string>;

class SqlRow {
public:
    explicit SqlRow(std::vector<std::string> col_names, std::vector<std::string> col_values, std::vector<bool> is_null);

    [[nodiscard]] bool IsNull(const std::string& col_name) const;
    [[nodiscard]] std::string GetString(const std::string& col_name, const std::string& default_val = "") const;
    [[nodiscard]] int GetInt(const std::string& col_name, int default_val = 0) const;
    [[nodiscard]] int64_t GetInt64(const std::string& col_name, int64_t default_val = 0) const;
    [[nodiscard]] double GetDouble(const std::string& col_name, double default_val = 0.0) const;

private:
    [[nodiscard]] int FindColIndex(const std::string& col_name) const;

    std::vector<std::string> col_names_;
    std::vector<std::string> col_values_;
    std::vector<bool> is_null_;
};

class SqlResultSet {
public:
    void AddRow(SqlRow row);
    [[nodiscard]] size_t Size() const noexcept { return rows_.size(); }
    [[nodiscard]] bool IsEmpty() const noexcept { return rows_.empty(); }
    [[nodiscard]] const SqlRow& Get(size_t index) const { return rows_.at(index); }
    [[nodiscard]] const std::vector<SqlRow>& GetRows() const noexcept { return rows_; }

private:
    std::vector<SqlRow> rows_;
};

class DatabaseConnection {
public:
    static DatabaseConnection& GetInstance();

    void Initialize(const std::string& db_type, const std::string& connection_str);
    void Close();

    [[nodiscard]] bool IsInitialized() const;
    [[nodiscard]] bool IsHealthy();

    SqlResultSet ExecuteQuery(const std::string& sql, const std::vector<SqlParam>& params = {});
    int64_t ExecuteUpdate(const std::string& sql, const std::vector<SqlParam>& params = {}); // Returns last_insert_id or affected_rows
    bool ExecuteBatchScript(const std::string& multi_sql);

    bool BeginTransaction();
    bool CommitTransaction();
    bool RollbackTransaction();

    template <typename T>
    T InTransaction(std::function<T()> fn) {
        if (!BeginTransaction()) {
            throw std::runtime_error("Failed to begin transaction");
        }
        try {
            T result = fn();
            if (!CommitTransaction()) {
                RollbackTransaction();
                throw std::runtime_error("Failed to commit transaction");
            }
            return result;
        } catch (...) {
            RollbackTransaction();
            throw;
        }
    }

private:
    DatabaseConnection() = default;
    ~DatabaseConnection();
    DatabaseConnection(const DatabaseConnection&) = delete;
    DatabaseConnection& operator=(const DatabaseConnection&) = delete;

    std::string db_type_{"sqlite"};
    std::string connection_str_{"manimart.db"};
    sqlite3* sqlite_db_{nullptr};
    std::recursive_mutex db_mutex_;
    bool is_in_transaction_{false};
};

} // namespace manimart::repository
