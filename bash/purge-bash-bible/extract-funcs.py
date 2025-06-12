#!/usr/bin/env python

filename = 'pure-bash-bible.md'

with open(filename, "r", encoding="utf-8") as f:
    lines = f.readlines()

i = 0
while i < len(lines):
    line = lines[i]
    if line.startswith(r'```shell$'):
        i += 1  # Skip the ```sh line
        if i < len(lines) and lines[i].strip() == "```":
            i += 1
    else:
        i += 1  # Move to the next line if not a ```


#   line = lines[i]
    if line.startswith(r'```sh$'):
        i += 1  # Skip the ```sh line
        # Print lines until a line with only ```
        while i < len(lines) and lines[i].strip() != "```":
            print(lines[i], end="")
            i += 1
        print()  # Print an empty line after the block
        # Skip the closing ```
        if i < len(lines) and lines[i].strip() == "```":
            i += 1
    else:
        i += 1  # Move to the next line if not a ```

