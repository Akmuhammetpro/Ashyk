#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR="$1"

# Если CI не передал EXECUTABLE_NAME, используем Ashyk
EXECUTABLE_NAME=${EXECUTABLE_NAME:-Ashyk}
INPUT_FILE=${INPUT_FILENAME:-}

if [[ -n "${INPUT_FILE}" && -f "${INPUT_FILE}" ]]; then
  tr -d '\r' < "${INPUT_FILE}" | valgrind \
    --leak-check=full \
    --show-leak-kinds=all \
    --error-exitcode=1 \
    "./${BUILD_DIR}/${EXECUTABLE_NAME}"
else
  valgrind \
    --leak-check=full \
    --show-leak-kinds=all \
    --error-exitcode=1 \
    "./${BUILD_DIR}/${EXECUTABLE_NAME}"
fi
