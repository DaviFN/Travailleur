# Detect OS
ifeq ($(OS),Windows_NT)
    IS_WINDOWS := 1
else
    IS_WINDOWS := 0
endif

# Compiler
CXX = g++
CC = gcc

# File extensions and commands
ifeq ($(IS_WINDOWS),1)
    EXE_EXT = .exe
    RM = del /Q
    RMDIR = rmdir /S /Q
define MKDIR
    if not exist "$(subst /,\\,$1)" mkdir "$(subst /,\\,$1)"
endef
    PATHSEP = \\
    PYTHON = python
    MSG = echo
    VCPKG_TOOLCHAIN = C:/devtools/vcpkg/scripts/buildsystems/vcpkg.cmake
    CMAKE_GENERATOR = "Visual Studio 17 2022"
else
    EXE_EXT =
    RM = rm -f
    RMDIR = rm -rf
define MKDIR
    mkdir -p $1
endef
    PATHSEP = /
    PYTHON = python3
    MSG = echo
    VCPKG_TOOLCHAIN = $(HOME)/vcpkg/scripts/buildsystems/vcpkg.cmake
    CMAKE_GENERATOR = "Unix Makefiles"
endif

PROGRAM_NAME = travailleur$(EXE_EXT)
BINARY_DIR_PATH = bin
BUILDFILES_DIR_PATH = buildfiles

DEBUG_BUILD_PATH = $(BUILDFILES_DIR_PATH)$(PATHSEP)debug
DEBUG_PROGRAM_PATH = $(BINARY_DIR_PATH)$(PATHSEP)debug
DEBUG_PROGRAM_FILEPATH = $(DEBUG_PROGRAM_PATH)$(PATHSEP)$(PROGRAM_NAME)

RELEASE_BUILD_PATH = $(BUILDFILES_DIR_PATH)$(PATHSEP)relwithdebinfo
RELEASE_PROGRAM_PATH = $(BINARY_DIR_PATH)$(PATHSEP)relwithdebinfo
RELEASE_PROGRAM_FILEPATH = $(RELEASE_PROGRAM_PATH)$(PATHSEP)$(PROGRAM_NAME)

SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:src/%.cpp=$(DEBUG_BUILD_PATH)/%.o)

CXXFLAGS = -std=c++17 -O2 -Wall

.PHONY: clean run gen rel open openrel cmake_gen

# --- Python-based CMake generation (default) ---
gen:
	@$(PYTHON) build.py

rel:
	@$(PYTHON) build_release.py

open:
ifeq ($(IS_WINDOWS),1)
	cd $(DEBUG_BUILD_PATH) && start travailleur.sln
else
	@$(MSG) "No Visual Studio on Linux. Open your editor manually."
endif

openrel:
ifeq ($(IS_WINDOWS),1)
	cd $(RELEASE_BUILD_PATH) && start travailleur.sln
else
	@$(MSG) "No Visual Studio on Linux. Open your editor manually."
endif

run: $(DEBUG_PROGRAM_FILEPATH)
	@$(MSG) "Executing $(PROGRAM_NAME)..."
ifeq ($(IS_WINDOWS),1)
	@$(DEBUG_PROGRAM_FILEPATH)
else
	@./$(DEBUG_PROGRAM_FILEPATH)
endif
	@$(MSG) "$(PROGRAM_NAME) executed"

clean:
	@$(MSG) "Cleaning..."
ifeq ($(IS_WINDOWS),1)
	@if exist $(BINARY_DIR_PATH) $(RMDIR) $(BINARY_DIR_PATH)
	@if exist $(BUILDFILES_DIR_PATH) $(RMDIR) $(BUILDFILES_DIR_PATH)
else
	@$(RMDIR) $(BINARY_DIR_PATH)
	@$(RMDIR) $(BUILDFILES_DIR_PATH)
endif
	@$(MSG) "Cleaned."