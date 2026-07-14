# Strategy notes for `Gen765`

The player controls several wizards at once, but each unit ultimately receives
one action per round. `AIGen7.cc` builds those actions from a set of local rules
and breadth-first searches.

## Choosing a target

Books are the default objective. Nearby enemy wizards become useful targets
when they can be converted, and some situations switch priority to helping a
wizard, dealing with ghosts or collecting spell ingredients. The code evaluates
these cases in order and keeps a fallback when the preferred target is not
reachable.

## Movement

Searches work over the current board and return the first direction on a path
to the selected objective. Before issuing a move, the player checks the target
cell and applies the relevant rule for corridors, occupied positions and
special tiles. This is repeated for every controlled wizard each round.

The approach is deliberately heuristic. It was practical for a timed course
tournament and easy to adjust between matches, although it also led to large
functions and some repeated searches in the final source.

## Baseline comparison

`Basic_player` is a simpler reference player. It runs an independent BFS for
each wizard, chooses the nearest book or convertible opponent and takes one
step towards it. It does not coordinate targets or use the extra phase-specific
rules in `Gen765`.

Run `scripts/run_tournament.py` to compare the players over several seeds and
starting-position rotations.
