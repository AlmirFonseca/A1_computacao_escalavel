import streamlit as st
import pandas as pd
import numpy as np
import os
import time
import re

BASE_FOLDER = "./processed/"
FILE_NAMES = ["CountView", "CountBuy", "BuyRanking", "ProdView", "ViewRanking"]
UPDATE_INTERVAL = 3 # In seconds

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
    st.subheader(f"Número de produtos visualizados por minuto: {count_view_df["Count"].sum()}")

    st.divider()

    # Número de produtos comprados
    count_buy_df = data_dict["CountBuy"]["data"]
    st.subheader(f"Número de produtos comprados por minuto: {count_buy_df["Count"].sum()}")

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