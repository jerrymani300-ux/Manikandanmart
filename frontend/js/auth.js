/**
 * ManiMart Authentication and Session Management Module
 */

const Auth = {
  currentUser: null,
  initPromise: null,

  async init() {
    if (this.initPromise) return this.initPromise;

    this.initPromise = (async () => {
      try {
        const user = await API.get('/auth/me');
        this.currentUser = user;
        if (user) {
          localStorage.setItem('manimart_user', JSON.stringify(user));
          if (user.token || user.session_id) {
            localStorage.setItem('manimart_token', user.token || user.session_id);
          }
          // Sync any guest cart items to backend
          await this.syncGuestCart();
        }
      } catch {
        this.currentUser = null;
        localStorage.removeItem('manimart_user');
        localStorage.removeItem('manimart_token');
      }
      this.renderNav();
      return this.currentUser;
    })();

    return this.initPromise;
  },

  isLoggedIn() {
    return this.currentUser !== null;
  },

  isBuyer() {
    return this.currentUser?.role === 'BUYER';
  },

  isSeller() {
    return this.currentUser?.role === 'SELLER';
  },

  isAdmin() {
    return this.currentUser?.role === 'ADMIN';
  },

  getRedirectUrl(user) {
    const params = new URLSearchParams(window.location.search);
    const redirect = params.get('redirect');

    if (redirect === 'cart') return 'cart.html';
    if (redirect === 'checkout') return 'checkout.html';
    if (redirect === 'orders') return 'orders.html';
    if (redirect) return redirect.endsWith('.html') ? redirect : `${redirect}.html`;

    // Role-based default redirect
    if (user.role === 'ADMIN') return 'admin.html';
    if (user.role === 'SELLER') return 'seller.html';
    return 'index.html';
  },

  async login(email, password) {
    const user = await API.post('/auth/login', { email, password });
    this.currentUser = user;
    localStorage.setItem('manimart_user', JSON.stringify(user));
    if (user.token || user.session_id) {
      localStorage.setItem('manimart_token', user.token || user.session_id);
    }

    Toast.success(`Welcome back, ${user.name}!`);

    // Sync guest cart before navigating
    await this.syncGuestCart();

    const targetUrl = this.getRedirectUrl(user);
    setTimeout(() => {
      window.location.href = targetUrl;
    }, 600);

    return user;
  },

  async register(name, email, password, role) {
    const user = await API.post('/auth/register', { name, email, password, role });
    this.currentUser = user;
    localStorage.setItem('manimart_user', JSON.stringify(user));
    if (user.token || user.session_id) {
      localStorage.setItem('manimart_token', user.token || user.session_id);
    }

    Toast.success(`Account created successfully! Welcome, ${user.name}.`);

    // Sync guest cart before navigating
    await this.syncGuestCart();

    const targetUrl = this.getRedirectUrl(user);
    setTimeout(() => {
      window.location.href = targetUrl;
    }, 800);

    return user;
  },

  async logout() {
    try {
      await API.post('/auth/logout');
    } catch (e) {
      console.error(e);
    }
    this.currentUser = null;
    this.initPromise = null;
    localStorage.removeItem('manimart_user');
    localStorage.removeItem('manimart_token');
    Toast.info('Logged out successfully');
    setTimeout(() => {
      window.location.href = 'index.html';
    }, 400);
  },

  async syncGuestCart() {
    const guestCartStr = localStorage.getItem('manimart_guest_cart');
    if (!guestCartStr) return;

    try {
      const items = JSON.parse(guestCartStr);
      if (Array.isArray(items) && items.length > 0) {
        for (const item of items) {
          await API.post('/cart/items', {
            productId: item.productId || item.product_id,
            quantity: item.quantity || 1
          }).catch(err => console.warn('Failed to sync guest cart item:', err));
        }
        localStorage.removeItem('manimart_guest_cart');
      }
    } catch (err) {
      console.warn('Error parsing guest cart:', err);
    }
  },

  renderNav() {
    const navAuth = document.getElementById('nav-auth-controls');
    if (!navAuth) return;

    if (this.currentUser) {
      let roleSpecificLinks = '';
      if (this.isSeller()) {
        roleSpecificLinks = `<a href="seller.html" class="nav-link">Seller Portal</a>`;
      } else if (this.isAdmin()) {
        roleSpecificLinks = `<a href="admin.html" class="nav-link">Admin Panel</a>`;
      } else {
        roleSpecificLinks = `<a href="orders.html" class="nav-link">My Orders</a>`;
      }

      navAuth.innerHTML = `
        ${roleSpecificLinks}
        <a href="profile.html" class="nav-link">👤 ${this.currentUser.name}</a>
        <button onclick="Auth.logout()" class="nav-btn btn-outline" style="padding: 0.35rem 0.75rem; cursor: pointer;">Logout</button>
      `;
    } else {
      navAuth.innerHTML = `
        <a href="login.html" class="nav-link">Login</a>
        <a href="register.html" class="nav-btn btn-primary">Sign Up</a>
      `;
    }
  }
};

// Automatically initialize auth on page load
if (typeof document !== 'undefined') {
  document.addEventListener('DOMContentLoaded', () => {
    Auth.init();
  });
}
