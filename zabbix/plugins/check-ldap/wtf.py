#!/usr/bin/env python3

import re

txt='this is a paragraph with<[1> in between</[1> and then there are cases ... where the<[99> number ranges from 1-100</[99>.  and there are many other lines in the txt files with<[3> such tags </[3>'
xyz='this is a paragraph with in between and then there are cases ... where the number ranges from 1-100.  and there are many other lines in the txt files with such tags'

out = re.sub("(<[^>]+>)", '', txt)
print(out)
