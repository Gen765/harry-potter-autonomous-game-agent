# Build configuration for the Harry Potter game engine + players.
# Everything is compiled and linked with a single, unified C++17 standard.

CXX      ?= g++
CXXSTD   := -std=c++17
WARN     := -Wall -Wextra -Wno-unused-variable -Wno-unused-parameter
OPTIMIZE ?= 2
CXXFLAGS ?= $(CXXSTD) $(WARN) -fPIC -O$(OPTIMIZE)
LDFLAGS  ?= $(CXXSTD)

# Match parameters (overridable on the command line).
CONFIG   ?= config/default.cnf
SEED     ?= 1
OUTPUT   ?= output/match-seed-$(SEED).res
PLAYERS  ?= Gen765 Basic_player Demo Null

BIN         := Game
ENGINE_OBJ  := Structs.o Settings.o State.o Info.o Random.o Board.o Action.o Player.o Registry.o Utils.o Game.o Main.o

# Players are detected automatically from the AI*.cc pattern.
# No external student objects and no precompiled Dummy are linked.
PLAYERS_SRC := $(wildcard AI*.cc)
PLAYERS_OBJ := $(patsubst %.cc,%.o,$(PLAYERS_SRC))

.PHONY: all clean list smoke-test regression-test sanitize

all: $(BIN)

$(BIN): $(ENGINE_OBJ) $(PLAYERS_OBJ)
	$(CXX) $^ -o $@ $(LDFLAGS)

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

# List the registered players (should print exactly four).
list: $(BIN)
	./$(BIN) --list

# Run one full match with the four bundled players.
smoke-test: $(BIN)
	@mkdir -p output
	./$(BIN) --seed=$(SEED) --input=$(CONFIG) --output=$(OUTPUT) $(PLAYERS)
	@echo "Smoke test finished -> $(OUTPUT)"

# Run the automated Python regression suite.
regression-test: $(BIN)
	python3 -m pytest -q tests

# Diagnostic build with AddressSanitizer + UndefinedBehaviorSanitizer.
sanitize: clean
	$(MAKE) all \
	  CXXFLAGS="$(CXXSTD) $(WARN) -fsanitize=address,undefined -fno-omit-frame-pointer -g -O1" \
	  LDFLAGS="$(CXXSTD) -fsanitize=address,undefined"

# Auto-generated header dependencies (never committed; see .gitignore).
Makefile.deps: $(wildcard *.cc)
	$(CXX) $(CXXFLAGS) -MM $(wildcard *.cc) > Makefile.deps

clean:
	rm -f $(BIN) $(BIN).exe *.o Makefile.deps

-include Makefile.deps
