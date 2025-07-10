#!/usr/bin/env python3

import argparse
import re

def dec_to_hex(match):
    num = int(match.group(0))
    # Pad to two digits, add 0x prefix
    return f"0x{num:02x}"

def replace_decimals_in_file(filename):
    with open(filename, "r", encoding="utf-8") as f:
        content = f.read()
    # Replace all standalone decimal numbers
    new_content = re.sub(r'\b\d+\b', dec_to_hex, content)
    with open(filename, "w", encoding="utf-8") as f:
        f.write(new_content)


def main():
    parser = argparse.ArgumentParser(description="Replace decimals with hex in files.")
    parser.add_argument("filenames", nargs="+", help="Files in which to replace decimals with hex")
    args = parser.parse_args()

    for filename in args.filenames:
        replace_decimals_in_file(filename)

if __name__ == "__main__":
    main()

