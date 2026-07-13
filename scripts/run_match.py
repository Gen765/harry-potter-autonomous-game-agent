#!/usr/bin/env python3
"""Run a single Harry Potter game match and write a .res replay file."""

from __future__ import annotations

import argparse
import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
DEFAULT_CONFIG = ROOT / "config" / "default.cnf"
DEFAULT_PLAYERS = ["Gen765", "Basic_player", "Demo", "Null"]


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--seed", type=int, default=1)
    parser.add_argument("--config", type=Path, default=DEFAULT_CONFIG)
    parser.add_argument("--output", type=Path, default=None)
    parser.add_argument("--game", type=Path, default=ROOT / "Game")
    parser.add_argument("players", nargs="*", default=DEFAULT_PLAYERS)
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    output = args.output or (ROOT / "output" / f"match-seed-{args.seed}.res")
    output.parent.mkdir(parents=True, exist_ok=True)

    game_bin = args.game
    if not game_bin.exists() and (ROOT / "Game.exe").exists():
        game_bin = ROOT / "Game.exe"
    if not game_bin.exists():
        print(f"Game binary not found: {game_bin}", file=sys.stderr)
        return 1

    cmd = [
        str(game_bin),
        f"--seed={args.seed}",
        f"--input={args.config}",
        f"--output={output}",
        *args.players,
    ]
    print("Running:", " ".join(cmd))
    completed = subprocess.run(cmd, cwd=ROOT, check=False)
    if completed.returncode != 0:
        return completed.returncode
    print(f"Replay written to {output}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
