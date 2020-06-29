// bslstl_algorithm.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLSTL_ALGORITHM
#define INCLUDED_BSLSTL_ALGORITHM

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide implementations for algorithms not in the system library.
//
//@CLASSES:
//
//@SEE_ALSO: bsl+bslhdrs
//
//@DESCRIPTION: This component is for internal use only.  Please include
// '<bsl_algorithm.h>' instead.  This component provides a namespace for
// implementations for standard algorithms that are not provided by the
// underlying standard library implementation.  For example, 'any_of' is a
// C++11 algorithm, and it is provided here for code using C++03.
//
///Usage
///-----
// This component is for use by the 'bsl+bslhdrs' package.  Use
// 'bsl_algorithm.h' directly.

// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BOS_STDHDRS_PROLOGUE_IN_EFFECT)
#error "include <bsl_algorithm.h> instead of <bslstl_algorithm.h> in\
 BSL_OVERRIDES_STD mode"
#endif

#include <bslscm_version.h>
#include <bsls_libraryfeatures.h>

namespace bsl {

#ifndef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

template <class InputIter, class PREDICATE>
bool all_of(InputIter first, InputIter last, PREDICATE pred)
    // Return 'true' if, for the specified '[first, last)' range and the
    // specified predicate 'pred', the range is either empty or 'pred(*i)' is
    // 'true' for every iterator 'i' in the range, and 'false' otherwise.  Note
    // that at most 'last - first' applications of the predicate are performed.
{
    for (; first != last; ++first) {
        if (!pred(*first)) {
            return false;                                             // RETURN
        }
     }
    return true;
}

template <class InputIter, class PREDICATE>
bool any_of(InputIter first, InputIter last, PREDICATE pred)
    // Return 'false' if, for the specified '[first, last)' range and the
    // specified predicate 'pred', the range is either empty or 'pred(*i)' is
    // 'false' for every iterator 'i' in the range, and 'true' otherwise.  Note
    // that at most 'last - first' applications of the predicate are performed.
{
    for (; first != last; ++first) {
        if (pred(*first)) {
            return true;                                              // RETURN
        }
    }
    return false;
}

template <class InputIter, class PREDICATE>
bool none_of(InputIter first, InputIter last, PREDICATE pred)
    // Return 'true' if, for the specified '[first, last)' range and the
    // specified predicate 'pred', the range is either empty or 'pred(*i)' is
    // 'false' for every iterator 'i' in the range, and 'false' otherwise.
    // Note that at most 'last - first' applications of the predicate are
    // performed.
{
    for (; first != last; ++first) {
        if (pred(*first)) {
            return false;                                             // RETURN
        }
    }
    return true;
}

#endif

// C++11 Algorithms
//   one_of
//   is_sorted
//   is_partitioned
//   is_permutation
//   partition_point
//   partition_copy
//   copy_if (this is already in bslstl_algorithmworkaround.h)
//   copy_n
//   iota

// C++14 Algorithms
//   equal
//   mismatch
// C++17 Algorithms
//   for_each_n

}  // close namespace bsl

#endif

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
