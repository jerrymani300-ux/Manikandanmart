#pragma once

#include <gmock/gmock.h>
#include "repository/IUserRepository.hpp"
#include "repository/IProductRepository.hpp"
#include "repository/ICartRepository.hpp"
#include "repository/IOrderRepository.hpp"
#include "repository/IReviewRepository.hpp"
#include "provider/PaymentStrategy.hpp"

namespace manimart::test {

class MockUserRepository : public repository::IUserRepository {
public:
    MOCK_METHOD(std::optional<model::User>, FindById, (int64_t id), (override));
    MOCK_METHOD(std::optional<model::User>, FindByEmail, (const std::string& email), (override));
    MOCK_METHOD(int64_t, Create, (const model::User& user), (override));
    MOCK_METHOD(bool, Update, (const model::User& user), (override));
    MOCK_METHOD(bool, Delete, (int64_t id), (override));
    MOCK_METHOD(std::vector<model::User>, FindAll, (int limit, int offset), (override));
    MOCK_METHOD(int, CountByRole, (model::Role role), (override));
    MOCK_METHOD(int, CountAll, (), (override));
};

class MockProductRepository : public repository::IProductRepository {
public:
    MOCK_METHOD(std::optional<model::Product>, FindById, (int64_t id), (override));
    MOCK_METHOD(std::vector<model::Product>, FindAll, (const std::string& search, const std::string& category, int limit, int offset), (override));
    MOCK_METHOD(std::vector<model::Product>, FindBySellerId, (int64_t seller_id), (override));
    MOCK_METHOD(int64_t, Create, (const model::Product& product), (override));
    MOCK_METHOD(bool, Update, (const model::Product& product), (override));
    MOCK_METHOD(bool, UpdateStock, (int64_t product_id, int delta_quantity), (override));
    MOCK_METHOD(bool, Delete, (int64_t id), (override));
    MOCK_METHOD(int, CountAll, (), (override));
    MOCK_METHOD(int, CountBySellerId, (int64_t seller_id), (override));
    MOCK_METHOD(std::vector<std::string>, GetDistinctCategories, (), (override));
};

class MockCartRepository : public repository::ICartRepository {
public:
    MOCK_METHOD(std::vector<model::CartItem>, FindByUserId, (int64_t user_id), (override));
    MOCK_METHOD(std::optional<model::CartItem>, FindByUserAndProduct, (int64_t user_id, int64_t product_id), (override));
    MOCK_METHOD(bool, AddOrUpdateItem, (int64_t user_id, int64_t product_id, int quantity), (override));
    MOCK_METHOD(bool, UpdateQuantity, (int64_t user_id, int64_t product_id, int quantity), (override));
    MOCK_METHOD(bool, RemoveItem, (int64_t user_id, int64_t product_id), (override));
    MOCK_METHOD(bool, ClearCart, (int64_t user_id), (override));
    MOCK_METHOD(int, GetTotalItemCount, (int64_t user_id), (override));
};

class MockOrderRepository : public repository::IOrderRepository {
public:
    MOCK_METHOD(std::optional<model::Order>, FindById, (int64_t id), (override));
    MOCK_METHOD(std::vector<model::Order>, FindByBuyerId, (int64_t buyer_id), (override));
    MOCK_METHOD(std::vector<model::Order>, FindBySellerId, (int64_t seller_id), (override));
    MOCK_METHOD(std::vector<model::Order>, FindAll, (int limit, int offset), (override));
    MOCK_METHOD(int64_t, CreateOrder, (const model::Order& order), (override));
    MOCK_METHOD(bool, UpdateStatus, (int64_t order_id, model::OrderStatus status), (override));
    MOCK_METHOD(bool, HasUserPurchasedProduct, (int64_t user_id, int64_t product_id), (override));
    MOCK_METHOD(int, CountAll, (), (override));
    MOCK_METHOD(int, CountBySellerId, (int64_t seller_id), (override));
    MOCK_METHOD(int64_t, GetTotalGrossSalesCents, (), (override));
    MOCK_METHOD(int64_t, GetSellerRevenueCents, (int64_t seller_id), (override));
};

class MockReviewRepository : public repository::IReviewRepository {
public:
    MOCK_METHOD(std::vector<model::Review>, FindByProductId, (int64_t product_id), (override));
    MOCK_METHOD(std::optional<model::Review>, FindByProductAndUser, (int64_t product_id, int64_t user_id), (override));
    MOCK_METHOD(int64_t, Create, (const model::Review& review), (override));
    MOCK_METHOD(bool, Delete, (int64_t id), (override));
    MOCK_METHOD(double, GetAverageRating, (int64_t product_id), (override));
    MOCK_METHOD(int, GetReviewCount, (int64_t product_id), (override));
};

class MockPaymentStrategy : public provider::IPaymentStrategy {
public:
    MOCK_METHOD(provider::PaymentResult, ProcessPayment, (int64_t buyer_id, const util::Money& amount, const std::string& payment_method, const std::string& billing_details), (override));
    MOCK_METHOD(std::string, GetStrategyName, (), (const, override));
};

} // namespace manimart::test
