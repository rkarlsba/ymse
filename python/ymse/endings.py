#!/usr/bin/env python3
# vim:ts=4:sw=4:sts=4:et:ai:si:fdm=marker

import sys
import os
import argparse
import tempfile
import stat
import re
from typing import Optional

LINE_END = re.compile(br"\r\n|\r|\n")

def to_unix(data: bytes) -> bytes:
    # Normalize CRLF first, then lone CR
    return data.replace(b"\r\n", b"\n").replace(b"\r", b"\n")

def to_dos(data: bytes) -> bytes:
    # Replace any logical line end with CRLF
    return LINE_END.sub(b"\r\n", data)

def detect_mode_from_name(argv0: str) -> Optional[str]:
    """
    Return 'dos2unix' or 'unix2dos' based on the executable/script name.
    If it can't be determined, return None.
    """
    name = os.path.basename(argv0).lower()
    # Strip common extensions (e.g., .py, .exe) to make it friendlier
    for ext in (".py", ".exe", ".bat", ".cmd", ".sh"):
        if name.endswith(ext):
            name = name[: -len(ext)]
            break
    if "dos2unix" in name:
        return "dos2unix"
    if "unix2dos" in name:
        return "unix2dos"
    return None

def convert_stream(mode: str) -> None:
    data = sys.stdin.buffer.read()
    out = to_unix(data) if mode == "dos2unix" else to_dos(data)
    sys.stdout.buffer.write(out)

def convert_file_inplace(path: str, mode: str, keep_time: bool) -> None:
    st = os.stat(path)
    dir_name = os.path.dirname(path) or "."
    base = os.path.basename(path)
    fd, tmp_path = tempfile.mkstemp(prefix=f".{base}.", dir=dir_name)
    try:
        with os.fdopen(fd, "wb") as fout, open(path, "rb") as fin:
            data = fin.read()
            data = to_unix(data) if mode == "dos2unix" else to_dos(data)
            fout.write(data)

        # Preserve permissions
        os.chmod(tmp_path, stat.S_IMODE(st.st_mode))
        # Optionally preserve timestamps
        if keep_time:
            os.utime(tmp_path, (st.st_atime, st.st_mtime))

        # Atomic replace
        os.replace(tmp_path, path)
    finally:
        # Clean up temp file if something failed before replace
        try:
            if os.path.exists(tmp_path):
                os.remove(tmp_path)
        except Exception:
            pass

def build_parser(detected_mode: Optional[str]) -> argparse.ArgumentParser:
    epilog = (
        "Mode selection:\n"
        "  • If the script is named 'dos2unix*', it converts CRLF/CR → LF.\n"
        "  • If the script is named 'unix2dos*', it converts LF/CR → CRLF.\n"
        "  • If neither name matches, you must pass --mode."
    )
    parser = argparse.ArgumentParser(
        description="Line ending converter (name-aware: acts as dos2unix or unix2dos based on its filename).",
        epilog=epilog,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument(
        "files", nargs="*", help="Files to convert. If none given, read from stdin and write to stdout."
    )
    parser.add_argument(
        "-k", "--keep-time", action="store_true", help="Preserve timestamps when converting files."
    )
    parser.add_argument(
        "-m", "--mode",
        choices=["dos2unix", "unix2dos"],
        default=detected_mode,
        help=("Override mode. If not provided, mode is inferred from the script name. "
              "Required if the name doesn't include 'dos2unix' or 'unix2dos'.")
    )
    return parser

def main():
    detected = detect_mode_from_name(sys.argv[0])
    parser = build_parser(detected)
    args = parser.parse_args()

    if args.mode is None:
        parser.error("Cannot infer mode from script name. Use --mode dos2unix|unix2dos.")

    if not args.files:
        convert_stream(args.mode)
        return

    for path in args.files:
        convert_file_inplace(path, args.mode, keep_time=args.keep_time)

if __name__ == "__main__":
    main()

