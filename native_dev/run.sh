#!/bin/sh

# make
echo "Building..."
make
echo "Done"

# run executable
cd build/
./T4X
