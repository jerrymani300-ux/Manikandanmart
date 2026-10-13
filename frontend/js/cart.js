/**
 * ManiMart Cart Management Module
 */

const Cart = {
  async addItem(productId, quantity = 1) {
    await Auth.init();

    if (Auth.isLoggedIn()) {
      try {
        const cart = await API.post('/cart/items', { productId, quantity });
        this.updateBadge(cart.total_items);
        Toast.success('Item added to cart!');
      } catch (err) {
        Toast.error(err.message || 'Failed to add item to cart');
      }
    } else {
      // Guest Cart in localStorage
      let guestCart = [];
      try {
        guestCart = JSON.parse(localStorage.getItem('manimart_guest_cart') || '[]');
      } catch {
        guestCart = [];
      }

      const existingIndex = guestCart.findIndex(item => item.productId === productId || item.product_id === productId);
      if (existingIndex >= 0) {
        guestCart[existingIndex].quantity += quantity;
      } else {
        guestCart.push({ productId, product_id: productId, quantity });
      }

      localStorage.setItem('manimart_guest_cart', JSON.stringify(guestCart));
      const totalItems = guestCart.reduce((sum, i) => sum + i.quantity, 0);
      this.updateBadge(totalItems);
      Toast.success('Item added to cart! Sign in to checkout anytime.');
    }
  },

  async updateQuantity(productId, quantity) {
    await Auth.init();

    if (Auth.isLoggedIn()) {
      try {
        const cart = await API.put(`/cart/items/${productId}`, { quantity });
        this.updateBadge(cart.total_items);
        this.renderCart(cart);
        Toast.info('Cart updated');
      } catch (err) {
        Toast.error(err.message || 'Failed to update quantity');
      }
    } else {
      let guestCart = JSON.parse(localStorage.getItem('manimart_guest_cart') || '[]');
      const index = guestCart.findIndex(item => item.productId === productId || item.product_id === productId);
      if (index >= 0) {
        if (quantity <= 0) {
          guestCart.splice(index, 1);
        } else {
          guestCart[index].quantity = quantity;
        }
      }
      localStorage.setItem('manimart_guest_cart', JSON.stringify(guestCart));
      this.loadCartPage();
    }
  },

  async removeItem(productId) {
    await Auth.init();

    if (Auth.isLoggedIn()) {
      try {
        const cart = await API.delete(`/cart/items/${productId}`);
        this.updateBadge(cart.total_items);
        this.renderCart(cart);
        Toast.info('Item removed from cart');
      } catch (err) {
        Toast.error(err.message || 'Failed to remove item');
      }
    } else {
      let guestCart = JSON.parse(localStorage.getItem('manimart_guest_cart') || '[]');
      guestCart = guestCart.filter(item => item.productId !== productId && item.product_id !== productId);
      localStorage.setItem('manimart_guest_cart', JSON.stringify(guestCart));
      this.loadCartPage();
      Toast.info('Item removed from cart');
    }
  },

  updateBadge(count) {
    const badge = document.getElementById('cart-badge');
    if (badge) {
      badge.textContent = count || 0;
      badge.style.display = count > 0 ? 'inline-block' : 'none';
    }
  },

  async loadCartPage() {
    await Auth.init();

    if (!Auth.isLoggedIn()) {
      // If user is guest, check guest cart
      const guestCartStr = localStorage.getItem('manimart_guest_cart');
      let guestCart = [];
      try {
        guestCart = JSON.parse(guestCartStr || '[]');
      } catch {
        guestCart = [];
      }

      if (guestCart.length === 0) {
        this.renderEmptyCart();
        return;
      }

      // Fetch product details for guest cart items
      try {
        const allProducts = await API.get('/products');
        const items = [];
        let totalCents = 0;
        let totalItems = 0;

        for (const gItem of guestCart) {
          const pid = gItem.productId || gItem.product_id;
          const prod = allProducts.find(p => p.id === pid);
          if (prod) {
            const subtotal = prod.price_cents * gItem.quantity;
            totalCents += subtotal;
            totalItems += gItem.quantity;
            items.push({
              product_id: prod.id,
              product_name: prod.name,
              product_image_url: prod.image_url,
              formatted_unit_price: prod.formatted_price,
              unit_price_cents: prod.price_cents,
              product_stock: prod.stock_qty,
              quantity: gItem.quantity,
              formatted_subtotal: `$${(subtotal / 100).toFixed(2)}`
            });
          }
        }

        const syntheticCart = {
          items,
          total_items: totalItems,
          total_amount_cents: totalCents,
          formatted_total: `$${(totalCents / 100).toFixed(2)}`
        };

        this.updateBadge(totalItems);
        this.renderCart(syntheticCart, true);
        return;
      } catch (err) {
        console.error('Error rendering guest cart:', err);
      }
    }

    try {
      const cart = await API.get('/cart');
      this.updateBadge(cart.total_items);
      this.renderCart(cart);
    } catch (err) {
      Toast.error('Failed to load cart: ' + err.message);
    }
  },

  renderEmptyCart() {
    const container = document.getElementById('cart-items-list');
    const summaryContainer = document.getElementById('cart-summary');
    if (container) {
      container.innerHTML = `
        <div style="text-align: center; padding: 3rem; background: white; border-radius: 12px; border: 1px dashed var(--border);">
          <div style="font-size: 3rem; margin-bottom: 1rem;">🛒</div>
          <h3>Your cart is empty</h3>
          <p style="color: var(--text-muted); margin: 0.5rem 0 1.5rem;">Explore our marketplace catalog to find great deals.</p>
          <a href="products.html" class="nav-btn btn-primary">Start Shopping</a>
        </div>
      `;
    }
    if (summaryContainer) summaryContainer.style.display = 'none';
  },

  renderCart(cart, isGuest = false) {
    const container = document.getElementById('cart-items-list');
    const summaryContainer = document.getElementById('cart-summary');
    if (!container) return;

    if (!cart.items || cart.items.length === 0) {
      this.renderEmptyCart();
      return;
    }

    if (summaryContainer) summaryContainer.style.display = 'block';

    container.innerHTML = cart.items.map(item => `
      <div class="cart-item">
        <img src="${item.product_image_url}" alt="${item.product_name}" class="cart-item-img">
        <div class="cart-item-info">
          <a href="product.html?id=${item.product_id}" class="cart-item-title">${item.product_name}</a>
          <div class="cart-item-price">${item.formatted_unit_price} each</div>
        </div>
        <div class="qty-control">
          <button onclick="Cart.updateQuantity(${item.product_id}, ${item.quantity - 1})" class="qty-btn">-</button>
          <span class="qty-input" style="line-height: 42px;">${item.quantity}</span>
          <button onclick="Cart.updateQuantity(${item.product_id}, ${item.quantity + 1})" class="qty-btn" ${item.quantity >= item.product_stock ? 'disabled' : ''}>+</button>
        </div>
        <div class="cart-item-total" style="font-weight: 700; min-width: 80px; text-align: right;">
          ${item.formatted_subtotal}
        </div>
        <button onclick="Cart.removeItem(${item.product_id})" class="btn-remove" title="Remove item">&times;</button>
      </div>
    `).join('');

    const subtotalEl = document.getElementById('summary-subtotal');
    const totalEl = document.getElementById('summary-total');
    if (subtotalEl) subtotalEl.textContent = cart.formatted_total;
    if (totalEl) totalEl.textContent = cart.formatted_total;

    const checkoutBtn = document.getElementById('btn-proceed-checkout');
    if (checkoutBtn) {
      if (isGuest) {
        checkoutBtn.href = 'login.html?redirect=checkout';
        checkoutBtn.innerHTML = 'Sign In to Checkout &rarr;';
      } else {
        checkoutBtn.href = 'checkout.html';
        checkoutBtn.innerHTML = 'Proceed to Checkout &rarr;';
      }
    }
  }
};
