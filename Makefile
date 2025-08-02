# compiler
CC := gcc

# compiler flags
# -fanalyzer - shows deep analysis of code
CFLAGS := -g -Wall -Werror -std=c99 -pedantic -O0 -fanalyzer

CPPFLAGS := -Ideps/SDL3/include -Iinclude
LDLIBS := -lvulkan 

# SDL3 paths (built locally)
SDL3_LIB := deps/SDL3/libSDL3.a

# SDL3 requires some system libraries on Linux
ifeq ($(shell uname -s),Linux)
    LDLIBS += -lm -ldl -lpthread
    # Add more system libs as needed: -lasound -lpulse -lX11 -lXext -lXrandr -lXi
endif

# macOS system libraries for SDL3 (unnecessary)
ifeq ($(shell uname -s),Darwin)
    LDLIBS += -framework Cocoa -framework IOKit -framework CoreVideo -framework Carbon
endif

# src directory
SRCD := src

# obj directory
OBJD := obj

# bin directory
BIND := bin

# collect all source files
SRCS := $(wildcard $(SRCD)/*.c)

# convert all source files to their obj file
OBJS := $(subst .c,.o,$(subst $(SRCD),$(OBJD),$(SRCS)))

# Shader compilation
SHADER_SRC_DIR := shader
SHADER_BIN_DIR := $(BIND)/shader

# Find all shader source files
VERT_SHADERS := $(wildcard $(SHADER_SRC_DIR)/*.vert)
FRAG_SHADERS := $(wildcard $(SHADER_SRC_DIR)/*.frag)
GEOM_SHADERS := $(wildcard $(SHADER_SRC_DIR)/*.geom)
COMP_SHADERS := $(wildcard $(SHADER_SRC_DIR)/*.comp)

# Convert to compiled shader paths
VERT_SPV := $(VERT_SHADERS:$(SHADER_SRC_DIR)/%.vert=$(SHADER_BIN_DIR)/%.vert.spv)
FRAG_SPV := $(FRAG_SHADERS:$(SHADER_SRC_DIR)/%.frag=$(SHADER_BIN_DIR)/%.frag.spv)
GEOM_SPV := $(GEOM_SHADERS:$(SHADER_SRC_DIR)/%.geom=$(SHADER_BIN_DIR)/%.geom.spv)
COMP_SPV := $(COMP_SHADERS:$(SHADER_SRC_DIR)/%.comp=$(SHADER_BIN_DIR)/%.comp.spv)

ALL_SHADERS := $(VERT_SPV) $(FRAG_SPV) $(GEOM_SPV) $(COMP_SPV)

# output is cgame.exe
OUTPUT := cgame.exe
OUTPUT := $(addprefix $(BIND)/,$(OUTPUT))

# all target - compile
all: $(OUTPUT)

# compile shader files
$(SHADER_BIN_DIR)/%.vert.spv: $(SHADER_SRC_DIR)/%.vert | $(SHADER_BIN_DIR)
	glslc $< -o $@

$(SHADER_BIN_DIR)/%.frag.spv: $(SHADER_SRC_DIR)/%.frag | $(SHADER_BIN_DIR)
	glslc $< -o $@

$(SHADER_BIN_DIR)/%.geom.spv: $(SHADER_SRC_DIR)/%.geom | $(SHADER_BIN_DIR)
	glslc $< -o $@

$(SHADER_BIN_DIR)/%.comp.spv: $(SHADER_SRC_DIR)/%.comp | $(SHADER_BIN_DIR)
	glslc $< -o $@

# Create shader output directory
$(SHADER_BIN_DIR):
	mkdir -p $(SHADER_BIN_DIR)

# create directories if they don't exist
$(OBJD):
	mkdir -p $(OBJD)

$(BIND):
	mkdir -p $(BIND)

# Build SDL3 if not already built
$(SDL3_LIB):
	@echo "SDL3 not found. Run cmake on the SDL dependency or install it as a git submodule."
	@exit 1

# link each object file into the executable using gcc
$(OUTPUT): $(OBJS) $(SDL3_LIB) $(ALL_SHADERS) | $(BIND)
	$(CC) $(OBJS) $(SDL3_LIB) $(LDLIBS) -o $@

# compile each src file and put into obj directory
$(OBJD)/%.o: $(SRCD)/%.c | $(OBJD)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

shaders: $(ALL_SHADERS)

# clean the project of binaries and object files
clean:
	-rm -rf $(BIND)/*
	-rm -rf $(OBJD)/*

# clean everything including dependencies
clean-all: clean
	-rm -rf deps/

.PHONY: all clean clean-all shaders