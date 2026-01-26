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
    
    # Check if it's a git repository
    if [ ! -d "$INSTALL_DIR/.git" ]; then
        echo "ERROR: $INSTALL_DIR exists but is not a git repository."
        echo "Please remove it or choose a different installation directory."
        exit 1
    fi
    
    # Check if the remote origin URL matches
    cd "$INSTALL_DIR"
    CURRENT_ORIGIN=$(git remote get-url origin 2>/dev/null || echo "")
    if [ "$CURRENT_ORIGIN" != "$REPO_URL" ]; then
        echo "ERROR: $INSTALL_DIR is a git repository with a different origin."
        echo "Expected: $REPO_URL"
        echo "Found:    $CURRENT_ORIGIN"
        echo "Please remove it or choose a different installation directory."
        exit 1
    fi
    
    echo "Updating existing installation..."
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
