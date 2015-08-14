// bdlde_byteorder.cpp                                                -*-C++-*-

#include <bdlde_byteorder.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlde_byteorder_cpp,"$Id$ $CSID$")

#include <bdlb_print.h>

#include <bsl_ostream.h>

namespace BloombergLP {

namespace bdlde {
                        // ----------------------
                        // struct ByteOrder
                        // ----------------------

// CLASS METHODS
bsl::ostream& ByteOrder::print(bsl::ostream&         stream,
                                     ByteOrder::Enum value,
                                     int                   level,
                                     int                   spacesPerLevel)
{
    bdlb::Print::indent(stream, level, spacesPerLevel);

    stream << ByteOrder::toAscii(value);

    if (spacesPerLevel >= 0) {
        stream << '\n';
    }

    return stream;
}

const char *ByteOrder::toAscii(ByteOrder::Enum value)
{
#define CASE(X) case(e_ ## X): return #X;

    switch (value) {
      CASE(LITTLE_ENDIAN)
      CASE(BIG_ENDIAN)
      default: return "(* UNKNOWN *)";
    }

#undef CASE
}
}  // close package namespace

}  // close enterprise namespace

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
