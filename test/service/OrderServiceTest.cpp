#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "service/OrderService.hpp"
#include "test_helpers/MockRepositories.hpp"
#include "exception/AppException.hpp"
#include "repository/DatabaseConnection.hpp"

using namespace manimart;
using namespace testing;

class OrderServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize in-memory test database for transaction support
        repository::DatabaseConnection::GetInstance().Initialize("sqlite", ":memory:");

        mock_order_repo_ = std::make_shared<test::MockOrderRepository>();
        mock_cart_repo_ = std::make_shared<test::MockCartRepository>();
        mock_product_repo_ = std::make_shared<test::MockProductRepository>();
        mock_user_repo_ = std::make_shared<test::MockUserRepository>();
        mock_payment_strategy_ = std::make_shared<test::MockPaymentStrategy>();

        order_service_ = std::make_unique<service::OrderService>(
            mock_order_repo_,
            mock_cart_repo_,
            mock_product_repo_,
            mock_user_repo_,
            mock_payment_strategy_
        );
    }

    std::shared_ptr<test::MockOrderRepository> mock_order_repo_;
    std::shared_ptr<test::MockCartRepository> mock_cart_repo_;
    std::shared_ptr<test::MockProductRepository> mock_product_repo_;
    std::shared_ptr<test::MockUserRepository> mock_user_repo_;
    std::shared_ptr<test::MockPaymentStrategy> mock_payment_strategy_;
    std::unique_ptr<service::OrderService> order_service_;
};

TEST_F(OrderServiceTest, CheckoutEmptyCartThrowsValidation) {
    model::User buyer{.id = 10, .name = "Alice", .email = "alice@example.com"};
    EXPECT_CALL(*mock_user_repo_, FindById(10)).WillOnce(Return(buyer));
    EXPECT_CALL(*mock_cart_repo_, FindByUserId(10)).WillOnce(Return(std::vector<model::CartItem>{}));

    dto::CheckoutRequest req{.shipping_address = "123 Market St", .payment_method = "MOCK_CARD"};
    EXPECT_THROW(order_service_->Checkout(10, req), exception::ValidationException);
}

TEST_F(OrderServiceTest, CheckoutSuccess) {
    model::User buyer{.id = 10, .name = "Alice", .email = "alice@example.com"};
    EXPECT_CALL(*mock_user_repo_, FindById(10)).WillOnce(Return(buyer));

    model::CartItem item{
        .id = 1,
        .user_id = 10,
        .product_id = 101,
        .product_name = "Book",
        .product_price = util::Money::FromCents(1500),
        .product_stock = 10,
        .quantity = 2
    };
    EXPECT_CALL(*mock_cart_repo_, FindByUserId(10)).WillOnce(Return(std::vector<model::CartItem>{item}));

    model::Product product{
        .id = 101,
        .name = "Book",
        .price = util::Money::FromCents(1500),
        .stock_qty = 10
    };
    EXPECT_CALL(*mock_product_repo_, FindById(101)).WillOnce(Return(product));

    provider::PaymentResult payment_res{
        .success = true,
        .transaction_id = "TXN-12345",
        .message = "Payment successful"
    };
    EXPECT_CALL(*mock_payment_strategy_, ProcessPayment(10, util::Money::FromCents(3000), "MOCK_CARD", "123 Market St"))
        .WillOnce(Return(payment_res));

    EXPECT_CALL(*mock_order_repo_, CreateOrder(_)).WillOnce(Return(777));
    EXPECT_CALL(*mock_product_repo_, UpdateStock(101, -2)).WillOnce(Return(true));
    EXPECT_CALL(*mock_cart_repo_, ClearCart(10)).WillOnce(Return(true));

    model::Order created_order{
        .id = 777,
        .buyer_id = 10,
        .buyer_name = "Alice",
        .buyer_email = "alice@example.com",
        .status = model::OrderStatus::CONFIRMED,
        .total_amount = util::Money::FromCents(3000),
        .created_at = "2026-09-22 10:00:00"
    };
    EXPECT_CALL(*mock_order_repo_, FindById(777)).WillOnce(Return(created_order));

    dto::CheckoutRequest req{.shipping_address = "123 Market St", .payment_method = "MOCK_CARD"};
    auto res = order_service_->Checkout(10, req);

    EXPECT_EQ(res.id, 777);
    EXPECT_EQ(res.total_amount_cents, 3000);
    EXPECT_EQ(res.formatted_total, "$30.00");
    EXPECT_EQ(res.status, "CONFIRMED");
}
