#!/usr/bin/env python3

import argparse

def shore00_to_A(val: float) -> float:
    """
    Shore 00 -> Shore A
    Gyldig ca. 10–90 Shore 00
    """
    return 0.66 * val - 6.0


def shoreA_to_00(val: float) -> float:
    """
    Shore A -> Shore 00
    Gyldig ca. 0–50 Shore A
    """
    return (val + 6.0) / 0.66


def shoreA_to_D(val: float) -> float:
    """
    Shore A -> Shore D
    Kun overlappsone ca. 50–95 Shore A
    """
    return (val - 50.0) / 1.75


def shoreD_to_A(val: float) -> float:
    """
    Shore D -> Shore A
    Kun overlappsone ca. 10–45 Shore D
    """
    return 1.75 * val + 50.0

def main():
    print("wtf")

if __name__ == '__main__':
    sys.exit(main())
