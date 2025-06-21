#!/bin/sh
set -e
(
cd "$(dirname "$0")" # Ensure compile steps are run within the repository directory
./.codecrafters/compile.sh
)
source ./.codecrafters/run.sh
