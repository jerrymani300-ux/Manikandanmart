#include <gtest/gtest.h>
#include "repository/DatabaseConnection.hpp"
#include "repository/SqlUserRepository.hpp"
#include "repository/SqlProductRepository.hpp"
#include "repository/SqlCartRepository.hpp"
#include "repository/SqlOrderRepository.hpp"
#include "repository/SqlReviewRepository.hpp"
#include "repository/SqlMigrationRepository.hpp"
#include "service/MigrationService.hpp"
#include "util/PasswordHasher.hpp"

using namespace manimart;

class RepositoryIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize clean in-memory database
        repository::DatabaseConnection::GetInstance().Initialize("sqlite", ":memory:");

        // Apply migrations
        auto mig_repo = std::make_shared<repository::SqlMigrationRepository>();
        service::MigrationService mig_service(mig_repo);
        mig_service.RunMigrations("db/migrations");

        user_repo_ = std::make_shared<repository::SqlUserRepository>();
        product_repo_ = std::make_shared<repository::SqlProductRepository>();
        cart_repo_ = std::make_shared<repository::SqlCartRepository>();
        order_repo_ = std::make_shared<repository::SqlOrderRepository>();
        review_repo_ = std::make_shared<repository::SqlReviewRepository>();
    }

    std::shared_ptr<repository::SqlUserRepository> user_repo_;
    std::shared_ptr<repository::SqlProductRepository> product_repo_;
    std::shared_ptr<repository::SqlCartRepository> cart_repo_;
    std::shared_ptr<repository::SqlOrderRepository> order_repo_;
    std::shared_ptr<repository::SqlReviewRepository> review_repo_;
};

TEST_F(RepositoryIntegrationTest, UserCrudAndRoleCounts) {
    model::User user{
        .name = "John Doe",
        .email = "john@example.com",
        .password_hash = "hashed_pass",
        .role = model::Role::BUYER
    };

    int64_t uid = user_repo_->Create(user);
    EXPECT_GT(uid, 0);

    auto found = user_repo_->FindById(uid);
    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(found->name, "John Doe");
    EXPECT_EQ(found->email, "john@example.com");
    EXPECT_EQ(found->role, model::Role::BUYER);

    EXPECT_EQ(user_repo_->CountByRole(model::Role::BUYER), 1);
    EXPECT_EQ(user_repo_->CountByRole(model::Role::SELLER), 0);
    EXPECT_EQ(user_repo_->CountAll(), 1);
}

TEST_F(RepositoryIntegrationTest, ProductStockUpdateAndSearch) {
    model::User seller{
        .name = "Seller One",
        .email = "seller1@shop.com",
        .password_hash = "hash",
        .role = model::Role::SELLER
    };
    int64_t sid = user_repo_->Create(seller);

    model::Product prod{
        .seller_id = sid,
        .name = "Noise Cancelling Headphones",
        .description = "Studio grade over-ear headphones",
        .price = util::Money::FromCents(19999),
        .stock_qty = 20,
        .category = "Electronics",
        .image_url = "https://example.com/headphones.jpg"
    };

    int64_t pid = product_repo_->Create(prod);
    EXPECT_GT(pid, 0);

    // Search by keyword
    auto search_results = product_repo_->FindAll("Noise", "Electronics");
    EXPECT_EQ(search_results.size(), 1);
    EXPECT_EQ(search_results[0].name, "Noise Cancelling Headphones");

    // Decrement stock
    bool stock_ok = product_repo_->UpdateStock(pid, -5);
    EXPECT_TRUE(stock_ok);

    auto updated_prod = product_repo_->FindById(pid);
    ASSERT_TRUE(updated_prod.has_value());
    EXPECT_EQ(updated_prod->stock_qty, 15);
}

TEST_F(RepositoryIntegrationTest, CartAddUpdateClear) {
    model::User buyer{.name = "Buyer", .email = "b@b.com", .password_hash = "h", .role = model::Role::BUYER};
    int64_t bid = user_repo_->Create(buyer);

    model::User seller{.name = "Seller", .email = "s@s.com", .password_hash = "h", .role = model::Role::SELLER};
    int64_t sid = user_repo_->Create(seller);

    model::Product prod{
        .seller_id = sid,
        .name = "Keyboard",
        .description = "Mech keyboard",
        .price = util::Money::FromCents(5000),
        .stock_qty = 10,
        .category = "Electronics",
        .image_url = "https://example.com/kb.jpg"
    };
    int64_t pid = product_repo_->Create(prod);

    // Add to cart
    cart_repo_->AddOrUpdateItem(bid, pid, 2);
    EXPECT_EQ(cart_repo_->GetTotalItemCount(bid), 2);

    auto items = cart_repo_->FindByUserId(bid);
    EXPECT_EQ(items.size(), 1);
    EXPECT_EQ(items[0].quantity, 2);

    // Clear cart
    cart_repo_->ClearCart(bid);
    EXPECT_EQ(cart_repo_->GetTotalItemCount(bid), 0);
}
