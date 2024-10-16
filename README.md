# Raspberry Pico Starter Code

## Environment setup
1. cd /opt
2. git pull https://github.com/raspberrypi/pico-sdk.git --branch master
3. cd pico-sdk/
4. git submodule update --init
5. export PICO_SDK_PATH=/opt/pico-sdk

## Update SDK
1. cd /opt/pico-sdk
2. git pull
3. git submodule update

## To build
    bash scripts/build.sh

## To load on Raspberry PICO
1. Hold BOOTSEL button and plug PICO into USB
2. Copy build/network_scanner.uf2 to RPI-RP2 Volume
