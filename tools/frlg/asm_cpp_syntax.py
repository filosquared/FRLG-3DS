#!/usr/bin/env python3
"""Make decomp assembly safe to run through the C preprocessor.

The 3DS build (3ds_port/full.mk) feeds the game's data .s/.inc files through
cpp. GAS's `@` line comments survive cpp badly (an apostrophe in a comment
opens a character literal), so they become C block comments, the same
transformation patches/pokeemerald/0002 carries for Emerald, and the
`.4byte`/`.word`/`.2byte` pseudo-ops become `.int`/`.short`.

    python tools/frlg/asm_cpp_syntax.py FILE...     # rewrite in place
"""
from __future__ import annotations
import re
import sys


def convert_line(line: str) -> str:
    in_str = False
    i = 0
    while i < len(line):
        c = line[i]
        if c == '"' and (i == 0 or line[i - 1] != '\\'):
            in_str = not in_str
        elif c == '@' and not in_str and not (i > 0 and line[i - 1] == '\\'):
            body = line[i + 1:].rstrip('\n')
            nl = '\n' if line.endswith('\n') else ''
            body = body.replace('*/', '* /')
            return line[:i] + '/*' + body + '*/' + nl
        i += 1
    return line


_PSEUDO = re.compile(r'(^|\s)\.(4byte|word|2byte)(?=\s)')
_PSEUDO_TO = {'4byte': '.int', 'word': '.int', '2byte': '.short'}


def convert_ops(line: str) -> str:
    code, sep, comment = line.partition('/*')
    code = _PSEUDO.sub(lambda m: m.group(1) + _PSEUDO_TO[m.group(2)], code)
    return code + sep + comment


def convert_line_full(line: str) -> str:
    return convert_ops(convert_line(line))


def convert(text: str) -> str:
    return ''.join(convert_line_full(l) for l in text.splitlines(keepends=True))


def main(argv: list[str]) -> int:
    for path in argv:
        with open(path, encoding='utf-8', newline='') as f:
            old = f.read()
        new = convert(old)
        if new != old:
            with open(path, 'w', encoding='utf-8', newline='') as f:
                f.write(new)
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
