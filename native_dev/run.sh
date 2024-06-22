#!/bin/sh

# reset build dir
if [ -d "build/" ]; then
  rm -r build/
fi
mkdir build/

# make
echo "Building..."
make


# copy resources
echo "Copying resources..."
for file in resources/textures/*; do
    cp $file build/
done

while read line; do
    for file in resources/$line/*; do
        cp $file build/
    done
done < required_shaders_list.txt

echo "Done"

# run executable
cd build/
./T4X
