#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# topwisdom-reader.py

from __future__ import annotations
import os
import sys
from pathlib import Path
from typing import List, Optional, BinaryIO, TextIO


class BitConverter:
    @staticmethod
    def to_number(a: int, b: int) -> float:
        """
        C#:
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
        C#:
            int value =
                  (cb[0] << 7 * 4)
                + (cb[1] << 7 * 3)
                + (cb[2] << 7 * 2)
                + (cb[3] << 7)
                + cb[4];
            return value / 1000f;
        5 bytes, 7-bit «pakking» per byte, delt på 1000.
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


class Reader:
    def __init__(self, base_path: Optional[Path] = None, out_stream: Optional[TextIO] = None):
        self._offset: int = 0
        self._length: int = 0
        self._buffer: bytes = b""
        self._out_stream: Optional[TextIO] = out_stream
        self.base_path = base_path if base_path is not None else Path(__file__).parent / "../../../Samples/"
        # Normaliser stier
        self.base_path = self.base_path.resolve()

    # --- raw byte ops (XOR 0x63) ---
    def peek(self, offset: int = 0) -> int:
        idx = self._offset + offset
        if idx < 0 or idx >= len(self._buffer):
            raise IndexError("peek out of range")
        return self._buffer[idx] ^ 0x63

    def read(self) -> int:
        if self._offset >= len(self._buffer):
            raise IndexError("read out of range")
        b = self._buffer[self._offset] ^ 0x63
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
        # Lik C#: Directory.GetFiles("../../../Samples/", "*.out")
        if not self.base_path.exists():
            print(f"Samples-katalog finnes ikke: {self.base_path}", file=sys.stderr)
            return

        files = sorted([p for p in self.base_path.glob("*.out") if p.is_file()])
        for in_file in files:
            out_file = in_file.with_suffix(".txt")
            with out_file.open("w", encoding="utf-8", newline="") as f:
                self._out_stream = f
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
            # ellers gjør vi som originalen (ingen default/return)

        elif b == 0xD0:
            # ingen ekstra lesing i originalen
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
            # «Horizontal line?»
            self._out_stream.write(f" Horizontal line? {BitConverter.to_number(self.next(), self.next())}")

        elif b == 0xA3:
            # «Vertical line?»
            self._out_stream.write(f" Vertical line? {BitConverter.to_number(self.next(), self.next())}")

        else:
            self._out_stream.write(f"Unknown category {b:02X}")
            return


def main() -> None:
    # Standard oppførsel: samme som C# - bruk ../../../Samples/
    base = Path(__file__).parent / "../../../Samples/"
    reader = Reader(base_path=base)
    reader.process_files()
    print("Done")


if __name__ == "__main__":
    main()
