#!/usr/bin/env python3
import argparse
import subprocess
import sys
from pathlib import Path
import difflib
import time


def run_compare(binary, input_file, expected_file) -> int:
    input_path = Path(input_file)
    expected_path = Path(expected_file)

    if not input_path.exists():
        print(f"[ERROR] input file not found: {input_path}", file=sys.stderr)
        return 2
    if not expected_path.exists():
        print(f"[ERROR] expected file not found: {expected_path}", file=sys.stderr)
        return 2

    with input_path.open("r", encoding="utf-8") as f:
        inp = f.read()
    with expected_path.open("r", encoding="utf-8") as f:
        expected = f.read()

    start = time.perf_counter()
    proc = subprocess.run(
        [binary],
        input=inp.encode("utf-8"),
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    elapsed = time.perf_counter() - start

    output = proc.stdout.decode("utf-8")

    if proc.returncode != 0:
        print(f"[ERROR] program exited with code {proc.returncode}", file=sys.stderr)
        print(proc.stderr.decode("utf-8"), file=sys.stderr)
        return proc.returncode or 1

    if output != expected:
        print("Output differs!\n")
        diff = difflib.unified_diff(
            expected.splitlines(),
            output.splitlines(),
            fromfile="expected",
            tofile="actual",
            lineterm="",
        )
        print("\n".join(diff))
        print(f"\n[INFO] elapsed: {elapsed:.6f} s")
        return 1

    print(f"[OK] output matches. elapsed: {elapsed:.6f} s")
    return 0


def run_bench(binary, input_file) -> int:
    input_path = Path(input_file)

    if not input_path.exists():
        print(f"[ERROR] input file not found: {input_path}", file=sys.stderr)
        return 2

    with input_path.open("r", encoding="utf-8") as f:
        inp = f.read()

    start = time.perf_counter()
    proc = subprocess.run(
        [binary],
        input=inp.encode("utf-8"),
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    elapsed = time.perf_counter() - start

    if proc.returncode != 0:
        print(f"[ERROR] program exited with code {proc.returncode}", file=sys.stderr)
        print(proc.stderr.decode("utf-8"), file=sys.stderr)
        return proc.returncode or 1

    # Вывод нам не важен, просто не даём ему засорять лог
    # Если хочешь – можно раскомментить:
    # print(proc.stdout.decode("utf-8"))

    print(f"[BENCH] elapsed: {elapsed:.6f} s")
    return 0


def main() -> int:
    parser = argparse.ArgumentParser(
        description="E2E launcher for rb_tree (compare / bench modes)"
    )
    parser.add_argument(
        "--mode",
        choices=["compare", "bench"],
        required=True,
        help="compare: check output vs expected; bench: just run and measure time",
    )
    parser.add_argument("binary", help="Path to rb_tree binary")
    parser.add_argument("input", help="Input file for stdin")
    parser.add_argument(
        "expected",
        nargs="?",
        help="Expected-output file (only for mode=compare)",
    )

    args = parser.parse_args()

    if args.mode == "compare":
        if not args.expected:
            print("[ERROR] expected file is required in compare mode", file=sys.stderr)
            return 2
        return run_compare(args.binary, args.input, args.expected)
    else:  # bench
        return run_bench(args.binary, args.input)


if __name__ == "__main__":
    raise SystemExit(main())
