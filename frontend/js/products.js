/**
 * ManiMart Products Catalog and Details Module
 */

const Products = {
  currentCategory: '',
  searchQuery: '',

  async loadProducts(containerId = 'products-grid', category = '', search = '') {
    const container = document.getElementById(containerId);
    if (!container) return;

    container.innerHTML = `<div style="grid-column: 1/-1; text-align: center; padding: 2rem; color: #64748b;">Loading products...</div>`;

    try {
      const params = {};
      if (category) params.category = category;
      if (search) params.search = search;

      const products = await API.get('/products', params);

      if (!products || products.length === 0) {
        container.innerHTML = `
          <div style="grid-column: 1/-1; text-align: center; padding: 3rem; background: white; border-radius: 12px; border: 1px dashed #cbd5e1;">
            <p style="font-size: 1.1rem; color: #64748b; margin-bottom: 1rem;">No products found matching your criteria.</p>
            <button onclick="Products.filterByCategory('')" class="nav-btn btn-outline">Clear Filters</button>
          </div>`;
        return;
      }

      container.innerHTML = products.map(p => this.renderProductCard(p)).join('');
    } catch (err) {
      container.innerHTML = `<div style="grid-column: 1/-1; text-align: center; color: #ef4444;">Failed to load products: ${err.message}</div>`;
    }
  },

  renderProductCard(p) {
    const stockClass = p.stock_qty > 10 ? 'stock-in' : (p.stock_qty > 0 ? 'stock-low' : 'stock-out');
    const stockText = p.stock_qty > 10 ? 'In Stock' : (p.stock_qty > 0 ? `Only ${p.stock_qty} left` : 'Out of Stock');
    const stars = '★'.repeat(Math.round(p.average_rating)) + '☆'.repeat(5 - Math.round(p.average_rating));

    return `
      <div class="product-card">
        <a href="product.html?id=${p.id}" class="product-img-wrapper">
          <img src="${p.image_url}" alt="${p.name}" class="product-img" loading="lazy" onerror="this.src='https://images.unsplash.com/photo-1505740420928-5e560c06d30e?w=600'">
          <span class="product-category-tag">${p.category}</span>
        </a>
        <div class="product-body">
          <span class="product-seller">Sold by ${p.seller_name || 'Verified Seller'}</span>
          <a href="product.html?id=${p.id}" class="product-name" title="${p.name}">${p.name}</a>
          <div class="product-rating">
            <span>${stars}</span>
            <span class="product-rating-count">(${p.review_count})</span>
          </div>
          <div class="product-footer">
            <div>
              <div class="product-price">${p.formatted_price}</div>
              <span class="stock-status ${stockClass}">${stockText}</span>
            </div>
            ${p.stock_qty > 0 ? `
              <button onclick="Cart.addItem(${p.id}, 1)" class="nav-btn btn-primary" style="padding: 0.45rem 0.85rem; font-size: 0.85rem;">
                + Cart
              </button>
            ` : `
              <button disabled class="nav-btn btn-outline" style="padding: 0.45rem 0.85rem; font-size: 0.85rem; opacity: 0.5;">
                Sold Out
              </button>
            `}
          </div>
        </div>
      </div>
    `;
  },

  filterByCategory(category) {
    this.currentCategory = category;
    document.querySelectorAll('.category-chip').forEach(chip => {
      if (chip.dataset.category === category || (!category && chip.dataset.category === 'ALL')) {
        chip.classList.add('active');
      } else {
        chip.classList.remove('active');
      }
    });
    this.loadProducts('products-grid', this.currentCategory, this.searchQuery);
  },

  async loadProductDetails() {
    const params = new URLSearchParams(window.location.search);
    const productId = params.get('id');
    if (!productId) {
      window.location.href = 'products.html';
      return;
    }

    try {
      const product = await API.get(`/products/${productId}`);
      const reviewsData = await API.get(`/products/${productId}/reviews`);

      document.title = `${product.name} - ManiMart`;
      document.getElementById('product-title').textContent = product.name;
      document.getElementById('product-category').textContent = product.category;
      document.getElementById('product-seller').textContent = `Sold by ${product.seller_name}`;
      document.getElementById('product-price').textContent = product.formatted_price;
      document.getElementById('product-desc').textContent = product.description;
      document.getElementById('product-img').src = product.image_url;

      const stockEl = document.getElementById('product-stock');
      if (product.stock_qty > 0) {
        stockEl.textContent = `In Stock (${product.stock_qty} units available)`;
        stockEl.className = 'stock-status stock-in';
      } else {
        stockEl.textContent = 'Out of Stock';
        stockEl.className = 'stock-status stock-out';
        const addBtn = document.getElementById('btn-add-cart');
        if (addBtn) addBtn.disabled = true;
      }

      // Render reviews
      const reviewsList = document.getElementById('reviews-list');
      if (reviewsList) {
        if (!reviewsData.reviews || reviewsData.reviews.length === 0) {
          reviewsList.innerHTML = `<p style="color: #64748b; font-style: italic;">No reviews yet. Be the first verified buyer to review this product!</p>`;
        } else {
          reviewsList.innerHTML = reviewsData.reviews.map(r => `
            <div style="padding: 1rem 0; border-bottom: 1px solid var(--border);">
              <div style="display: flex; justify-content: space-between; align-items: center; margin-bottom: 0.35rem;">
                <span style="font-weight: 600;">${r.user_name} <span class="badge badge-buyer" style="margin-left: 0.5rem; font-size: 0.7rem;">Verified Buyer</span></span>
                <span style="color: var(--accent);">${'★'.repeat(r.rating)}${'☆'.repeat(5 - r.rating)}</span>
              </div>
              <p style="color: #334155; font-size: 0.95rem;">${r.comment}</p>
              <span style="font-size: 0.75rem; color: #94a3b8;">${r.created_at}</span>
            </div>
          `).join('');
        }
      }

      // Review submission button
      const reviewForm = document.getElementById('review-form');
      if (reviewForm) {
        reviewForm.onsubmit = async (e) => {
          e.preventDefault();
          const rating = parseInt(document.getElementById('review-rating').value);
          const comment = document.getElementById('review-comment').value;

          try {
            await API.post(`/products/${productId}/reviews`, { rating, comment });
            Toast.success('Review submitted successfully!');
            setTimeout(() => location.reload(), 1000);
          } catch (err) {
            Toast.error(err.message || 'Failed to submit review');
          }
        };
      }

      // Add to cart click
      const addBtn = document.getElementById('btn-add-cart');
      if (addBtn) {
        addBtn.onclick = () => {
          const qty = parseInt(document.getElementById('product-qty')?.value || 1);
          Cart.addItem(product.id, qty);
        };
      }

    } catch (err) {
      Toast.error('Failed to load product details: ' + err.message);
    }
  }
};
