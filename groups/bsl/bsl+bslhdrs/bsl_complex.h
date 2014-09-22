// bsl_complex.h                                                      -*-C++-*-
#ifndef INCLUDED_BSL_COMPLEX
#define INCLUDED_BSL_COMPLEX

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@SEE_ALSO: package bsl+stdhdrs
//
//@AUTHOR: Arthur Chiu (achiu21)
//
//@DESCRIPTION: Provide types, in the 'bsl' namespace, equivalent to those
// defined in the corresponding C++ standard header.  Include the native
// compiler-provided standard header, and also directly include Bloomberg's
// implementation of the C++ standard type (if one exists).  Finally, place the
// included symbols from the 'std' namespace (if any) into the 'bsl' namespace.

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#include <complex>

namespace bsl
{
    // Import selected symbols into bsl namespace

    using native_std::abs;
    using native_std::arg;
    using native_std::complex;
    using native_std::conj;
    using native_std::cos;
    using native_std::cosh;
    using native_std::exp;
    using native_std::imag;
    using native_std::log;
    using native_std::log10;
    using native_std::norm;
    using native_std::polar;
    using native_std::pow;
    using native_std::real;
    using native_std::sin;
    using native_std::sinh;
    using native_std::sqrt;
    using native_std::tan;
    using native_std::tanh;

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    using native_std::basic_ios;
    using native_std::basic_iostream;
    using native_std::basic_istream;
    using native_std::basic_ostream;
    using native_std::basic_streambuf;
    using native_std::ios_base;
    using native_std::istreambuf_iterator;
    using native_std::locale;
    using native_std::ostreambuf_iterator;
#endif  // BDE_OMIT_INTERNAL_DEPRECATED
}

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
