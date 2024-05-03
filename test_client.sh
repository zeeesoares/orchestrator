#!/bin/bash

# Verifica se o número de argumentos é válido
if [ "$#" -lt 3 ]; then
    echo "Uso: $0 <numero_de_execucoes> <tempo> <comando> [<argumentos> ...]"
    exit 1
fi

# Extrai os argumentos
num_execucoes="$1"
tempo="$2"
comando="$3"
shift 3  # Remove os três primeiros argumentos (número de execuções, tempo e comando)

# Loop para executar o comando o número de vezes especificado
for ((i = 0; i < num_execucoes; i++)); do
    # Executa o comando
    ./bin/client execute "$tempo" -u "$comando" "$@"
done