# Verification results

## Clean build and automated tests

Verified on WSL Ubuntu with g++ 11.4.0 and Python 3.10.12:

```sh
make clean
make -j2
python3 -m pytest -q tests
```

The engine and four authorised players built with C++17 without compiler
warnings, `Game --list` exposed only `Basic_player`, `Demo`, `Gen765` and
`Null`, and all 3 regression tests passed.

## Reproducible local tournament

```sh
python3 scripts/run_tournament.py --seeds 10 --rotations 4
```

- 40/40 matches completed.
- 40/40 replays ended at round 200.
- `Gen765` won 40/40 matches against the three bundled baselines.
- Mean `Gen765` score: 10,551.23 (total 422,049).
- Mean baseline scores: Basic_player 5,908.60; Demo 1,420.60; Null 720.48.

The committed [`tournament-summary.md`](tournament-summary.md) includes wins
and mean scores by player and starting position. The underlying aggregate rows
are in [`tournament-results.csv`](tournament-results.csv); large replay files
remain generated artefacts and are not committed.

These measurements apply only to the bundled engine, configuration and
baselines. They do not establish an external tournament placement. Historical
evidence supports only the cautious statement that the agent reached the
academic tournament's final phase; no third-place claim is made.

## Sanitizers

The engine and players were rebuilt with AddressSanitizer and
UndefinedBehaviorSanitizer, then a complete seed-1 match was run with leak
detection and halt-on-error enabled. The first leak-enabled run exposed 691
unreleased allocations (268,812 bytes) from factory-created players and the
global registry. Player ownership was converted to `std::unique_ptr`, the base
destructor made virtual, and the registry changed to automatic static storage.
The complete instrumented match then exited successfully with no sanitizer or
leak diagnostic.
