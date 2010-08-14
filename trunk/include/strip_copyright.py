""" Simple tool for stripping copyrightable elements from include files
    Basically, all comments include copyrighted prose, so have to go
    It is wise to double-check the output
"""
import sys
for line in sys.stdin:
    line = line.strip()
    if line.startswith('/*') or line.startswith('*/') or line.startswith(' *') or line.startswith('*') or line.startswith('//'):
       pass
    else:
       print line
