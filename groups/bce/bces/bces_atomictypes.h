// bces_atomictypes.h                                                 -*-C++-*-
#ifndef INCLUDED_BCES_ATOMICTYPES
#define INCLUDED_BCES_ATOMICTYPES

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide wrappers for atomic operations.
//
//@CLASSES:
//       bces_AtomicInt: atomic 32-bit integer wrapper
//     bces_AtomicInt64: atomic 64-bit integer wrapper
//   bces_AtomicPointer: parameterized atomic pointer wrapper
//        bces_SpinLock: spin lock wrapper
//
//@AUTHOR: Ilougino Rocha (irocha), Alexei Zakharov (azakhar1)
//
//@SEE_ALSO: bsls_atomic, bsls_atomicoperations
//
//@DESCRIPTION: This component provides convenient wrappers for the types and
// operations provided by the 'bces_atomicutil' component.  Classes
// 'bces_AtomicInt' and 'bces_AtomicInt64' wrap the corresponding atomic
// integers, and provide overloaded operators and functions for common
// arithmetic operations.  The class 'bces_AtomicPointer' provides atomic
// operations to manipulate and dereference a pointer.  The class
// 'bces_SpinLock' provides an object wrapper for a spin lock.
//
//@DEPRECATED: Use bsls_atomic components instead of bces_atomictypes, except
// for 'bces_SpinLock'.

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCES_ATOMICUTIL
#include <bces_atomicutil.h>
#endif

#ifndef INCLUDED_BSLS_ATOMICS
#include <bsls_atomic.h>
#endif

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
    // Permit reliance on transitive includes within robo.
#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#endif // BDE_OMIT_INTERNAL_DEPRECATED

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

namespace BloombergLP {

                               // ====================
                               // class bces_AtomicInt
                               // ====================

class bces_AtomicInt : private bsls::AtomicInt {
    // This class is a wrapper for an integer, supporting common integer
    // operations in a way that is guaranteed to be atomic.

  public:
    // CREATORS
    bces_AtomicInt();
        // Construct an atomic integer object and initialize it to 0.

    bces_AtomicInt(int value);
        // Construct an atomic integer object and initialize it to the
        // specified 'value'.

    bces_AtomicInt(const bces_AtomicInt& original);
        // Construct an atomic integer object and initialize it to the
        // value of the specified 'original' object.

    // ~bces_AtomicInt();
        // Destroy this atomic integer object.  Note that this trivial
        // destructor is generated by the compiler.

    // MANIPULATORS
    bces_AtomicInt& operator=(const bces_AtomicInt& rhs);
        // Atomically assign to this atomic integer object the value of the
        // specified 'rhs' object.

    bces_AtomicInt& operator=(int value);
        // Atomically assign to this atomic integer object the specified
        // 'value'.

    void operator+=(int value);
        // Atomically add to this atomic integer the specified 'value'.

    void operator-=(int value);
        // Atomically subtract from this atomic integer the specified 'value'.

    int operator++();
        // Atomically increment the value of this atomic integer by 1 and
        // return the resulting value.

    int operator++(int);
        // Atomically increment the value of this atomic integer by 1 and
        // return the value prior to being incremented.

    int operator--();
        // Atomically decrement the value of this atomic integer by 1 and
        // return the resulting value.

    int operator--(int);
        // Atomically decrement the value of this atomic integer by 1 and
        // return the value prior to being decremented.

    int add(int value);
        // Atomically add to this atomic integer the specified 'value' and
        // return the resulting value.

    int relaxedAdd(int value);
        // Atomically add to this atomic integer the specified 'value' and
        // return the resulting value, without additional ordering
        // constraints.

    void relaxedStore(int value);
        // Atomically assign to this atomic integer object the specified
        // 'value', without additional ordering constraints.

    int swap(int swapValue);
        // Atomically set the value of this atomic integer to the specified
        // 'swapValue' and return its previous value.

    int testAndSwap(int compareValue, int swapValue);
        // Compare the value of this atomic integer to the specified
        // 'compareValue'.  If they are equal, set the value of this atomic
        // integer to the specified 'swapValue', otherwise leave this value
        // unchanged.  Return the previous value of this atomic integer,
        // whether or not the swap occurred.  Note that the entire
        // test-and-swap operation is performed atomically.

    // ACCESSORS
    operator int() const;
        // Return the current value of this atomic integer.

    int relaxedLoad() const;
        // Return the current value of this atomic integer without a memory
        // barrier.
};

                       // ======================
                       // class bces_AtomicInt64
                       // ======================

class bces_AtomicInt64 : private bsls::AtomicInt64 {
    // This class is a wrapper for a 64-bit integer, supporting common integer
    // operations in a way that is guaranteed to be atomic.

  public:
    // CREATORS
    bces_AtomicInt64();
        // Construct an atomic 64-bit integer object and initialize it to 0.

    bces_AtomicInt64(bsls::Types::Int64 value);
        // Construct an atomic 64-bit integer object and initialize it to the
        // specified 'value'.

    bces_AtomicInt64(const bces_AtomicInt64& original);
        // Construct an atomic 64-bit integer object and initialize it to the
        // value of the specified 'original' object.

    // ~bces_AtomicInt64();
        // Destroy this atomic 64-bit integer object.  Note that this trivial
        // destructor is generated by the compiler.

    // MANIPULATORS
    bces_AtomicInt64& operator=(const bces_AtomicInt64& rhs);
        // Atomically assign to this atomic 64-bit integer object the value of
        // the specified 'rhs' object.

    bces_AtomicInt64& operator=(bsls::Types::Int64 value);
        // Atomically assign to this atomic 64-bit integer object the
        // specified 'value'.

    void operator+=(bsls::Types::Int64 value);
        // Atomically add to this atomic 64-bit integer the specified 'value'.

    void operator-=(bsls::Types::Int64 value);
        // Atomically subtract from this atomic 64-bit integer the specified
        // 'value'.

    bsls::Types::Int64 operator++();
        // Atomically increment the value of this atomic 64-bit integer by 1
        // and return the resulting value.

    bsls::Types::Int64 operator++(int);
        // Atomically increment the value of this atomic 64-bit integer by 1
        // and return the value prior to being incremented.

    bsls::Types::Int64 operator--();
        // Atomically decrement the value of this atomic 64-bit integer by 1
        // and return the resulting value.

    bsls::Types::Int64 operator--(int);
        // Atomically decrement the value of this atomic 64-bit integer by 1
        // and return the value prior to being decremented.

    bsls::Types::Int64 add(bsls::Types::Int64 value);
        // Atomically add to this atomic 64-bit integer the specified 'value'
        // and return the result value.

    bsls::Types::Int64 relaxedAdd(bsls::Types::Int64 value);
        // Atomically add to this atomic 64-bit integer the specified 'value'
        // and return the result value, without additional ordering
        // constraints.

    void relaxedStore(bsls::Types::Int64 value);
        // Atomically assign to this atomic 64-bit integer object the
        // specified 'value', without additional ordering constraints.

    bsls::Types::Int64 swap(bsls::Types::Int64 swapValue);
        // Atomically set the value of this atomic 64-bit integer to the
        // specified 'swapValue' and return its previous value.

    bsls::Types::Int64 testAndSwap(bsls::Types::Int64 compareValue,
                                   bsls::Types::Int64 swapValue);
        // Compare the value of this atomic 64-bit integer to the specified
        // 'compareValue'.  If they are equal, set the value of this atomic
        // 64-bit integer to the specified 'swapValue', otherwise leave this
        // value unchanged.  Return the previous value of this atomic 64-bit
        // integer, whether or not the swap occurred.  Note that the entire
        // test-and-swap operation is performed atomically.

    // ACCESSORS
    operator bsls::Types::Int64() const;
        // Return the value of this atomic 64-bit integer.

    bsls::Types::Int64 relaxedLoad() const;
        // Return the current value of this atomic 64-bit integer without
        // additional ordering constraints.
};

                     // ========================
                     // class bces_AtomicPointer
                     // ========================

template <class TYPE>
class bces_AtomicPointer : private bsls::AtomicPointer<TYPE> {
    // This class provides a wrapper for pointers to a parameterized 'TYPE',
    // supporting pointer operations in a way that is guaranteed to be atomic.

    // TYPES
    typedef bsls::AtomicPointer<TYPE> AtomicPointer;

  public:
    // CREATORS
    bces_AtomicPointer();
        // Construct an atomic pointer to an object of the parameterized
        // 'TYPE' and initialize it to null.

    bces_AtomicPointer(TYPE *value);
        // Construct an atomic pointer to an object of the parameterized 'TYPE'
        // and initialize it to the specified pointer 'value'.

    bces_AtomicPointer(const bces_AtomicPointer<TYPE>& original);
        // Construct an atomic pointer and initialize it to point to the
        // same 'TYPE' instance as the specified 'original' object.

    // ~bces_AtomicPointer();
        // Destroy this atomic pointer.  Note that this trivial destructor is
        // generated by the compiler.

    // MANIPULATORS
    bces_AtomicPointer<TYPE>& operator=(const bces_AtomicPointer<TYPE>& rhs);
        // Atomically assign to this atomic pointer the value of the specified
        // 'rhs' object.

    bces_AtomicPointer<TYPE>& operator=(TYPE *value);
        // Atomically assign to this atomic pointer the specified pointer
        // 'value'.

    void relaxedStore(TYPE *value);
        // Atomically assign to this atomic pointer the specified 'value',
        // without additional ordering constraints.

    TYPE *swap(TYPE *swapValue);
        // Atomically set the value of this atomic pointer to the specified
        // 'swapValue' and return the previous value of this atomic pointer.

    TYPE *testAndSwap(const TYPE *compareValue, TYPE *swapValue);
        // Compare the value of this atomic pointer to the specified
        // 'compareValue'.  If they are equal, set the value of this atomic
        // pointer to the specified 'swapValue', otherwise leave this value
        // unchanged.  Return the previous value of this atomic pointer,
        // whether or not the swap occurred.  Note that the entire
        // test-and-swap operation is performed atomically.

    // ACCESSORS
    TYPE& operator*() const;
        // Return a reference to the object currently pointed to by this atomic
        // pointer.  The behavior is undefined if this pointer has a value of
        // 0.

    TYPE *operator->() const;
        // Return the value of this atomic pointer.

    operator TYPE*() const;
        // Return the value of this atomic pointer.  Note that this method can
        // be used to determine if this pointer is null.

    TYPE *relaxedLoad() const;
        // Return the value of this atomic pointer without additional
        // ordering constraints.

};

                     // =========================
                     // class bces_AtomicSpinLock
                     // =========================

class bces_SpinLock {
    // This class is a wrapper to an atomic spin lock, supporting the 'lock',
    // 'tryLock' and 'unlock' methods.

    // DATA
    bces_AtomicUtil::SpinLock d_spin;

    // NOT IMPLEMENTED
    bces_SpinLock(const bces_SpinLock&);
    bces_SpinLock& operator=(const bces_SpinLock&);

  public:
    // CREATORS
    bces_SpinLock();
        // Construct a spin lock and initialize it to the unlock state.

    // ~bces_SpinLock();
        // Destroy this spin lock.  Note that this trivial destructor is
        // generated by the compiler.

    // MANIPULATORS
    void lock();
        // Lock this spinlock.

    int tryLock(int numRetries = 100);
        // Try to lock this spin lock.  Optionally specify 'numRetries'
        // indicating the maximum number of subsequent attempts to lock this
        // spin lock if the first attempt fails.  If 'numRetries' is not
        // specified, attempt to lock this spin-lock 100 times after the
        // initial attempt.  Return 0 on success, and a non-zero value if the
        // lock could not be acquired.

    void unlock();
        // Unlock this spinlock which was previously locked by a successful
        // call to either 'lock' or 'tryLock'.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

              // ---------------------------------------------
              // inlined methods used by other inlined methods
              // ---------------------------------------------

inline
bces_AtomicInt::operator int() const
{
    return bsls::AtomicInt::operator int();
}

inline
bces_AtomicInt64::operator bsls::Types::Int64() const
{
    return bsls::AtomicInt64::operator bsls::Types::Int64();
}

template <class TYPE>
inline
bces_AtomicPointer<TYPE>::operator TYPE*() const
{
    return AtomicPointer::operator TYPE*();
}

                               // --------------------
                               // class bces_AtomicInt
                               // --------------------

// CREATORS
inline
bces_AtomicInt::bces_AtomicInt()
{
}

inline
bces_AtomicInt::bces_AtomicInt(int value)
    : bsls::AtomicInt(value)
{
}

inline
bces_AtomicInt::bces_AtomicInt(const bces_AtomicInt& original)
    : bsls::AtomicInt(original.loadRelaxed())
{
}

// MANIPULATORS
inline
bces_AtomicInt& bces_AtomicInt::operator=(const bces_AtomicInt& rhs)
{
    bsls::AtomicInt::operator=(rhs.loadRelaxed());
    return *this;
}

inline
bces_AtomicInt& bces_AtomicInt::operator=(int value)
{
    bsls::AtomicInt::operator=(value);
    return *this;
}

inline
void bces_AtomicInt::operator+=(int value)
{
    bsls::AtomicInt::operator+=(value);
}

inline
void bces_AtomicInt::operator-=(int value)
{
    bsls::AtomicInt::operator-=(value);
}

inline
int bces_AtomicInt::operator++()
{
    return bsls::AtomicInt::operator++();
}

inline
int bces_AtomicInt::operator++(int)
{
    return bsls::AtomicInt::operator++(int());
}

inline
int bces_AtomicInt::operator--()
{
    return bsls::AtomicInt::operator--();
}

inline
int bces_AtomicInt::operator--(int)
{
    return bsls::AtomicInt::operator--(int());
}

inline
int bces_AtomicInt::add(int value)
{
    return bsls::AtomicInt::add(value);
}

inline
int bces_AtomicInt::relaxedAdd(int value)
{
    return bsls::AtomicInt::addRelaxed(value);
}

inline
void bces_AtomicInt::relaxedStore(int value)
{
    bsls::AtomicInt::storeRelaxed(value);
}

inline
int bces_AtomicInt::swap(int value)
{
    return bsls::AtomicInt::swap(value);
}

inline
int bces_AtomicInt::testAndSwap(int compareValue, int swapValue)
{
    return bsls::AtomicInt::testAndSwap(compareValue, swapValue);
}

// ACCESSORS
inline
int bces_AtomicInt::relaxedLoad() const
{
    return bsls::AtomicInt::loadRelaxed();
}

                       // ----------------------
                       // class bces_AtomicInt64
                       // ----------------------

// CREATORS
inline
bces_AtomicInt64::bces_AtomicInt64()
{
}

inline
bces_AtomicInt64::bces_AtomicInt64(bsls::Types::Int64 value)
    : bsls::AtomicInt64(value)
{
}

inline
bces_AtomicInt64::bces_AtomicInt64(const bces_AtomicInt64& original)
    : bsls::AtomicInt64(original.loadRelaxed())
{
}

// MANIPULATORS
inline
bces_AtomicInt64& bces_AtomicInt64::operator=(const bces_AtomicInt64& rhs)
{
    bsls::AtomicInt64::operator=(rhs.loadRelaxed());
    return *this;
}

inline
bces_AtomicInt64& bces_AtomicInt64::operator=(bsls::Types::Int64 value)
{
    bsls::AtomicInt64::operator=(value);
    return *this;
}

inline
void bces_AtomicInt64::operator+=(bsls::Types::Int64 value)
{
    bsls::AtomicInt64::operator+=(value);
}

inline
void bces_AtomicInt64::operator-=(bsls::Types::Int64 value)
{
    bsls::AtomicInt64::operator-=(value);
}

inline
bsls::Types::Int64 bces_AtomicInt64::operator++()
{
    return bsls::AtomicInt64::operator++();
}

inline
bsls::Types::Int64 bces_AtomicInt64::operator++(int)
{
    return bsls::AtomicInt64::operator++(int());
}

inline
bsls::Types::Int64 bces_AtomicInt64::operator--()
{
    return bsls::AtomicInt64::operator--();
}

inline
bsls::Types::Int64 bces_AtomicInt64::operator--(int)
{
    return bsls::AtomicInt64::operator--(int());
}

inline
bsls::Types::Int64 bces_AtomicInt64::add(bsls::Types::Int64 value)
{
    return bsls::AtomicInt64::add(value);
}

inline
bsls::Types::Int64 bces_AtomicInt64::relaxedAdd(bsls::Types::Int64 value)
{
    return bsls::AtomicInt64::addRelaxed(value);
}

inline
void bces_AtomicInt64::relaxedStore(bsls::Types::Int64 value)
{
    bsls::AtomicInt64::storeRelaxed(value);
}

inline
bsls::Types::Int64 bces_AtomicInt64::swap(bsls::Types::Int64 value)
{
    return bsls::AtomicInt64::swap(value);
}

inline
bsls::Types::Int64
 bces_AtomicInt64::testAndSwap(bsls::Types::Int64 compareValue,
                               bsls::Types::Int64 swapValue)
{
    return bsls::AtomicInt64::testAndSwap(compareValue, swapValue);
}

// ACCESSORS
inline
bsls::Types::Int64 bces_AtomicInt64::relaxedLoad() const
{
    return bsls::AtomicInt64::loadRelaxed();
}

                     // ------------------------
                     // class bces_AtomicPointer
                     // ------------------------

// CREATORS
template <class TYPE>
inline
bces_AtomicPointer<TYPE>::bces_AtomicPointer()
{
}

template <class TYPE>
inline
bces_AtomicPointer<TYPE>::bces_AtomicPointer(TYPE *value)
    : AtomicPointer(value)
{
}

template <class TYPE>
inline
bces_AtomicPointer<TYPE>::bces_AtomicPointer(
                                      const bces_AtomicPointer<TYPE>& original)
    : AtomicPointer(original.loadRelaxed())
{
}

// MANIPULATORS
template <class TYPE>
inline
bces_AtomicPointer<TYPE>&
bces_AtomicPointer<TYPE>::operator=(const bces_AtomicPointer<TYPE>& rhs)
{
    AtomicPointer::operator=(rhs.loadRelaxed());
    return *this;
}

template <class TYPE>
inline
bces_AtomicPointer<TYPE>&
bces_AtomicPointer<TYPE>::operator=(TYPE *value)
{
    AtomicPointer::operator=(value);
    return *this;
}

template <class TYPE>
inline
TYPE *bces_AtomicPointer<TYPE>::swap(TYPE *swapValue)
{
    return AtomicPointer::swap(swapValue);
}

template <class TYPE>
inline
TYPE *bces_AtomicPointer<TYPE>::testAndSwap(const TYPE *compareValue,
                                            TYPE       *swapValue)
{
    return AtomicPointer::testAndSwap(compareValue, swapValue);
}

template <class TYPE>
inline
void bces_AtomicPointer<TYPE>::relaxedStore(TYPE *value)
{
    AtomicPointer::storeRelaxed(value);
}

// ACCESSORS
template <class TYPE>
inline
TYPE& bces_AtomicPointer<TYPE>::operator*() const
{
    return AtomicPointer::operator*();
}

template <class TYPE>
inline
TYPE *bces_AtomicPointer<TYPE>::operator->() const
{
    return AtomicPointer::operator->();
}

template <class TYPE>
inline
TYPE *bces_AtomicPointer<TYPE>::relaxedLoad() const
{
    return AtomicPointer::loadRelaxed();
}

                     // -------------------------
                     // class bces_AtomicSpinLock
                     // -------------------------

// CREATORS
inline
bces_SpinLock::bces_SpinLock()
{
    bces_AtomicUtil::initSpinLock(&d_spin);
}

// MANIPULATORS
inline
void bces_SpinLock::lock()
{
    bces_AtomicUtil::spinLock(&d_spin);
}

inline
int bces_SpinLock::tryLock(int numRetries)
{
    return bces_AtomicUtil::spinTryLock(&d_spin, numRetries);
}

inline
void bces_SpinLock::unlock()
{
    bces_AtomicUtil::spinUnlock(&d_spin);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002, 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
