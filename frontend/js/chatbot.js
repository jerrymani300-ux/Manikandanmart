/**
 * ManiMart AI Chatbot Floating Widget
 */

const Chatbot = {
  isOpen: false,

  init() {
    this.createWidgetDOM();
    this.bindEvents();
  },

  createWidgetDOM() {
    const launcher = document.createElement('button');
    launcher.id = 'chatbot-launcher';
    launcher.className = 'chatbot-launcher';
    launcher.innerHTML = '💬';
    launcher.title = 'Ask ManiMart AI Assistant';

    const panel = document.createElement('div');
    panel.id = 'chat-panel';
    panel.className = 'chat-panel hidden';
    panel.innerHTML = `
      <div class="chat-header">
        <div style="display: flex; align-items: center; gap: 0.5rem;">
          <span style="font-size: 1.25rem;">🤖</span>
          <div>
            <div class="chat-title">ManiMart Assistant</div>
            <div style="font-size: 0.75rem; opacity: 0.85;">Online • AI Support</div>
          </div>
        </div>
        <button id="chat-close-btn" class="chat-close-btn">&times;</button>
      </div>

      <div id="chat-messages" class="chat-messages">
        <div class="chat-msg chat-msg-bot">
          Hello! I'm your ManiMart AI assistant. How can I help you with our products, checkout, orders, or selling today?
        </div>
      </div>

      <div class="chat-suggestions">
        <span class="chat-chip" onclick="Chatbot.sendPrompt('What products do you sell?')">Products catalog?</span>
        <span class="chat-chip" onclick="Chatbot.sendPrompt('How does checkout work?')">Checkout process?</span>
        <span class="chat-chip" onclick="Chatbot.sendPrompt('How can I track my order?')">Order tracking?</span>
        <span class="chat-chip" onclick="Chatbot.sendPrompt('How can a seller add products?')">Selling info?</span>
      </div>

      <form id="chat-form" class="chat-input-bar">
        <input type="text" id="chat-input" placeholder="Type a question..." autocomplete="off">
        <button type="submit" class="nav-btn btn-primary" style="padding: 0.4rem 0.8rem; border-radius: 9999px;">
          ➤
        </button>
      </form>
    `;

    document.body.appendChild(launcher);
    document.body.appendChild(panel);
  },

  bindEvents() {
    const launcher = document.getElementById('chatbot-launcher');
    const closeBtn = document.getElementById('chat-close-btn');
    const form = document.getElementById('chat-form');

    launcher.addEventListener('click', () => this.toggle());
    closeBtn.addEventListener('click', () => this.close());

    form.addEventListener('submit', (e) => {
      e.preventDefault();
      const input = document.getElementById('chat-input');
      const text = input.value.trim();
      if (!text) return;
      input.value = '';
      this.sendMessage(text);
    });
  },

  toggle() {
    this.isOpen = !this.isOpen;
    const panel = document.getElementById('chat-panel');
    if (this.isOpen) {
      panel.classList.remove('hidden');
      document.getElementById('chat-input').focus();
    } else {
      panel.classList.add('hidden');
    }
  },

  close() {
    this.isOpen = false;
    document.getElementById('chat-panel').classList.add('hidden');
  },

  sendPrompt(prompt) {
    this.sendMessage(prompt);
  },

  appendMessage(text, sender = 'bot', isPending = false) {
    const container = document.getElementById('chat-messages');
    const msg = document.createElement('div');
    msg.className = `chat-msg chat-msg-${sender}`;
    if (isPending) msg.id = 'chat-pending-msg';
    msg.textContent = text;
    container.appendChild(msg);
    container.scrollTop = container.scrollHeight;
    return msg;
  },

  async sendMessage(message) {
    this.appendMessage(message, 'user');
    const pendingMsg = this.appendMessage('Thinking...', 'bot', true);

    try {
      const res = await API.post('/chat', { message });
      pendingMsg.remove();
      this.appendMessage(res.reply, 'bot');
    } catch (err) {
      pendingMsg.remove();
      this.appendMessage(err.message || 'I encountered an issue processing your request. Please try again.', 'bot');
    }
  }
};

document.addEventListener('DOMContentLoaded', () => {
  Chatbot.init();
});
