#!/usr/bin/env python3
# vim:ts=4:sw=4:sts=4:et:ai:si:fdm=marker:tw=80

# This is a simple shore calculator. It's not exact, since that's impossible due
# to the diffferences in measuring the shore scales, but then, it's a 

import argparse
import sys


# Shore 00 -> Shore A (approx. valid range: 10–90 Shore 00)"""
def shore00_to_A(val: float) -> float:
    return 0.66 * val - 6.0


# Shore A -> Shore 00 (approx. valid range: 0–50 Shore A)"""
def shoreA_to_00(val: float) -> float:
    return (val + 6.0) / 0.66


# Shore A -> Shore D (overlap zone ~50–95 Shore A)"""
def shoreA_to_D(val: float) -> float:
    return (val - 50.0) / 1.75


# Shore D -> Shore A (overlap zone ~10–45 Shore D)"""
def shoreD_to_A(val: float) -> float:
    return 1.75 * val + 50.0


# Parse input like '30D', '80A', '60'. Returns (value, scale)
def parse_value(token: str) -> tuple[float, str]:
    token = token.strip()

    if token[-1].isalpha():
        value = float(token[:-1])
        scale = token[-1]
    else:
        value = float(token)
        scale = "00"

    return value, scale

# Do the conversion
def convert(value: float, src: str, dst: str) -> float:
    #src = src.upper();
    #dst = dst.upper();

    if src == dst:
        return value

    if src == "00" and dst == "A":
        return shore00_to_A(value)

    if src == "A" and dst == "00":
        return shoreA_to_00(value)

    if src == "A" and dst == "D":
        return shoreA_to_D(value)

    if src == "D" and dst == "A":
        return shoreD_to_A(value)

    raise ValueError(f"Conversion {src} -> {dst} is not supported")

# The real shit
def main() -> int:
    parser = argparse.ArgumentParser(
        description="Approximate Shore hardness conversion (00, A, D)"
    )
    parser.add_argument(
        "input",
        help="Input hardness value, e.g. 30d, 80a, 60 (defaults to Shore 00)",
    )
    parser.add_argument(
        "target",
        choices=["00", "a", "A", "d", "D"],
        help="Target Shore scale",
    )
    parser.add_argument(
        "--verbose",
        action="store_true",
        help="Print conversion details",
    )

    args = parser.parse_args()

    try:
        value, src = parse_value(args.input.upper())
        dst = args.target.upper()

        result = convert(value, src, dst)

        if args.verbose:
            print(f"Input : {value:.2f} Shore {src.upper()}")
            print(f"Output: {result:.2f} Shore {dst.upper()}")
        else:
            print(f"{result:.2f}{dst}")

        return 0

    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        return 1

# Are we ran or simply included?
if __name__ == "__main__":
    sys.exit(main())

