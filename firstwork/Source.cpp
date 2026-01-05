#include <iostream>
#include <fstream>
#include <sstream>
#include "EngineBackend.h" // <--- We include your new file here!

// helper function to load csv
std::vector<MarketBar> load_csv(std::string filname) {
	std::vector<MarketBar> data;
	std::ifstream file(filname);

	if (!file.is_open()) return data;

	std::string line;
	std::getline(file, line); // skip header

	while (std::getline(file, line)) {
		std::stringstream ss(line);
		std::string segment;
		MarketBar bar;
		std::getline(ss, bar.date, ',');
		std::getline(ss, segment, ',');
		bar.price = std::stod(segment);
		data.push_back(bar);
	}
	return data;
}

int main() {
	// load data
	std::vector<MarketBar> my_data = load_csv("market_data.csv");
	if (my_data.empty()) {
		std::cout << "Error: No data loaded." << std::endl;
		return 1;
	}

	// pick a strategy
	SimpleDipStrategy my_strategy;

	// setup the engine - pass the data, 1000$ cash, address & of our strategy
	BacktestEngine engine(my_data, 1000.0, &my_strategy);

	//run
	engine.run();
	engine.print_report();

	return 0;
}