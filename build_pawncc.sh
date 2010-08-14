#!/bin/sh
cd pawn/source/compiler
cmake -G "Unix Makefiles"
make
cp pawncc ../../../bin
cd ../../../
