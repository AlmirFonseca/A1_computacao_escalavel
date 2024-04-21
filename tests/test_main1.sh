#!/bin/bash

# Define arguments to pass to your program (modify if needed)
arg1=1
arg2=10
arg3=100

# Compile your C++ code (replace with your compiler and flags)
g++ -o main ../src/main.cpp

# Check if compilation was successful
if [ $? -ne 0 ]; then
  echo "Error: Compilation failed."
  exit 1
fi

# Execute the compiled program with arguments
./main $arg1 $arg2 $arg3

# Optional: Capture or display the program's output
output=$(./main $arg1 $arg2 $arg3)
echo "Program output: $output"

# Clean up (optional)
rm main  # Remove the compiled executable (if not needed)
