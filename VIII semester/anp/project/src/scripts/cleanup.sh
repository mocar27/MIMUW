#!/bin/bash

# Store the current directory
current_dir=$(pwd)

# Change to the directory where the script is located
cd "$(dirname "$0")" || exit

cleanup_bin() {
  rm -rf ../bin
}

echo "Cleaning up the project..."
echo "Cleaning up the bin directory..."
cleanup_bin

echo "Cleanup finished"

# Change back to the original directory
cd "${current_dir}" || exit
