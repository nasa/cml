# CML Mocks Library

This directory contains mock objects which wrap CML and Trick functionality.
Users may include these mocks in their own unit tests to set expectations for
CML and Trick behavior.

## Inclusion

Boith the CML and Trick mock libraries are created if the `CML_BUILD_MOCKS`
CMake variable is true.

```cmake
set(CML_BUILD_MOCKS ON)
```

To build the mocks, the `GTest::gmock_main` target must exist. CML does not
provide GTest targets for external projects; you must include GTest either from
your system, via `FetchContent`, or by any other mechanism your project has
decided upon.

You can link the CML and Trick mocks into your unit test executable like so:

```cmake
target_link_libraries(test_executable
    PRIVATE
        cml::cml_mocks
        cml::trick_mocks
)
```

Note that you may include any combination of the CML or Trick mocks; neither is
dependent upon the other. Note also that you should include the mock libraries
_before_ including the real versions of CML or Trick libraries so that the
mocks take precedence at link-time.

The include path provided by CMake is `${CML_HOME}/utility`. Mocks can be
included in your unit tests like so:

```c++
#include "mocks/cml/some_cml_mock.hh"
#include "mocks/trick/some_trick_mock.hh"
```

## Caveats

If Trick provides support for mocking the interfaces present in CML's Trick
Mocks library, CML may remove our support for those mocks in the future.