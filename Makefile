ifeq ($(OS), Windows_NT)
	ifdef MSYSTEM
		CXX = clang++

		PRINT = echo
		RM = rm -f
		MKDIR = mkdir -p
		TOUCH = touch
		CP = cp -f
	else
		$(error "This Makefile support only MSYS2/MinGW64 environment")
	endif
else
	$(info "This OS is not supported")
endif

SRC_DIR = src
BIN_DIR = bin
OBJ_DIR = $(BIN_DIR)/src

CXXFLAGS = -Wall -Wextra -Werror -O3

SRC = $(shell find $(SRC_DIR) -name '*.cpp')
OBJ = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC))
DEP = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.d,$(SRC))

ifeq ($(OS), Windows_NT)
	EXE = main.exe
else
	EXE = main
endif

TARGET = $(EXE)

all: $(TARGET)

$(TARGET): $(OBJ)
	@$(PRINT) "Linking $@"
	@$(CXX) -o $@ $^ $(CXXFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@$(MKDIR) $(dir $@)
	@$(PRINT) "Compiling $<"
	@$(CXX) -c -o $@ $< $(CXXFLAGS)

clean:
	@$(PRINT) "Cleaning up"
	@$(RM) $(OBJ_DIR)/*.o $(OBJ_DIR)/*.d $(TARGET)

.PHONY: all clean
