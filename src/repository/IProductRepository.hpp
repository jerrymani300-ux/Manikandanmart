#pragma once

#include <string>
#include <vector>
#include <optional>
#include <memory>
#include "model/Product.hpp"

namespace manimart::repository {

class IProductRepository {
public:
    virtual ~IProductRepository() = default;

    virtual std::optional<model::Product> FindById(int64_t id) = 0;
    virtual std::vector<model::Product> FindAll(const std::string& search = "", const std::string& category = "", int limit = 100, int offset = 0) = 0;
    virtual std::vector<model::Product> FindBySellerId(int64_t seller_id) = 0;
    virtual int64_t Create(const model::Product& product) = 0;
    virtual bool Update(const model::Product& product) = 0;
    virtual bool UpdateStock(int64_t product_id, int delta_quantity) = 0;
    virtual bool Delete(int64_t id) = 0;
    virtual int CountAll() = 0;
    virtual int CountBySellerId(int64_t seller_id) = 0;
    virtual std::vector<std::string> GetDistinctCategories() = 0;
};

} // namespace manimart::repository
