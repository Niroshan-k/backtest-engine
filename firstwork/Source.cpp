#include <iostream>
#include <fstream>
#include <sstream>
#include "EngineBackend.h" // <--- We include your new file here!

int main() {
    // --- STEP 1: LOAD DATA ---
    std::vector<MarketBar> market_data;
    std::ifstream file("market_data.csv");

    if (!file.is_open()) {
        std::cout << "Error: CSV not found." << std::endl;
        return 1;
    }

    std::string line;
    std::getline(file, line); // Skip header

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string segment;
        MarketBar bar;

        std::getline(ss, bar.date, ',');
        std::getline(ss, segment, ',');
        bar.price = std::stod(segment);

        market_data.push_back(bar);
    }

    // --- STEP 2: INITIALIZE ENGINE ---
    Portfolio my_portfolio(1000.0);      // Start with $1000
    SimpleDipStrategy my_strategy;       // Use our simple rules

    std::cout << "--- STARTING ENGINE ---" << std::endl;

    // --- STEP 3: RUN SIMULATION ---
    for (const auto& bar : market_data) {

        // 1. Ask Strategy what to do
        std::string signal = my_strategy.check_signal(bar.price);

        // 2. Execute decision
        if (signal == "BUY") {
            my_portfolio.buy(bar.price);
            std::cout << "[BUY]  " << bar.date << " @ " << bar.price << std::endl;
        }
        else if (signal == "SELL") {
            my_portfolio.sell(bar.price);
            std::cout << "[SELL] " << bar.date << " @ " << bar.price << std::endl;
        }
    }

    // --- STEP 4: REPORT ---
    double final_val = my_portfolio.get_total_value(market_data.back().price);
    std::cout << "\n--- RESULTS ---" << std::endl;
    std::cout << "Final Portfolio Value: $" << final_val << std::endl;
    std::cout << "Max Drawdown: " << (my_portfolio.max_drawdown * 100) << "%" << std::endl;
    return 0;
}