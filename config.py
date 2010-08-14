""" Configuration settings for the open PDK
"""
include_path = 'include/'

# use this format string with tuple (input_file,include_path,output_file)
pawn_cmd_line = 'bin/pawncc %s -V2048 -O2 -S64 -v2 -C- -i%s TARGET_PLEO=100 TARGET=100 -o%s'

