# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
# import os
# import sys
# sys.path.insert(0, os.path.abspath('.'))


# -- Project information -----------------------------------------------------

project = 'Reiji'
copyright = '2019-present, Mitca Dumitru'
author = 'Mitca Dumitru'


# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [ "breathe", "sphinx.ext.extlinks" ]

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

pygments_style = 'sphinx'
highlight_language = 'c++'
primary_domain = 'cpp'

# Sphinx wants the strings below to contain %s... so we solve that in a hacky way
extlinks = {
    'doxygen': ('https://www.doxygen.nl/download.html%s', ''),
    'sphinx': ('https://www.sphinx-doc.org/en/master/usage/installation.html%s',''),
    'breathe': ('https://breathe.readthedocs.io/en/latest/#download%s', '')
}

# -- Breathe config ----------------------------------------------------------
breathe_default_project = 'Reiji'
breathe_domain_by_extension = {"h" : "cpp"}
# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = 'sphinx_rtd_theme'

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']
