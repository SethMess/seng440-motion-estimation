#!/usr/bin/env bash
# profile.sh — run in the ARM VM. One callgrind pass per variant.
set -euo pipefail

VARIANTS="baseline unroll pipeline asm neon"
OUT=Results
mkdir -p "$OUT"
CSV="$OUT/summary.csv"
echo "variant,total_Ir,sad_Ir,text_bytes" > "$CSV"

make clean >/dev/null
make all >/dev/null

for v in $VARIANTS; do
    echo "=== $v ==="
    cg="$OUT/callgrind_$v.out"
    txt="$OUT/callgrind_$v.txt"

    valgrind --tool=callgrind \
             --toggle-collect=find_all_motion_vectors \
             --callgrind-out-file="$cg" \
             ./"$v" >/dev/null 2>&1

    callgrind_annotate "$cg" > "$txt"

    # Total Ir: the "PROGRAM TOTALS" line. Commas stripped.
    total=$(awk '/PROGRAM TOTALS/ {gsub(/,/,"",$1); print $1; exit}' "$txt")
    # Ir attributed to any sad_* function (needs -DPROFILE noinline build
    # for this to be nonzero at -O3 — otherwise it's inlined into the caller).
    sad=$(awk '/sad_/ {gsub(/,/,"",$1); s+=$1} END {print s+0}' "$txt")
    text=$(size ./"$v" | awk 'NR==2 {print $1}')

    echo "$v,$total,$sad,$text" >> "$CSV"
done

# Speedup column vs baseline, appended as a second table.
awk -F, 'NR==1 {next}
         $1=="baseline" {base=$2}
         {rows[NR]=$0}
         END {
           print "\nvariant  total_Ir  speedup_vs_baseline"
           for (i in rows) { split(rows[i],f,","); printf "%-9s %12d  %.2fx\n", f[1], f[2], base/f[2] }
         }' "$CSV"

echo "CSV: $CSV — graph total_Ir and speedup per variant."
