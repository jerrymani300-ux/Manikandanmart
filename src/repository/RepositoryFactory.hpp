#pragma once

#include <memory>
#include "repository/IUserRepository.hpp"
#include "repository/IProductRepository.hpp"
#include "repository/ICartRepository.hpp"
#include "repository/IOrderRepository.hpp"
#include "repository/IReviewRepository.hpp"
#include "repository/IWishlistRepository.hpp"
#include "repository/IMigrationRepository.hpp"

namespace manimart::repository {

class RepositoryFactory {
public:
    static std::shared_ptr<IUserRepository> CreateUserRepository();
    static std::shared_ptr<IProductRepository> CreateProductRepository();
    static std::shared_ptr<ICartRepository> CreateCartRepository();
    static std::shared_ptr<IOrderRepository> CreateOrderRepository();
    static std::shared_ptr<IReviewRepository> CreateReviewRepository();
    static std::shared_ptr<IWishlistRepository> CreateWishlistRepository();
    static std::shared_ptr<IMigrationRepository> CreateMigrationRepository();
};

} // namespace manimart::repository
