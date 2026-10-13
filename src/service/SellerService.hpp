#pragma once

#include <memory>
#include "repository/IProductRepository.hpp"
#include "repository/IOrderRepository.hpp"
#include "dto/SellerDto.hpp"

namespace manimart::service {

class SellerService {
public:
    SellerService(
        std::shared_ptr<repository::IProductRepository> product_repo,
        std::shared_ptr<repository::IOrderRepository> order_repo
    );

    dto::SellerDashboardStatsResponse GetDashboardStats(int64_t seller_id);

private:
    std::shared_ptr<repository::IProductRepository> product_repo_;
    std::shared_ptr<repository::IOrderRepository> order_repo_;
};

} // namespace manimart::service
