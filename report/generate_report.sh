#!/bin/bash
LOG_FILE="/home/marvin/tcc_experimentos/log/docker_stats.log"
RELATORIO_DIR="/home/marvin/tcc_experimentos/reports"
DATA_ATUAL=$(date '+%Y-%m-%d')
ARQUIVO_FINAL="$RELATORIO_DIR/relatorio_$DATA_ATUAL.txt"

if [ ! -f "$LOG_FILE" ] || [ ! -s "$LOG_FILE" ]; then
    echo "Nenhum dado coletado para o relat  rio."
    exit 1
fi

echo "==================================================" > "$ARQUIVO_FINAL"
echo "  RELATARIO DIARIO DE CONSUMO DOCKER - $DATA_ATUAL" >> "$ARQUIVO_FINAL"
echo "==================================================" >> "$ARQUIVO_FINAL"
printf "%-25s %-15s %-15s\n" "Container" "Media CPU" "Media memoria" >> "$ARQUIVO_FINAL"
echo "--------------------------------------------------" >> "$ARQUIVO_FINAL"

# Processa o log eliminando o caractere '%' para fazer o calculo
cat "$LOG_FILE" | sed 's/%//g' | awk '{
    container=$3; cpu=$4; mem=$5;
    sum_cpu[container]+=cpu;
    sum_mem[container]+=mem;
    count[container]++;
}
END {
    for (c in sum_cpu) {
        printf "%-25s %-14.2f%% %-14.2f%%\n", c, sum_cpu[c]/count[c], sum_mem[c]/count[c]
    }
}' >> "$ARQUIVO_FINAL"

echo "==================================================" >> "$ARQUIVO_FINAL"

# Limpa o arquivo de logs
> "$LOG_FILE"