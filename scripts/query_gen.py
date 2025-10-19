#!/usr/bin/env python3
import argparse
from pathlib import Path
from typing import Iterable, List

def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description=(
            "Query generator for the Red-Black Tree project. "
            "Generates a sequence of insert (k) and range (q) queries."
        )
    )
    parser.add_argument("elem_num", type=int, help="Number of elements to insert (>= 0)")
    parser.add_argument("filename", help="Output filename for generated queries")
    mode = parser.add_mutually_exclusive_group()
    mode.add_argument(
        "--unordered", action="store_true",
        help="Generate one q-query per unordered pair {i, j} where i < j (default)"
    )
    mode.add_argument(
        "--ordered", action="store_true",
        help="Generate q-queries for every ordered pair (i, j) and (j, i); "
             "both printed as 'q min max' (duplicates appear)"
    )
    return parser.parse_args()

def gen_inserts(n: int) -> Iterable[str]:
    """Generate k-insert commands."""
    for i in range(n):
        yield f"k {i}"

def gen_queries(n: int, ordered: bool) -> Iterable[str]:
    """Generate q-range commands."""
    if n <= 1:
        return []
    if ordered:
        for i in range(n):
            for j in range(n):
                if i == j:
                    continue
                a, b = (i, j) if i <= j else (j, i)
                yield f"q {a} {b}"
    else:
        for i in range(n):
            for j in range(i + 1, n):
                yield f"q {i} {j}"

def write_lines(path: Path, lines: Iterable[str]) -> None:
    """Write generated queries to a file, one per line."""
    with path.open("w", encoding="utf-8") as f:
        for line in lines:
            f.write(line)
            f.write("\n")

def main() -> None:
    args = parse_args()
    n = args.elem_num
    if n < 0:
        raise SystemExit("ERROR: elem_num must be a non-negative integer.")

    ordered = bool(args.ordered)
    out_path = Path(args.filename)

    inserts = list(gen_inserts(n))
    queries = list(gen_queries(n, ordered=ordered))

    all_lines: List[str] = inserts + queries
    write_lines(out_path, all_lines)

    print(f"[query_gen] Elements: {n}")
    print(f"[query_gen] Mode: {'ordered' if ordered else 'unordered'} pairs")
    print(f"[query_gen] Total lines: {len(all_lines)}")
    print(f"[query_gen] Output file: {out_path}")

if __name__ == "__main__":
    main()
