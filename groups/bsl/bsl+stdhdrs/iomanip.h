// iomanip.h                                                          -*-C++-*-

//@PURPOSE: Provide backwards compatibility for STLPort header
//
//@SEE_ALSO: package bsl+stdhdrs
//
//@AUTHOR: Arthur Chiu (achiu21)
//
//@DESCRIPTION: This component provides backwards compatibility for Bloomberg
// libraries that depend on non-standard STLPort header files.  This header
// includes the corresponding C++ header and provides any additional symbols
// that the STLPort header would define via transitive includes.

#include <bsl_iomanip.h>

using std::setiosflags;
using std::resetiosflags;
using std::setbase;
using std::setfill;
using std::setprecision;
using std::setw;

// _lib.std.ios.manip_, manipulators:
using std::boolalpha;
using std::noboolalpha;
using std::showbase;
using std::noshowbase;
using std::showpoint;
using std::noshowpoint;
using std::showpos;
using std::noshowpos;
using std::skipws;
using std::noskipws;
using std::uppercase;
using std::nouppercase;

// _lib.adjustfield.manip_ adjustfield:
using std::internal;
using std::left;
using std::right;

// _lib.basefield.manip_ basefield:
using std::dec;
using std::hex;
using std::oct;

// _lib.floatfield.manip_ floatfield:
using std::fixed;
using std::scientific;

// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
