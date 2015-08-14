// bdlmxxx_bdemdecoderoptions.h   -*-C++-*-   GENERATED FILE -- DO NOT EDIT
#ifndef INCLUDED_BDLMXXX_BDEMDECODEROPTIONS
#define INCLUDED_BDLMXXX_BDEMDECODEROPTIONS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide value-semantic attribute classes
//
//@DEPRECATED: Do not use.
//
//@CLASSES:
// bdlmxxx::BdemDecoderOptions:
//
//@AUTHOR: Rohan Bhindwale (rbhindwale@bloomberg.net)
//
//@DESCRIPTION:
// Schema of options records for bdem codecs

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BDLAT_ATTRIBUTEINFO
#include <bdlat_attributeinfo.h>
#endif

#ifndef INCLUDED_BDLAT_TYPETRAITS
#include <bdlat_typetraits.h>
#endif

#ifndef INCLUDED_BDLAT_VALUETYPEFUNCTIONS
#include <bdlat_valuetypefunctions.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BDLXXXX_INSTREAMFUNCTIONS
#include <bdlxxxx_instreamfunctions.h>
#endif

#ifndef INCLUDED_BDLXXXX_OUTSTREAMFUNCTIONS
#include <bdlxxxx_outstreamfunctions.h>
#endif

#ifndef INCLUDED_BDLB_PRINTMETHODS
#include <bdlb_printmethods.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

namespace bdlmxxx {

                    // ===================================
                    // class BdemDecoderOptions
                    // ===================================

class BdemDecoderOptions {
    // BDEM decoding options

  private:
    int  d_bdemVersion;
        // BDEM decoding version
    int  d_maxDepth;
        // maximum recursion depth

  public:
    // TYPES
    enum {
        NUM_ATTRIBUTES = 2
    };

    enum {
        ATTRIBUTE_INDEX_BDEM_VERSION = 0,
        ATTRIBUTE_INDEX_MAX_DEPTH = 1
    };

    enum {
        ATTRIBUTE_ID_BDEM_VERSION = 0,
        ATTRIBUTE_ID_MAX_DEPTH = 1
    };

  public:
    // CONSTANTS
    static const char CLASS_NAME[];

    static const int DEFAULT_BDEM_VERSION;

    static const int DEFAULT_MAX_DEPTH;

    static const bdeat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    static const bdeat_AttributeInfo *lookupAttributeInfo(int id);
        // Return attribute information for the attribute indicated by the
        // specified 'id' if the attribute exists, and 0 otherwise.

    static const bdeat_AttributeInfo *lookupAttributeInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return attribute information for the attribute indicated by the
        // specified 'name' of the specified 'nameLength' if the attribute
        // exists, and 0 otherwise.

    // CREATORS
    BdemDecoderOptions();
        // Create an object of type 'BdemDecoderOptions' having the
        // default value.

    BdemDecoderOptions(const BdemDecoderOptions& original);
        // Create an object of type 'BdemDecoderOptions' having the
        // value of the specified 'original' object.

    // ~BdemDecoderOptions();
        // Destroy this object.  Note that this trivial destructor is generated
        // by the compiler.

    // MANIPULATORS
    BdemDecoderOptions& operator=(const BdemDecoderOptions& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template<class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'name' of the
        // specified 'nameLength', supplying 'manipulator' with the
        // corresponding attribute information structure.  Return the value
        // returned from the invocation of 'manipulator' if 'name' identifies
        // an attribute of this class, and -1 otherwise.

    void setBdemVersion(int value);
        // Set the "BdemVersion" attribute of this object to the specified
        // 'value'.

    void setMaxDepth(int value);
        // Set the "MaxDepth" attribute of this object to the specified
        // 'value'.

    // ACCESSORS
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // optionally specified indentation 'level' and return a reference to
        // the modifiable 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation level
        // for this and all of its nested objects.  Each line is indented by
        // the absolute value of 'level * spacesPerLevel'.  If 'level' is
        // negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, suppress line breaks and format the
        // entire output on one line.  If 'stream' is initially invalid, this
        // operation has no effect.  Note that a trailing newline is provided
        // in multiline mode only.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.

    template<class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    const int& bdemVersion() const;
        // Return a reference to the non-modifiable "BdemVersion" attribute of
        // this object.

    const int& maxDepth() const;
        // Return a reference to the non-modifiable "MaxDepth" attribute of
        // this object.
};

// FREE OPERATORS
inline
bool operator==(const BdemDecoderOptions& lhs,
                const BdemDecoderOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const BdemDecoderOptions& lhs,
                const BdemDecoderOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream,
                         const BdemDecoderOptions& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

BDLAT_DECL_SEQUENCE_TRAITS(bdlmxxx::BdemDecoderOptions)

namespace bdlmxxx {

// TRAITS

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================


                    // -----------------------------------
                    // class BdemDecoderOptions
                    // -----------------------------------

// CLASS METHODS
inline
int BdemDecoderOptions::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// CREATORS
inline
BdemDecoderOptions::BdemDecoderOptions()
: d_bdemVersion(DEFAULT_BDEM_VERSION)
, d_maxDepth(DEFAULT_MAX_DEPTH)
{
}

inline
BdemDecoderOptions::BdemDecoderOptions(
                                       const BdemDecoderOptions& original)
: d_bdemVersion(original.d_bdemVersion)
, d_maxDepth(original.d_maxDepth)
{
}

// MANIPULATORS
inline
BdemDecoderOptions&
BdemDecoderOptions::operator=(const BdemDecoderOptions& rhs)
{
    if (this != &rhs) {
        d_bdemVersion = rhs.d_bdemVersion;
        d_maxDepth = rhs.d_maxDepth;
    }
    return *this;
}

template <class STREAM>
inline
STREAM& BdemDecoderOptions::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_bdemVersion, 1);
            bdex_InStreamFunctions::streamIn(stream, d_maxDepth, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

inline
void BdemDecoderOptions::reset()
{
    d_bdemVersion = DEFAULT_BDEM_VERSION;
    d_maxDepth = DEFAULT_MAX_DEPTH;
}

template <class MANIPULATOR>
inline
int BdemDecoderOptions::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_bdemVersion,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BDEM_VERSION]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_maxDepth,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_DEPTH]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
inline
int BdemDecoderOptions::manipulateAttribute(MANIPULATOR& manipulator,
                                                 int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_BDEM_VERSION: {
        return manipulator(&d_bdemVersion,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BDEM_VERSION]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_MAX_DEPTH: {
        return manipulator(&d_maxDepth,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_DEPTH]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;                                             // RETURN
    }
}

template <class MANIPULATOR>
inline
int BdemDecoderOptions::manipulateAttribute(
        MANIPULATOR&  manipulator,
        const char   *name,
        int           nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;                                             // RETURN
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

inline
void BdemDecoderOptions::setBdemVersion(int value)
{
    d_bdemVersion = value;
}

inline
void BdemDecoderOptions::setMaxDepth(int value)
{
    d_maxDepth = value;
}

// ACCESSORS
template <class STREAM>
inline
STREAM& BdemDecoderOptions::bdexStreamOut(STREAM& stream,
                                               int version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_bdemVersion, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_maxDepth, 1);
      } break;
    }
    return stream;
}

template <class ACCESSOR>
inline
int BdemDecoderOptions::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_bdemVersion,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BDEM_VERSION]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_maxDepth,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_DEPTH]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
inline
int BdemDecoderOptions::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_BDEM_VERSION: {
        return accessor(d_bdemVersion,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BDEM_VERSION]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_MAX_DEPTH: {
        return accessor(d_maxDepth,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_DEPTH]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;                                             // RETURN
    }
}

template <class ACCESSOR>
inline
int BdemDecoderOptions::accessAttribute(
        ACCESSOR&   accessor,
        const char *name,
        int         nameLength) const
{
    enum { NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return NOT_FOUND;                                              // RETURN
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

inline
const int& BdemDecoderOptions::bdemVersion() const
{
    return d_bdemVersion;
}

inline
const int& BdemDecoderOptions::maxDepth() const
{
    return d_maxDepth;
}
}  // close package namespace


// FREE FUNCTIONS

inline
bool bdlmxxx::operator==(
        const BdemDecoderOptions& lhs,
        const BdemDecoderOptions& rhs)
{
    return  lhs.bdemVersion() == rhs.bdemVersion()
         && lhs.maxDepth() == rhs.maxDepth();
}

inline
bool bdlmxxx::operator!=(
        const BdemDecoderOptions& lhs,
        const BdemDecoderOptions& rhs)
{
    return  lhs.bdemVersion() != rhs.bdemVersion()
         || lhs.maxDepth() != rhs.maxDepth();
}

inline
bsl::ostream& bdlmxxx::operator<<(
        bsl::ostream& stream,
        const BdemDecoderOptions& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close enterprise namespace
#endif

// GENERATED BY BLP_BAS_CODEGEN_3.0.19 Thu Jul 31 13:45:53 2008
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
