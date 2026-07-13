//////// STUDENTS DO NOT NEED TO READ BELOW THIS LINE ////////

#include "Registry.hh"


typedef map<string, Registry::Factory> dict_;


namespace {
dict_& player_registry () {
  static dict_ registry;
  return registry;
}
}


int Registry::Register (const char* name, Factory factory) {
  player_registry()[name] = factory;
  return 999;
}


Player* Registry::new_player (string name) {
  auto& registry = player_registry();
  auto it = registry.find(name);
  _my_assert(it != registry.end(), "Player " + name + " not registered.");
  return (it->second)();
}


void Registry::print_players (ostream& os) {
  for (const auto& it : player_registry()) os << it.first << endl;
}
