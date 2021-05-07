#!/bin/sh
gcc -o checkers checkers.c interface.c movement.c game_state.c util.c language.c checkers.h -lncurses
