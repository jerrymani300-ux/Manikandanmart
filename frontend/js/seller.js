/**
 * ManiMart Seller Portal JavaScript Module
 */

const Seller = {
  currentEditingId: null,

  async init() {
    await Auth.init();

    if (!Auth.isLoggedIn() || !Auth.isSeller()) {
      window.location.href = 'login.html?redirect=seller';
      return;
    }

    try {
      const dashboard = await API.get('/seller/dashboard');
      this.renderDashboard(dashboard);
      this.loadProducts();
    } catch (err) {
      Toast.error('Failed to load seller portal: ' + err.message);
    }
  },

  renderDashboard(stats) {
    document.getElementById('seller-total-products').textContent = stats.total_products || 0;
    document.getElementById('seller-total-orders').textContent = stats.total_orders || 0;
    document.getElementById('seller-total-revenue').textContent = stats.formatted_revenue || '$0.00';
    document.getElementById('seller-pending-orders').textContent = stats.pending_orders || 0;
  },

  async loadProducts() {
    const tableBody = document.getElementById('seller-products-table');
    if (!tableBody) return;

    try {
      const products = await API.get('/seller/products');
      if (!products || products.length === 0) {
        tableBody.innerHTML = `<tr><td colspan="6" style="text-align: center; color: #64748b;">You haven't listed any products yet. Click 'Add New Product' to get started.</td></tr>`;
        return;
      }

      tableBody.innerHTML = products.map(p => `
        <tr>
          <td>
            <div style="display: flex; align-items: center; gap: 0.75rem;">
              <img src="${p.image_url}" alt="${p.name}" style="width: 40px; height: 40px; border-radius: 4px; object-fit: cover;">
              <span style="font-weight: 600;">${p.name}</span>
            </div>
          </td>
          <td><span class="badge" style="background: #f1f5f9; color: #475569;">${p.category}</span></td>
          <td style="font-weight: 600;">${p.formatted_price}</td>
          <td>${p.stock_qty}</td>
          <td>${p.average_rating > 0 ? `★ ${p.average_rating.toFixed(1)} (${p.review_count})` : 'No reviews'}</td>
          <td>
            <button onclick="Seller.openEditModal(${JSON.stringify(p).replace(/"/g, '&quot;')})" class="nav-btn btn-outline" style="padding: 0.25rem 0.5rem; font-size: 0.8rem; margin-right: 0.35rem;">Edit</button>
            <button onclick="Seller.deleteProduct(${p.id})" class="nav-btn btn-outline" style="padding: 0.25rem 0.5rem; font-size: 0.8rem; color: var(--danger); border-color: #fca5a5;">Delete</button>
          </td>
        </tr>
      `).join('');
    } catch (err) {
      Toast.error('Failed to load products: ' + err.message);
    }
  },

  openAddModal() {
    this.currentEditingId = null;
    document.getElementById('modal-title').textContent = 'Add New Product';
    document.getElementById('product-form').reset();
    document.getElementById('product-modal').classList.remove('hidden');
  },

  openEditModal(product) {
    this.currentEditingId = product.id;
    document.getElementById('modal-title').textContent = 'Edit Product';
    document.getElementById('prod-name').value = product.name;
    document.getElementById('prod-category').value = product.category;
    document.getElementById('prod-price').value = (product.price_cents / 100).toFixed(2);
    document.getElementById('prod-stock').value = product.stock_qty;
    document.getElementById('prod-image').value = product.image_url;
    document.getElementById('prod-desc').value = product.description;
    document.getElementById('product-modal').classList.remove('hidden');
  },

  closeModal() {
    document.getElementById('product-modal').classList.add('hidden');
  },

  async handleProductSubmit(e) {
    e.preventDefault();

    const priceInput = parseFloat(document.getElementById('prod-price').value);
    const priceCents = Math.round(priceInput * 100);

    const payload = {
      name: document.getElementById('prod-name').value,
      category: document.getElementById('prod-category').value,
      price_cents: priceCents,
      stock_qty: parseInt(document.getElementById('prod-stock').value),
      image_url: document.getElementById('prod-image').value,
      description: document.getElementById('prod-desc').value
    };

    try {
      if (this.currentEditingId) {
        await API.put(`/products/${this.currentEditingId}`, payload);
        Toast.success('Product updated successfully!');
      } else {
        await API.post('/products', payload);
        Toast.success('Product added successfully!');
      }
      this.closeModal();
      this.init();
    } catch (err) {
      Toast.error(err.message || 'Failed to save product');
    }
  },

  async deleteProduct(id) {
    if (!confirm('Are you sure you want to permanently delete this product?')) return;

    try {
      await API.delete(`/products/${id}`);
      Toast.success('Product deleted successfully');
      this.init();
    } catch (err) {
      Toast.error(err.message || 'Failed to delete product');
    }
  }
};
