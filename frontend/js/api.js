/**
 * ManiMart Unified API and Toast Notification Client
 */

const API = {
  baseUrl: '/api/v1',

  async request(endpoint, options = {}) {
    const token = localStorage.getItem('manimart_token');
    const defaultHeaders = {
      'Content-Type': 'application/json',
      'X-Requested-With': 'XMLHttpRequest'
    };

    if (token) {
      defaultHeaders['Authorization'] = `Bearer ${token}`;
      defaultHeaders['X-Session-ID'] = token;
    }

    const config = {
      credentials: 'include',
      ...options,
      headers: {
        ...defaultHeaders,
        ...options.headers
      }
    };

    if (options.body && typeof options.body === 'object') {
      config.body = JSON.stringify(options.body);
    }

    try {
      const response = await fetch(`${this.baseUrl}${endpoint}`, config);
      const data = await response.json().catch(() => null);

      if (!response.ok) {
        const errorMsg = data?.error?.message || `Request failed with status ${response.status}`;
        const errorField = data?.error?.field;
        throw { status: response.status, message: errorMsg, field: errorField, data };
      }

      const result = data?.data !== undefined ? data.data : data;

      // Automatically store session token if returned
      if (result && (result.token || result.session_id)) {
        const sessId = result.token || result.session_id;
        localStorage.setItem('manimart_token', sessId);
      }

      return result;
    } catch (err) {
      console.error(`API Error on ${endpoint}:`, err);
      throw err;
    }
  },

  get(endpoint, params = {}) {
    const query = new URLSearchParams(params).toString();
    const url = query ? `${endpoint}?${query}` : endpoint;
    return this.request(url, { method: 'GET' });
  },

  post(endpoint, body = {}) {
    return this.request(endpoint, { method: 'POST', body });
  },

  put(endpoint, body = {}) {
    return this.request(endpoint, { method: 'PUT', body });
  },

  delete(endpoint) {
    return this.request(endpoint, { method: 'DELETE' });
  }
};

// Toast Notifications
const Toast = {
  show(message, type = 'info', duration = 3500) {
    let container = document.getElementById('toast-container');
    if (!container) {
      container = document.createElement('div');
      container.id = 'toast-container';
      document.body.appendChild(container);
    }

    const toast = document.createElement('div');
    toast.className = `toast toast-${type}`;
    toast.textContent = message;

    container.appendChild(toast);

    setTimeout(() => {
      toast.style.opacity = '0';
      toast.style.transform = 'translateX(100%)';
      toast.style.transition = 'all 0.3s ease';
      setTimeout(() => toast.remove(), 300);
    }, duration);
  },

  success(msg) { this.show(msg, 'success'); },
  error(msg) { this.show(msg, 'error'); },
  info(msg) { this.show(msg, 'info'); }
};
