// bdes_platformutil.h                                                -*-C++-*-
#ifndef INCLUDED_BDES_PLATFORMUTIL
#define INCLUDED_BDES_PLATFORMUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Encapsulate platform-dependent operations behind stable interface.
//
//@DEPRECATED: Use 'bsls_alignmentutil', 'bsls_byteorder', 'bsls_platform', and
// 'bsls_types' instead.
//
//@CLASSES:
//   bdes_PlatformUtil: namespace for platform-dependent functionality
//
//@AUTHOR: John Lakos (jlakos)
//
//@DESCRIPTION: This component provides a namespace for a collection of
// typedefs and pure procedures that would otherwise have platform-dependent
// interfaces.  By platform, we mean the combination of the compiler, operating
// system, and hardware used.  For example:
//..
//     bdes_PlatformUtil::Int64    and    bdes_PlatformUtil::Uint64
//..
// identify the preferred fundamental types denoting signed and unsigned
// 64-bit integers, respectively, and:
//..
//     bdes_PlatformUtil::size_type
//..
// identifies the preferred integral type denoting a number of elements in
// allocators and containers.  Similarly, the functions:
//..
//   int bdes_PlatformUtil::isLittleEndian();
//   int bdes_PlatformUtil::isBigEndian();
//..
// encapsulate the capability of determining whether a machine is big- or
// little-endian across all supported platforms.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>  // required by 'bdes'
#endif

#ifndef INCLUDED_BSLS_ALIGNMENTUTIL
#include <bsls_alignmentutil.h>
#endif

#ifndef INCLUDED_BSLS_BYTEORDER
#include <bsls_byteorder.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

namespace BloombergLP {

                          // ------------------------
                          // struct bdes_PlatformUtil
                          // ------------------------

// TBD Note that the symbols defined below are a subset of the original as
// governed by Robo use of this component.  (Also see bsls_platformutil.h.)

struct bdes_PlatformUtil {
    // Provide a namespace for a suite of 'typedef's and pure procedures that
    // encapsulate, platform-dependent types and APIs.

    // TYPES
    typedef bsls::Types::size_type size_type;
        // The alias 'size_type' refers to the preferred type for denoting a
        // number of elements in either 'bslma' allocators or container types.
        // Note that this type is signed, as negative values may make sense in
        // certain contexts.  Also note that the standard-compliant allocators
        // (e.g., 'bslstl_allocator' and 'std::allocator') use an *unsigned*
        // size type, but that is fine because they also have a mechanism
        // ('max_size') to determine overflows resulting from converting from
        // one size type to the other.
        //
        // DEPRECATED: Use 'bsls::Types::size_type' instead.

    typedef bsls::Types::UintPtr UintPtr;
    typedef bsls::Types::IntPtr  IntPtr;
        // The aliases 'UintPtr' and 'IntPtr' are guaranteed to have the same
        // size as pointers.
        //
        // DEPRECATED: Use 'bsls::Types::UintPtr' and 'bsls::Types::IntPtr'
        // instead.

    typedef bsls::Types::Int64  Int64;
    typedef bsls::Types::Uint64 Uint64;
        // The aliases 'Int64' and 'Uint64' stand for whatever type 'Types'
        // implements for the appropriate supported platforms.
        //
        // DEPRECATED: Use 'bsls::Types::Int64' and 'bsls::Types::Uint64'
        // instead.

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    typedef bsls::AlignmentUtil::MaxAlignedType MaxAlign;
        // The alias 'MaxAlign' refers to a type that is maximally-aligned on
        // the current platform.
        //
        // DEPRECATED: Use 'bsls::AlignmentUtil::MaxAlignedType' instead.
#endif // BDE_OMIT_INTERNAL_DEPRECATED

    // CLASS METHODS
    static bool isLittleEndian();
        // Return 'true' if this platform is "little-endian", and 'false'
        // otherwise.  Note that "little-endian" (i.e., the least significant
        // byte of data at the lowest byte address) is inconsistent with
        // network byte order.
        //
        // DEPRECATED: Use preprocessor macro
        // 'BSLS_PLATFORM_IS_BIG_ENDIAN' defined in 'bsls_platform' instead.
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                          // ------------------------
                          // struct bdes_PlatformUtil
                          // ------------------------

// CLASS METHODS
inline
bool bdes_PlatformUtil::isLittleEndian()
{
#if defined(BSLS_PLATFORM_IS_LITTLE_ENDIAN)
    return BSLS_PLATFORM_IS_LITTLE_ENDIAN;
#else
    return false;
#endif
}

}  // close namespace BloombergLP

// ============================================================================
//                 COMPILE-TIME CONSTANT PRE-PROCESSOR MACROS
// ============================================================================

// single consumer remains in:
//   .../pc_controls/controls/blpmonsvc/apire/apire_nethost.h 
#ifndef BDES_PLATFORMUTIL_NTOHS
#define BDES_PLATFORMUTIL_NTOHS(X)           BSLS_BYTEORDER_NTOHS(X)
#endif

#if !defined(BSL_DOUBLE_UNDERSCORE_XLAT) || 1 == BSL_DOUBLE_UNDERSCORE_XLAT

#ifndef BDES_PLATFORMUTIL__HTONL
#define BDES_PLATFORMUTIL__HTONL(X)          BSLS_BYTEORDER_HTONL(X)
#endif

#ifndef BDES_PLATFORMUTIL__NTOHL
#define BDES_PLATFORMUTIL__NTOHL(X)          BSLS_BYTEORDER_NTOHL(X)
#endif

// Note BDES_PLATFORMUTIL__NO_LONG_HEADER_NAMES is now never defined

#endif

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
