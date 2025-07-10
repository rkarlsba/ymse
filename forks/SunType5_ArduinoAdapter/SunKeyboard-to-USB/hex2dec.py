#!/usr/bin/env python3

import argparse
import re
import os
import sys

def dec_to_hex(match):
    num = int(match.group(0))
    return f"0x{num:02x}"

def hex_to_dec(match):
    num = int(match.group(0), 16)
    return str(num)

def replace_nth_occurrence(line, pattern, repl_func, occurrences):
    matches = list(re.finditer(pattern, line))
    if occurrences == 'all':
        return re.sub(pattern, repl_func, line)
    elif isinstance(occurrences, int):
        occurrences = [occurrences]
    new_line = line
    offset = 0
    for idx, match in enumerate(matches, start=1):
        if idx in occurrences:
            start, end = match.start() + offset, match.end() + offset
            replacement = repl_func(match)
            new_line = new_line[:start] + replacement + new_line[end:]
            offset += len(replacement) - (end - start)
    return new_line

def process_file(filename, direction, occurrences):
    with open(filename, "r", encoding="utf-8") as f:
        lines = f.readlines()
    new_lines = []
    if direction == 'dec2hex':
        pattern = r'\b\d+\b'
        repl_func = dec_to_hex
    elif direction == 'hex2dec':
        pattern = r'\b0x[0-9a-fA-F]+\b'
        repl_func = hex_to_dec
    else:
        raise ValueError("Unknown direction")
    for line in lines:
        new_line = replace_nth_occurrence(line, pattern, repl_func, occurrences)
        new_lines.append(new_line)
    with open(filename, "w", encoding="utf-8") as f:
        f.writelines(new_lines)

def parse_occurrences(arg):
    if arg == 'all':
        return 'all'
    try:
        return int(arg)
    except ValueError:
        return [int(x) for x in arg.split(',')]

def detect_direction_from_invocation():
    prog = os.path.basename(sys.argv[0]).lower()
    if 'dec2hex' in prog:
        return 'dec2hex'
    elif 'hex2dec' in prog:
        return 'hex2dec'
    else:
        # Default or error
        print("Could not determine direction from script name. Please use --direction.")
        sys.exit(1)

def main():
    parser = argparse.ArgumentParser(
        description="Replace decimals <-> hex in files, modularly. "
                    "Direction is determined by script name (hex2dec or dec2hex), "
                    "but can be overridden."
    )
    parser.add_argument("filenames", nargs="+", help="Files to process")
    parser.add_argument("--direction", choices=['dec2hex', 'hex2dec'], required=False,
                        help="Override conversion direction: dec2hex or hex2dec")
    parser.add_argument("--occurrence", default='all',
                        help="Which occurrence(s) to replace per line: all, a number (e.g. 2), or comma-separated list (e.g. 2,4)")
    args = parser.parse_args()

    direction = args.direction or detect_direction_from_invocation()
    occurrences = parse_occurrences(args.occurrence)
    for filename in args.filenames:
        process_file(filename, direction, occurrences)

if __name__ == "__main__":
    main()

