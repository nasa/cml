#!/usr/bin/env bash
# Reverses Doxygen's ndash/mdash entity substitution in generated XML,
# restoring literal "--" and "---" sequences.
#
# Usage: fix_dashes.sh <path-to-xml-output-dir>
set -euo pipefail

XML_DIR="${1:?Usage: fix_dashes.sh <xml-output-dir>}"

if [ ! -d "$XML_DIR" ]; then
    echo "Error: XML output directory '$XML_DIR' not found." >&2
    exit 1
fi

count=0
while IFS= read -r -d '' file; do
    # -i edits in place; mdash (---) before ndash (--) since <mdash/>
    # must map to three dashes, not two.
    sed -i \
        -e 's/<mdash\/>/---/g' \
        -e 's/<ndash\/>/--/g' \
        "$file"
    count=$((count + 1))
done < <(find "$XML_DIR" -name '*.xml' -print0)

if [ "$count" -eq 0 ]; then
    echo "Warning: no .xml files found under '$XML_DIR'" >&2
fi
