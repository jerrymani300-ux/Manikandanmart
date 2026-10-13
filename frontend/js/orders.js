/**
 * ManiMart Orders History and Lifecycle Tracking Module
 */

const Orders = {
  async init() {
    await Auth.init();

    if (!Auth.isLoggedIn()) {
      window.location.href = 'login.html?redirect=orders';
      return;
    }

    try {
      const orders = await API.get('/orders');
      this.renderOrdersList(orders);
    } catch (err) {
      Toast.error('Failed to load orders: ' + err.message);
    }
  },

  renderOrdersList(orders) {
    const container = document.getElementById('orders-list');
    if (!container) return;

    if (!orders || orders.length === 0) {
      container.innerHTML = `
        <div style="text-align: center; padding: 3rem; background: white; border-radius: 12px; border: 1px dashed var(--border);">
          <div style="font-size: 3rem; margin-bottom: 1rem;">📦</div>
          <h3>No orders yet</h3>
          <p style="color: var(--text-muted); margin: 0.5rem 0 1.5rem;">You have not placed any orders on ManiMart so far.</p>
          <a href="products.html" class="nav-btn btn-primary">Discover Products</a>
        </div>
      `;
      return;
    }

    container.innerHTML = orders.map(ord => `
      <div class="card" style="margin-bottom: 1.5rem;">
        <div style="display: flex; justify-content: space-between; align-items: center; border-bottom: 1px solid var(--border); padding-bottom: 1rem; margin-bottom: 1rem;">
          <div>
            <span style="font-weight: 800; font-size: 1.1rem;">Order #${ord.id}</span>
            <span style="color: var(--text-muted); font-size: 0.85rem; margin-left: 0.75rem;">Placed on ${ord.created_at}</span>
          </div>
          <div>
            <span class="badge badge-${ord.status.toLowerCase()}">${ord.status}</span>
          </div>
        </div>

        <div>
          ${ord.items.map(item => `
            <div style="display: flex; justify-content: space-between; align-items: center; padding: 0.6rem 0;">
              <div style="display: flex; align-items: center; gap: 0.75rem;">
                <img src="${item.product_image}" alt="${item.product_name}" style="width: 50px; height: 50px; border-radius: 6px; object-fit: cover;">
                <div>
                  <a href="product.html?id=${item.product_id}" style="font-weight: 600;">${item.product_name}</a>
                  <div style="font-size: 0.8rem; color: var(--text-muted);">${item.quantity} × ${item.formatted_unit_price}</div>
                </div>
              </div>
              <div style="font-weight: 600;">${item.formatted_subtotal}</div>
            </div>
          `).join('')}
        </div>

        <div style="display: flex; justify-content: space-between; align-items: center; border-top: 1px solid var(--border); padding-top: 1rem; margin-top: 1rem;">
          <div>
            ${(ord.status === 'PENDING' || ord.status === 'CONFIRMED') ? `
              <button onclick="Orders.cancelOrder(${ord.id})" class="nav-btn btn-outline" style="color: var(--danger); border-color: #fca5a5; font-size: 0.85rem; cursor: pointer;">
                Cancel Order
              </button>
            ` : ''}
            ${ord.status === 'DELIVERED' ? `
              <span style="color: var(--success); font-size: 0.85rem; font-weight: 600;">✓ Delivered - Eligible for Review</span>
            ` : ''}
          </div>
          <div style="text-align: right;">
            <span style="font-size: 0.9rem; color: var(--text-muted);">Total: </span>
            <span style="font-size: 1.25rem; font-weight: 800; color: var(--secondary);">${ord.formatted_total}</span>
          </div>
        </div>
      </div>
    `).join('');
  },

  async cancelOrder(orderId) {
    if (!confirm(`Are you sure you want to cancel Order #${orderId}?`)) return;

    try {
      await API.post(`/orders/${orderId}/cancel`);
      Toast.success(`Order #${orderId} was cancelled successfully`);
      this.init();
    } catch (err) {
      Toast.error(err.message || 'Failed to cancel order');
    }
  }
};
