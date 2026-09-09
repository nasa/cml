MAKEFILE_DIR    := $(dir $(realpath $(lastword $(MAKEFILE_LIST))))
CML_HOME        := $(abspath $(MAKEFILE_DIR)/../..)
DOCS_BUILD_DIR  := ${CML_HOME}/build/docs
BUILDDIR        := ${DOCS_BUILD_DIR}/sphinx
DOXYGEN_XML_DIR := ${DOCS_BUILD_DIR}/doxygen/xml

SPHINXOPTS      := -Dbreathe_projects.CML=${DOXYGEN_XML_DIR}
SPHINXBUILD     := sphinx-build
SPHINXAUTOBUILD := sphinx-autobuild
SOURCEDIR       := ${MAKEFILE_DIR}

# Put it first so that "make" without argument is like "make help".
help:
	@$(SPHINXBUILD) -M help "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)
	@echo ""
	@echo Custom targets:
	@echo -e "  \033[38;5;62mlivehtml\033[0m    to run sphinx-autobuild and watch for changes"

.PHONY: help Makefile check-doxygen

# Verify that the Doxygen XML outputs have been generated before any Sphinx build.
check-doxygen:
	@test -f "$(DOXYGEN_XML_DIR)/index.xml" || { \
		echo -e "\033[1;31mERROR:\033[0m The CML documentation requires that the Doxygen XML output already exists."; \
		echo "The file $(DOXYGEN_XML_DIR)/index.xml does not exist."; \
		echo "You're seeing this because you're attempting to run Sphinx independently. Try building the documentation with CMake."; \
		exit 1; \
	}

# Catch-all target: route all unknown targets to Sphinx using the new
# "make mode" option.  $(O) is meant as a shortcut for $(SPHINXOPTS).
%: Makefile check-doxygen
	@$(SPHINXBUILD) -M $@ "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)

# Autobuild option
livehtml:
	@$(SPHINXAUTOBUILD) "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS)
