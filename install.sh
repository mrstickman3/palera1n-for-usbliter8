#!/bin/bash

set -e

echo "=== palelitera1n installer ==="

# Check macOS
if [[ "$(uname)" != "Darwin" ]]; then
    echo "Error: This installer only supports macOS."
    exit 1
fi

# Check Homebrew
if ! command -v brew >/dev/null 2>&1; then
    echo "[+] Homebrew not found. Installing..."

    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

    if [ -x "/opt/homebrew/bin/brew" ]; then
        eval "$(/opt/homebrew/bin/brew shellenv)"
    elif [ -x "/usr/local/bin/brew" ]; then
        eval "$(/usr/local/bin/brew shellenv)"
    fi
else
    echo "[+] Homebrew found."
fi

echo "[+] Installing dependencies..."

brew update
brew install \
  libirecovery \
  libusbmuxd \
  libimobiledevice \
  pkg-config

echo "[+] Building palelitera1n..."

make clean || true
make

if [ ! -f "./palera1n" ]; then
    echo "Error: Build completed but palera1n binary was not found."
    exit 1
fi

echo "[+] Installing palera1n command..."

sudo cp ./palera1n /usr/local/bin/palera1n
sudo chmod +x /usr/local/bin/palera1n

echo ""
echo "================================="
echo " Done!"
echo " Run: palera1n --help"
echo "================================="
