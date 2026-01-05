import yfinance as yf
import trading_engine as cpp
import pandas as pd

# 1. Download Real Data from Yahoo Finance
print("Downloading Apple (AAPL) data...")
# Download 1 year of data, daily interval
df = yf.download("AAPL", period="1y", interval="1d")

# 2. Feed it to C++
print("Converting data to C++ format...")
c_data = []

# yfinance returns a DataFrame with a DateTime index
for date, row in df.iterrows():
    bar = cpp.MarketBar()
    bar.date = str(date.date()) # Convert timestamp to string
    # We use the 'Close' price for the simulation
    bar.price = float(row['Close']) 
    c_data.append(bar)

print(f"Loaded {len(c_data)} days of market data.")

# 3. Define Strategy
class RealMarketStrategy(cpp.Strategy):
    def check_signal(self, price):
        # Simple logic: Buy dips below $200, Sell rips above $220
        # (You will need to adjust these numbers based on current prices!)
        if price < 200.0: return "BUY"
        if price > 230.0: return "SELL"
        return "HOLD"

# 4. Run Engine
engine = cpp.BacktestEngine(c_data, 10000.0, RealMarketStrategy()) # $10k start
engine.run()

# 5. Result
final_val = engine.get_final_balance()
print(f"Final Value: ${final_val:.2f}")
print(f"Profit:      ${final_val - 10000.0:.2f}")