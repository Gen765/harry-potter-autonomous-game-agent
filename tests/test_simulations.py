"""Smoke and regression checks for bundled tournament scripts."""

from __future__ import annotations

import csv
import subprocess
import sys
from pathlib import Path

import pytest


ROOT = Path(__file__).resolve().parents[1]
GAME = ROOT / "Game"
if not GAME.exists() and (ROOT / "Game.exe").exists():
    GAME = ROOT / "Game.exe"


@pytest.fixture(scope="session")
def game_binary() -> Path:
    if not GAME.exists():
        pytest.skip("Game binary not built; run `make` first")
    return GAME


def test_game_lists_four_players(game_binary: Path) -> None:
    completed = subprocess.run([str(game_binary), "--list"], cwd=ROOT, check=True, capture_output=True, text=True)
    names = [line.strip() for line in completed.stdout.splitlines() if line.strip()]
    assert names == ["Basic_player", "Demo", "Gen765", "Null"]


def test_smoke_match_produces_replay(game_binary: Path, tmp_path: Path) -> None:
    output = tmp_path / "smoke.res"
    cmd = [
        str(game_binary),
        "--seed=42",
        "--input=config/default.cnf",
        f"--output={output}",
        "Gen765",
        "Basic_player",
        "Demo",
        "Null",
    ]
    subprocess.run(cmd, cwd=ROOT, check=True)
    text = output.read_text(encoding="utf-8", errors="replace")
    assert "score" in text.lower()
    assert "names" in text


def test_tournament_script_runs_small_grid(game_binary: Path, tmp_path: Path) -> None:
    out_dir = tmp_path / "tournament"
    cmd = [
        sys.executable,
        "scripts/run_tournament.py",
        "--seeds",
        "2",
        "--rotations",
        "2",
        "--game",
        str(game_binary),
        "--output-dir",
        str(out_dir),
    ]
    subprocess.run(cmd, cwd=ROOT, check=True)
    csv_path = out_dir / "tournament.csv"
    summary_path = out_dir / "summary.md"
    assert csv_path.exists()
    assert summary_path.exists()
    with csv_path.open(encoding="utf-8") as handle:
        rows = list(csv.reader(handle))
    assert len(rows) == 5  # header + 4 matches
    assert rows[0] == [
        "seed",
        "rotation",
        "player_order",
        "final_round",
        "winner",
        "Gen765",
        "Basic_player",
        "Demo",
        "Null",
    ]
    assert "Gen765" in summary_path.read_text(encoding="utf-8")
