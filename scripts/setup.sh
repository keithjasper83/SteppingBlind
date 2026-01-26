#!/bin/bash
set -e

# Detect user
USER=$(whoami)
REPO_DIR=$(pwd)
VENV_DIR="$REPO_DIR/venv"

if [ "$EUID" -eq 0 ]; then
  echo "Please run as non-root user (e.g., pi). Sudo will be requested when needed."
  exit 1
fi

echo "Installing system dependencies..."
# Update apt only if we can (might fail if no network or lock)
sudo apt-get update || echo "Apt update failed, continuing..."
sudo apt-get install -y git python3-pip python3-venv qrencode

echo "Setting up Python virtual environment..."
if [ ! -d "$VENV_DIR" ]; then
    python3 -m venv "$VENV_DIR"
fi

source "$VENV_DIR/bin/activate"
pip install --upgrade pip
pip install -r requirements.txt

echo "Configuring Systemd Service..."
# Generate service file with correct paths
cat > blind-controller.service <<EOF
[Unit]
Description=Smart Blind Controller Service
After=network.target

[Service]
Type=simple
User=$USER
WorkingDirectory=$REPO_DIR
ExecStart=$VENV_DIR/bin/python main.py
Restart=always
# Environment=MQTT_HOST=localhost

[Install]
WantedBy=multi-user.target
EOF

sudo cp blind-controller.service /etc/systemd/system/blind-controller.service
sudo systemctl daemon-reload
sudo systemctl enable blind-controller.service
sudo systemctl restart blind-controller.service

echo "Setting up Nightly Update Cron Job..."
# 9 PM every night
CRON_CMD="cd $REPO_DIR && $VENV_DIR/bin/python scripts/update.py >> $REPO_DIR/update.log 2>&1"
# Check if job already exists to avoid duplicates
(crontab -l 2>/dev/null | grep -v "scripts/update.py"; echo "0 21 * * * $CRON_CMD") | crontab -

echo "Setup Complete!"
echo "Service is running."

# Get IP address
IP_ADDR=$(hostname -I | awk '{print $1}')
URL="http://$IP_ADDR:8080"

echo "Access the controller at: $URL"
echo "Scan the QR code below for instant access:"

if command -v qrencode &> /dev/null; then
    qrencode -t ANSI "$URL"
else
    echo "qrencode not found. Install it to see QR code."
fi
