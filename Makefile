# compiler
CC := gcc

# compiler flags
# -fanalyzer - shows deep analysis of code
CFLAGS := -g -Wall -std=c99 -pedantic -O0

# include dirs
CPPFLAGS := -Iinclude

# SDL3 paths (built locally)
SDL3_INCLUDE := deps/SDL3/include
SDL3_LIB := deps/SDL3/libSDL3.a

# Include SDL3 headers
CPPFLAGS += -I$(SDL3_INCLUDE)

# Vulkan (system installation via Vulkan SDK)
LDLIBS := -lvulkan

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

# output is cgame.exe
OUTPUT := cgame.exe
OUTPUT := $(addprefix $(BIND)/,$(OUTPUT))

# all target - compile
all: $(OUTPUT)

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
$(OUTPUT): $(OBJS) $(SDL3_LIB) | $(BIND)
	$(CC) $(OBJS) $(SDL3_LIB) $(LDLIBS) -o $@

# compile each src file and put into obj directory
$(OBJD)/%.o: $(SRCD)/%.c | $(OBJD)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

# clean the project of binaries and object files
clean:
	-rm -rf $(BIND)/*
	-rm -rf $(OBJD)/*

# clean everything including dependencies
clean-all: clean
	-rm -rf deps/

.PHONY: all clean clean-all