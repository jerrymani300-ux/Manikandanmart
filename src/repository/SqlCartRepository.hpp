#pragma once

#include "repository/ICartRepository.hpp"
#include "repository/DatabaseConnection.hpp"

namespace manimart::repository {

class SqlCartRepository : public ICartRepository {
public:
    std::vector<model::CartItem> FindByUserId(int64_t user_id) override;
    std::optional<model::CartItem> FindByUserAndProduct(int64_t user_id, int64_t product_id) override;
    bool AddOrUpdateItem(int64_t user_id, int64_t product_id, int quantity) override;
    bool UpdateQuantity(int64_t user_id, int64_t product_id, int quantity) override;
    bool RemoveItem(int64_t user_id, int64_t product_id) override;
    bool ClearCart(int64_t user_id) override;
    int GetTotalItemCount(int64_t user_id) override;

private:
    static model::CartItem MapRow(const SqlRow& row);
};

} // namespace manimart::repository
