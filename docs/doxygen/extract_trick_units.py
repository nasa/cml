#!/usr/bin/env python3
"""
extract_trick_units.py

Extracts Trick-style units annotations ("(kg)" or "trick_units(kg)") from the
start of Doxygen member descriptions and re-emits them as a standard Doxygen
<xrefsect> block. Doxygen already uses that for @todo/@bug/@deprecated, which
Breathe already renders out of the box.

Usage: extract_trick_units.py <xml-output-dir>
"""

import re
import sys
import xml.etree.ElementTree as ET
from pathlib import Path

UNITS_RE = re.compile(r"^\s*(?:trick_units)?\(([^()]*)\)\s*(.*)$", re.DOTALL)
XREF_TITLE = "Units"


def make_xrefsect(unit_id: str, units_value: str) -> ET.Element:
    """Build a Doxygen-style <xrefsect> element containing a units value.

    Constructs the same XML shape Doxygen itself generates for built-in
    cross-reference commands like @todo, @bug, and @deprecated, so that Breathe
    (which already knows how to render <xrefsect> blocks) can display the
    units without requiring any custom Breathe configuration or patching.

    Args:
        unit_id: A unique string used to build the element's "id" attribute.
        units_value: The extracted unit string to display.

    Returns:
        An <xrefsect> Element with the structure:
            <xrefsect id="trickunits_{unit_id}">
              <xreftitle>Units</xreftitle>
              <xrefdescription><para>{units_value}</para></xrefdescription>
            </xrefsect>
    """
    xrefsect = ET.Element("xrefsect", {"id": f"trickunits_{unit_id}"})
    title = ET.SubElement(xrefsect, "xreftitle")
    title.text = XREF_TITLE
    desc = ET.SubElement(xrefsect, "xrefdescription")
    para = ET.SubElement(desc, "para")
    para.text = units_value
    return xrefsect


def process_file(path: Path) -> bool:
    """Extract and relocate Trick units annotations within a single Doxygen XML file.

    Scans every <memberdef kind="variable"> element in the file for a
    description whose text begins with a Trick units annotation, i.e.
    "(unit)" or "trick_units(unit)". Strips the units specifier from the
    description text and re-inserts it into the member's <detaileddescription>
    as a standard <xrefsect> block (see make_xrefsect), which Breathe renders as
    a separate "Units" field below the variable description text.

    If there are no unit specifiers in the file, this function does not make
    any changes.

    Args:
        path: Path to a single Doxygen-generated XML file to process in place.
    """
    tree = ET.parse(path)
    root = tree.getroot()
    changed = False

    for memberdef in root.iter("memberdef"):
        if memberdef.get("kind") != "variable":
            continue

        units_value = None
        for tag in ("briefdescription", "detaileddescription"):
            desc = memberdef.find(tag)
            if desc is None:
                continue
            para = desc.find("para")
            if para is None or not para.text:
                continue
            m = UNITS_RE.match(para.text)
            if not m:
                continue
            units_value, rest = m.group(1), m.group(2)
            para.text = rest
            break

        if units_value is None:
            continue

        detaileddescription = memberdef.find("detaileddescription")
        if detaileddescription is None:
            # Insert right after briefdescription to preserve schema order.
            brief = memberdef.find("briefdescription")
            idx = (
                list(memberdef).index(brief) + 1
                if brief is not None
                else len(memberdef)
            )
            detaileddescription = ET.Element("detaileddescription")
            memberdef.insert(idx, detaileddescription)

        member_id = memberdef.get("id", path.stem)
        xref_para = ET.SubElement(detaileddescription, "para")
        xref_para.append(make_xrefsect(member_id, units_value))
        changed = True

    if changed:
        tree.write(path, encoding="UTF-8", xml_declaration=True)


def main():
    """Extract Trick units from Doxygen-generated XML files."""
    if len(sys.argv) != 2:
        sys.exit("Usage: extract_trick_units.py <xml-output-dir>")

    xml_dir = Path(sys.argv[1])
    if not xml_dir.is_dir():
        sys.exit(f"Error: XML output directory '{xml_dir}' not found.")

    for filename in xml_dir.glob("*.xml"):
        process_file(filename)


if __name__ == "__main__":
    main()
