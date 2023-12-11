CXX			:= g++
CXX_FLAGS	:= -std=c++17

BIN			:= bin
SRC			:= src
EXECUTABLE	:= main
CHILD 		:= child

build_static: $(BIN)/$(EXECUTABLE)_static
	@echo "Building..."

build_dynamic: $(BIN)/$(EXECUTABLE)_dynamic
	@echo "Building..."

static: clear build_static
	@echo "Executing..."
	$(BIN)/$(EXECUTABLE)_static

dynamic: clear build_dynamic
	@echo "Executing..."
	$(BIN)/$(EXECUTABLE)_dynamic

$(BIN):
	mkdir $@

lib:
	mkdir $@

# -L[относительный путь, где искать библиотеку]
# -l[имя библиотеки = основное имя, исключая lib и .so]
# -Xlinker [option=-rpath - где искать shared objects в рантайме]
$(BIN)/$(EXECUTABLE)_static: $(BIN) | libs_dynamic
	$(CXX) $(CXX_FLAGS) -o $@ $(SRC)/$(EXECUTABLE)_static.cpp -l2 -L./lib/dynamic -Xlinker -rpath=./lib/dynamic

$(BIN)/$(EXECUTABLE)_dynamic: $(BIN) | libs_dynamic
	$(CXX) $(CXX_FLAGS) -o $@ $(SRC)/$(EXECUTABLE)_dynamic.cpp

# -fpic = position independent code - надо для 64х версии
libs_dynamic: lib
	$(CXX) $(CXX_FLAGS) -fpic -c $(SRC)/lib1.cpp -o ./bin/lib1.o
	$(CXX) $(CXX_FLAGS) -fpic -c $(SRC)/lib2.cpp -o ./bin/lib2.o
	$(CXX) $(CXX_FLAGS) -shared ./bin/lib1.o -o ./lib/dynamic/lib1.so
	$(CXX) $(CXX_FLAGS) -shared ./bin/lib2.o -o ./lib/dynamic/lib2.so

clear:
	@echo "Clearing..."
	-rm -r $(BIN)