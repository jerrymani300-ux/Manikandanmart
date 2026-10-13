#include "provider/MockChatProvider.hpp"
#include <algorithm>
#include <cctype>

namespace manimart::provider {

static std::string ToLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

std::string MockChatProvider::GetReply(const std::string& user_message, const std::string& /*context*/) {
    std::string msg = ToLower(user_message);

    if (msg.find("product") != std::string::npos && (msg.find("sell") != std::string::npos || msg.find("what") != std::string::npos || msg.find("category") != std::string::npos)) {
        return "ManiMart features a wide range of verified marketplace products across Electronics, Fashion, Home, Books, Fitness, and Accessories! You can browse the complete catalog or filter by category on our Products page.";
    }

    if (msg.find("cart") != std::string::npos || msg.find("add") != std::string::npos) {
        return "To add an item to your cart, click on any product card or detail page and press 'Add to Cart'. You can adjust quantities and review your subtotal anytime from the Cart page.";
    }

    if (msg.find("checkout") != std::string::npos || msg.find("pay") != std::string::npos || msg.find("buy") != std::string::npos) {
        return "Checkout on ManiMart is secure and instant! Open your Cart, click 'Proceed to Checkout', select your mock payment method (Card, UPI, or COD), and confirm your order.";
    }

    if (msg.find("track") != std::string::npos || msg.find("status") != std::string::npos || msg.find("order") != std::string::npos) {
        return "You can track your orders by navigating to the 'My Orders' section in the top menu. Orders progress through PENDING, CONFIRMED, SHIPPED, and DELIVERED states.";
    }

    if (msg.find("seller") != std::string::npos || msg.find("list") != std::string::npos) {
        return "Sellers can manage listings, update inventory, and track incoming orders via the Seller Dashboard. Register as a Seller or log in to access product creation tools.";
    }

    if (msg.find("review") != std::string::npos || msg.find("rating") != std::string::npos || msg.find("star") != std::string::npos) {
        return "Buyers who have purchased and received a delivered product can submit a 1 to 5-star rating with detailed comments on the product details page.";
    }

    if (msg.find("return") != std::string::npos || msg.find("refund") != std::string::npos || msg.find("cancel") != std::string::npos) {
        return "Orders in PENDING or CONFIRMED status can be cancelled directly from your Orders page. For delivered items, ManiMart offers standard buyer protection.";
    }

    if (msg.find("hello") != std::string::npos || msg.find("hi") != std::string::npos || msg.find("hey") != std::string::npos) {
        return "Hello! I am ManiMart Assistant. I can help you with product discovery, cart & checkout guidance, order tracking, seller inquiries, and platform policies. How may I assist you today?";
    }

    return "Welcome to ManiMart Support! I can answer questions regarding products, adding items to cart, order status, seller listings, and checkout. What would you like assistance with?";
}

} // namespace manimart::provider
