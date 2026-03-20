#!/bin/bash
set -e

SERVICE_NAME="pipoc"
BINARY="/home/pi/pipoc/bin/pipoc"
SERVICE_DIR="$HOME/.config/systemd/user"
SERVICE_FILE="$SERVICE_DIR/${SERVICE_NAME}.service"

if [[ "$1" == "--uninstall" ]]; then
    systemctl --user disable --now "$SERVICE_NAME"
    rm -f "$SERVICE_FILE"
    systemctl --user daemon-reload
    echo "Uninstalled."
    exit 0
fi

[[ -f "$BINARY" ]] || { echo "Binary not found. Run 'make' first."; exit 1; }

mkdir -p "$SERVICE_DIR"
cat > "$SERVICE_FILE" <<EOF
[Unit]
Description=PiPOC RX-8 Head Unit
After=graphical-session.target

[Service]
ExecStart=${BINARY}
WorkingDirectory=/home/pi/pipoc
Restart=on-failure
RestartSec=3s
Environment=DISPLAY=:0
Environment=XAUTHORITY=%h/.Xauthority

[Install]
WantedBy=graphical-session.target
EOF

systemctl --user daemon-reload
systemctl --user enable --now "$SERVICE_NAME"
echo "Done. Logs: journalctl --user -u $SERVICE_NAME -f"