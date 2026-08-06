# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html
import os
import re

_CONF_DIR = os.path.abspath(os.path.dirname(__file__))
_CML_HOME = os.path.abspath(os.path.join(_CONF_DIR, "..", ".."))
def _get_cml_release():
    """Read the project version from our CMakeLists.txt file, which is the authoritative source."""
    cmake_file = os.path.join(_CML_HOME, "CMakeLists.txt")
    with open(cmake_file) as f:
        cmake_content = f.read()

    match = re.search(
        r"project\s*\([^)]*?\bVERSION\s+(\d+\.\d+(?:\.\d+){0,2})",
        cmake_content,
        re.IGNORECASE | re.DOTALL,
    )
    if not match:
        raise RuntimeError(f"Unable to determine version from {cmake_file}")
    return match.group(1)

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = "The Common Model Library"
copyright = "%Y, National Aeronautics and Space Administration"

author = "National Aeronautics and Space Administration"
release = _get_cml_release()
version = ".".join(release.split(".")[:2])

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [
    "breathe",
    "sphinx.ext.graphviz",
]

templates_path = ["_templates"]
exclude_patterns = ["_build", "Thumbs.db", ".DS_Store"]

# -- Breathe configuration ---------------------------------------------------
breathe_default_project = "CML"
smartquotes = False

# -- Graphviz configuration --------------------------------------------------
# Breathe hands Doxygen's inheritance graphs to sphinx.ext.graphviz as dot
# source. SVG keeps them legible at any zoom level; the default is PNG.
graphviz_output_format = "svg"

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = "sphinx_rtd_theme"
#html_theme = "furo"
#html_theme = "pydata_sphinx_theme"
html_theme_options = {"collapse_navigation": False}
html_static_path = ["_static"]
html_js_files = ["trickunits.js"]
html_css_files = ["trickunits.css", "cml_theme.css"]
