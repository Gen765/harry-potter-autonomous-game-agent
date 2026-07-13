# Architecture

## Components

| Layer | Files | Responsibility |
| --- | --- | --- |
| Engine core | `Game.*`, `Board.*`, `State.*`, `Action.*` | Round loop, board updates, combat and spell rules |
| Player API | `Player.*`, `Registry.*`, `Info.*` | Per-player hooks and read-only game queries |
| Configuration | `Settings.*`, `config/default.cnf` | Match parameters and random seed handling |
| Agents | `AI*.cc` | Decision policies registered through `RegisterPlayer` |
| Replay I/O | `Main.cc`, `Utils.*` | CLI, `.res` replay export |
| Viewer | `Viewer/` | Browser replay of `.res` files |
| Tooling | `scripts/`, `tests/` | Tournament batch runs and smoke/regression checks |

## Build model

`Makefile` compiles one engine binary (`Game`) and links every `AI*.cc` player
object discovered in the repository root. No precompiled student objects or
`AIDummy` bots are included.

## Simulation flow

1. `Game` loads configuration and registers players from linked factories.
2. Each round calls `Player::play()` for every clan in order.
3. The engine writes a full replay to the requested `.res` output.
4. Python helpers parse the final `names` and `score` lines for tournament summaries.

## Design constraints from the course API

Players query the board through helper methods (`cell`, `unit`, `wizards`) and
issue actions with `move`, `convert`, `spell`, etc. The bundled baseline uses
BFS over corridor cells and only moves when a valid direction exists
(`std::optional<Dir>`), avoiding sentinel-direction bugs.
