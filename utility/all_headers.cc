/* This file exists for CML's static analysis tools. Headers which aren't included
 * by any source file aren't analyzed by clang-tidy. Therefore, we use this empty
 * source file to include the Trickification S_source.hh file, which by necessity
 * includes all CML headers.
 */
#include "../trickified/S_source.hh" // NOLINT(misc-include-cleaner)
