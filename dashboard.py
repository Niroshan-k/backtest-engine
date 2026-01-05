import streamlit as st
import yfinance as yf
import trading_engine as cpp
import pandas as pd
import plotly.graph_objects as go

st.set_page_config(page_title="Quant C++ Engine", layout="wide")
st.title("⚡ C++ High-Frequency Trading Engine")

# --- SIDEBAR: CONTROLS ---
st.sidebar.header("1. Select Data")
ticker = st.sidebar.text_input("Stock Ticker", value="AAPL")
period = st.sidebar.selectbox("Time Period", ["1mo", "3mo", "6mo", "1y", "5y"], index=3)

st.sidebar.header("2. Strategy Settings")
# We use simple Moving Average logic for this demo
buy_price = st.sidebar.number_input("Buy if Price Below ($)", value=150.0)
sell_price = st.sidebar.number_input("Sell if Price Above ($)", value=200.0)
start_cash = st.sidebar.number_input("Starting Cash ($)", value=10000.0)

# --- LOAD DATA ---
if st.sidebar.button("Run Simulation"):
    with st.spinner(f"Downloading {ticker} data..."):
        # 1. Get Data from Yahoo
        df = yf.download(ticker, period=period, interval="1d")
        
        if df.empty:
            st.error("No data found! Check ticker symbol.")
            st.stop()

        # 2. Convert to C++ Format
        c_data = []
        # Handle yfinance multi-index columns if present
        try:
            prices = df['Close']
            if isinstance(prices, pd.DataFrame):
                prices = prices.iloc[:, 0] # Fix for some yfinance versions
            
            for date, price in prices.items():
                bar = cpp.MarketBar()
                bar.date = str(date.date())
                bar.price = float(price)
                c_data.append(bar)
        except Exception as e:
            st.error(f"Data Error: {e}")
            st.stop()

    # --- RUN C++ ENGINE ---
    # Define Strategy dynamically based on sidebar inputs
    class UserStrategy(cpp.Strategy):
        def check_signal(self, price):
            if price < buy_price: return "BUY"
            if price > sell_price: return "SELL"
            return "HOLD"

    engine = cpp.BacktestEngine(c_data, start_cash, UserStrategy())
    engine.run()
    
    # --- RESULTS ---
    final_val = engine.get_final_balance()
    profit = final_val - start_cash
    trade_count = engine.get_total_trades()
    
    # Visuals
    col1, col2, col3 = st.columns(3)
    col1.metric("Final Balance", f"${final_val:,.2f}")
    col2.metric("Total Profit", f"${profit:,.2f}", 
                delta_color="normal" if profit > 0 else "inverse")
    col3.metric("Trades Executed", trade_count)
    
    st.subheader(f"Price History: {ticker}")
    
    # --- DISPLAY CANDLESTICK CHART ---
    st.subheader(f"Market Analysis: {ticker}")

    # Create the fancy chart
    fig = go.Figure(data=[go.Candlestick(
        x=df.index,
        open=df['Open'],
        high=df['High'],
        low=df['Low'],
        close=df['Close'],
        name=ticker
    )])

    # Make it look nice
    fig.update_layout(
        title=f"{ticker} Price History",
        yaxis_title="Price (USD)",
        xaxis_rangeslider_visible=False, # Hide the bottom slider to save space
        height=500
    )

    # Add key="market_chart" to fix the crash
    st.plotly_chart(fig, use_container_width=True, key="market_chart")