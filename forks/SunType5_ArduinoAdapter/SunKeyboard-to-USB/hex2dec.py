#!/usr/bin/env python3

import argparse
import re

def dec_to_hex(match):
    num = int(match.group(0))
    return f"0x{num:02x}"

def hex_to_dec(match):
    num = int(match.group(0), 16)
    return str(num)

def replace_nth_occurrence(line, pattern, repl_func, occurrences):
    """
    Replace only the nth (or list of n) occurrence(s) in a line.
    If occurrences is 'all', replace all.
    If occurrences is an int, replace only that occurrence (1-based).
    If occurrences is a list, replace those occurrences.
    """
    matches = list(re.finditer(pattern, line))
    if occurrences == 'all':
        # Replace all
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
            # Adjust offset for next matches due to length change
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
        # Try as single int
        return int(arg)
    except ValueError:
        # Try as comma-separated list
        return [int(x) for x in arg.split(',')]

def main():
    parser = argparse.ArgumentParser(description="Replace decimals <-> hex in files, modularly.")
    parser.add_argument("filenames", nargs="+", help="Files to process")
    parser.add_argument("--direction", choices=['dec2hex', 'hex2dec'], required=True,
                        help="Choose conversion direction: dec2hex or hex2dec")
    parser.add_argument("--occurrence", default='all',
                        help="Which occurrence(s) to replace per line: all, a number (e.g. 2), or comma-separated list (e.g. 2,4)")
    args = parser.parse_args()

    occurrences = parse_occurrences(args.occurrence)
    for filename in args.filenames:
        process_file(filename, args.direction, occurrences)

if __name__ == "__main__":
    main()

