#pragma once
#include <vector>
#include <string>
#include <iostream>

// 1. DATA STRUCTURE
struct MarketBar {
    std::string date;
    double price;
    // We can add volume later
};

// 2. PORTFOLIO MANAGER (Tracks Money)
class Portfolio {
public:
    double cash;
    int shares;

    // NEW: Risk Metrics
    double initial_cash;
    double highest_cash_seen;
    double max_drawdown; // Worst drop from the peak (percentage)

    Portfolio(double start_cash) {
        cash = start_cash;
        shares = 0;

        // Initialize metrics
        initial_cash = start_cash;
        highest_cash_seen = start_cash;
        max_drawdown = 0.0;
    }

    void buy(double price) {
        if (cash >= price) {
            cash -= price;
            shares++;
        }
        update_metrics(price); // Check risk after every trade
    }

    void sell(double price) {
        if (shares > 0) {
            cash += price;
            shares--;
        }
        update_metrics(price);
    }

    // Helper to calculate total value
    double get_total_value(double current_price) {
        return cash + (shares * current_price);
    }

private:
    // NEW: The Risk Calculator
    void update_metrics(double current_price) {
        double total_value = get_total_value(current_price);

        // 1. Is this a new record high?
        if (total_value > highest_cash_seen) {
            highest_cash_seen = total_value;
        }

        // 2. How far have we fallen from the peak?
        double current_drawdown = (highest_cash_seen - total_value) / highest_cash_seen;

        // 3. Is this the worst fall we've seen?
        if (current_drawdown > max_drawdown) {
            max_drawdown = current_drawdown;
        }
    }
};

// 3. STRATEGY INTERFACE (The Brain)
// This is a "Base Class". Later, Python or AI will override this.
class Strategy {
public:
    // Virtual function = "Logic to be defined later"
    virtual std::string check_signal(double price) {
        return "HOLD"; // Default behavior
    }
};

// Example Strategy: The one we used before
class SimpleDipStrategy : public Strategy {
public:
    std::string check_signal(double price) override {
        if (price < 100.0) return "BUY";
        if (price > 101.0) return "SELL";
        return "HOLD";
    }
};