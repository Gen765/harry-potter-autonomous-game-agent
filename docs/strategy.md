# Strategy Notes (`Gen765`)

This document summarises the decision policy implemented in `AIGen7.cc` without
claiming properties that were not measured in the bundled tournament scripts.

## High-level behaviour

The primary agent coordinates multiple wizards on a large grid with books,
convertible enemy wizards, ghosts and spell ingredients. The implementation
combines:

- Target selection for books and capturable enemy wizards.
- Pathfinding over corridor cells to reach short-term objectives.
- Phase-aware handling of conversion, resting ghosts and spell preparation.
- Per-wizard action assignment each round through the course `Player` API.

## Baseline comparison

`Basic_player` intentionally remains a weaker reference:

- Independent BFS per wizard without global coordination.
- Nearest book or capturable enemy wizard as the target.
- One step along the shortest path when available; otherwise no move.

## Evidence limits

Tournament CSVs and summaries under `output/tournament/` report wins and total
scores from bundled matches. They **do not** prove an official external ranking
unless a separate competition artifact is provided.

## Reproducibility

Given the same seed, configuration and player order, the engine replay is
deterministic. Use `scripts/run_tournament.py` to regenerate the published CSV
and Markdown summary.
