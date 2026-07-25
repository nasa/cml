#!/usr/bin/env python3
"""
extract_trick_units.py

Extracts Trick-style units annotations ("(kg)" or "trick_units(kg)") from the
start of Doxygen member descriptions and re-emits them as a standard Doxygen
<xrefsect> block -- the same schema element Doxygen uses for \\todo/\\bug/
\\deprecated -- which Breathe already renders out of the box, no Breathe
patching required.

Must run AFTER fix_dashes.sh -- Trick's dimensionless unit marker is
literally "trick_units(--)", and this regex won't recognize it as a unit
string unless <ndash/> has already been restored to a literal "--".

Usage: extract_trick_units.py <xml-output-dir>
"""
import re
import sys
import xml.etree.ElementTree as ET
from pathlib import Path

UNITS_RE = re.compile(r'^\s*(?:trick_units)?\(([^()]*)\)\s*(.*)$', re.DOTALL)
XREF_TITLE = "Units"


def make_xrefsect(unit_id: str, units_value: str) -> ET.Element:
    xrefsect = ET.Element('xrefsect', {'id': f'trickunits_{unit_id}'})
    title = ET.SubElement(xrefsect, 'xreftitle')
    title.text = XREF_TITLE
    desc = ET.SubElement(xrefsect, 'xrefdescription')
    para = ET.SubElement(desc, 'para')
    para.text = units_value
    return xrefsect


def process_file(path: Path) -> bool:
    tree = ET.parse(path)
    root = tree.getroot()
    changed = False

    for memberdef in root.iter('memberdef'):
        if memberdef.get('kind') != 'variable':
            continue

        units_value = None
        for tag in ('briefdescription', 'detaileddescription'):
            desc = memberdef.find(tag)
            if desc is None:
                continue
            para = desc.find('para')
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

        detaileddescription = memberdef.find('detaileddescription')
        if detaileddescription is None:
            # Insert right after briefdescription to preserve schema order.
            brief = memberdef.find('briefdescription')
            idx = list(memberdef).index(brief) + 1 if brief is not None else len(memberdef)
            detaileddescription = ET.Element('detaileddescription')
            memberdef.insert(idx, detaileddescription)

        member_id = memberdef.get('id', path.stem)
        xref_para = ET.SubElement(detaileddescription, 'para')
        xref_para.append(make_xrefsect(member_id, units_value))
        changed = True

    if changed:
        tree.write(path, encoding='UTF-8', xml_declaration=True)

    return changed


def main():
    if len(sys.argv) != 2:
        sys.exit("Usage: extract_trick_units.py <xml-output-dir>")

    xml_dir = Path(sys.argv[1])
    if not xml_dir.is_dir():
        sys.exit(f"Error: XML output directory '{xml_dir}' not found.")

    count = sum(process_file(f) for f in xml_dir.glob('*.xml'))
    print(f"extract_trick_units.py: updated {count} XML file(s) in {xml_dir}")


if __name__ == '__main__':
    main()
