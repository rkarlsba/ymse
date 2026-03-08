#!/usr/bin/env python3
# vim:ts=4:sw=4:sts=4:et:ai:si:fdm=marker

import sys
import argparse

CHUNK_SIZE = 1024 * 1024  # 1 MiB

def parse_key(s: str) -> int:
    """Parseer nøkkelen som desimal eller heks (f.eks. 99 eller 0x63)."""
    try:
        v = int(s, 0)  # base 0 = auto, tolker 0x.. som hex
    except ValueError as e:
        raise argparse.ArgumentTypeError(f"Ugyldig nøkkel: {s}") from e
    if not (0 <= v <= 0xFF):
        raise argparse.ArgumentTypeError("Nøkkel må være et heltall 0..255 (0x00..0xFF).")
    return v

def build_argparser() -> argparse.ArgumentParser:
    p = argparse.ArgumentParser(
        description="XOR-enkoder/-dekoder for byte-strømmer. Standard nøkkel = 0x63."
    )
    p.add_argument(
        "-i", "--input", metavar="FIL",
        help="Inndatafil (binær). Hvis utelatt, leses fra stdin."
    )
    p.add_argument(
        "-o", "--output", metavar="FIL",
        help="Utdatafil (binær). Hvis utelatt, skrives til stdout."
    )
    p.add_argument(
        "-k", "--key", type=parse_key, default=0x63,
        help="XOR-nøkkel som heltall (f.eks. 99) eller hex (f.eks. 0x63). Default: 0x63."
    )
    return p

def xor_stream(fin, fout, key: int):
    """XOR’er data fra fin til fout i CHUNK_SIZE-biter."""
    # For best ytelse, lag en 256-byte oversettelsestabell for translate()
    table = bytes([b ^ key for b in range(256)])
    while True:
        chunk = fin.read(CHUNK_SIZE)
        if not chunk:
            break
        # translate er raskere enn listeforståelse på store blokker
        fout.write(chunk.translate(table))

def main():
    parser = build_argparser()
    args = parser.parse_args()

    # Velg input- og output-strømmer
    fin = None
    fout = None
    try:
        fin = open(args.input, "rb") if args.input else sys.stdin.buffer
        fout = open(args.output, "wb") if args.output else sys.stdout.buffer

        # Hvis begge peker til samme filbane, gi en vennlig feilmelding
        if args.input and args.output and args.input == args.output:
            parser.error("Input og output kan ikke være samme fil.")

        xor_stream(fin, fout, args.key)
    finally:
        # Lukk kun hvis vi åpnet selv (ikke lukk stdin/stdout)
        if fin is not None and fin is not sys.stdin.buffer and not fin.closed:
            fin.close()
        if fout is not None and fout is not sys.stdout.buffer and not fout.closed:
            fout.close()

if __name__ == "__main__":
    main()

