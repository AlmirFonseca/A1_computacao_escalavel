import streamlit as st
import pandas as pd
import numpy as np
import os
import time
import re

BASE_FOLDER = "../processed/"
FILE_NAMES = ["CountView", "CountBuy", "BuyRanking", "ProdView", "ViewRanking",
              "times_CountView", "times_CountBuy", "times_BuyRanking", "times_ProdView", "times_ViewRanking"]
UPDATE_INTERVAL = 3 # In seconds

line_CountView = list()
line_CountBuy = list()
line_BuyRanking = list()
line_ProdView = list()
line_ViewRanking = list()
line_timestamps = list()

def load_data(file_name):
    """Load data from a CSV file."""
    return pd.read_csv(BASE_FOLDER + file_name + ".csv")

def load_all_data():
    """Load all data from defined files and return a dictionary with their content and metadata."""
    data_dict = {}
    for file_name in FILE_NAMES:
        file_path = BASE_FOLDER + file_name + ".csv"
        data = {
            "path": file_path,
            "data": load_data(file_name),
            "mod_time": os.path.getmtime(file_path)
        }
        data_dict[file_name] = data
    return data_dict

def check_for_updates(data_dict):
    """Check for updates in the file modification times and reload data if changed."""
    changes = False
    for filename, file_data in data_dict.items():
        mod_time = os.path.getmtime(file_data["path"])
        if mod_time != file_data["mod_time"]:
            file_data["data"] = load_data(filename)
            file_data["mod_time"] = mod_time
            changes = True
    return changes

def display_data(data_dict):
    """Display data using Streamlit."""
    st.divider()

    # Número de produtos visualizados
    count_view_df = data_dict["CountView"]["data"]
    num = count_view_df["Count"].sum()
    st.subheader(f"Número de produtos visualizados por minuto: {num}")

    st.divider()

    # Número de produtos comprados
    count_buy_df = data_dict["CountBuy"]["data"]
    num2 = count_buy_df["Count"].sum()
    st.subheader(f"Número de produtos comprados por minuto: {num2}")

    st.divider()

    # Número de usuários únicos visualizando cada produto por minuto
    prod_view_df = data_dict["ProdView"]["data"]
    # Apply regex to extract product ID from product name
    prod_view_df["Value"] = prod_view_df["Value"].apply(lambda x: re.findall(r"\d+", x)[0])
    # Group by product ID and sum the counts
    prod_view_df = prod_view_df.groupby("Value").sum().reset_index()
    # Sort by count in descending order
    prod_view_df = prod_view_df.sort_values(by="Count", ascending=False, ignore_index=True)

    st.subheader(f"Número de usuários únicos visualizando cada produto por minuto:")
    st.write("Top 10 produtos visualizados")
    filtered_prod_view_df = prod_view_df.head(10)
    filtered_prod_view_df.reset_index(drop=True, inplace=True)
    # Rename columns for better display
    filtered_prod_view_df.columns = ["Product ID", "Unique Viewers"]
    st.bar_chart(filtered_prod_view_df, x="Product ID", y="Unique Viewers")

    st.divider()

    # Ranking de produtos mais comprados na última hora
    buy_ranking_df = data_dict["BuyRanking"]["data"]
    # Sort by count in descending order
    buy_ranking_df = buy_ranking_df.sort_values(by="Count", ascending=False, ignore_index=True)

    st.subheader(f"Ranking de produtos mais comprados na última hora:")
    st.write("Top 10 produtos comprados")
    filtered_buy_ranking_df = buy_ranking_df.head(10)
    filtered_buy_ranking_df.reset_index(drop=True, inplace=True)
    # Rename columns for better display
    filtered_buy_ranking_df.columns = ["Product ID", "Sales Count"]
    st.bar_chart(filtered_buy_ranking_df, x="Product ID", y="Sales Count")

    st.divider()

    # Ranking de produtos mais visualizados na última hora
    view_ranking_df = data_dict["ViewRanking"]["data"]
    # Apply regex to extract product ID from product name
    view_ranking_df["Value"] = view_ranking_df["Value"].apply(lambda x: re.findall(r"\d+", x)[0])
    # Sort by count in descending order
    view_ranking_df = view_ranking_df.sort_values(by="Count", ascending=False, ignore_index=True)

    st.subheader(f"Ranking de produtos mais visualizados na última hora:")
    st.write("Top 10 produtos visualizados")
    filtered_view_ranking_df = view_ranking_df.head(10)
    filtered_view_ranking_df.reset_index(drop=True, inplace=True)
    # Rename columns for better display
    filtered_view_ranking_df.columns = ["Product ID", "View Count"]
    st.bar_chart(filtered_view_ranking_df, x="Product ID", y="View Count")

    st.divider()

    # Load test output
    time_CountView_df = data_dict["times_CountView"]["data"]
    time_CountBuy_df = data_dict["times_CountBuy"]["data"]
    time_BuyRanking_df = data_dict["times_BuyRanking"]["data"]
    time_ProdView_df = data_dict["times_ProdView"]["data"]
    time_ViewRanking_df = data_dict["times_ViewRanking"]["data"]

    # Get the mean of each time column
    mean_time_CountView = time_CountView_df["time"].mean()
    mean_time_CountBuy = time_CountBuy_df["time"].mean()
    mean_time_BuyRanking = time_BuyRanking_df["time"].mean()
    mean_time_ProdView = time_ProdView_df["time"].mean()
    mean_time_ViewRanking = time_ViewRanking_df["time"].mean()

    # Plot a line chart with one line for each time column
    line_CountView.append(mean_time_CountView)
    line_CountBuy.append(mean_time_CountBuy)
    line_BuyRanking.append(mean_time_BuyRanking)
    line_ProdView.append(mean_time_ProdView)
    line_ViewRanking.append(mean_time_ViewRanking)
    line_timestamps.append(time.strftime("%H:%M:%S", time.localtime(time.time())))

    # Convert into a dataframe
    df = pd.DataFrame({
        "CountView": line_CountView,
        "CountBuy": line_CountBuy,
        "BuyRanking": line_BuyRanking,
        "ProdView": line_ProdView,
        "ViewRanking": line_ViewRanking,
        "Time": line_timestamps
    })

    # Plot a line chart
    st.subheader("Tempo médio de execução de cada query em milisegundos:")
    st.line_chart(df, x="Time")


st.title("E-commerce Dashboard")
st.write("This dashboard displays e-commerce data, updated in real-time.")
st.write("Last update: ", time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(time.time())))

# Refresh button to manually refresh data
if st.button("Manually Refresh Data", type="primary", use_container_width=True):
    st.session_state.data_dict = load_all_data()
    st.rerun()

# Load data if not already loaded
if 'data_dict' not in st.session_state:
    st.session_state.data_dict = load_all_data()

# Display data from loaded files
display_data(st.session_state.data_dict)

# Automatically refresh data every 3 seconds
if check_for_updates(st.session_state.data_dict):
    st.rerun()

# Schedule next rerun after some seconds
st.session_state.timer = st.empty()
time.sleep(UPDATE_INTERVAL)
st.rerun()

if __name__ == "__main__":
    # Call the command that runs the Streamlit app
    os.system("python -m streamlit run main.py")