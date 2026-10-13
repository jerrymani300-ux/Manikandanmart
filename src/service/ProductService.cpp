#include "service/ProductService.hpp"
#include "util/ValidationUtil.hpp"
#include "exception/AppException.hpp"
#include <spdlog/spdlog.h>

namespace manimart::service {

ProductService::ProductService(
    std::shared_ptr<repository::IProductRepository> product_repo,
    std::shared_ptr<repository::IUserRepository> user_repo)
    : product_repo_(std::move(product_repo)), user_repo_(std::move(user_repo)) {
    if (!product_repo_ || !user_repo_) {
        throw exception::AppException(exception::ErrorCode::INTERNAL_SERVER_ERROR, "ProductService: repositories cannot be null");
    }
}

dto::ProductResponse ProductService::GetProductById(int64_t id) {
    auto product_opt = product_repo_->FindById(id);
    if (!product_opt.has_value()) {
        throw exception::NotFoundException("Product not found", "id");
    }
    return dto::ProductDtoBuilder().FromModel(*product_opt).Build();
}

std::vector<dto::ProductResponse> ProductService::GetProducts(const std::string& search, const std::string& category, int limit, int offset) {
    auto products = product_repo_->FindAll(search, category, limit, offset);
    std::vector<dto::ProductResponse> dtos;
    dtos.reserve(products.size());
    for (const auto& prod : products) {
        dtos.push_back(dto::ProductDtoBuilder().FromModel(prod).Build());
    }
    return dtos;
}

std::vector<dto::ProductResponse> ProductService::GetProductsBySeller(int64_t seller_id) {
    auto products = product_repo_->FindBySellerId(seller_id);
    std::vector<dto::ProductResponse> dtos;
    dtos.reserve(products.size());
    for (const auto& prod : products) {
        dtos.push_back(dto::ProductDtoBuilder().FromModel(prod).Build());
    }
    return dtos;
}

std::vector<std::string> ProductService::GetCategories() {
    return product_repo_->GetDistinctCategories();
}

dto::ProductResponse ProductService::CreateProduct(int64_t seller_id, const dto::CreateProductRequest& req) {
    // 1. Verify seller exists
    auto seller = user_repo_->FindById(seller_id);
    if (!seller.has_value() || seller->role != model::Role::SELLER) {
        throw exception::AuthorizationException("Only registered sellers can create products");
    }

    // 2. Validate product input
    util::ValidationUtil::ValidateProduct(
        req.name, req.description, req.price_cents, req.stock_qty, req.category, req.image_url
    );

    model::Product product{
        .seller_id = seller_id,
        .seller_name = seller->name,
        .name = util::ValidationUtil::Trim(req.name),
        .description = util::ValidationUtil::Trim(req.description),
        .price = util::Money::FromCents(req.price_cents),
        .stock_qty = req.stock_qty,
        .category = req.category,
        .image_url = req.image_url
    };

    int64_t id = product_repo_->Create(product);
    if (id <= 0) {
        throw exception::DatabaseException("Failed to create product listing");
    }

    product.id = id;
    spdlog::info("Seller {} created product '{}' with id {}", seller_id, product.name, id);
    return dto::ProductDtoBuilder().FromModel(product).Build();
}

dto::ProductResponse ProductService::UpdateProduct(int64_t seller_id, int64_t product_id, const dto::UpdateProductRequest& req, bool is_admin) {
    auto existing_opt = product_repo_->FindById(product_id);
    if (!existing_opt.has_value()) {
        throw exception::NotFoundException("Product not found", "id");
    }

    auto product = *existing_opt;

    // Strict ownership check: seller can only update their own product, unless Admin
    if (!is_admin && product.seller_id != seller_id) {
        throw exception::AuthorizationException("You can only edit your own products");
    }

    // Validate fields
    util::ValidationUtil::ValidateProduct(
        req.name, req.description, req.price_cents, req.stock_qty, req.category, req.image_url
    );

    product.name = util::ValidationUtil::Trim(req.name);
    product.description = util::ValidationUtil::Trim(req.description);
    product.price = util::Money::FromCents(req.price_cents);
    product.stock_qty = req.stock_qty;
    product.category = req.category;
    product.image_url = req.image_url;

    if (!product_repo_->Update(product)) {
        throw exception::DatabaseException("Failed to update product");
    }

    spdlog::info("Product {} updated by seller/admin {}", product_id, seller_id);
    return dto::ProductDtoBuilder().FromModel(product).Build();
}

bool ProductService::DeleteProduct(int64_t seller_id, int64_t product_id, bool is_admin) {
    auto existing_opt = product_repo_->FindById(product_id);
    if (!existing_opt.has_value()) {
        throw exception::NotFoundException("Product not found", "id");
    }

    // Ownership check
    if (!is_admin && existing_opt->seller_id != seller_id) {
        throw exception::AuthorizationException("You can only delete your own products");
    }

    bool success = product_repo_->Delete(product_id);
    if (success) {
        spdlog::info("Product {} deleted by user {}", product_id, seller_id);
    }
    return success;
}

} // namespace manimart::service
