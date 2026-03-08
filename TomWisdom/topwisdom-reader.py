#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# topwisdom-reader.py

from __future__ import annotations
import sys
import argparse
from pathlib import Path
from typing import Optional, TextIO

# --------- Konfigurerbare standarder (kan overstyres med CLI-flagg) ---------
default_input_dir = Path("samples")     # <- relativ samples/
default_output_dir = None               # None => skriv .txt ved siden av .out (i input-dir)
default_xor_byte = 0x63                 # XOR-masken brukt ved lesing
# ---------------------------------------------------------------------------


class BitConverter:
    @staticmethod
    def to_number(a: int, b: int) -> float:
        """
        Port av C#-logikk:
            if (a == 0) return b;
            int v = (b - a);
            if (a > 64) return v - (0x7F * (0x7F - a));
            if (a >= 1) return a * 0x7F + v;
            return v;
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
    def to_float(cb: bytes) -> float:
        """
        5 byte, 7-bit «pakking» per byte, delt på 1000.
        C#:
            int value =
                  (cb[0] << 7 * 4)
                + (cb[1] << 7 * 3)
                + (cb[2] << 7 * 2)
                + (cb[3] << 7)
                + cb[4];
            return value / 1000f;
        """
        if len(cb) != 5:
            raise ValueError(f"to_float expects 5 bytes, got {len(cb)}")
        value = (
            (cb[0] << (7 * 4))
            + (cb[1] << (7 * 3))
            + (cb[2] << (7 * 2))
            + (cb[3] << 7)
            + cb[4]
        )
        return value / 1000.0

    @staticmethod
    def to_percentage(a: int, b: int) -> float:
        """
        C#:
            int l = (a * 0x7f) + b;
            int f = 0x7f << 7;
            float v = (l * 10000f) / f;
            var g = v + 0.5f;
            return ((int)g) / 100f;
        """
        l = (a * 0x7F) + b
        f = (0x7F << 7)
        v = (l * 10000.0) / f
        g = v + 0.5
        return int(g) / 100.0


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


class Reader:
    def __init__(
        self,
        base_path: Optional[Path] = None,
        out_dir: Optional[Path] = None,
        out_stream: Optional[TextIO] = None,
        xorbyte: int = default_xor_byte,
        echo_stdout: bool = False,
    ):

        self._offset: int = 0
        self._length: int = 0
        self._buffer: bytes = b""
        self._raw_out_stream: Optional[TextIO] = out_stream  # rå stream (kun ved enkel bruk)
        self._out_stream: Optional[TeeWriter] = None

        # konfig
        self.xorbyte = xorbyte
        self.echo_stdout = echo_stdout

        # standard: relativ "samples/"
        self.base_path = Path(base_path) if base_path else default_input_dir
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
    def read_percentage(self) -> float:
        a = self.next()
        b = self.next()
        return BitConverter.to_percentage(a, b)

    def print_float(self, name: str = "f") -> None:
        val = BitConverter.to_float(self.next_n(5))
        self._out_stream.write(f"{name}:{val:.2f}")

    # --- main logic ---
    def process_files(self) -> None:
        if not self.base_path.exists():
            print(f"Samples-katalog finnes ikke: {self.base_path}", file=sys.stderr)
            return

        files = sorted([p for p in self.base_path.glob("*.out") if p.is_file()])
        for in_file in files:
            out_file = (
                (self.out_dir / in_file.name).with_suffix(".txt")
                if self.out_dir
                else in_file.with_suffix(".txt")
            )
            out_file.parent.mkdir(parents=True, exist_ok=True)

            with out_file.open("w", encoding="utf-8", newline="") as f:
                # Tee til stdout hvis ønsket
                self._out_stream = TeeWriter(f, also_stdout=self.echo_stdout)
                # (hvis noen har satt _raw_out_stream manuelt — brukes ikke i denne flyten)
                self.read_file(in_file)
                self._out_stream.flush()
                self._out_stream = None

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
                # Filnavn-del (9 bytes UTF-8)
                s = self.next_n(9).decode("utf-8", errors="replace")
                self._out_stream.write(f" File name (or a part of it): {s}")
            else:
                a = self.next()
                b2 = self.next()
                c = self.next()
                d = self.next()
                self._out_stream.write(f" Payloadsize: {BitConverter.to_number(a, b2)}")
                payload_size = int(BitConverter.to_number(c, d))
                self._out_stream.write(f" + {payload_size}")

        elif b == 0xE3:
            b2 = self.next()
            if b2 == 0x01:
                self._out_stream.write(" ? ")
                self.next()
                self.next()
                self.print_float("?")
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
                # line position / x,y
                self.next_n(4)
                self.print_float(" x")
                self._out_stream.write(" ")
                self.print_float(" y")
            elif b2 == 0x05:
                self.next_n(10)
            elif b2 == 0x06:
                self.print_float()
            elif b2 == 0x07:
                self.print_float("x")
            elif b2 == 0x08:
                self.print_float()
            elif b2 == 0x09:
                self.print_float("y")
            elif b2 == 0x0A:
                self.next()
            elif b2 == 0x0B:
                self.next()
            elif b2 == 0x0C:
                self.print_float()
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
                self.print_float("cut speed")
            elif b2 == 0x04:
                self.print_float("free speed")
            else:
                self._out_stream.write(f"Unknown category {b2:02X}")
                return

        elif b == 0xC0:
            b2 = self.next()
            if b2 == 0x00:
                self.next_n(2)
            elif b2 == 0x01:
                self._out_stream.write(" Corner power1: ")
                self._out_stream.write(f"{self.read_percentage():.2f}%")
            elif b2 == 0x02:
                self._out_stream.write(" Work power1: ")
                self._out_stream.write(f"{self.read_percentage():.2f}%")
            elif b2 == 0x03:
                self._out_stream.write(" Work power2: ")
                self._out_stream.write(f"{self.read_percentage():.2f}%")
            elif b2 == 0x04:
                self._out_stream.write(f"{BitConverter.to_number(self.next(), self.next())}")
            elif b2 == 0x05:
                self.next_n(2)
            elif b2 == 0x06:
                self.next_n(2)
            elif b2 == 0x07:
                self.next_n(2)
            elif b2 == 0x08:
                self._out_stream.write(" Corner power2: ")
                self._out_stream.write(f"{self.read_percentage():.2f}%")
            elif b2 == 0x09:
                a3 = self.next()
                b3 = self.next()
                self._out_stream.write(f"{BitConverter.to_number(a3, b3)}")
            elif b2 == 0x10:
                self.print_float(" Point mode, delay")
            elif b2 == 0x11:
                self.print_float()
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
            # ellers som originalen

        elif b == 0xD0:
            # ingenting ekstra
            pass

        elif b == 0x80:
            self._out_stream.write(" Move to: ")
            self.print_float(" x")
            self._out_stream.write(" ")
            self.print_float(" y")

        elif b == 0x81:
            self._out_stream.write(" Carve? ")
            self.next_n(4)

        elif b == 0x82:
            self._out_stream.write(" Start laser? ")
            self.next_n(2)

        elif b == 0xA0:
            self._out_stream.write(" Line to: ")
            self.print_float(" x")
            self._out_stream.write(" ")
            self.print_float(" y")

        elif b == 0xA1:
            self._out_stream.write(" Short line to: ")
            a1 = self.next()
            b1 = self.next()
            c1 = self.next()
            d1 = self.next()
            self._out_stream.write(f"[{BitConverter.to_number(a1, b1)},{BitConverter.to_number(c1, d1)}],")

        elif b == 0xA2:
            self._out_stream.write(f" Horizontal line? {BitConverter.to_number(self.next(), self.next())}")

        elif b == 0xA3:
            self._out_stream.write(f" Vertical line? {BitConverter.to_number(self.next(), self.next())}")

        else:
            self._out_stream.write(f"Unknown category {b:02X}")
            return


def parse_args() -> argparse.Namespace:
    p = argparse.ArgumentParser(
        description="TopWisdom .out-filleser med C#-kompatibel tolkning. Skriver .txt-dump per fil."
    )
    p.add_argument(
        "-i", "--input",
        type=Path,
        default=default_input_dir,
        help="Inndatakatalog med .out-filer (default: ./samples)"
    )
    p.add_argument(
        "-o", "--output",
        type=Path,
        default=default_output_dir,
        help="Utkatalog for .txt (default: ved siden av .out i input-dir)"
    )
    p.add_argument(
        "--xor",
        type=lambda x: int(x, 0),
        default=default_xor_byte,
        help="XOR-byte i heks/dec (default: 0x63)"
    )
    p.add_argument(
        "--echo-stdout",
        action="store_true",
        help="Skriv også til stdout i tillegg til fil."
    )
    return p.parse_args()


def main() -> None:
    ns = parse_args()

    reader = Reader(
        base_path=ns.input,
        out_dir=ns.output,
        xorbyte=ns.xor,
        echo_stdout=ns.echo_stdout,
    )

    reader.process_files()
    print("Done")

