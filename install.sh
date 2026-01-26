#!/bin/bash
set -e

REPO_URL="https://github.com/keithjasper83/SteppingBlind.git"
INSTALL_DIR="$HOME/blind-controller"
BRANCH="main"

echo "=========================================="
echo "  Smart Blind Controller Installer"
echo "=========================================="

# Ensure we are not root
if [ "$EUID" -eq 0 ]; then
  echo "Please run as a non-root user (e.g., pi)."
  exit 1
fi

# 1. Install Git if missing
if ! command -v git &> /dev/null; then
    echo "Git not found. Installing..."
    sudo apt-get update
    sudo apt-get install -y git
fi

# 2. Clone or Update Repository
if [ -d "$INSTALL_DIR" ]; then
    echo "Directory $INSTALL_DIR already exists."
    echo "Updating existing installation..."
    cd "$INSTALL_DIR"
    # We might not be on the right branch if the user messed with it
    git fetch origin
    git checkout "$BRANCH"
    git reset --hard "origin/$BRANCH"
else
    echo "Cloning repository into $INSTALL_DIR..."
    git clone -b "$BRANCH" "$REPO_URL" "$INSTALL_DIR"
    cd "$INSTALL_DIR"
fi

# 3. Handover to internal setup script
echo "Running setup script..."
chmod +x scripts/setup.sh
./scripts/setup.sh
