ifeq ($(OS), Windows_NT)
	ifdef MSYSTEM
		CXX = clang++

		CMAKE = cmake

		PRINT = echo
		RM = rm -rf
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
LIB_DIR = lib

CXXFLAGS = -Wall -Wextra -Werror -O3

SRC = $(shell find $(SRC_DIR) -name '*.cpp')
OBJ = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC))
DEP = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.d,$(SRC))

DEPFLAGS = -MMD -MP -MF "$(@:%.o=%.d)"

CXXFLAGS += $(DEPFLAGS)

CXXFLAGS += -I$(SRC_DIR)

GLFW_DIR = $(LIB_DIR)/glfw
GLFW_INC = $(GLFW_DIR)/include
GLFW_BIN = $(BIN_DIR)/glfw
GLFW_LIB = $(GLFW_BIN)/src/libglfw3.a

GLFW_FLAGS = -DGLFW_BUILD_EXAMPLES=OFF \
			 -DGLFW_BUILD_TESTS=OFF \
			 -DGLFW_BUILD_DOCS=OFF

CXXFLAGS += -I$(GLFW_INC)
LDFLAGS += $(GLFW_LIB) -lgdi32

GLM_DIR = $(LIB_DIR)/glm

CXXFLAGS += -I$(GLM_DIR)

VULKAN_INC = $(VULKAN_SDK)/Include
VULKAN_LIB = "$(VULKAN_SDK)/Lib/vulkan-1.lib"

CXXFLAGS += -I$(VULKAN_INC)
LDFLAGS += $(VULKAN_LIB)

VMA_DIR = $(LIB_DIR)/vma
VMA_INC = $(VMA_DIR)/include

CXXFLAGS += -I$(VMA_INC)

VOLK_DIR = $(LIB_DIR)/volk

VOLK_SRC = $(VOLK_DIR)/volk.c
VOLK_OBJ = $(BIN_DIR)/volk/volk.o

CXXFLAGS += -I$(VOLK_DIR)

GLSLC = $(VULKAN_SDK)/Bin/glslc

SHADERS_DIR = $(SRC_DIR)/shaders
SHADERS_SRC = $(shell find $(SHADERS_DIR) -name '*.vert' -o -name '*.frag')
SHADERS_BIN = assets/shaders
SHADERS_OBJ = $(patsubst $(SHADERS_DIR)/%.vert,$(SHADERS_BIN)/%.vert.spv,$(SHADERS_SRC))
SHADERS_OBJ += $(patsubst $(SHADERS_DIR)/%.frag,$(SHADERS_BIN)/%.frag.spv,$(SHADERS_SRC))

ifeq ($(OS), Windows_NT)
	EXE = main.exe
else
	EXE = main
endif

TARGET = $(BIN_DIR)/$(EXE)

all: $(GLFW_LIB) $(TARGET) $(SHADERS_OBJ)

$(TARGET): $(OBJ)
	@$(PRINT) "Linking $@"
	@$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@$(MKDIR) $(dir $@)
	@$(PRINT) "Compiling $< -> $@"
	@$(CXX) -c -o $@ $< $(CXXFLAGS)

$(VOLK_OBJ): $(VOLK_SRC)
	@$(MKDIR) $(dir $@)
	@$(PRINT) "Compiling $< -> $@"
	@$(CXX) -c -o $@ $< $(CXXFLAGS)

-include $(DEP)

$(SHADERS_BIN)/%.vert.spv: $(SHADERS_DIR)/%.vert
	@$(MKDIR) $(dir $@)
	@$(PRINT) "Compiling $< -> $@"
	@$(GLSLC) -fshader-stage=vertex -o $@ $<

$(SHADERS_BIN)/%.frag.spv: $(SHADERS_DIR)/%.frag
	@$(MKDIR) $(dir $@)
	@$(PRINT) "Compiling $< -> $@"
	@$(GLSLC) -fshader-stage=fragment -o $@ $<

glfw: $(GLFW_LIB)

$(GLFW_LIB):
	@$(PRINT) "Building GLFW"
	@$(MKDIR) $(GLFW_BIN)
	@$(CMAKE) -S $(GLFW_DIR) -B $(GLFW_BIN) $(GLFW_FLAGS)
	@$(CMAKE) --build $(GLFW_BIN)

clean-glfw:
	@$(PRINT) "Cleaning GLFW"
	@$(RM) $(GLFW_BIN)

clean:
	@$(PRINT) "Cleaning up"
	@$(RM) $(OBJ_DIR)

clean-all:
	@$(PRINT) "Cleaning all"
	@$(RM) $(BIN_DIR)

.PHONY: all clean
