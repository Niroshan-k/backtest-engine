import sys
import pandas as pd
import trading_engine as cpp

# 1. Load Real Data using Pandas
print("Loading data from CSV...")
df = pd.read_csv("market_data.csv")

# Convert the Pandas DataFrame into a C++ Vector
# (This acts like a data pipeline)
c_data = []
for index, row in df.iterrows():
    bar = cpp.MarketBar()
    bar.date = str(row['Date'])
    bar.price = float(row['Price'])
    c_data.append(bar)

print(f"Successfully loaded {len(c_data)} bars into C++ memory.")

# 2. Define Strategy (The Python Logic)
class DipStrategy(cpp.Strategy):
    def check_signal(self, price):
        # Buy if price drops below $100
        if price < 100.0:
            return "BUY"
        # Sell if price goes above $101
        elif price > 101.0:
            return "SELL"
        return "HOLD"

# 3. Run the Engine
print("Running Backtest...")
strategy = DipStrategy()
engine = cpp.BacktestEngine(c_data, 1000.0, strategy)
engine.run()

# 4. Show Results
final_val = engine.get_final_balance()
profit = final_val - 1000.0

print("-" * 30)
print(f"Final Balance: ${final_val:.2f}")
print(f"Total Profit:  ${profit:.2f}")
print("-" * 30)