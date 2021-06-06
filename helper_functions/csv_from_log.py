#!/usr/bin/env python
""" read log output from stdin and return csv log

e.g. used like this:

grazer problem_data.json | ./csv_from_log.py > grazer_log.csv
"""

import re
import sys


def parse_log_output(log_output):
    """ find csv table in lot output and return it """
    rx = r" === simulation start === (.*) === simulation end === "

    match = re.search(log_output, rx, flags=re.S) # re.S allows multiline capture
    return match.group(1)

def main():
    if not sys.stdin.isatty():
        print(parse_log_output(sys.stdin.readlines()))

if __name__ == "__main__":
    main()
