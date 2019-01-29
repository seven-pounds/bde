// bsl_system_error.h                                                 -*-C++-*-
#ifndef INCLUDED_BSL_SYSTEM_ERROR
#define INCLUDED_BSL_SYSTEM_ERROR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@SEE_ALSO: package bsl+stdhdrs
//
//@DESCRIPTION: Provide types, in the 'bsl' namespace, equivalent to those
// defined in the corresponding C++ standard header.  Include the native
// compiler-provided standard header, and also directly include Bloomberg's
// implementation of the C++ standard type (if one exists).  Finally, place the
// included symbols from the 'std' namespace (if any) into the 'bsl' namespace.

#include <bsls_libraryfeatures.h>
#include <bsls_nativestd.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

#include <system_error>

namespace bsl {

    using native_std::error_category;
    using native_std::error_code;
    using native_std::error_condition;
    using native_std::system_error;
    using native_std::is_error_code_enum;
    using native_std::is_error_condition_enum;
    using native_std::errc;
    using native_std::generic_category;
    using native_std::system_category;
    using native_std::make_error_code;
    using native_std::make_error_condition;

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    using native_std::is_error_code_enum_v;
    using native_std::is_error_condition_enum_v;
#elif defined BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
    template <class TYPE>
    constexpr bool is_error_code_enum_v =
                                   native_std::is_error_code_enum<TYPE>::value;
        // This template variable represents the result value of the
        // 'native_std::is_error_code_enum' meta-function.

    template <class TYPE>
    constexpr bool is_error_condition_enum_v =
                              native_std::is_error_condition_enum<TYPE>::value;
        // This template variable represents the result value of the
        // 'native_std::is_error_condition_enum' meta-function.

#endif

}  // close package namespace

#else

#include <bslstl_systemerror.h>

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
