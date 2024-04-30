#!/bin/bash

# Verifica se o número de argumentos é válido
if [ "$#" -ne 3 ]; then
    echo "Uso: $0 <numero_de_execucoes> <tempo> <argumentos>"
    exit 1
fi

# Extrai os argumentos
num_execucoes="$1"
tempo="$2"

# Loop para executar o comando o número de vezes especificado
for ((i = 0; i < num_execucoes; i++)); do
    # Executa o comando
    ./bin/client execute "$tempo" -u "grep -v ˆ# /etc/passwd"
done
