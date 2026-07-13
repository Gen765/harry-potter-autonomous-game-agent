#!/usr/bin/env python3
"""Copy a replay file into the Viewer/ folder for the HTML replay viewer."""

from __future__ import annotations

import argparse
import shutil
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
VIEWER_DIR = ROOT / "Viewer"
DEFAULT_REPLAY = ROOT / "output" / "match-seed-1.res"


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", type=Path, default=DEFAULT_REPLAY, help="Replay .res file")
    parser.add_argument("--name", default="sample.out", help="Output filename inside Viewer/")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    if not args.input.exists():
        print(f"Replay not found: {args.input}", file=sys.stderr)
        return 1

    VIEWER_DIR.mkdir(parents=True, exist_ok=True)
    target = VIEWER_DIR / args.name
    shutil.copy2(args.input, target)
    print(f"Copied {args.input} -> {target}")
    print(f"Open {VIEWER_DIR / 'viewer.html'} in a browser and load {args.name}.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
