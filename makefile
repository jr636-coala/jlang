SRC = $(shell find src -name \*.cpp)
RT = $(shell find runtime -name \*.cpp)

OBJ = $(patsubst src/%.cpp,bin/%.o,$(SRC))
RTOBJ = $(patsubst runtime/%.cpp,bin/runtime/%.o,$(RT))

INC = -Iinclude
CPPFLAGS += -std=gnu++20 $(INC)

all: compiler

compiler: runtime lang
	@mkdir -p bin
	$(CXX) -o bin/jlang $(OBJ) $(RTOBJ)

runtime: $(RTOBJ)

lang: lang.tab.cc $(OBJ)

bin/%.o: src/%.cpp
	@mkdir -p bin
	$(CXX) $(CPPFLAGS) -c $< -o $@

bin/runtime/%.o: runtime/%.cpp
	@mkdir -p bin/runtime
	$(CXX) $(CPPFLAGS) -c $< -o $@

lang.tab.cc: lang.yy
	bison lang.yy

clean:
	rm -drf bin
	rm */*.o

run: all
	bin/jlang progs/sb.jl