""" For finishing off, if some files have comments at the end of a line (NOT inside a function prototype), use this
"""
import sys
for line in sys.stdin:
    line = line.strip()
    if '//' in line:
       print line.split('//')[0]
    elif '/*' in line:
       print line.split('/*')[0]
    else:
       print line
