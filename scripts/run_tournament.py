#!/usr/bin/env python3
"""Run a round-robin tournament and export CSV + Markdown summary."""

from __future__ import annotations

import argparse
import csv
import subprocess
import sys
from collections import defaultdict
from dataclasses import dataclass
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
DEFAULT_PLAYERS = ["Gen765", "Basic_player", "Demo", "Null"]


@dataclass(frozen=True)
class MatchResult:
    seed: int
    rotation: int
    players: tuple[str, ...]
    final_round: int
    winner: str
    scores: dict[str, int]


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--seeds", type=int, default=10, help="Number of seeds (default: 10)")
    parser.add_argument("--rotations", type=int, default=4, help="Player-order rotations per seed")
    parser.add_argument("--config", type=Path, default=ROOT / "config" / "default.cnf")
    parser.add_argument("--game", type=Path, default=ROOT / "Game")
    parser.add_argument("--output-dir", type=Path, default=ROOT / "output" / "tournament")
    parser.add_argument("--players", nargs="*", default=DEFAULT_PLAYERS)
    parser.add_argument(
        "--summarize-existing",
        action="store_true",
        help="Do not run matches; rebuild CSV/Markdown from expected replay files",
    )
    return parser.parse_args()


def rotate(players: list[str], shift: int) -> list[str]:
    shift %= len(players)
    return players[shift:] + players[:shift]


def run_match(game: Path, config: Path, seed: int, players: list[str], output: Path) -> None:
    output.parent.mkdir(parents=True, exist_ok=True)
    cmd = [
        str(game),
        f"--seed={seed}",
        f"--input={config}",
        f"--output={output}",
        *players,
    ]
    completed = subprocess.run(cmd, cwd=ROOT, check=True, capture_output=True, text=True)
    if completed.stdout:
        print(completed.stdout.rstrip())


def parse_scores(res_path: Path) -> tuple[str, dict[str, int], int]:
    """Parse final scores from a .res replay file."""
    lines = res_path.read_text(encoding="utf-8", errors="replace").splitlines()
    player_names: list[str] = []
    score_values: list[int] = []
    final_round = -1

    for line in lines:
        if line.startswith("names"):
            player_names = line.split()[1:]
        elif line.startswith("score"):
            parts = line.split()
            score_values = [int(value) for value in parts[1:]]
        elif line.startswith("round "):
            final_round = int(line.split()[1])

    if not player_names or not score_values:
        raise ValueError(f"Could not parse scores from {res_path}")
    if len(player_names) != len(score_values):
        raise ValueError(
            f"Player/score count mismatch in {res_path}: "
            f"{len(player_names)} names, {len(score_values)} scores"
        )
    if final_round < 0:
        raise ValueError(f"Could not parse final round from {res_path}")

    scores = dict(zip(player_names, score_values))
    winner = max(scores, key=scores.get)
    return winner, scores, final_round


def collect_results(args: argparse.Namespace) -> list[MatchResult]:
    game = args.game
    if not game.exists() and (ROOT / "Game.exe").exists():
        game = ROOT / "Game.exe"
    if not game.exists():
        raise FileNotFoundError(f"Game binary not found: {game}")

    players = list(args.players)
    results: list[MatchResult] = []
    match_id = 0
    for seed in range(1, args.seeds + 1):
        for rotation in range(args.rotations):
            order = rotate(players, rotation)
            match_id += 1
            out = args.output_dir / f"seed{seed:02d}-rot{rotation}.res"
            print(f"[{match_id}] seed={seed} rotation={rotation} order={' '.join(order)}")
            if not args.summarize_existing:
                run_match(game, args.config, seed, order, out)
            elif not out.exists():
                raise FileNotFoundError(f"Expected replay not found: {out}")
            winner, scores, final_round = parse_scores(out)
            results.append(
                MatchResult(seed, rotation, tuple(order), final_round, winner, scores)
            )
    return results


def write_csv(path: Path, results: list[MatchResult], player_names: list[str]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", newline="", encoding="utf-8") as handle:
        writer = csv.writer(handle)
        writer.writerow(
            ["seed", "rotation", "player_order", "final_round", "winner", *player_names]
        )
        for result in results:
            row = [
                result.seed,
                result.rotation,
                " ".join(result.players),
                result.final_round,
                result.winner,
            ]
            row.extend(result.scores.get(name, 0) for name in player_names)
            writer.writerow(row)


def write_summary(path: Path, results: list[MatchResult]) -> None:
    wins = defaultdict(int)
    total_score = defaultdict(int)
    position_score = defaultdict(int)
    position_matches = defaultdict(int)
    position_wins = defaultdict(int)
    for result in results:
        wins[result.winner] += 1
        for player, score in result.scores.items():
            total_score[player] += score
        for position, player in enumerate(result.players, start=1):
            key = (player, position)
            position_score[key] += result.scores[player]
            position_matches[key] += 1
            if result.winner == player:
                position_wins[key] += 1

    lines = [
        "# Tournament Summary",
        "",
        f"- Matches played: **{len(results)}**",
        f"- Seeds: **{len({r.seed for r in results})}**",
        f"- Rotations per seed: **{len(results) // max(1, len({r.seed for r in results}))}**",
        f"- Matches reaching round 200: **{sum(r.final_round == 200 for r in results)}**",
        "",
        "## Wins",
        "",
        "| Player | Wins | Total score | Mean score |",
        "| --- | ---: | ---: | ---: |",
    ]
    for player in sorted(total_score, key=lambda p: (-wins[p], -total_score[p], p)):
        mean_score = total_score[player] / len(results)
        lines.append(
            f"| {player} | {wins[player]} | {total_score[player]} | {mean_score:.2f} |"
        )

    lines.extend(
        [
            "",
            "## Results by starting position",
            "",
            "| Player | Position | Matches | Wins | Mean score |",
            "| --- | ---: | ---: | ---: | ---: |",
        ]
    )
    for player, position in sorted(position_matches):
        key = (player, position)
        mean_score = position_score[key] / position_matches[key]
        lines.append(
            f"| {player} | {position} | {position_matches[key]} | "
            f"{position_wins[key]} | {mean_score:.2f} |"
        )

    lines.extend(
        [
            "",
            "This summary reports measured outcomes from bundled replay files.",
            "It does not claim an external competition ranking unless explicitly evidenced.",
        ]
    )
    path.write_text("\n".join(lines) + "\n", encoding="utf-8")


def main() -> int:
    args = parse_args()
    if args.seeds < 1 or args.rotations < 1:
        print("--seeds and --rotations must both be positive", file=sys.stderr)
        return 2
    if not args.players:
        print("At least one player is required", file=sys.stderr)
        return 2
    try:
        results = collect_results(args)
    except (FileNotFoundError, subprocess.CalledProcessError, ValueError) as exc:
        print(exc, file=sys.stderr)
        return 1

    csv_path = args.output_dir / "tournament.csv"
    summary_path = args.output_dir / "summary.md"
    write_csv(csv_path, results, list(args.players))
    write_summary(summary_path, results)
    print(f"CSV -> {csv_path}")
    print(f"Summary -> {summary_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
