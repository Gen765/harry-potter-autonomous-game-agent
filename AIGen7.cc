#include "Player.hh"

/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Gen765

struct PLAYER_NAME : public Player {

  /**
   * Factory: returns a new instance of this class.
   * Do not modify this function.
   */
  static Player* factory() {
    return new PLAYER_NAME;
  }

  const vector<Dir> dirs = {Left, Right, Up, Down}; //direcciones magos cangrejo
  const vector<Dir> normal_dirs = {Up, Down, Left, Right}; //direcciones magos gallina
  const vector<Dir> all_dirs = {DR, RU, UL, LD, Up, Down, Left, Right}; //direcciones fantasma
  const Dir Invalid = Up; // centinela

    //Se prioriza atacar libros segun cercania, si mas de un mago va por el mismo libro, los que no sean el mas cercano iran a por un mago
void manejar_libros_y_magos_early(set<int>& magos_sanos, map<Pos, pair<int, int>>& book_assignments, set<int>& assigned_wizards) {
    for (auto it = magos_sanos.begin(); it != magos_sanos.end();) {
        int wizard_id = *it;
        Pos book_pos = bfs_find_book_mage(unit(wizard_id).pos);
        bool eliminado = false;

        if (book_pos.i != -1 && cell(book_pos).book) {
            int distance = distance_mapa(unit(wizard_id).pos, book_pos);
            if (book_assignments.count(book_pos)) {
                if (distance < book_assignments[book_pos].second) {
                    int previous_wizard = book_assignments[book_pos].first;
                    Pos target = bfs_mage(unit(previous_wizard).pos);
                    Dir dir = bfs_direction(unit(previous_wizard).pos, target);
                    if (cell(target+dir).id != -1) {
                        dir = bfs_direction_reversed(unit(previous_wizard).pos, target);
                        if (cell(target+dir).id != -1) {
                            target = bfs_mage_reversed(unit(previous_wizard).pos);
                            dir = bfs_direction(unit(previous_wizard).pos, target);
                            if (cell(target+dir).id != -1) {
                                 dir = bfs_direction_reversed(unit(previous_wizard).pos, target);
                            }
                        }
                    }
                    move(previous_wizard, dir);
                    magos_sanos.erase(previous_wizard);
                    assigned_wizards.erase(previous_wizard);
                    book_assignments[book_pos] = {wizard_id, distance};
                    assigned_wizards.insert(wizard_id);
                } else {
                    Pos target = bfs_mage(unit(wizard_id).pos);
                    Dir dir = bfs_direction(unit(wizard_id).pos, target);
                    if (cell(target+dir).id != -1) {
                        dir = bfs_direction_reversed(unit(wizard_id).pos, target);
                        if (cell(target+dir).id != -1) {
                            target = bfs_mage_reversed(unit(wizard_id).pos);
                            dir = bfs_direction(unit(wizard_id).pos, target);
                            if (cell(target+dir).id != -1) {
                                 dir = bfs_direction_reversed(unit(wizard_id).pos, target);
                            }
                        }
                    }
                    move(wizard_id, dir);
                    eliminado = true;
                }
            } else {
                book_assignments[book_pos] = {wizard_id, distance};
                assigned_wizards.insert(wizard_id);
            }
        } else {
            Dir dir = bfs_direction(unit(wizard_id).pos, book_pos);
             if (cell(book_pos+dir).id != -1) {
                        dir = bfs_direction_reversed(unit(wizard_id).pos, book_pos);
                        if (cell(book_pos+dir).id != -1) {
                            book_pos = bfs_mage_reversed(unit(wizard_id).pos);
                            dir = bfs_direction(unit(wizard_id).pos, book_pos);
                            if (cell(book_pos+dir).id != -1) {
                                dir = bfs_direction_reversed(unit(wizard_id).pos, book_pos);
                            }
                        }
                    }
            move(wizard_id, dir);
            eliminado = true;
        }
        if (eliminado) {
            it = magos_sanos.erase(it);
        } else {
            ++it;
        }
    }
}

//magos van al objetivo mas cercano (libro, mago enemigo o aliado en conversion)
void manejar_libros_y_magos_midgame(set<int>& magos_sanos, map<Pos, pair<int, int>>& book_assignments, set<int>& assigned_wizards, set<Pos>& atacados) {
    for (auto it = magos_sanos.begin(); it != magos_sanos.end();) {
        int wizard_id = *it;
        Pos book_pos = bfs_target(unit(wizard_id).pos);
        bool eliminado = false;

        if (book_pos.i != -1 && cell(book_pos).book) {
            int distance = distance_mapa(unit(wizard_id).pos, book_pos);
            if (book_assignments.count(book_pos)) {
                if (distance < book_assignments[book_pos].second) {
                    int previous_wizard = book_assignments[book_pos].first;
                    Pos target = bfs_mage(unit(previous_wizard).pos);
                    Dir dir = bfs_direction(unit(previous_wizard).pos, target);

                    move(previous_wizard, dir);
                    magos_sanos.erase(previous_wizard);
                    assigned_wizards.erase(previous_wizard);
                    book_assignments[book_pos] = {wizard_id, distance};
                    assigned_wizards.insert(wizard_id);
                } else {
                    Pos target = bfs_mage(unit(wizard_id).pos);
                    Dir dir = bfs_direction(unit(wizard_id).pos, target);

                    move(wizard_id, dir);
                    eliminado = true;
                }
            } else {
                book_assignments[book_pos] = {wizard_id, distance};
                assigned_wizards.insert(wizard_id);
            }
        } else {
            eliminado = true;
            if (unit(cell(book_pos).id).player != me()) {
              if (atacados.find(book_pos) != atacados.end() and distance_mapa(unit(wizard_id).pos, book_pos) > 3) {
                Dir dir = bfs_direction_rodear(unit(wizard_id).pos, book_pos);
                move(wizard_id, dir);
                if (cell(book_pos).id != -1) atacados.insert(book_pos);
              }
              else {
                Dir dir = bfs_direction(unit(wizard_id).pos, book_pos);
                if (cell(unit(wizard_id).pos + dir).id != -1) dir = bfs_direction_reversed(unit(wizard_id).pos, book_pos);
                if (cell(unit(wizard_id).pos + dir).id != -1) {
                  book_pos = bfs_mage_reversed(unit(wizard_id).pos);
                  dir = bfs_direction(unit(wizard_id).pos, book_pos);
                  if (cell(unit(wizard_id).pos + dir).id != -1) dir = bfs_direction_reversed(unit(wizard_id).pos, book_pos);
                }
                move(wizard_id, dir);
                if (atacados.find(book_pos) == atacados.end()) atacados.insert(book_pos);
              }
            }
            else {
              Dir dir = bfs_direction(unit(wizard_id).pos, book_pos);
              if (cell(book_pos+dir).id != -1) {
                        dir = bfs_direction_reversed(unit(wizard_id).pos, book_pos);
                        if (cell(book_pos+dir).id != -1) {
                            book_pos = bfs_mage_reversed(unit(wizard_id).pos);
                            dir = bfs_direction(unit(wizard_id).pos, book_pos);
                            if (cell(book_pos+dir).id != -1) {
                                dir = bfs_direction_reversed(unit(wizard_id).pos, book_pos);
                            }
                        }
                    }
              move(wizard_id, dir);
            }
        }
        if (eliminado) {
            it = magos_sanos.erase(it);
        } else {
            ++it;
        }
    }
}

void manejar_libros_y_late(set<int>& magos_sanos) {
  for (int wizard_id : magos_sanos) {
    Pos target = bfs_mage(unit(wizard_id).pos);
    if (distance_mapa(unit(wizard_id).pos, target) > 5) bfs_control_map_mage(unit(wizard_id).pos);
    Dir dir = bfs_direction(unit(wizard_id).pos, target);
    if (cell(unit(wizard_id).pos+dir).id != -1) dir = bfs_direction_rodear(unit(wizard_id).pos, target);
    move(wizard_id, dir);
  }
}

//magos en conversion buscan ayuda
Pos wizard_socorro(Pos start, set<int>& magos_sanos) {
  queue<Pos> q;
    vector<vector<bool>> visited(board_rows(), vector<bool>(board_cols(), false));
    q.push(start);
    visited[start.i][start.j] = true;

    while (!q.empty()) {
      Pos current = q.front();
      q.pop();

      Cell c = cell(current);
      if ((c.id != -1 and (magos_sanos.find(unit(c.id).id)) != magos_sanos.end())) {
        return current;
      }

      for (Dir d : dirs) {
        Pos next = current + d;
        if (pos_ok(next) && !visited[next.i][next.j] && cell(next).type == Corridor) {
          visited[next.i][next.j] = true;
          q.push(next);
        }
      }
    }

    return Pos(-1, -1);
  }

  Pos wizard_socorro_suicide(Pos start) {
  queue<Pos> q;
    vector<vector<bool>> visited(board_rows(), vector<bool>(board_cols(), false));
    q.push(start);
    visited[start.i][start.j] = true;

    while (!q.empty()) {
      Pos current = q.front();
      q.pop();

      Cell c = cell(current);
      if ((current == pos_voldemort()) or (c.book)) {
        return current;
      }

      for (Dir d : normal_dirs) {
        Pos next = current + d;
        if (pos_ok(next) && !visited[next.i][next.j] && cell(next).type == Corridor) {
          visited[next.i][next.j] = true;
          q.push(next);
        }
      }
    }

    return Pos(-1, -1);
  }

//distancia
int distance_mapa(Pos a, Pos b) {
    if (a == b) return 0;

    queue<pair<Pos, int>> q;
    vector<vector<bool>> visited(board_rows(), vector<bool>(board_cols(), false));

    q.push({a, 0});
    visited[a.i][a.j] = true;

    while (!q.empty()) {
        pair<Pos, int> front = q.front();
        q.pop();

        Pos current = front.first;
        int dist = front.second;

        if (current == b) return dist;

        for (Dir d : dirs) {
            Pos next = current + d;

            if (pos_ok(next) and !visited[next.i][next.j] and cell(next).type == Corridor) {
                visited[next.i][next.j] = true;
                q.push({next, dist + 1});
            }
        }
    }
    return 0;
}

//fantasma busca celdas que no sean del clan
Pos bfs_control_map_ghost(Pos start) {
    queue<Pos> q;
    vector<vector<bool>> visited(board_rows(), vector<bool>(board_cols(), false));
    q.push(start);
    visited[start.i][start.j] = true;

    while (!q.empty()) {
        Pos current = q.front();
        q.pop();

        Cell c = cell(current);
        if ((c.type == Corridor and c.id == -1 and c.owner != me()) or c.book) return current;

        for (Dir d : all_dirs) {
            Pos next = current + d;
            if (pos_ok(next) && !visited[next.i][next.j] && cell(next).type == Corridor) {
                visited[next.i][next.j] = true;
                q.push(next);
            }
        }
    }

    return Pos(-1, -1);
    }

    Pos bfs_control_map_mage(Pos start) {
    queue<Pos> q;
    vector<vector<bool>> visited(board_rows(), vector<bool>(board_cols(), false));
    q.push(start);
    visited[start.i][start.j] = true;

    while (!q.empty()) {
        Pos current = q.front();
        q.pop();

        Cell c = cell(current);
        if (c.type == Corridor and c.id == -1 and c.owner != me()) return current;

        for (Dir d : dirs) {
            Pos next = current + d;
            if (pos_ok(next) && !visited[next.i][next.j] && cell(next).type == Corridor) {
                visited[next.i][next.j] = true;
                q.push(next);
            }
        }
    }

    return Pos(-1, -1);
    }


// mago busca entre el libro, mago enemigo sin convertir o mago aliado en conversion
  Pos bfs_target(Pos start) {
    queue<Pos> q;
    vector<vector<bool>> visited(board_rows(), vector<bool>(board_cols(), false));
    q.push(start);
    visited[start.i][start.j] = true;

    while (!q.empty()) {
      Pos current = q.front();
      q.pop();

      Cell c = cell(current);
      if (c.book or (c.id != -1 && unit(c.id).type == Wizard && unit(c.id).player != me() and !unit(c.id).is_in_conversion_process()) or (c.id != -1 and unit(c.id).type == Wizard and unit(c.id).player == me() and unit(c.id).is_in_conversion_process())) {
        return current;
      }

      for (Dir d : dirs) {
        Pos next = current + d;
        if (pos_ok(next) && !visited[next.i][next.j] && cell(next).type == Corridor) {
          visited[next.i][next.j] = true;
          q.push(next);
        }
      }
    }

    return Pos(-1, -1);
  }

   Pos bfs_mage_reversed(Pos start) {
    queue<Pos> q;
    vector<vector<bool>> visited(board_rows(), vector<bool>(board_cols(), false));
    q.push(start);
    visited[start.i][start.j] = true;

    while (!q.empty()) {
      Pos current = q.front();
      q.pop();

      Cell c = cell(current);
      if ((c.id != -1 && unit(c.id).type == Wizard && unit(c.id).player != me() and !unit(c.id).is_in_conversion_process()) or (c.id != -1 and unit(c.id).type == Wizard and unit(c.id).player == me() and unit(c.id).is_in_conversion_process())) {
        return current;
      }

      for (Dir d : normal_dirs) {
        Pos next = current + d;
        if (pos_ok(next) && !visited[next.i][next.j] && cell(next).type == Corridor) {
          visited[next.i][next.j] = true;
          q.push(next);
        }
      }
    }

    return Pos(-1, -1);
  }

  Pos bfs_late(Pos start) {
    queue<Pos> q;
    vector<vector<bool>> visited(board_rows(), vector<bool>(board_cols(), false));
    q.push(start);
    visited[start.i][start.j] = true;

    while (!q.empty()) {
      Pos current = q.front();
      q.pop();

      Cell c = cell(current);
      if ((c.type == Corridor and c.owner != me() and c.id == -1) or (c.id != -1 && unit(c.id).type == Wizard && unit(c.id).player != me() and !unit(c.id).is_in_conversion_process()) or (c.id != -1 and unit(c.id).type == Wizard and unit(c.id).player == me() and unit(c.id).is_in_conversion_process())) {
        return current;
      }

      for (Dir d : dirs) {
        Pos next = current + d;
        if (pos_ok(next) && !visited[next.i][next.j] && cell(next).type == Corridor) {
          visited[next.i][next.j] = true;
          q.push(next);
        }
      }
    }

    return Pos(-1, -1);
  }

//mago busca entre mago enemigo sin convertir o mago aliado en conversion
  Pos bfs_mage(Pos start) {
    queue<Pos> q;
    vector<vector<bool>> visited(board_rows(), vector<bool>(board_cols(), false));
    q.push(start);
    visited[start.i][start.j] = true;

    while (!q.empty()) {
      Pos current = q.front();
      q.pop();

      Cell c = cell(current);
      if ((c.id != -1 && unit(c.id).type == Wizard && unit(c.id).player != me() and !unit(c.id).is_in_conversion_process()) or (c.id != -1 and unit(c.id).type == Wizard and unit(c.id).player == me() and unit(c.id).is_in_conversion_process())) {
        return current;
      }

      for (Dir d : dirs) {
        Pos next = current + d;
        if (pos_ok(next) && !visited[next.i][next.j] && cell(next).type == Corridor) {
          visited[next.i][next.j] = true;
          q.push(next);
        }
      }
    }

    return Pos(-1, -1);
  }

  Dir bfs_direction_reversed(Pos start, Pos target) {
    if (start == target) return Invalid;

    queue<Pos> q;
    map<Pos, Pos> camino;
    vector<vector<bool>> visited(board_rows(), vector<bool>(board_cols(), false));

    q.push(start);
    visited[start.i][start.j] = true;

    while (!q.empty()) {
      Pos current = q.front();
      q.pop();

      if (current == target) break;

      for (Dir d : normal_dirs) {
        Pos next = current + d;
        if (pos_ok(next) && !visited[next.i][next.j] && cell(next).type == Corridor) {
          visited[next.i][next.j] = true;
          camino[next] = current;
          q.push(next);
        }
      }
    }


    if (camino.find(target) == camino.end()) return Invalid;

    Pos step = target;
    while (camino[step] != start) {
      step = camino[step];
    }


    for (Dir d : normal_dirs) {
      if (start + d == step) return d;
    }

    return Invalid;
  }

  //Fantasma busca libro mas cercano
  Pos bfs_find_book_ghost(Pos start) {
    queue<Pos> q;
    vector<vector<bool>> visited(board_rows(), vector<bool>(board_cols(), false));
    q.push(start);
    visited[start.i][start.j] = true;

    while (!q.empty()) {
        Pos current = q.front();
        q.pop();

        Cell c = cell(current);
        if (c.book) return current;

        for (Dir d : all_dirs) {
            Pos next = current + d;
            if (pos_ok(next) && !visited[next.i][next.j] && cell(next).type == Corridor) {
                visited[next.i][next.j] = true;
                q.push(next);
            }
        }
    }

    return Pos(-1, -1);
}

//magos buscan libro mas cercano
 Pos bfs_find_book_mage(Pos start) {
    queue<Pos> q;
    vector<vector<bool>> visited(board_rows(), vector<bool>(board_cols(), false));
    q.push(start);
    visited[start.i][start.j] = true;

    while (!q.empty()) {
        Pos current = q.front();
        q.pop();

        Cell c = cell(current);
        if (c.book) return current;

        for (Dir d : dirs) {
            Pos next = current + d;
            if (pos_ok(next) && !visited[next.i][next.j] && cell(next).type == Corridor) {
                visited[next.i][next.j] = true;
                q.push(next);
            }
        }
    }

    return Pos(-1, -1);
}

  //devuelve el primer paso del camino mas cercano de un mago entre start y target
  Dir bfs_direction(Pos start, Pos target) {
    if (start == target) return Invalid;

    queue<Pos> q;
    map<Pos, Pos> camino;
    vector<vector<bool>> visited(board_rows(), vector<bool>(board_cols(), false));

    q.push(start);
    visited[start.i][start.j] = true;

    while (!q.empty()) {
      Pos current = q.front();
      q.pop();

      if (current == target) break;

      for (Dir d : dirs) {
        Pos next = current + d;
        if (pos_ok(next) && !visited[next.i][next.j] && cell(next).type == Corridor) {
          visited[next.i][next.j] = true;
          camino[next] = current;
          q.push(next);
        }
      }
    }


    if (camino.find(target) == camino.end()) return Invalid;

    Pos step = target;
    while (camino[step] != start) {
      step = camino[step];
    }


    for (Dir d : dirs) {
      if (start + d == step) return d;
    }

    return Invalid;
  }

 //devuelve el primer paso del camino mas cercano de un fantasma entre start y target
Dir bfs_direction_ghost(Pos start, Pos target) {
    if (start == target) return Down;

    queue<Pos> q;
    map<Pos, Pos> camino;
    vector<vector<bool>> visited(board_rows(), vector<bool>(board_cols(), false));

    q.push(start);
    visited[start.i][start.j] = true;

    while (!q.empty()) {
        Pos current = q.front();
        q.pop();

        if (current == target) break;

        for (Dir d : all_dirs) {
            Pos next = current + d;
            if (pos_ok(next) && !visited[next.i][next.j] && cell(next).type == Corridor) {
                visited[next.i][next.j] = true;
                camino[next] = current;
                q.push(next);
            }
        }
    }


    if (camino.find(target) == camino.end()) return Down;

    Pos step = target;
    while (camino[step] != start) {
        step = camino[step];
    }

    // Determinar la dirección correspondiente al primer paso
    for (Dir d : all_dirs) {
        if (start + d == step) return d;
    }

    return Down;
}

Dir bfs_direction_rodear(Pos start, Pos target) {
    if (start == target) return Invalid;

    queue<Pos> q;
    map<Pos, Pos> camino;
    vector<vector<bool>> visited(board_rows(), vector<bool>(board_cols(), false));

    q.push(start);
    visited[start.i][start.j] = true;

    while (!q.empty()) {
        Pos current = q.front();
        q.pop();

        if (current == target) break;

        for (Dir d : dirs) {
            Pos next = current + d;


            if (pos_ok(next) and !visited[next.i][next.j] and
                (cell(next).id == -1 or next == target) and
                cell(next).type == Corridor) {
                visited[next.i][next.j] = true;
                camino[next] = current;
                q.push(next);
            }
        }
    }


    if (camino.find(target) == camino.end()) return Invalid;

    Pos step = target;
    while (camino[step] != start) {
        step = camino[step];
    }

    for (Dir d : dirs) {
        if (start + d == step) return d;
    }

    return Invalid;
}

Pos wizard_socorro_reversed(Pos start, set<int>& magos_sanos) {
  queue<Pos> q;
    vector<vector<bool>> visited(board_rows(), vector<bool>(board_cols(), false));
    q.push(start);
    visited[start.i][start.j] = true;

    while (!q.empty()) {
      Pos current = q.front();
      q.pop();

      Cell c = cell(current);
      if ((c.id != -1 and (magos_sanos.find(unit(c.id).id)) != magos_sanos.end())) {
        return current;
      }

      for (Dir d : normal_dirs) {
        Pos next = current + d;
        if (pos_ok(next) and !visited[next.i][next.j] and cell(next).type == Corridor) {
          visited[next.i][next.j] = true;
          q.push(next);
        }
      }
    }

    return Pos(-1, -1);
  }

void ghost_early_mid() {
  int ghost_id = ghost(me());
    Unit ghost_unit = unit(ghost_id);


    Pos book_pos = bfs_find_book_ghost(ghost_unit.pos);
    if (book_pos.i != -1 && book_pos.j != -1) {
      Dir dir = bfs_direction_ghost(ghost_unit.pos, book_pos);
      Pos next_pos = ghost_unit.pos + dir;
      if (cell(next_pos).id != -1) {
        next_pos = bfs_control_map_ghost(ghost_unit.pos);
        dir = bfs_direction_ghost(ghost_unit.pos, next_pos);
      }
      move(ghost_id, dir);
    }
}

void ghost_late() {
  int ghost_id = ghost(me());
  Unit ghost_unit = unit(ghost_id);

  Pos target = bfs_control_map_ghost(ghost_unit.pos);
  Dir dir = bfs_direction_ghost(ghost_unit.pos, target);
  move(ghost_id, dir);
}



  virtual void play() {
    vector<int> my_wizards = wizards(me());


    set<int> magos_sanos;
    set<int> magos_conversion;

    for (int wizard_id : my_wizards) {
        if (unit(wizard_id).is_in_conversion_process()) {
            magos_conversion.insert(wizard_id);
        } else {
            bool found = false;
            for (Dir d : dirs) {
                Pos next_posicion = unit(wizard_id).pos + d;
                if (cell(next_posicion).id != -1) {
                    if (unit(cell(next_posicion).id).type == Wizard and unit(cell(next_posicion).id).player == me() and unit(cell(next_posicion).id).is_in_conversion_process()) {
                        if (magos_conversion.find(unit(cell(next_posicion).id).id) == magos_conversion.end()) {
                            move(wizard_id, d);
                            magos_sanos.insert(unit(cell(next_posicion).id).id);
                            found = true;
                        }
                        else {
                            move(wizard_id, d);
                            magos_conversion.erase(unit(cell(next_posicion).id).id);
                            magos_sanos.insert(unit(cell(next_posicion).id).id);
                            found = true;
                        }
                    }
                    else if (unit(cell(next_posicion).id).type == Wizard and unit(cell(next_posicion).id).player != me() and !unit(cell(next_posicion).id).is_in_conversion_process()) {
                        move(wizard_id, d);
                        found = true;
                    }
                }
            }
            if (not found) magos_sanos.insert(wizard_id);
        }
    }


     for (int wizard_id : magos_conversion) {
        Pos target = wizard_socorro(unit(wizard_id).pos, magos_sanos);
        if (cell(target).id != -1 and distance_mapa(unit(wizard_id).pos, target) <= 2 * unit(wizard_id).rounds_for_converting()) {
            Dir dir = bfs_direction(unit(wizard_id).pos, target);
            if (cell(unit(wizard_id).pos+dir).id != -1) dir = bfs_direction_reversed(unit(wizard_id).pos, target);
            if (cell(unit(wizard_id).pos+dir).id != -1) {
              target = wizard_socorro_reversed(unit(wizard_id).pos, magos_sanos);
              dir = bfs_direction(unit(wizard_id).pos, target);
              if(cell(unit(wizard_id).pos + dir).id != -1) dir = bfs_direction_reversed(unit(wizard_id).pos, target);
              if(cell(unit(wizard_id).pos + dir).id != -1) dir = bfs_direction_rodear(unit(wizard_id).pos, target);
            }
            move(wizard_id, dir);
            Dir rescate = bfs_direction(target, unit(wizard_id).pos + dir);
            if (cell(target+rescate).id != -1 and (unit(cell(target+rescate).id).player != me() or magos_sanos.find(unit(cell(target+rescate).id).id) != magos_sanos.end())) rescate = bfs_direction_reversed(target, unit(wizard_id).pos);
            move(cell(target).id, rescate);
            magos_sanos.erase(unit(cell(target+rescate).id).id);
        }
        else {
            target = wizard_socorro_suicide(unit(wizard_id).pos);
            Dir dir = bfs_direction(unit(wizard_id).pos, target);
            move(wizard_id, dir);
        }
    }

    map<Pos, pair<int, int>> book_assignments;
    set<int> assigned_wizards;
    set<Pos> atacados;

     if (round() < 50) {
        ghost_early_mid();
        manejar_libros_y_magos_early(magos_sanos, book_assignments, assigned_wizards);
     }

    else if (round() < 180) {
        ghost_early_mid();
        manejar_libros_y_magos_midgame(magos_sanos, book_assignments, assigned_wizards, atacados);
    }
    else {
      ghost_late();
      manejar_libros_y_late(magos_sanos);
    }

     for (int wizard_id : assigned_wizards) {
        Pos target = bfs_find_book_mage(unit(wizard_id).pos);
        Dir dir = bfs_direction(unit(wizard_id).pos, target);
        move(wizard_id, dir);
    }
}

};

// Registrar el jugador
RegisterPlayer(PLAYER_NAME);
