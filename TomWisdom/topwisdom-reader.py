#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
TopWisdom .out-filleser (port av C#-logikken).

- Leser en enkelt .out-fil eller alle .out i en katalog.
- XOR-dekoder med konfigurerbar maske (default 0x63).
- Tolker bytes til menneskelesbar tekst, og skriver .txt ved siden av .out,
  eller i valgfri output-katalog.
- Kan speile output til stdout (--echo-stdout).
- Skriver statuslinjer som standard + mer logging med --verbose.
- Overskriver ALDRI eksisterende filer som standard (no-clobber). Bruk --overwrite for å tillate overskriving.
- Norsk tallformat med komma, riktig fortegn og riktig avrunding (half-up).
- Fjerner NUL-terminatorer fra filnavn-felt (slik at ^@ ikke forurenser output).
"""

from __future__ import annotations

import sys
import argparse
from pathlib import Path
from typing import Optional, TextIO, List


# --------- Konfigurerbare standarder (kan overstyres med CLI-flagg) ---------
DEFAULT_INPUT_DIR = Path("samples")        # <- relativ samples/
DEFAULT_OUTPUT_DIR: Optional[Path] = None  # None => skriv .txt ved siden av .out (i input)
DEFAULT_XOR_BYTE = 0x63                    # XOR-masken brukt ved lesing
# ---------------------------------------------------------------------------


# -------------------- Formateringshjelpere (norsk) --------------------

def fmt_hundredths_to_str(hundredths: int) -> str:
    """
    Fra 2 desimalers heltallsrepresentasjon (hundredths) til norsk str med komma.
    Eksempel: 78 -> "0,78", 7301 -> "73,01", -78 -> "-0,78".
    """
    sign = "-" if hundredths < 0 else ""
    v = abs(hundredths)
    whole = v // 100
    frac = v % 100
    return f"{sign}{whole},{frac:02d}"


def fmt_milli_to_2dec(milli: int) -> str:
    """
    Fra tusendels-hele (milli) til 2 desimaler (half-up), norsk komma.
    Eksempel: 73005 -> "73,01".
    """
    sign = -1 if milli < 0 else 1
    v = abs(milli)
    hundredths = (v + 5) // 10
    return fmt_hundredths_to_str(hundredths * sign)


def fmt_int_if_whole(x: float) -> str:
    """
    Hvis x er (nesten) et helt tall, formater som heltall. Ellers norsk desimal med komma.
    Brukes for felter som i praksis forventes å være heltall (Payloadsize, to_number, A1 etc.).
    """
    xi = int(round(x))
    if abs(x - xi) < 1e-9:
        return str(xi)
    hundredths = int(round(x * 100))
    return fmt_hundredths_to_str(hundredths)


def safe_text_from_bytes(b: bytes, max_len: int | None = None) -> str:
    """
    Trygg tekst fra bytes: klipp ved første NUL, dekod som UTF-8 (replace), fjern kontrolltegn.
    """
    if max_len is not None:
        b = b[:max_len]
    b = b.split(b'\x00', 1)[0]
    s = b.decode("utf-8", errors="replace")
    return "".join(ch for ch in s if ch >= " " or ch == "\t")


# -------------------- Dekoding/konvertering --------------------

class BitConverter:
    @staticmethod
    def to_number(a: int, b: int) -> float:
        """
        Port av C#-logikk. Brukes for de 2-byte tallene (ofte heltall).
        """
        if a == 0:
            return float(b)
        v = b - a
        if a > 64:
            return float(v - (0x7F * (0x7F - a)))
        if a >= 1:
            return float(a * 0x7F + v)
        return float(v)

    @staticmethod
    def to_milli(cb: bytes) -> int:
        """
        Dekoder 5 bytes med 7-bit «pakking» til SIGNERT 35-bits heltall i tusendeler (milli).
        Kombiner 5 x 7-bit (MSB først). Hvis sign-bit (bit 34) er 1, trekk 1<<35 (to's complement).
        """
        if len(cb) != 5:
            raise ValueError(f"to_milli expects 5 bytes, got {len(cb)}")
        raw = (
            (cb[0] << (7 * 4))
            + (cb[1] << (7 * 3))
            + (cb[2] << (7 * 2))
            + (cb[3] << 7)
            + cb[4]
        )
        if cb[0] & 0x40:  # sign bit i høyeste 7-bit
            raw -= (1 << 35)
        return raw

    @staticmethod
    def to_percentage_hundredths(a: int, b: int) -> int:
        """
        Returner prosent i hundredths (to desimaler) avrundet half-up.
        Basert på C#-formelen i originalkoden.
        """
        l = (a * 0x7F) + b
        f = (0x7F << 7)  # 16256
        hundredths = (l * 10000 + (f // 2)) // f
        return int(hundredths)


class TeeWriter:
    """Skriver samtidig til fil (obligatorisk) og evt. stdout."""
    def __init__(self, file_stream: TextIO, also_stdout: bool = False):
        self.file_stream = file_stream
        self.also_stdout = also_stdout

    def write(self, s: str) -> None:
        self.file_stream.write(s)
        if self.also_stdout:
            sys.stdout.write(s)

    def flush(self) -> None:
        self.file_stream.flush()
        if self.also_stdout:
            sys.stdout.flush()


def is_out_file(p: Path) -> bool:
    return p.is_file() and p.suffix.lower() == ".out"


def list_out_files(path: Path) -> List[Path]:
    if path.is_file():
        return [path] if is_out_file(path) else []
    if path.is_dir():
        return sorted([p for p in path.glob("*.out") if p.is_file()])
    return []


def open_unique_text(path: Path):
    """
    Åpner en ny tekstfil i eksklusiv modus uten å overskrive eksisterende.
    Hvis 'path' finnes, prøver path.stem + .1 + suffix, .2, osv.
    Returnerer (file_obj, final_path).
    """
    try:
        f = path.open("x", encoding="utf-8", newline="")
        return f, path
    except FileExistsError:
        pass

    stem = path.stem
    suffix = path.suffix
    parent = path.parent
    i = 1
    while True:
        candidate = parent / f"{stem}.{i}{suffix}"
        try:
            f = candidate.open("x", encoding="utf-8", newline="")
            return f, candidate
        except FileExistsError:
            i += 1


class Reader:
    def __init__(
        self,
        base_path: Optional[Path] = None,
        out_dir: Optional[Path] = None,
        out_stream: Optional[TextIO] = None,
        xorbyte: int = DEFAULT_XOR_BYTE,
        echo_stdout: bool = False,
        overwrite: bool = False,
    ):
        self._offset: int = 0
        self._length: int = 0
        self._buffer: bytes = b""
        self._raw_out_stream: Optional[TextIO] = out_stream  # ikke brukt i vanlig flyt
        self._out_stream: Optional[TeeWriter] = None

        # konfig
        self.xorbyte = xorbyte
        self.echo_stdout = echo_stdout
        self.overwrite = overwrite

        # standard: relativ "samples/"
        self.base_path = Path(base_path) if base_path else DEFAULT_INPUT_DIR
        self.base_path = self.base_path.resolve()

        # output-dir (valgfritt). Hvis None: skriv .txt ved siden av .out
        self.out_dir = Path(out_dir).resolve() if out_dir else None

    # --- raw byte ops (XOR self.xorbyte) ---
    def peek(self, offset: int = 0) -> int:
        idx = self._offset + offset
        if idx < 0 or idx >= len(self._buffer):
            raise IndexError("peek out of range")
        return self._buffer[idx] ^ self.xorbyte

    def read(self) -> int:
        if self._offset >= len(self._buffer):
            raise IndexError("read out of range")
        b = self._buffer[self._offset] ^ self.xorbyte
        self._offset += 1
        return b

    def next(self) -> int:
        b = self.read()
        self._out_stream.write(f"{b:02X} ")
        return b

    def read_n(self, length: int) -> bytes:
        out = bytearray(length)
        for i in range(length):
            out[i] = self.read()
        return bytes(out)

    def next_n(self, length: int) -> bytes:
        out = bytearray(length)
        for i in range(length):
            out[i] = self.next()
        return bytes(out)

    # --- helpers matching C# ---
    def read_percentage_str(self) -> str:
        a = self.next()
        b = self.next()
        hundredths = BitConverter.to_percentage_hundredths(a, b)
        return fmt_hundredths_to_str(hundredths) + "%"

    def print_milli(self, name: str = "f") -> None:
        milli = BitConverter.to_milli(self.next_n(5))
        self._out_stream.write(f"{name}:{fmt_milli_to_2dec(milli)}")

    # --- main logic ---
    def process_path(self, in_path: Path, verbose: bool = False) -> int:
        """
        Prosesserer in_path:
        - Hvis fil: bare denne.
        - Hvis katalog: alle *.out i denne.
        Returnerer antall prosesserte filer.
        """
        files = list_out_files(in_path)
        if not files:
            print(f"Ingen .out-filer funnet i/for: {in_path}", file=sys.stderr)
            return 0

        # Kort statuslinje (alltid)
        where = in_path if in_path.is_dir() else in_path.parent
        print(f"Prosesserer {len(files)} fil(er) fra {where}")

        for in_file in files:
            desired_out = (
                (self.out_dir / in_file.name).with_suffix(".txt")
                if self.out_dir
                else in_file.with_suffix(".txt")
            )
            desired_out.parent.mkdir(parents=True, exist_ok=True)

            if self.overwrite:
                out_file = desired_out
                print(f"  {in_file} -> {out_file} (overskriver)")
                f = out_file.open("w", encoding="utf-8", newline="")
            else:
                f, out_file = open_unique_text(desired_out)
                if out_file == desired_out:
                    print(f"  {in_file} -> {out_file}")
                else:
                    print(f"  {in_file} -> {desired_out} finnes; skriver til: {out_file}")

            with f:
                self._out_stream = TeeWriter(f, also_stdout=self.echo_stdout)
                if verbose:
                    print(f"[debug] Leser bytes fra: {in_file}")
                self.read_file(in_file)
                self._out_stream.flush()
                self._out_stream = None

        return len(files)

    def read_file(self, file_path: Path) -> None:
        self._buffer = file_path.read_bytes()
        self._length = len(self._buffer)
        self._offset = 0
        self._out_stream.write(f"Got {self._length} bytes\n\n")

        while self._offset < len(self._buffer):
            self._out_stream.write(f"{self._offset}:\t")
            self.read_chunk()
            self._out_stream.write("\n")

    def read_chunk(self) -> None:
        try:
            b = self.next()
        except IndexError:
            return

        # Switch på første byte
        if b == 0x00:
            self.next_n(8)

        elif b == 0xE2:
            if self.next() == 0x01:
                # Filnavn-del (9 bytes UTF-8) – klipp ved NUL og rens kontrolltegn
                raw = self.next_n(9)
                s = safe_text_from_bytes(raw)
                self._out_stream.write(f" File name (or a part of it): {s}")
            else:
                a = self.next()
                b2 = self.next()
                c = self.next()
                d = self.next()
                n1 = BitConverter.to_number(a, b2)
                self._out_stream.write(f" Payloadsize: {fmt_int_if_whole(n1)}")
                payload_size = int(BitConverter.to_number(c, d))
                self._out_stream.write(f" + {payload_size}")

        elif b == 0xE3:
            b2 = self.next()
            if b2 == 0x01:
                self._out_stream.write(" ? ")
                self.next()
                self.next()
                self.print_milli("?")
            elif b2 == 0x02:
                self._out_stream.write(" <- END ")
                self.next_n(2)
            elif b2 == 0x03:
                self._out_stream.write(" <- START ")
                self.next_n(2)
            else:
                self._out_stream.write(f"Unknown category {b2:02X}")
                return

        elif b == 0xE0:
            b2 = self.next()
            if b2 == 0x00:
                pass
            elif b2 == 0x04:
                self.next_n(4)
                self.print_milli(" x")
                self._out_stream.write(" ")
                self.print_milli(" y")
            elif b2 == 0x05:
                self.next_n(10)
            elif b2 == 0x06:
                self.print_milli()
            elif b2 == 0x07:
                self.print_milli("x")
            elif b2 == 0x08:
                self.print_milli()
            elif b2 == 0x09:
                self.print_milli("y")
            elif b2 == 0x0A:
                self.next()
            elif b2 == 0x0B:
                self.next()
            elif b2 == 0x0C:
                self.print_milli()
            elif b2 == 0x0E:
                self.next()
            elif b2 == 0x11:
                self.next_n(8)
            elif b2 == 0x12:
                self.next_n(68)
            else:
                self._out_stream.write(f"Unknown category {b2:02X}")
                return

        elif b == 0xC5:
            b2 = self.next()
            if b2 == 0x00:
                self.next()
            elif b2 == 0x02:
                self.print_milli("cut speed")
            elif b2 == 0x04:
                self.print_milli("free speed")
            else:
                self._out_stream.write(f"Unknown category {b2:02X}")
                return

        elif b == 0xC0:
            b2 = self.next()
            if b2 == 0x00:
                self.next_n(2)
            elif b2 == 0x01:
                self._out_stream.write(" Corner power1: ")
                self._out_stream.write(self.read_percentage_str())
            elif b2 == 0x02:
                self._out_stream.write(" Work power1: ")
                self._out_stream.write(self.read_percentage_str())
            elif b2 == 0x03:
                self._out_stream.write(" Work power2: ")
                self._out_stream.write(self.read_percentage_str())
            elif b2 == 0x04:
                self._out_stream.write(fmt_int_if_whole(BitConverter.to_number(self.next(), self.next())))
            elif b2 == 0x05:
                self.next_n(2)
            elif b2 == 0x06:
                self.next_n(2)
            elif b2 == 0x07:
                self.next_n(2)
            elif b2 == 0x08:
                self._out_stream.write(" Corner power2: ")
                self._out_stream.write(self.read_percentage_str())
            elif b2 == 0x09:
                a3 = self.next()
                b3 = self.next()
                self._out_stream.write(fmt_int_if_whole(BitConverter.to_number(a3, b3)))
            elif b2 == 0x10:
                self._out_stream.write(" Point mode, delay")
                self.print_milli("")  # beholder leading space i label
            elif b2 == 0x11:
                self.print_milli()
            else:
                self._out_stream.write(f"Unknown category {b2:02X}")
                return

        elif b == 0xC1:
            self.next_n(2)

        elif b == 0xC2:
            self.next_n(2)

        elif b == 0xCD:
            b2 = self.next()
            if b2 == 0x00:
                self.next_n(2)
            elif b2 == 0x01:
                self.next()

        elif b == 0xD0:
            pass

        elif b == 0x80:
            self._out_stream.write(" Move to: ")
            self.print_milli(" x")
            self._out_stream.write(" ")
            self.print_milli(" y")

        elif b == 0x81:
            self._out_stream.write(" Carve? ")
            self.next_n(4)

        elif b == 0x82:
            self._out_stream.write(" Start laser? ")
            self.next_n(2)

        elif b == 0xA0:
            self._out_stream.write(" Line to: ")
            self.print_milli(" x")
            self._out_stream.write(" ")
            self.print_milli(" y")

        elif b == 0xA1:
            self._out_stream.write(" Short line to: ")
            a1 = BitConverter.to_number(self.next(), self.next())
            c1 = BitConverter.to_number(self.next(), self.next())
            self._out_stream.write(f"[{int(round(a1))},{int(round(c1))}],")

        elif b == 0xA2:
            self._out_stream.write(f" Horizontal line? {fmt_int_if_whole(BitConverter.to_number(self.next(), self.next()))}")

        elif b == 0xA3:
            self._out_stream.write(f" Vertical line? {fmt_int_if_whole(BitConverter.to_number(self.next(), self.next()))}")

        else:
            self._out_stream.write(f"Unknown category {b:02X}")
            return


def parse_args() -> argparse.Namespace:
    p = argparse.ArgumentParser(
        description="TopWisdom .out-filleser med C#-kompatibel tolkning. Skriver .txt-dump per fil."
    )
    # Posisjonelt: valgfritt INPUT som kan være fil ELLER katalog.
    # Hvis utelatt → default = ./samples
    p.add_argument(
        "input",
        nargs="?",
        default=str(DEFAULT_INPUT_DIR),
        help="Fil ELLER katalog. Hvis katalog: alle *.out i denne (default: ./samples)."
    )
    p.add_argument(
        "-o", "--output",
        type=Path,
        default=DEFAULT_OUTPUT_DIR,
        help="Utkatalog for .txt (default: ved siden av .out i input)"
    )
    p.add_argument(
        "--xor",
        type=lambda x: int(x, 0),
        default=DEFAULT_XOR_BYTE,
        help="XOR-byte i heks/dec (default: 0x63)"
    )
    p.add_argument(
        "--echo-stdout",
        action="store_true",
        help="Skriv også til stdout i tillegg til fil."
    )
    p.add_argument(
        "--overwrite",
        action="store_true",
        help="Tillat overskriving av eksisterende filer (default: AV)."
    )
    p.add_argument(
        "-v", "--verbose",
        action="store_true",
        help="Mer utskrift om hva som skjer."
    )
    return p.parse_args()


def main() -> None:
    ns = parse_args()

    in_path = Path(ns.input).resolve()
    if not in_path.exists():
        print(f"Feil: sti finnes ikke: {in_path}", file=sys.stderr)
        sys.exit(2)

    reader = Reader(
        base_path=in_path if in_path.is_dir() else in_path.parent,
        out_dir=ns.output,
        xorbyte=ns.xor,
        echo_stdout=ns.echo_stdout,
        overwrite=ns.overwrite,
    )

    count = reader.process_path(in_path, verbose=ns.verbose)
    if count == 0:
        if ns.verbose:
            print("Ingen filer prosessert.")
        sys.exit(2)

    print(f"Ferdig. Prosesserte {count} fil(er).")
    print("Done")


if __name__ == "__main__":
    main()

