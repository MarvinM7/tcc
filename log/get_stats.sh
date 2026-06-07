#!/bin/bash
LOG_FILE="/home/marvin/tcc_experimentos/log/docker_stats.log"

# Formato: Data Hora | Nome | CPU% | Mem%
docker stats --no-stream --format "{{.Name}} {{.CPUPerc}} {{.MemPerc}}" | while read -r line; do
    echo "$(date '+%Y-%m-%d %H:%M:%S') $line" >> "$LOG_FILE"
done