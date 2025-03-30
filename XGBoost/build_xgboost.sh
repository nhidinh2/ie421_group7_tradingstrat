#!/bin/bash

# Clone the repository with submodules
git clone --recursive https://github.com/dmlc/xgboost.git

# Navigate to the xgboost directory
cd xgboost

# Create a build directory
mkdir build
cd build

# Run CMake to configure the project
cmake ..

# Compile the project using all available processors
make -j$(nproc)

# Navigate to the target directory
cd /home/vagrant/ss/sdk/RCM/StrategyStudio/includes/
