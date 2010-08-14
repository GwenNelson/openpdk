""" Give it a UPF and it'll build stuff
    No, seriously - it will!
"""

import config
import parse_upf
import sys

parsed_upf = parse_upf.parse_upf_file(sys.argv[1])
