/**
 * ManiMart Admin Portal JavaScript Module
 */

const Admin = {
  async init() {
    await Auth.init();

    if (!Auth.isLoggedIn() || !Auth.isAdmin()) {
      window.location.href = 'login.html?redirect=admin';
      return;
    }

    try {
      const stats = await API.get('/admin/dashboard');
      this.renderStats(stats);
      this.loadUsers();
      this.loadProducts();
      this.loadOrders();
    } catch (err) {
      Toast.error('Failed to load admin panel: ' + err.message);
    }
  },

  renderStats(stats) {
    document.getElementById('admin-total-users').textContent = stats.total_users || 0;
    document.getElementById('admin-total-products').textContent = stats.total_products || 0;
    document.getElementById('admin-total-orders').textContent = stats.total_orders || 0;
    document.getElementById('admin-total-sales').textContent = stats.formatted_gross_sales || '$0.00';
  },

  async loadUsers() {
    const tbody = document.getElementById('admin-users-table');
    if (!tbody) return;

    try {
      const users = await API.get('/admin/users');
      tbody.innerHTML = users.map(u => `
        <tr>
          <td>#${u.id}</td>
          <td style="font-weight: 600;">${u.name}</td>
          <td>${u.email}</td>
          <td><span class="badge badge-${u.role.toLowerCase()}">${u.role}</span></td>
          <td style="color: #64748b; font-size: 0.85rem;">${u.created_at}</td>
        </tr>
      `).join('');
    } catch (err) {
      Toast.error('Failed to load users: ' + err.message);
    }
  },

  async loadProducts() {
    const tbody = document.getElementById('admin-products-table');
    if (!tbody) return;

    try {
      const products = await API.get('/admin/products');
      tbody.innerHTML = products.map(p => `
        <tr>
          <td>#${p.id}</td>
          <td>
            <div style="display: flex; align-items: center; gap: 0.5rem;">
              <img src="${p.image_url}" alt="${p.name}" style="width: 32px; height: 32px; border-radius: 4px; object-fit: cover;">
              <span style="font-weight: 600;">${p.name}</span>
            </div>
          </td>
          <td>${p.seller_name}</td>
          <td><span class="badge" style="background: #f1f5f9; color: #475569;">${p.category}</span></td>
          <td style="font-weight: 600;">${p.formatted_price}</td>
          <td>${p.stock_qty}</td>
          <td>
            <button onclick="Admin.moderateDeleteProduct(${p.id}, '${p.name.replace(/'/g, "\\'")}')" class="nav-btn btn-danger" style="padding: 0.25rem 0.5rem; font-size: 0.75rem;">
              Remove / Moderate
            </button>
          </td>
        </tr>
      `).join('');
    } catch (err) {
      Toast.error('Failed to load products: ' + err.message);
    }
  },

  async loadOrders() {
    const tbody = document.getElementById('admin-orders-table');
    if (!tbody) return;

    try {
      const orders = await API.get('/admin/orders');
      tbody.innerHTML = orders.map(o => `
        <tr>
          <td>#${o.id}</td>
          <td>${o.buyer_name} (${o.buyer_email})</td>
          <td><span class="badge badge-${o.status.toLowerCase()}">${o.status}</span></td>
          <td style="font-weight: 700;">${o.formatted_total}</td>
          <td>${o.items.length} items</td>
          <td style="color: #64748b; font-size: 0.85rem;">${o.created_at}</td>
        </tr>
      `).join('');
    } catch (err) {
      Toast.error('Failed to load orders: ' + err.message);
    }
  },

  async moderateDeleteProduct(id, name) {
    if (!confirm(`ADMIN CONFIRMATION:\nAre you sure you want to remove listing '${name}' (ID: ${id}) from the platform?`)) return;

    try {
      await API.delete(`/admin/products/${id}`);
      Toast.success(`Listing #${id} successfully moderated and deleted.`);
      this.init();
    } catch (err) {
      Toast.error(err.message || 'Failed to moderate product');
    }
  }
};
