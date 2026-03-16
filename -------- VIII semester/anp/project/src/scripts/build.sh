#!/bin/bash

# Store the current directory
current_dir=$(pwd)

# Change to the directory where the script is located
cd "$(dirname "$0")" || exit

build_main() {
  mkdir -p ../bin
  go build -o ../bin/peer ../main/peer.go
}

echo "Building the project..."

# Install dependencies
echo "Installing dependencies..."
go mod tidy

# Build main
echo "Building main..."
build_main

echo "Finished"

# Change back to the original directory
cd "${current_dir}" || exit
