#pragma once

#include <memory>
#include <vector>
#include <string>
#include "repository/IProductRepository.hpp"
#include "repository/IUserRepository.hpp"
#include "dto/ProductDto.hpp"
#include "model/Product.hpp"

namespace manimart::service {

class ProductService {
public:
    ProductService(
        std::shared_ptr<repository::IProductRepository> product_repo,
        std::shared_ptr<repository::IUserRepository> user_repo
    );

    dto::ProductResponse GetProductById(int64_t id);
    std::vector<dto::ProductResponse> GetProducts(const std::string& search = "", const std::string& category = "", int limit = 100, int offset = 0);
    std::vector<dto::ProductResponse> GetProductsBySeller(int64_t seller_id);
    std::vector<std::string> GetCategories();

    dto::ProductResponse CreateProduct(int64_t seller_id, const dto::CreateProductRequest& req);
    dto::ProductResponse UpdateProduct(int64_t seller_id, int64_t product_id, const dto::UpdateProductRequest& req, bool is_admin = false);
    bool DeleteProduct(int64_t seller_id, int64_t product_id, bool is_admin = false);

private:
    std::shared_ptr<repository::IProductRepository> product_repo_;
    std::shared_ptr<repository::IUserRepository> user_repo_;
};

} // namespace manimart::service
