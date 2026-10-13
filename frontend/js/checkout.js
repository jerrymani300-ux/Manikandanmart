/**
 * ManiMart Checkout and Payment Confirmation Module
 */

const Checkout = {
  async init() {
    await Auth.init();

    if (!Auth.isLoggedIn()) {
      window.location.href = 'login.html?redirect=checkout';
      return;
    }

    // Sync any guest cart before reviewing order
    await Auth.syncGuestCart();

    try {
      const cart = await API.get('/cart');
      if (!cart.items || cart.items.length === 0) {
        Toast.info('Your cart is empty. Add items before checking out.');
        setTimeout(() => window.location.href = 'products.html', 1000);
        return;
      }

      this.renderOrderReview(cart);
    } catch (err) {
      Toast.error('Failed to load checkout details: ' + err.message);
    }
  },

  renderOrderReview(cart) {
    const list = document.getElementById('checkout-items-list');
    if (!list) return;

    list.innerHTML = cart.items.map(item => `
      <div style="display: flex; justify-content: space-between; align-items: center; padding: 0.75rem 0; border-bottom: 1px solid var(--border);">
        <div style="display: flex; align-items: center; gap: 0.75rem;">
          <img src="${item.product_image_url}" alt="${item.product_name}" style="width: 45px; height: 45px; border-radius: 6px; object-fit: cover;">
          <div>
            <div style="font-weight: 600; font-size: 0.95rem;">${item.product_name}</div>
            <div style="font-size: 0.8rem; color: var(--text-muted);">${item.quantity} × ${item.formatted_unit_price}</div>
          </div>
        </div>
        <div style="font-weight: 600;">${item.formatted_subtotal}</div>
      </div>
    `).join('');

    const subtotalEl = document.getElementById('checkout-subtotal');
    const totalEl = document.getElementById('checkout-total');
    if (subtotalEl) subtotalEl.textContent = cart.formatted_total;
    if (totalEl) totalEl.textContent = cart.formatted_total;
  },

  async placeOrder(event) {
    event.preventDefault();

    const address = document.getElementById('shipping-address').value;
    const paymentMethod = document.querySelector('input[name="payment_method"]:checked')?.value || 'MOCK_CARD';

    const btn = document.getElementById('btn-confirm-order');
    btn.disabled = true;
    btn.textContent = 'Processing Mock Payment...';

    try {
      const order = await API.post('/orders', {
        shipping_address: address,
        payment_method: paymentMethod
      });

      Toast.success('Order placed successfully!');
      setTimeout(() => {
        window.location.href = `orders.html?new_order_id=${order.id}`;
      }, 1000);
    } catch (err) {
      btn.disabled = false;
      btn.textContent = 'Confirm Order & Pay';
      Toast.error(err.message || 'Checkout failed');
    }
  }
};
