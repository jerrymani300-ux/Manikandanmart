#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "service/ProductService.hpp"
#include "test_helpers/MockRepositories.hpp"
#include "exception/AppException.hpp"

using namespace manimart;
using namespace testing;

class ProductServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        mock_product_repo_ = std::make_shared<test::MockProductRepository>();
        mock_user_repo_ = std::make_shared<test::MockUserRepository>();
        product_service_ = std::make_unique<service::ProductService>(mock_product_repo_, mock_user_repo_);
    }

    std::shared_ptr<test::MockProductRepository> mock_product_repo_;
    std::shared_ptr<test::MockUserRepository> mock_user_repo_;
    std::unique_ptr<service::ProductService> product_service_;
};

TEST_F(ProductServiceTest, CreateProductSuccess) {
    model::User seller{
        .id = 10,
        .name = "Tech Seller",
        .email = "seller@tech.com",
        .role = model::Role::SELLER
    };

    EXPECT_CALL(*mock_user_repo_, FindById(10))
        .WillOnce(Return(seller));

    EXPECT_CALL(*mock_product_repo_, Create(_))
        .WillOnce(Return(501));

    dto::CreateProductRequest req{
        .name = "Wireless Mouse",
        .description = "Ergonomic 2.4GHz wireless optical mouse with USB receiver",
        .price_cents = 2499,
        .stock_qty = 50,
        .category = "Electronics",
        .image_url = "https://images.unsplash.com/photo-mouse.jpg"
    };

    auto res = product_service_->CreateProduct(10, req);
    EXPECT_EQ(res.id, 501);
    EXPECT_EQ(res.name, "Wireless Mouse");
    EXPECT_EQ(res.price_cents, 2499);
    EXPECT_EQ(res.formatted_price, "$24.99");
    EXPECT_EQ(res.category, "Electronics");
}

TEST_F(ProductServiceTest, CreateProductBuyerForbidden) {
    model::User buyer{
        .id = 20,
        .name = "Buyer User",
        .email = "buyer@gmail.com",
        .role = model::Role::BUYER
    };

    EXPECT_CALL(*mock_user_repo_, FindById(20))
        .WillOnce(Return(buyer));

    dto::CreateProductRequest req{
        .name = "Laptop",
        .description = "High end laptop",
        .price_cents = 99900,
        .stock_qty = 10,
        .category = "Electronics",
        .image_url = "https://example.com/laptop.jpg"
    };

    EXPECT_THROW(product_service_->CreateProduct(20, req), exception::AuthorizationException);
}

TEST_F(ProductServiceTest, UpdateProductOwnershipEnforcement) {
    model::Product existing_prod{
        .id = 55,
        .seller_id = 10, // Owned by seller 10
        .name = "Original Name",
        .description = "Original Description",
        .price = util::Money::FromCents(1000),
        .stock_qty = 10,
        .category = "Electronics",
        .image_url = "https://example.com/img.jpg"
    };

    EXPECT_CALL(*mock_product_repo_, FindById(55))
        .WillOnce(Return(existing_prod));

    dto::UpdateProductRequest update_req{
        .name = "Hacked Name",
        .description = "Hacked Description",
        .price_cents = 500,
        .stock_qty = 5,
        .category = "Electronics",
        .image_url = "https://example.com/img.jpg"
    };

    // Seller 99 trying to update Seller 10's product must throw AuthorizationException
    EXPECT_THROW(product_service_->UpdateProduct(99, 55, update_req, false), exception::AuthorizationException);
}
