#!/bin/bash
# Testing script

set -e

echo "Running tests..."

# Lint shell scripts
for script in scripts/*.sh build.sh; do
    if command -v shellcheck >/dev/null; then
        shellcheck "$script"
    fi
done

# Validate config
if ! jq . "$CONFIG_FILE" >/dev/null; then
    echo "Invalid JSON config"
    exit 1
fi

# Check if ISO exists
if [ -f "$OUTPUT_ISO" ]; then
    echo "ISO validation passed"
else
    echo "ISO not found"
    exit 1
fi

echo "Tests passed"