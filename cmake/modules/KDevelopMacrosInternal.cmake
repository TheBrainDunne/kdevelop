#
# KDevelop Private Macros
#
# The following macros are defined here:
#
#   add_compile_flag_if_supported(<flag> [CXX_ONLY])
#   add_target_compile_flag_if_supported(<target> <INTERFACE|PUBLIC|PRIVATE> <flag>)
#   declare_qt_logging_category(<sourcevarname> HEADER <filename> IDENTIFIER <id> CATEGORY_NAME <name> EXPORT <target> DESCRIPTION <desc>)
#   install_qt_logging_categories(EXPORT <target> FILE <name>)
#
#=============================================================================
# Copyright 2018 Friedrich W. H. Kossebau <kossebau@kde.org>
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================


# helper method to ensure consistent cache var names
function(_varname_for_compile_flag_check_result _varname _flag )
    string(REGEX REPLACE "[-=]" "_" _varname ${_flag})
    string(TOUPPER ${_varname} _varname)
    set(_varname "KDEV_HAVE${_varname}" PARENT_SCOPE)
endfunction()


# add_compile_flag_if_supported(<flag> [CXX_ONLY])
# Optional argument CXX_ONLY to set the flag only for C++ code.
# 
# Example:
#   add_compile_flag_if_supported(-Wsuggest-override CXX_ONLY)
function(add_compile_flag_if_supported _flag)
    _varname_for_compile_flag_check_result(_varname ${_flag})
    check_cxx_compiler_flag("${_flag}" "${_varname}")

    if (${${_varname}})
        if("${ARGV1}" STREQUAL "CXX_ONLY")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${_flag}" PARENT_SCOPE)
        else()
            add_compile_options(${_flag})
        endif()
    endif()
endfunction()


# add_target_compile_flag_if_supported(<target> <INTERFACE|PUBLIC|PRIVATE> <flag>)
# 
# Example:
#   add_target_compile_flag_if_supported(kdevqtc-qmlsupport PRIVATE "-Wno-suggest-override")
function(add_target_compile_flag_if_supported _target _scope _flag)
    _varname_for_compile_flag_check_result(_varname ${_flag})
    check_cxx_compiler_flag("${_flag}" "${_varname}")

    if (${${_varname}})
        target_compile_options(${_target} ${_scope} "${_flag}")
    endif()
endfunction()


# declare_qt_logging_category(<sourcevarname> HEADER <filename> IDENTIFIER <id> CATEGORY_NAME <name> EXPORT <target> DESCRIPTION <desc>)
#
# Example:
#   declare_qt_logging_category(Foo_LIB_SRCS
#       HEADER debug.h
#       IDENTIFIER DOCUMENTATION
#       CATEGORY_NAME "bar.foo"
#       DESCRIPTION "The foo of bar"
#       EXPORT BarCategories
#   )
macro(declare_qt_logging_category _sources)
    set(options )
    set(oneValueArgs HEADER IDENTIFIER CATEGORY_NAME EXPORT DESCRIPTION)
    set(multiValueArgs)

    cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT DEFINED ARGS_HEADER)
        message(FATAL_ERROR "HEADER needs to be defined when calling declare_qt_logging_category().")
    endif()
    if(NOT DEFINED ARGS_IDENTIFIER)
        message(FATAL_ERROR "IDENTIFIER needs to be defined when calling declare_qt_logging_category().")
    endif()
    if(NOT DEFINED ARGS_CATEGORY_NAME)
        message(FATAL_ERROR "CATEGORY_NAME needs to be defined when calling declare_qt_logging_category().")
    endif()
    if(NOT DEFINED ARGS_DESCRIPTION)
        message(FATAL_ERROR "DESCRIPTION needs to be defined when calling declare_qt_logging_category().")
    endif()

    ecm_qt_declare_logging_category(${_sources}
        HEADER ${ARGS_HEADER}
        IDENTIFIER ${ARGS_IDENTIFIER}
        CATEGORY_NAME ${ARGS_CATEGORY_NAME}
    )

    # Nasty hack: we create a target just to store all the category data in some build-system global object
    # which then can be accessed from other places like install_qt_logging_categories().
    # we also create it here on first usage, to spare some additional call.
    # Better idea how to solve that welcome
    set(_targetname "qt_logging_category_${ARGS_EXPORT}")
    if (NOT TARGET ${_targetname})
        add_custom_target(${_targetname})
        set(_categories ${ARGS_CATEGORY_NAME})
    else()
        get_target_property(_value ${_targetname} CATEGORIES)
        set(_categories "${_value};${ARGS_CATEGORY_NAME}")
    endif()
    set_property(TARGET ${_targetname} PROPERTY CATEGORIES "${_categories}")
    set_property(TARGET ${_targetname} PROPERTY "IDENTIFIER_${ARGS_CATEGORY_NAME}" "${ARGS_IDENTIFIER}")
    set_property(TARGET ${_targetname} PROPERTY "DESCRIPTION_${ARGS_CATEGORY_NAME}" "${ARGS_DESCRIPTION}")
endmacro()


# install_qt_logging_categories(EXPORT <target> FILE <name>)
#
# Needs to be called after the last declare_qt_logging_category call which uses the same EXPORT name.
#
# Example:
#   install_qt_logging_categories(
#       EXPORT KDevPlatformCategories
#       FILE kdevplatform.test.categories
#   )
function(install_qt_logging_categories)
    set(options )
    set(oneValueArgs FILE EXPORT)
    set(multiValueArgs)

    cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT DEFINED ARGS_FILE)
        message(FATAL_ERROR "FILE needs to be defined when calling install_qt_logging_categories().")
    endif()

    if(NOT DEFINED ARGS_EXPORT)
        message(FATAL_ERROR "EXPORT needs to be defined when calling install_qt_logging_categories().")
    endif()

    set(_targetname "qt_logging_category_${ARGS_EXPORT}")
    if (NOT TARGET ${_targetname})
        message(FATAL_ERROR "${ARGS_EXPORT} is an unknown qt logging category export name.")
    endif()

    get_target_property(_categories ${_targetname} CATEGORIES)
    list(SORT _categories)

    set(_content
"# KDebugSettings data file
# This file was generated by install_qt_logging_categories(). DO NOT EDIT!

")

    foreach(_category IN LISTS _categories)
        get_target_property(_description ${_targetname} "DESCRIPTION_${_category}")

        # Format:
        # logname<space>description
        string(APPEND _content "${_category} ${_description}\n")

        # TODO: support newer not backward-compatible format as supported by kdebugsettings 18.12
        # Format:
        # logname<space>description(optional <space> DEFAULT_SEVERITY [DEFAULT_CATEGORY] as WARNING/DEBUG/INFO/CRITICAL) optional IDENTIFIER [...])
        # example: logname<space>description<space>DEFAULT_SEVERITY<space>[DEBUG]<space>IDENTIFIER<space>[foo]
        # Needs idea how to ensure that at runtime kdebugsettings is min KA 18.12
    endforeach()

    if (NOT IS_ABSOLUTE ${ARGS_FILE})
        set(ARGS_FILE "${CMAKE_CURRENT_BINARY_DIR}/${ARGS_FILE}")
    endif()
    file(GENERATE
        OUTPUT "${ARGS_FILE}"
        CONTENT "${_content}"
    )

    install(
        FILES "${ARGS_FILE}"
        DESTINATION ${KDE_INSTALL_CONFDIR}
    )
endfunction()
