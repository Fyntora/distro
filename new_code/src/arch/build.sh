#!/bin/bash

# 1. Safety & Error Handling
set -euo pipefail

# 2. Color Definitions
BOLD="\033[1m"
RED="\033[1;31m"
GREEN="\033[1;32m"
YELLOW="\033[1;33m"
BLUE="\033[1;34m"
RESET="\033[0m"

# 3. Configuration Variables
WORK_DIR="work"
OUT_DIR="out"
PROFILE_DIR="."

# 4. User Check
# We ensure you are NOT root. If you are root, run_archiso will fail or warn.
if [[ $EUID -eq 0 ]]; then
   echo -e "${RED}[ERROR] Please do NOT run this script as root/sudo.${RESET}"
   echo -e "Run it as your normal user. The script will ask for sudo when needed."
   exit 1
fi

# 5. Cleanup
echo -e "${YELLOW}[-] Cleaning up previous build artifacts...${RESET}"
# We use sudo here because previous build artifacts might be owned by root
if [ -d "$WORK_DIR" ]; then sudo rm -rf "$WORK_DIR"; fi
if [ -d "$OUT_DIR" ]; then sudo rm -rf "$OUT_DIR"; fi

# 6. Setup
echo -e "${BLUE}[+] Creating build directories...${RESET}"
# We create these as the current user, but mkarchiso will write into them as root
mkdir -p "$WORK_DIR" "$OUT_DIR"

# 7. Build (Requires Root)
echo -e "${BLUE}[+] Starting Arch ISO build (sudo required)...${RESET}"
echo -e "${BOLD}----------------------------------------${RESET}"

# This is the only part that runs as root
sudo mkarchiso -v -w "$WORK_DIR" -o "$OUT_DIR" "$PROFILE_DIR"

echo -e "${BOLD}----------------------------------------${RESET}"
echo -e "${GREEN}[SUCCESS] Build complete!${RESET}"

# 8. Fix Permissions
# The build output is currently owned by root. We must give it back to you.
echo -e "${BLUE}[+] Fixing file ownership for user: ${BOLD}$USER${RESET}..."
sudo chown -R "$USER:$(id -g)" "$OUT_DIR" "$WORK_DIR"

# 9. Interactive Prompt
echo -e "${BLUE}Would you like to run the ISO now using run_archiso? [y/N]${RESET} "
read -n 1 -r REPLY
echo 

if [[ $REPLY =~ ^[Yy]$ ]]; then
    
    if ! command -v run_archiso &> /dev/null; then
        echo -e "${RED}[ERROR] 'run_archiso' command not found.${RESET}"
        exit 1
    fi

    # Find the ISO file
    ISO_FILE=$(find "$OUT_DIR" -name "*.iso" | head -n 1)

    if [ -n "$ISO_FILE" ]; then
        echo -e "${BLUE}[+] Launching VM as user ${BOLD}$USER${RESET}..."
        
        # Runs as YOUR normal user, not root
        run_archiso -i "$ISO_FILE"
    else
        echo -e "${RED}[ERROR] No ISO file found in ${OUT_DIR}.${RESET}"
    fi

else
    echo -e "Exiting. ISO is located in: ${BOLD}${OUT_DIR}${RESET}"
fi