##
## EPITECH PROJECT, 2022
## asmminilibc
## File description:
## Makefile for asmminilibc tests
##

# Makefiles are prettier like this
ifeq ($(origin .RECIPEPREFIX), undefined)
  $(error This Make does not support .RECIPEPREFIX. \
	Please use GNU Make 3.82 or later)
endif
.RECIPEPREFIX = >

# Use bash as the shell
SHELL := bash

# ...And use strict flags with it to make sure things fail if a step in there
# fails
.SHELLFLAGS := -eu -o pipefail -c

# Delete the target file of a Make rule if it fails - this guards against
# broken files
.DELETE_ON_ERROR:

# --warn-undefined-variables: Referencing undefined variables is probably
# wrong...
# --no-builtin-rules: I'd rather make my own rules myself, make, thanks :)
MAKEFLAGS += --warn-undefined-variables --no-builtin-rules

# We use `override` to enable setting part of CFLAGS on the command line

# This makes the compiler generate dependency files, which will solve any
# header-related dependency problems we could have had
override CFLAGS += -MMD -MP -MF $@.d

# We're using Criterion and libm here
override CFLAGS += -lcriterion -lm

# LDFLAGS should contain CFLAGS (seperate so command-line can add to it, and
# to correspond to usual practice)
override LDFLAGS += $(CFLAGS)

.PHONY: all clean fclean re

.PREVIOUS: obj/%.o

BINARY_NAME := tests_binary

all: $(BINARY_NAME)

# Program sources files
SOURCE_FILES := string strlen strchr strrchr memset memcpy strcmp memmove

OBJECT_FILES := $(addprefix obj/criterion_tests/, $(addsuffix .o, $(SOURCE_FILES)))

$(BINARY_NAME): $(OBJECT_FILES)
> $(CC) $(LDFLAGS) -o $@ $(OBJECT_FILES)

obj/criterion_tests/%.o: criterion_tests/%.c
> @mkdir --parents obj/criterion_tests
> $(CC) -c $< -o $@ $(CFLAGS)

# Include dependencies for the object files
include $(shell [ -d obj ] && find obj/ -type f -name '*.d')

# Remove all object files
clean:
> rm --recursive --force obj

# Remove all object, binary and other produced files
fclean: clean
> rm --recursive --force $(BINARY_NAME)

# "Remakes" the project.
re:
> $(MAKE) clean
> $(MAKE) all
