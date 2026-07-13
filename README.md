# Autonomous Game Agent (Harry Potter Edition)

C++17 autonomous agents for the FIB–UPC EDA tournament. The repository ships the
authorized course engine, an HTML replay viewer, four bundled players, Python
simulation helpers, and automated smoke/regression checks.

> **Category:** Academic project — EDA course, FIB–UPC.
> **Type:** Individual player implementation on a shared course engine.

## Bundled players

| Registered name | Source file | Role |
| --- | --- | --- |
| `Gen765` | `AIGen7.cc` | Primary tournament agent |
| `Basic_player` | `AIBasic_player.cc` | Reproducible baseline opponent |
| `Demo` | `AIDemo.cc` | Course demo player |
| `Null` | `AINull.cc` | No-op reference player |

Run `./Game --list` after building to verify exactly these four names.

## Requirements

- GCC or Clang with C++17 support. Verified locally with **g++ 11.4.0** on WSL Ubuntu.
- GNU Make.
- Python 3.10+ with `pytest` for regression tests.

## Build and smoke test

```sh
make clean
make
make list
make smoke-test SEED=1
```

## Tournament reproduction

```sh
python3 scripts/run_tournament.py --seeds 10 --rotations 4
python3 scripts/prepare_viewer.py --input output/tournament/seed01-rot0.res
```

This produces **40 matches** (10 seeds × 4 rotations), a CSV file and a
Markdown summary under `output/tournament/`.

The release-preparation run completed all 40 matches at round 200. `Gen765`
won all 40 against the three included baselines, with mean score 10,551.23.
These are reproducible local-baseline results, not an external tournament
ranking. The aggregate evidence is committed as
[`docs/tournament-summary.md`](docs/tournament-summary.md) and
[`docs/tournament-results.csv`](docs/tournament-results.csv).

## Automated tests

```sh
make regression-test
```

## Sanitizer build (Linux)

```sh
make sanitize
./Game --list
```

## Viewer

Open `Viewer/viewer.html` in a browser. Use `scripts/prepare_viewer.py` to copy a
`.res` replay into `Viewer/sample.out`.

## Verification status

**Core gameplay and the full local tournament are verified.** A clean Linux
build, four-player list, 3 regression tests, 40 complete 200-round matches and
sanitizer checks have been verified. The exporter records CSV data plus
per-player and per-position statistics. See
[`docs/results.md`](docs/results.md) for the current evidence boundary.

## Third-party code

The game engine and viewer are course materials. See
[`THIRD_PARTY_NOTICES.md`](THIRD_PARTY_NOTICES.md).

## Documentation

- [`docs/architecture.md`](docs/architecture.md)
- [`docs/strategy.md`](docs/strategy.md)
- [`docs/results.md`](docs/results.md)

## License

No permissive license is asserted for the course engine or tournament materials.
The custom player code is described with academic context only.
