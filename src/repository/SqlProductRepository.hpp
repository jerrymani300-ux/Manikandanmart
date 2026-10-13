#pragma once

#include "repository/IProductRepository.hpp"
#include "repository/DatabaseConnection.hpp"

namespace manimart::repository {

class SqlProductRepository : public IProductRepository {
public:
    std::optional<model::Product> FindById(int64_t id) override;
    std::vector<model::Product> FindAll(const std::string& search = "", const std::string& category = "", int limit = 100, int offset = 0) override;
    std::vector<model::Product> FindBySellerId(int64_t seller_id) override;
    int64_t Create(const model::Product& product) override;
    bool Update(const model::Product& product) override;
    bool UpdateStock(int64_t product_id, int delta_quantity) override;
    bool Delete(int64_t id) override;
    int CountAll() override;
    int CountBySellerId(int64_t seller_id) override;
    std::vector<std::string> GetDistinctCategories() override;

private:
    static model::Product MapRow(const SqlRow& row);
};

} // namespace manimart::repository
