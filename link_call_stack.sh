#!/usr/bin/env bash

if [[ "$#" < 2 ]]
then
    echo "Usage: $0 call_stack rom_dump"
    exit 1
fi

OLDIFS="$IFS"
IFS=

call_stack="$1"
rom_dump="$2"

declare -A symbols
while IFS="$OLDIFS" read addr symb
do
    if [ -z "${symbols["$addr"]}" ]
    then
        symbols["$addr"]="$symb"
    fi
done < <(grep -E '^[[:xdigit:]]{8} <[^>]*>:$' "$rom_dump" | sed 's/:$//')

while read line
do
    addr="${line:0:9}"
    line="${line#$addr}"
    sym_addr="$(echo -n "$line" | grep -o '[[:xdigit:]]*')"
    symbol="${symbols[$sym_addr]}"
    echo "$addr $line $symbol"
done <"$call_stack"

