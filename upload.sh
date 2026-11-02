#!/bin/bash
set -e

TARGET="rad-macro-pad"

export PICO_PLATFORM=rp2040
export PICO_BOARD=pico

mkdir -p build
cd build

cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --target "${TARGET}"

if [[ "$OSTYPE" == "darwin"* ]]; then
    MOUNT_DIR="/Volumes/RPI-RP2"
else
    MOUNT_DIR="/media/$USER/RPI-RP2"
fi

echo "Copying ${TARGET}.uf2 to ${MOUNT_DIR}..."
cp "${TARGET}.uf2" "${MOUNT_DIR}/"
echo "Uploaded successfully!"
