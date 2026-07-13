#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
JOBS="${1:-4}"
cd "$ROOT"
mkdir -p output/tournament

run_one() {
    local seed="$1"
    local rotation="$2"
    local players=(Gen765 Basic_player Demo Null)
    local order=("${players[@]:rotation}" "${players[@]:0:rotation}")
    local output
    output="output/tournament/seed$(printf '%02d' "$seed")-rot${rotation}.res"
    echo "running seed=$seed rotation=$rotation"
    ./Game --seed="$seed" --input=config/default.cnf --output="$output" "${order[@]}"
}
export -f run_one

for seed in {1..10}; do
    for rotation in {0..3}; do
        replay="output/tournament/seed$(printf '%02d' "$seed")-rot${rotation}.res"
        if ! grep -q '^round 200$' "$replay" 2>/dev/null; then
            printf '%s %s\n' "$seed" "$rotation"
        fi
    done
done | xargs -r -n2 -P "$JOBS" bash -c 'run_one "$1" "$2"' _
