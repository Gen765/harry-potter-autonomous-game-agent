#include "Player.hh"

#include <optional>

/**
 * Baseline player used as a reproducible reference opponent.
 *
 * Strategy (intentionally simple, clearly weaker than Gen765):
 *   - Each wizard independently looks for the nearest book or the nearest
 *     enemy wizard using breadth-first search (BFS).
 *   - It then takes one step along the shortest path towards that target.
 *   - There is no global coordination between wizards, no ghost control and
 *     no phase-based strategy. This is on purpose: it is a baseline.
 *
 * Robustness note: the original draft used a real direction (`Up`) as an
 * "invalid" sentinel, which could order an unintended move when no target or
 * no path existed. This version uses std::optional<Dir> so that a move is only
 * issued when a valid direction is actually found.
 */
#define PLAYER_NAME Basic_player

struct PLAYER_NAME : public Player {

  /**
   * Factory: returns a new instance of this class.
   * Do not modify this function.
   */
  static Player* factory() {
    return new PLAYER_NAME;
  }

  const vector<Dir> dirs = {Up, Down, Left, Right};

  /**
   * Main method, executed every round.
   */
  virtual void play() {
    vector<int> my_wizards = wizards(me());
    for (int id : my_wizards) {
      Pos from = unit(id).pos;
      Pos target = bfs_target(from);
      if (not is_valid_pos(target)) continue;      // no reachable target: stay put
      std::optional<Dir> step = first_step_towards(from, target);
      if (step.has_value()) move(id, step.value()); // only move on a real path
    }
  }

private:
  bool is_valid_pos(const Pos& p) const {
    return p.i != -1 and p.j != -1;
  }

  bool is_enemy_wizard(const Cell& c) const {
    return c.id != -1
       and unit(c.id).type == Wizard
       and unit(c.id).player != me()
       and not unit(c.id).is_in_conversion_process();
  }

  /**
   * BFS that returns the position of the nearest book or capturable enemy
   * wizard, or {-1, -1} if none is reachable.
   */
  Pos bfs_target(Pos start) {
    queue<Pos> q;
    vector<vector<bool>> visited(board_rows(), vector<bool>(board_cols(), false));
    q.push(start);
    visited[start.i][start.j] = true;

    while (not q.empty()) {
      Pos current = q.front();
      q.pop();

      Cell c = cell(current);
      if (c.book or is_enemy_wizard(c)) return current;

      for (Dir d : dirs) {
        Pos next = current + d;
        if (pos_ok(next) and not visited[next.i][next.j] and cell(next).type == Corridor) {
          visited[next.i][next.j] = true;
          q.push(next);
        }
      }
    }
    return Pos(-1, -1);
  }

  /**
   * BFS shortest path from start to target. Returns the first step direction,
   * or std::nullopt when start == target or no path exists.
   */
  std::optional<Dir> first_step_towards(Pos start, Pos target) {
    if (start == target) return std::nullopt;

    queue<Pos> q;
    map<Pos, Pos> parent;
    vector<vector<bool>> visited(board_rows(), vector<bool>(board_cols(), false));

    q.push(start);
    visited[start.i][start.j] = true;
    bool reached = false;

    while (not q.empty() and not reached) {
      Pos current = q.front();
      q.pop();
      if (current == target) { reached = true; break; }

      for (Dir d : dirs) {
        Pos next = current + d;
        if (pos_ok(next) and not visited[next.i][next.j] and cell(next).type == Corridor) {
          visited[next.i][next.j] = true;
          parent[next] = current;
          q.push(next);
        }
      }
    }

    if (parent.find(target) == parent.end()) return std::nullopt; // unreachable

    // Walk back from target to the cell adjacent to start.
    Pos step = target;
    while (parent[step] != start) step = parent[step];

    for (Dir d : dirs) {
      if (start + d == step) return d;
    }
    return std::nullopt; // should not happen, but never guess a direction
  }
};

// Register the player so the engine can instantiate it by name.
RegisterPlayer(PLAYER_NAME);
