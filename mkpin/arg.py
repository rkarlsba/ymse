#!/usr/bin/env python3

import sys

debug = 1
argv = sys.argv[1:]

def debprint(x):
    if (debug):
        print(x) 

debprint(f"argc == {len(sys.argv)}");
