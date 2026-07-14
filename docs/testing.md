# Testing

## Regression suite

Build the game before running the Python tests:

```sh
make clean
make
python3 -m pip install -r requirements-dev.txt
make regression-test
```

The three tests check that:

- the executable registers `Basic_player`, `Demo`, `Gen765` and `Null`;
- a complete seed-42 match produces a replay with player names and scores; and
- the tournament script can run a small two-seed, two-rotation grid and write
  both CSV and Markdown summaries.

## Sanitizers

The diagnostic target enables AddressSanitizer and UndefinedBehaviorSanitizer:

```sh
make sanitize
ASAN_OPTIONS=detect_leaks=1:halt_on_error=1 ./Game \
  --seed=1 \
  --input=config/default.cnf \
  --output=/tmp/sanitizer.res \
  Gen765 Basic_player Demo Null
```

An early leak-enabled run found 691 unreleased allocations (268,812 bytes) in
factory-created players and the global registry. Fixing it involved storing
players in `std::unique_ptr`, making the base `Player` destructor virtual and
changing the registry to automatic static storage.

After those changes, a complete seed-1 match finished without sanitizer or
leak diagnostics.
