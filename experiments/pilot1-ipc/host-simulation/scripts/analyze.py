#!/usr/bin/env python3
"""
OMEGA — Pilot 1 IPC — Host simulation analysis

Analiza los resultados del benchmark simulado.

Advertencia: estos datos NO son evidencia sobre el rendimiento real de
ningún sistema operativo. Son datos del host prototype ejecutándose en
Termux, midiendo operaciones en memoria.
"""

import csv
import random
import statistics
import sys
from pathlib import Path
from collections import defaultdict

random.seed(42)

BOOTSTRAP_ITERATIONS = 2000
BOOTSTRAP_SAMPLE_MAX = 1000


def percentile(sorted_data, p):
    if not sorted_data:
        return 0
    k = (len(sorted_data) - 1) * (p / 100.0)
    f = int(k)
    c = min(f + 1, len(sorted_data) - 1)
    if f == c:
        return sorted_data[f]
    return sorted_data[f] + (sorted_data[c] - sorted_data[f]) * (k - f)


def bootstrap_ci_median(samples, n_boot=BOOTSTRAP_ITERATIONS):
    """IC bootstrap de la mediana, con submuestreo si n > BOOTSTRAP_SAMPLE_MAX."""
    n = len(samples)
    if n == 0:
        return 0, 0

    if n > BOOTSTRAP_SAMPLE_MAX:
        sample_pool = random.sample(samples, BOOTSTRAP_SAMPLE_MAX)
    else:
        sample_pool = samples

    m = len(sample_pool)
    medians = []
    for _ in range(n_boot):
        resample = [sample_pool[random.randrange(m)] for _ in range(m)]
        medians.append(statistics.median(resample))

    medians.sort()
    lo = medians[int(n_boot * 0.025)]
    hi = medians[int(n_boot * 0.975)]
    return lo, hi


def main():
    csv_path = Path(sys.argv[1]) if len(sys.argv) > 1 else Path("results/pilot1_host.csv")
    if not csv_path.exists():
        print(f"ERROR: no existe {csv_path}", file=sys.stderr)
        return 1

    by_size = defaultdict(list)
    with open(csv_path) as f:
        reader = csv.DictReader(f)
        for row in reader:
            size = int(row["size"])
            latency = int(row["latency_ns"])
            by_size[size].append(latency)

    print(f"=== Analysis of {csv_path} ===\n")
    print("ADVERTENCIA: datos del host prototype, no evidencia real.\n")

    for size in sorted(by_size.keys()):
        samples = by_size[size]
        samples_sorted = sorted(samples)
        n = len(samples)
        median = statistics.median(samples_sorted)
        mean = statistics.mean(samples_sorted)
        stdev = statistics.stdev(samples_sorted) if n > 1 else 0.0
        p50 = percentile(samples_sorted, 50)
        p95 = percentile(samples_sorted, 95)
        p99 = percentile(samples_sorted, 99)
        lo, hi = bootstrap_ci_median(samples_sorted)

        print(f"Size: {size} bytes (n={n})")
        print(f"  mean   = {mean:>10.0f} ns")
        print(f"  median = {median:>10.0f} ns")
        print(f"  stdev  = {stdev:>10.0f} ns")
        print(f"  p50    = {p50:>10.0f} ns")
        print(f"  p95    = {p95:>10.0f} ns")
        print(f"  p99    = {p99:>10.0f} ns")
        print(f"  min    = {samples_sorted[0]:>10d} ns")
        print(f"  max    = {samples_sorted[-1]:>10d} ns")
        print(f"  CI95 median = [{lo:.0f}, {hi:.0f}] ns")
        print()

    return 0


if __name__ == "__main__":
    sys.exit(main())
