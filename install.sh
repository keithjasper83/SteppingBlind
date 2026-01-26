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

    # Check if it is a git repository
    if [ ! -d "$INSTALL_DIR/.git" ]; then
        echo "Error: $INSTALL_DIR exists but is not a git repository."
        echo "Please rename or delete it and try again."
        exit 1
    fi

    cd "$INSTALL_DIR"

    # Check if it matches our remote
    CURRENT_REMOTE=$(git remote get-url origin 2>/dev/null || echo "")
    if [ "$CURRENT_REMOTE" != "$REPO_URL" ]; then
        echo "Error: $INSTALL_DIR is a git repository but points to $CURRENT_REMOTE"
        echo "Expected: $REPO_URL"
        exit 1
    fi

    echo "Updating existing installation..."
    echo "WARNING: This will discard any local changes in $INSTALL_DIR."
    read -p "Are you sure you want to continue? (y/N) " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        echo "Aborted by user."
        exit 1
    fi

    # We might not be on the right branch if the user messed with it
    git fetch origin
    git checkout "$BRANCH"
    
    # Check if there are local changes that would be discarded
    has_unstaged_changes=false
    has_staged_changes=false
    has_local_commits=false
    
    if ! git diff --quiet; then
        has_unstaged_changes=true
    fi
    
    if ! git diff --cached --quiet; then
        has_staged_changes=true
    fi
    
    if [ -n "$(git log origin/$BRANCH..$BRANCH 2>/dev/null)" ]; then
        has_local_commits=true
    fi
    
    if [ "$has_unstaged_changes" = true ] || [ "$has_staged_changes" = true ] || [ "$has_local_commits" = true ]; then
        echo ""
        echo "=========================================="
        echo "  WARNING: LOCAL CHANGES DETECTED"
        echo "=========================================="
        echo "The following will be permanently lost:"
        echo "  - Any uncommitted changes"
        echo "  - Any local commits not pushed to GitHub"
        echo ""
        echo "This operation will reset to origin/$BRANCH."
        echo ""
        read -p "Continue with reset? This cannot be undone! (yes/no): " -r
        echo ""
        if [[ ! $REPLY =~ ^[Yy][Ee][Ss]$ ]]; then
            echo "Installation cancelled. Your local changes are preserved."
            exit 1
        fi
    fi
    
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
