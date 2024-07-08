CXX = "g++"

CC = "gcc"

.PHONY: clean run gen

RM = del

RMDIR = rd /q /s

MSG = echo

PROGRAM_NAME = travailleur.exe

BINARY_DIR_PATH = bin

BUILDFILES_DIR_PATH = buildfiles

DEBUG_BUILD_PATH = $(BUILDFILES_DIR_PATH)/debug

RELEASE_SOLUTION_FILEPATH = $(DEBUG_BUILD_PATH)/travailleur.sln

DEBUG_PROGRAM_PATH = $(BINARY_DIR_PATH)/debug

DEBUG_PROGRAM_FILEPATH = $(DEBUG_PROGRAM_PATH)/$(PROGRAM_NAME)

RELEASE_BUILD_PATH = $(BUILDFILES_DIR_PATH)/relwithdebinfo

RELEASE_SOLUTION_FILEPATH = $(RELEASE_BUILD_PATH)/travailleur.sln

RELEASE_PROGRAM_PATH = $(BINARY_DIR_PATH)/relwithdebinfo

RELEASE_PROGRAM_FILEPATH = $(RELEASE_PROGRAM_PATH)/$(PROGRAM_NAME)

gen:
	@python build.py

rel:
	@python build_release.py

open:
	cd $(DEBUG_BUILD_PATH) && devenv travailleur.sln

openrel:
	cd $(RELEASE_BUILD_PATH) && devenv travailleur.sln

run:
	@$(MSG) ATTENTION: BROKEN
	@python build.py
	@$(MSG) executing $(PROGRAM_NAME)...
	@$(PROGRAM_FILEPATH)
	@$(MSG) $(PROGRAM_NAME) executed
	
clean:
	@$(MSG) cleaning...
	@if exist $(BINARY_DIR_PATH) $(RMDIR) $(BINARY_DIR_PATH)
	@if exist $(BUILDFILES_DIR_PATH) $(RMDIR) $(BUILDFILES_DIR_PATH)
	@$(MSG) cleaned