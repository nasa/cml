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
    # If the compiler is GCC, continue. Otherwise, return immediately.
    if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        return()
    endif()

    set(CML_SHARED_WARNING_FLAGS
        # Keep these first since we may override some of the levels set for
        # certain warnings below.
        -Wall
        -Wextra
        -Wpedantic

        # TODO Nino Tarantino 8/11/2026: Re-enable as part of
        # https://github.com/nasa/cml/issues/27
        -Wno-overloaded-virtual

        # Stricter warnings.
        -Walloca
        -Wcast-align
        -Wcast-qual
        -Wduplicated-branches
        -Wduplicated-cond
        -Wformat=1
        -Wformat-overflow=2
        -Wformat-security
        -Wformat-signedness
        -Wformat-truncation
        -Wimplicit-fallthrough=5
        -Wlogical-op
        -Wmissing-declarations
        -Wmissing-include-dirs
        -Wnull-dereference
        -Wshadow
        -Wsign-conversion
        -Wswitch-enum
        -Wtrampolines
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
    set(CML_CPP_WARNING_FLAGS
        -Wcatch-value=3
        -Wconditionally-supported
        -Wctor-dtor-privacy
        -Wextra-semi
        -Wnoexcept
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Wplacement-new=2
        -Wstrict-null-sentinel
        -Wsuggest-override
        -Wuseless-cast
        -Wzero-as-null-pointer-constant
    )

    target_compile_options(${TARGET}
        PRIVATE
            ${CML_SHARED_WARNING_FLAGS}
            $<$<COMPILE_LANGUAGE:C>:${CML_C_WARNING_FLAGS}>
            $<$<COMPILE_LANGUAGE:CXX>:${CML_CPP_WARNING_FLAGS}>
            $<$<BOOL:${CML_WARNINGS_AS_ERRORS}>:-Werror>
    )
endfunction()
