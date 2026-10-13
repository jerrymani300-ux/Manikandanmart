#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "service/CartService.hpp"
#include "test_helpers/MockRepositories.hpp"
#include "exception/AppException.hpp"

using namespace manimart;
using namespace testing;

class CartServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        mock_cart_repo_ = std::make_shared<test::MockCartRepository>();
        mock_product_repo_ = std::make_shared<test::MockProductRepository>();
        cart_service_ = std::make_unique<service::CartService>(mock_cart_repo_, mock_product_repo_);
    }

    std::shared_ptr<test::MockCartRepository> mock_cart_repo_;
    std::shared_ptr<test::MockProductRepository> mock_product_repo_;
    std::unique_ptr<service::CartService> cart_service_;
};

TEST_F(CartServiceTest, AddToCartSuccess) {
    model::Product product{
        .id = 1,
        .name = "Headphones",
        .price = util::Money::FromCents(5000),
        .stock_qty = 10
    };

    EXPECT_CALL(*mock_product_repo_, FindById(1))
        .WillOnce(Return(product));

    EXPECT_CALL(*mock_cart_repo_, FindByUserAndProduct(100, 1))
        .WillOnce(Return(std::nullopt));

    EXPECT_CALL(*mock_cart_repo_, AddOrUpdateItem(100, 1, 2))
        .WillOnce(Return(true));

    model::CartItem cart_item{
        .id = 1,
        .user_id = 100,
        .product_id = 1,
        .product_name = "Headphones",
        .product_price = util::Money::FromCents(5000),
        .product_stock = 10,
        .quantity = 2
    };

    EXPECT_CALL(*mock_cart_repo_, FindByUserId(100))
        .WillOnce(Return(std::vector<model::CartItem>{cart_item}));

    dto::AddToCartRequest req{.product_id = 1, .quantity = 2};
    auto res = cart_service_->AddToCart(100, req);

    EXPECT_EQ(res.total_items, 2);
    EXPECT_EQ(res.total_amount_cents, 10000);
    EXPECT_EQ(res.formatted_total, "$100.00");
}

TEST_F(CartServiceTest, AddToCartExceedsStockThrowsValidation) {
    model::Product product{
        .id = 1,
        .name = "Rare Item",
        .price = util::Money::FromCents(1000),
        .stock_qty = 3
    };

    EXPECT_CALL(*mock_product_repo_, FindById(1))
        .WillOnce(Return(product));

    EXPECT_CALL(*mock_cart_repo_, FindByUserAndProduct(100, 1))
        .WillOnce(Return(std::nullopt));

    dto::AddToCartRequest req{.product_id = 1, .quantity = 5}; // 5 > 3 stock
    EXPECT_THROW(cart_service_->AddToCart(100, req), exception::ValidationException);
}
