#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR="$1"

# Если CI не передал EXECUTABLE_NAME, используем Ashyk
EXECUTABLE_NAME=${EXECUTABLE_NAME:-Ashyk}

valgrind \
  --leak-check=full \
  --show-leak-kinds=all \
  --error-exitcode=1 \
  "./${BUILD_DIR}/${EXECUTABLE_NAME}"
