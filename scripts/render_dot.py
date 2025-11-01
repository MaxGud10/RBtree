#!/usr/bin/env python3
import argparse
import shutil
import subprocess
from pathlib import Path
import sys

def main():
    parser = argparse.ArgumentParser(description="Render Graphviz DOT to PNG")
    parser.add_argument("dot", help="Input .dot file")
    parser.add_argument("png", help="Output .png file")
    args = parser.parse_args()

    dot_path = Path(args.dot)
    png_path = Path(args.png)

    if not dot_path.exists():
        print(f"[ERROR] DOT file not found: {dot_path}", file=sys.stderr)
        sys.exit(1)

    if shutil.which("dot") is None:
        print("[ERROR] Graphviz 'dot' not found in PATH. Please install graphviz.", file=sys.stderr)
        sys.exit(2)

    png_path.parent.mkdir(parents=True, exist_ok=True)
    cmd = ["dot", "-Tpng", str(dot_path), "-o", str(png_path)]

    try:
        subprocess.run(cmd, check=True)
    except subprocess.CalledProcessError as e:
        print(f"[ERROR] dot failed with code {e.returncode}", file=sys.stderr)
        sys.exit(e.returncode)

    print(f"[OK] PNG written to: {png_path}")

if __name__ == "__main__":
    main()

# python3 scripts/render_dot.py graphviz/file_graph.dot graphviz/tree.png
