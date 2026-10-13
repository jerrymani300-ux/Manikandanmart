#include "repository/RepositoryFactory.hpp"
#include "repository/SqlUserRepository.hpp"
#include "repository/SqlProductRepository.hpp"
#include "repository/SqlCartRepository.hpp"
#include "repository/SqlOrderRepository.hpp"
#include "repository/SqlReviewRepository.hpp"
#include "repository/SqlWishlistRepository.hpp"
#include "repository/SqlMigrationRepository.hpp"

namespace manimart::repository {

std::shared_ptr<IUserRepository> RepositoryFactory::CreateUserRepository() {
    return std::make_shared<SqlUserRepository>();
}

std::shared_ptr<IProductRepository> RepositoryFactory::CreateProductRepository() {
    return std::make_shared<SqlProductRepository>();
}

std::shared_ptr<ICartRepository> RepositoryFactory::CreateCartRepository() {
    return std::make_shared<SqlCartRepository>();
}

std::shared_ptr<IOrderRepository> RepositoryFactory::CreateOrderRepository() {
    return std::make_shared<SqlOrderRepository>();
}

std::shared_ptr<IReviewRepository> RepositoryFactory::CreateReviewRepository() {
    return std::make_shared<SqlReviewRepository>();
}

std::shared_ptr<IWishlistRepository> RepositoryFactory::CreateWishlistRepository() {
    return std::make_shared<SqlWishlistRepository>();
}

std::shared_ptr<IMigrationRepository> RepositoryFactory::CreateMigrationRepository() {
    return std::make_shared<SqlMigrationRepository>();
}

} // namespace manimart::repository
