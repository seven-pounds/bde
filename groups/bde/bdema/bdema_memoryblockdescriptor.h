// bdema_memoryblockdescriptor.h                                      -*-C++-*-
#ifndef INCLUDED_BDEMA_MEMORYBLOCKDESCRIPTOR
#define INCLUDED_BDEMA_MEMORYBLOCKDESCRIPTOR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a class describing a block of memory.
//
//@CLASSES:
//  bdema_MemoryBlockDescriptor: describes a block of memory
//
//@AUTHOR: Henry Verschell (hverschell)
//
//@SEE_ALSO: bdema_protectablememoryblockdispenser
//
//@DESCRIPTION: This component defines an in-core value-semantic class for
// describing a block of memory, namely 'bdema_MemoryBlockDescriptor'.  Each
// descriptor object contains the address of the block of memory and the size
// of the block.  The distinguished "null" descriptor contains an address and a
// size that are both 0.
//
///Usage
///-----
// This example demonstrates how to create and test the state of a
// 'bdema_MemoryBlockDescriptor'.
//..
//   char buffer[100];
//
//   bdema_MemoryBlockDescriptor a(buffer, sizeof buffer);
//                               assert(!a.isNull());
//                               assert(buffer         == a.address());
//                               assert(sizeof buffer  == a.size());
//
//   bdema_MemoryBlockDescriptor b;
//                               assert( b.isNull());
//                               assert(0              == b.address());
//                               assert(0              == b.size());
//                               assert(a              != b);
//
//   b = a;
//                               assert(!b.isNull());
//                               assert(buffer         == b.address());
//                               assert(sizeof buffer  == b.size());
//                               assert(a              == b);
//
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
    // Permit reliance on transitive includes within robo.
#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

namespace BloombergLP {

                   // =================================
                   // class bdema_MemoryBlockDescriptor
                   // =================================

class bdema_MemoryBlockDescriptor {
    // This is an in-core value-semantic class describing the size and
    // address of a block of memory.  A null block descriptor is defined as a
    // descriptor having an address and size of 0.  The behavior is undefined
    // for a descriptor whose address is 0, but whose size if not also 0.

  public:
    // PUBLIC TYPES
    typedef bsls::Types::size_type size_type;  // type for block size

  private:
    // DATA
    void      *d_address_p;  // address of the memory block
    size_type  d_size;       // size of the block

  public:
    // CREATORS
    bdema_MemoryBlockDescriptor();
        // Create a memory block descriptor having an address and size of 0.

    bdema_MemoryBlockDescriptor(void *address, size_type size);
        // Create a memory block descriptor having the specified 'address' and
        // 'size'.  The behavior is undefined if 'address' is 0 but 'size' is
        // not also 0.

    bdema_MemoryBlockDescriptor(const bdema_MemoryBlockDescriptor& original);
        // Create a memory block descriptor having the same value as the
        // specified 'original' descriptor.  Two descriptors have the same
        // value if and only if they have the same address and size.

    // ~bdema_MemoryBlockDescriptor();
        // Destroy this memory block descriptor.  Note that this trivial
        // destructor is generated by the compiler.

    // MANIPULATORS
    bdema_MemoryBlockDescriptor& operator=(
                                    const bdema_MemoryBlockDescriptor& rhs);
        // Assign to this memory block descriptor the value of the specified
        // 'rhs' descriptor and return a reference to this modifiable
        // descriptor.

    void setAddressAndSize(void *address, size_type size);
        // Set the address and size of the memory block described by this
        // object to the specified 'address' and 'size.  The behavior is
        // undefined if 'address' is 0 but 'size' is not also 0.

    // ACCESSORS
    bool isNull() const;
        // Return 'true' if this memory block descriptor describes a null
        // memory block and 'false' otherwise.  A null memory block has an
        // address and size of 0.

    void *address() const;
        // Return the address of the modifiable memory block described by this
        // object, or 0 if this is a null descriptor.

    size_type size() const;
        // Return the size of the memory block described by this object.

    void print() const;
        // Format the attributes of this memory block descriptor to 'stdout' in
        // some reasonable (single-line) format.
};

// FREE OPERATORS
inline
bool operator==(const bdema_MemoryBlockDescriptor& lhs,
                const bdema_MemoryBlockDescriptor& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' memory block descriptors
    // have the same value and 'false' otherwise.  Two descriptors have the
    // same value if and only if they have the same address and size.

inline
bool operator!=(const bdema_MemoryBlockDescriptor& lhs,
                const bdema_MemoryBlockDescriptor& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' memory block descriptors
    // do not have the same value and 'false' otherwise.  Two descriptors
    // differ in value if they differ in either their address or size.

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                  // ---------------------------------
                  // class bdema_MemoryBlockDescriptor
                  // ---------------------------------

// CREATORS
inline
bdema_MemoryBlockDescriptor::
bdema_MemoryBlockDescriptor()
: d_address_p(0)
, d_size(0)
{
}

inline
bdema_MemoryBlockDescriptor::bdema_MemoryBlockDescriptor(void      *address,
                                                         size_type  size)
: d_address_p(address)
, d_size(size)
{
    BSLS_ASSERT_SAFE(0 <= size);
    BSLS_ASSERT_SAFE(address || 0 == size);
}

inline
bdema_MemoryBlockDescriptor::bdema_MemoryBlockDescriptor(
                                 const bdema_MemoryBlockDescriptor& original)
: d_address_p(original.d_address_p)
, d_size(original.d_size)
{
}

// MANIPULATORS
inline
bdema_MemoryBlockDescriptor& bdema_MemoryBlockDescriptor::operator=(
                                        const bdema_MemoryBlockDescriptor& rhs)
{
    d_address_p = rhs.d_address_p;
    d_size      = rhs.d_size;

    return *this;
}

inline
void bdema_MemoryBlockDescriptor::setAddressAndSize(void      *address,
                                                    size_type  size)
{
    BSLS_ASSERT_SAFE(0 <= size);
    BSLS_ASSERT_SAFE(address || 0 == size);

    d_address_p = address;
    d_size      = size;
}


// ACCESSORS
inline
void *bdema_MemoryBlockDescriptor::address() const
{
    return d_address_p;
}

inline
bdema_MemoryBlockDescriptor::size_type
bdema_MemoryBlockDescriptor::size() const
{
    return d_size;
}

inline
bool bdema_MemoryBlockDescriptor::isNull() const
{
    return 0 == d_address_p;
}

// FREE OPERATORS
inline
bool operator==(const bdema_MemoryBlockDescriptor& lhs,
                const bdema_MemoryBlockDescriptor& rhs)
{
    return lhs.address() == rhs.address() && lhs.size() == rhs.size();
}

inline
bool operator!=(const bdema_MemoryBlockDescriptor& lhs,
                const bdema_MemoryBlockDescriptor& rhs)
{
    return !(lhs == rhs);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
