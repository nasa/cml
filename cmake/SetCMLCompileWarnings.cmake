# SetCMLCompileWarnings
# --------
#
# This module defines a helper function which applies a common set of warnings
# to a target defined by the CML project. If enabled, it also treats warnings
# as errors.
#
# Arguments:
#
#     TARGET: The target name

include_guard()

function(set_cml_compile_warnings TARGET)
    set(CML_SHARED_WARNING_FLAGS
        # Keep these first since we may override some of the levels set for
        # certain warnings below.
        -Wall
        -Wextra
        -Wpedantic

        # Stricter warnings.
        -Walloca
        -Wcast-align
        -Wcast-qual
        -Wformat-security
        -Wformat-signedness
        -Wformat-truncation
        -Wmissing-declarations
        -Wmissing-include-dirs
        -Wnull-dereference
        -Wshadow
        -Wsign-conversion
        -Wswitch-enum
        -Wundef
        -Wuninitialized
        -Wunused
        -Wunused-macros
        -Wvla
    )

    set(CML_C_WARNING_FLAGS
        -Wbad-function-cast
        -Wjump-misses-init
        -Wmissing-prototypes
        -Wnested-externs
        -Wold-style-definition
        -Wstrict-prototypes
    )

    # GCC-only warnings
    set(GCC_ONLY_WARNINGS
        -Wduplicated-branches
        -Wduplicated-cond
        -Wformat=1
        -Wformat-overflow=2
        -Wimplicit-fallthrough=5
        -Wlogical-op
        -Wtrampolines
        -Wcatch-value=3
        -Wconditionally-supported
        -Wnoexcept
        -Wplacement-new=2
        -Wstrict-null-sentinel
        -Wuseless-cast
    )

    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
       # GCC -> add GCC-only warnings
       list(APPEND CML_CPP_WARNING_FLAGS ${GCC_ONLY_WARNINGS})
       list(APPEND CML_SHARED_WARNING_FLAGS ${GCC_ONLY_WARNINGS})
    endif()

    target_compile_options(${TARGET}
        PRIVATE
            ${CML_SHARED_WARNING_FLAGS}
            $<$<COMPILE_LANGUAGE:C>:${CML_C_WARNING_FLAGS}>
            $<$<COMPILE_LANGUAGE:CXX>:${CML_CPP_WARNING_FLAGS}>
            $<$<BOOL:${CML_WARNINGS_AS_ERRORS}>:-Werror>
    )
endfunction()
