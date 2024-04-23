#!/bin/bash

# Run MOCK script
echo "Running MOCK..."
cd mock || { echo "Error: Couldn't navigate to mock directory"; exit 1; }
python main.py & disown  # Run in background and detach

cd -  # Navigate back to root directory

# Run Streamlit dashboard
echo "Running Streamlit dashboard..."
cd  dashboard || { echo "Error: Couldn't navigate to dashboard directory"; exit 1; }
python -m streamlit run main.py & disown  # Run in background and detach

cd -  # Navigate back to root directory

# Run ETL script
echo "Running ETL..."
cd src || { echo "Error: Couldn't navigate to src directory"; exit 1; }
g++ main.cpp -o main || { echo "Error: Compilation failed"; exit 1; }
./main.exe
cd -  # Navigate back to root directory

echo "Done!"
