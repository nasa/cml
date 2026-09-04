# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html
import os
import re

_CONF_DIR = os.path.abspath(os.path.dirname(__file__))
_CML_HOME = os.path.abspath(os.path.join(_CONF_DIR, "..", ".."))
_toplevel_cmake_lists = os.path.join(_CML_HOME, "CMakeLists.txt")

def _get_cml_release():
    """Read the project version from our CMakeLists.txt file, which is the authoritative source."""
    with open(_toplevel_cmake_lists) as f:
        cmake_content = f.read()

    match = re.search(
        r"project\s*\([^)]*?\bVERSION\s+(\d+\.\d+(?:\.\d+){0,2})",
        cmake_content,
        re.IGNORECASE | re.DOTALL,
    )
    if not match:
        raise RuntimeError(f"Unable to determine version from {cmake_file}")
    return match.group(1)

def _get_dependency_minimum_versions():
    """Read the minimum versions of dependencies from our CMakeLists.txt file."""
    with open(_toplevel_cmake_lists) as f:
            cmake_content = f.read()

    pattern = re.compile(
        r"""find_package\s*\(\s*
            (?P<name>\w+)
            \s+
            (?P<version>\d+(?:\.\d+)*)
        """,
        re.VERBOSE,
    )
    versions = {}
    for match in pattern.finditer(cmake_content):
        versions[match.group("name")] = match.group("version")
    return versions

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = "The Common Model Library"
copyright = "%Y, National Aeronautics and Space Administration"

author = ""
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

# Necessary for labelling a system of equations with a single equation number.
mathjax3_config = {
    "tex": {"tags": "ams"}
}

# -- Breathe configuration ---------------------------------------------------
# Breathe is our link between the Doxygen-generated XML data for the source
# code and the Sphinx documentation generator.
breathe_default_project = "CML"
smartquotes = False
breathe_show_include = True

# -- Graphviz configuration --------------------------------------------------
# Breathe hands Doxygen's inheritance graphs to sphinx.ext.graphviz as dot
# source. SVG keeps them legible at any zoom level; the default is PNG.
graphviz_output_format = "svg"

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = "sphinx_book_theme"
html_theme_options = {
    "collapse_navigation": False,
    "search_as_you_type": True,
    "pygments_light_style": "tango",
    "pygments_dark_style":  "monokai",
    "repository_url": "https://github.com/nasa/cml",
    "use_repository_button": True,
    "use_issues_button": True,
    "announcement": (
        "⚠️ The transition to GitHub Pages is in-progress. Some model documentation "
        "will not be available via this webpage, but is present in the respective "
        "model docs folder ⚠️"
    )
}
html_context = {
    "default_mode": "light",  # Light mode looks better with equations.
}
html_static_path = ["_static"]
html_js_files = ["trickunits.js"]
html_css_files = ["trickunits.css", "cml_theme.css"]

_dependency_min_versions = _get_dependency_minimum_versions()
_trick_minimum_version = _dependency_min_versions["Trick"]
_jeod_minimum_version = _dependency_min_versions["JEOD"]

rst_epilog = f"""
.. |TrickVersionRequired| replace:: {_trick_minimum_version}
.. |JEODVersionRequired| replace:: {_jeod_minimum_version}
"""