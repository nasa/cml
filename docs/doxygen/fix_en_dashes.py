#!/usr/bin/env python3
"""
fix_en_dashes.py

When Doxygen parses Trick units, it converts double dashes (--) into an en-dash.
We want the literal "--" to show up in our documentation for units, so this
script finds em-dashes in the generated Doxygen XML and converts them back into
"--".

Usage: fix_en_dashes.py <path-to-xml-output-dir>
"""

import sys
from pathlib import Path


def process_file(path: Path) -> bool:
    """Convert em-dashes back into -- in a single Doxygen XML file.

    Replaces every occurrence of <ndash/> with a literal "--" and writes the
    changes back to the original file.

    Args:
        path: Path to a single Doxygen-generated XML file to process in place.
    """
    text = path.read_text(encoding="utf-8")
    new_text = text.replace("<ndash/>", "--")

    if new_text != text:
        path.write_text(new_text, encoding="utf-8")


def main():
    """Convert en-dashes back to "--" in Doxygen XML files."""
    if len(sys.argv) != 2:
        sys.exit("Usage: fix_en_dashes.py <xml-output-dir>")

    xml_dir = Path(sys.argv[1])
    if not xml_dir.is_dir():
        sys.exit(f"Error: XML output directory '{xml_dir}' not found.")

    xml_files = list(xml_dir.glob("*.xml"))
    if not xml_files:
        print(f"Warning: no .xml files found under '{xml_dir}'", file=sys.stderr)

    for filename in xml_files:
        process_file(filename)


if __name__ == "__main__":
    main()
