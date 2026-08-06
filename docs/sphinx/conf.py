# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = "The Common Model Library"
copyright = "2026, National Aeronautics and Space Administration"
author = ""
release = "0.2.0"

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
html_theme_options = {"collapse_navigation": False}
html_static_path = ["_static"]
html_js_files = ["trickunits.js"]
html_css_files = ["trickunits.css", "cml_theme.css"]
