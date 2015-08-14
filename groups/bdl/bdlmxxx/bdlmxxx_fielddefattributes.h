// bdlmxxx_fielddefattributes.h                                       -*-C++-*-
#ifndef INCLUDED_BDLMXXX_FIELDDEFATTRIBUTES
#define INCLUDED_BDLMXXX_FIELDDEFATTRIBUTES

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide the attributes for the definition of a field.
//
//@CLASSES:
//   bdlmxxx::FieldDefAttributes: attributes for the definition of a field
//
//@SEE_ALSO: bdlmxxx_schema, bdlmxxx_fielddef, bdlmxxx_recorddef
//
//@AUTHOR: Tom Marshall, Henry Verschell
//
//@DESCRIPTION: This component provides a variant of a complex constrained
// (value-semantic) attribute type, 'bdlmxxx::FieldDefAttributes', that describes
// the attributes used by a field definition to define a field.  A field, in
// this context, is a single value of a type supported by the 'bdem' package
// (see 'bdlmxxx_elemtype'), generally in the context of a record (or sequence of
// fields).  The attributes comprised by a 'bdlmxxx::FieldDefAttributes' object
// include the data type, default value, nullability, and formatting-mode of a
// field.  The 'elemType' defines the data type of the field.  The
// 'isNullable' flag indicates whether values of the described field may be
// null.  The formattingMode' is one of the enumerated mode values defined by
// 'bdeat_FormattingMode' (e.g., 'BDEAT_DEC', 'BDEAT_HEX', 'BDEAT_BASE64,
// etc).  The 'formattingMode' can be used to determine how a
// field's value should be written as text (e.g., if the value were being
// serialized as XML).  A default constructed 'bdlmxxx::FieldDefAttributes' object
// will have an element type 'BDEM_VOID', a formatting mode of
// 'bdeat_FormattingMode::BDEAT_DEFAULT', and both an 'isNullable' and
// 'hasDefaultValue' of 'false'.  The 'elemType' of a field def attributes
// object constrains the valid values for its 'defaultValue' to those of the
// indicated type.   However, these two attributes can not be set
// simulatneously (e.g., via 'reset'), clients must first reset the element
// type attribute (in turn setting default-value to null), and then assign a
// value (of that type to the element referenced returned by 'defaultValue()'.
//
///Usage
///-----
// The following example illustrates how to create a 'bdlmxxx::FieldDefAttributes'
// object, and both set and access its individual attribute values.
//
// We start by creating a 'bdlmxxx::FieldDefAttributes' objects for a couple of
// specific 'bdem' types:
//..
//  bslma::Allocator *allocator = bslma::Default::allocator();
//  bdlmxxx::FieldDefAttributes  intType(bdlmxxx::ElemType::BDEM_INT,  allocator);
//  bdlmxxx::FieldDefAttributes listType(bdlmxxx::ElemType::BDEM_LIST, allocator);
//..
// We verify the properties of the newly created field types:
//..
//  assert(bdlmxxx::ElemType::BDEM_INT   == intType.elemType());
//  assert(bdlmxxx::ElemType::BDEM_LIST  == listType.elemType());
//
//  assert(false == intType.hasDefaultValue());
//  assert(false == listType.hasDefaultValue());
//
//  assert(false == intType.isNullable());
//  assert(false == listType.isNullable());
//
//  assert(bdeat_FormattingMode::BDEAT_DEFAULT == intType.formattingMode());
//  assert(bdeat_FormattingMode::BDEAT_DEFAULT == listType.formattingMode());
//..
// We assign the 'isNullable' and 'formattingMode' for the two field types.
// Note that, depending on the context in which the field types are used,
// these formatting-modes may, or may not, be appropriate:
//..
//  intType.setIsNullable(true);
//  listType.setIsNullable(true);
//
//  intType.setFormattingMode(bdeat_FormattingMode::BDEAT_HEX);
//  listType.setFormattingMode(bdeat_FormattingMode::BDEAT_DEC);
//
//  assert(bdeat_FormattingMode::BDEAT_HEX == intType.formattingMode());
//  assert(bdeat_FormattingMode::BDEAT_DEC == listType.formattingMode());
//
//  assert(true == intType.isNullable());
//  assert(true == listType.isNullable());
//..
// We set the default value for 'intType'.  We do not set the default value
// for 'listType' because only scalar 'bdem' types may have a default
// field value:
//..
//  intType.defaultValue().theModifiableInt() = 100;
//  // listType.defaultValue();   // *error*, undefined behavior.
//
//  assert(true  == intType.hasDefaultValue());
//  assert(false == listType.hasDefaultValue());
//  assert(1     == intType.defaultValue().theInt());
//..
// Finally, we write the two field types to the console:
//..
//  bsl::cout << "intType:  " << intType  << bsl::endl;
//  bsl::cout << "listType: " << listType << bsl::endl;
//..
// The resulting console output looks like:
//..
//  intType:  { INT nullable 100 0x2 }
//  listType: { LIST nullable NO_DEFAULT 0x1 }
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLMXXX_ELEMREF
#include <bdlmxxx_elemref.h>
#endif

#ifndef INCLUDED_BDLMXXX_ELEMTYPE
#include <bdlmxxx_elemtype.h>
#endif

#ifndef INCLUDED_BDLAT_FORMATTINGMODE
#include <bdlat_formattingmode.h>
#endif

#ifndef INCLUDED_BDLT_DATE
#include <bdlt_date.h>
#endif

#ifndef INCLUDED_BDLT_DATETZ
#include <bdlt_datetz.h>
#endif

#ifndef INCLUDED_BDLT_DATETIME
#include <bdlt_datetime.h>
#endif

#ifndef INCLUDED_BDLT_DATETIMETZ
#include <bdlt_datetimetz.h>
#endif

#ifndef INCLUDED_BDLT_TIME
#include <bdlt_time.h>
#endif

#ifndef INCLUDED_BDLT_TIMETZ
#include <bdlt_timetz.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITBITWISEMOVEABLE
#include <bslalg_typetraitbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITUSESBSLMALLOCATOR
#include <bslalg_typetraitusesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLS_OBJECTBUFFER
#include <bsls_objectbuffer.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

namespace bdlmxxx {
                      // =============================
                      // class FieldDefAttributes
                      // =============================

class FieldDefAttributes {
    // This (value-semantic) attribute class aggregates a set of attribute
    // values that are used to define a field.  A 'FieldDefAttributes'
    // object comprises the values of the following attributes:
    //..
    //  (1) its 'elemType', which is one of the enumerated 'ElemType'
    //      types,
    //  (2) an indication as to whether the field may be null,
    //  (3) an indication as to whether the field has a default value
    //     (aggregate types -- i.e., 'LIST', 'TABLE', 'CHOICE', and
    //     'CHOICE_ARRAY' -- never do), and
    //  (4) an optional default value for fields that are of scalar or
    //      array type.
    //  (5) a formatting-mode, which is one of the modes enumerated in
    //     'bdeat_FormattingMode', describing how the field should be
    //      written to text.
    //..
    // More generally, this class supports a nearly complete set of *value*
    // *semantic* operations (it does not support bdex streaming), including
    // copy construction, assignment, equality comparison, and 'ostream'
    // printing.  This class is *exception* *neutral* with no guarantee of
    // rollback: if an exception is thrown during the invocation of a method
    // on a pre-existing object, the object is left in a valid state, but its
    // value is undefined.  In no event is memory leaked.  Finally, *aliasing*
    // (e.g., using all or part of an object as both source and destination)
    // is supported in all cases.

    // PRIVATE TYPES
    union DefaultValue {
        // Union capable of holding an object of any 'bdem' element type that
        // can have a default value.
        //
        // Note that all of the member types must be bitwise movable.  The
        // implementation of 'swap' depends on that trait to swap the object
        // buffer content without the knowledge of the concrete type of object
        // stored in the buffer.

        // DATA
        bsls::ObjectBuffer<bool>               d_bool;
        bsls::ObjectBuffer<char>               d_char;
        bsls::ObjectBuffer<short>              d_short;
        bsls::ObjectBuffer<int>                d_int;
        bsls::ObjectBuffer<bsls::Types::Int64> d_int64;
        bsls::ObjectBuffer<float>              d_float;
        bsls::ObjectBuffer<double>             d_double;
        bsls::ObjectBuffer<bsl::string>        d_string;
        bsls::ObjectBuffer<bdlt::Datetime>      d_datetime;
        bsls::ObjectBuffer<bdlt::DatetimeTz>    d_datetimeTz;
        bsls::ObjectBuffer<bdlt::Date>          d_date;
        bsls::ObjectBuffer<bdlt::DateTz>        d_dateTz;
        bsls::ObjectBuffer<bdlt::Time>          d_time;
        bsls::ObjectBuffer<bdlt::TimeTz>        d_timeTz;
    };

    enum Flags {
        // The following enumerators are used for managing 'd_flags'.  The
        // "default value is null" flag is initially 1, indicating that the
        // default value is null, i.e., there is *no* default value (the 1 bit
        // is used as the nullness bit, so it is toggled to 0 if and when a
        // default value is provided via the 'defaultValue()' manipulator).

        IS_NULLABLE_FLAG              = 0x1,

        DEFAULT_VALUE_IS_NULL_BIT_POS = 1,
        DEFAULT_VALUE_IS_NULL_FLAG    = 0x1 << DEFAULT_VALUE_IS_NULL_BIT_POS
    };

    // DATA
    ElemType::Type  d_elemType;        // element type of field
    int                  d_flags;           // see enum 'Flags' above
    DefaultValue         d_defaultValue;    // scalar default value
    int                  d_formattingMode;  // formatting-mode of field
    bslma::Allocator    *d_allocator_p;     // memory allocator (held, not
                                            // owned)
  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS2(FieldDefAttributes,
                                  bslalg::TypeTraitBitwiseMoveable,
                                  bslalg::TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit
    FieldDefAttributes(bslma::Allocator    *basicAllocator = 0);
    FieldDefAttributes(ElemType::Type  type,
                            bslma::Allocator    *basicAllocator = 0);
    FieldDefAttributes(ElemType::Type  type,
                            bool                 nullabilityFlag,
                            bslma::Allocator    *basicAllocator = 0);
    FieldDefAttributes(ElemType::Type  type,
                            bool                 nullabilityFlag,
                            int                  formattingMode,
                            bslma::Allocator    *basicAllocator = 0);
        // Create a field definition attributes object.  Optionally specify
        // the 'type', indicating the data type of a field described by these
        // attributes.  If no 'type' is specified, 'elemType()' will default to
        // 'BDEM_VOID'.  If 'type' is specified, optionally specify a
        // 'nullabilityFlag', indicating whether a field described by these
        // attributes may be null.  If a 'nullabilityFlag' is not provided,
        // 'isNullable()' will default to 'false'.   If 'nullabilityFlag' is
        // specified, optionally specify a 'formattingMode', indicating how a
        // field described by these attributes should be written to text.  If
        // no 'formattingMode' is provided, 'formattingMode' will default to
        // 'bdeat_FormattingMode::BDEAT_DEFAULT'.

    FieldDefAttributes(
                           const FieldDefAttributes&  original,
                           bslma::Allocator               *basicAllocator = 0);
        // Create a field definition attributes object having the value of the
        // specified 'original' field definition attributes.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    ~FieldDefAttributes();
        // Destroy this field-definition-attributes object.

    // MANIPULATORS
    FieldDefAttributes& operator=(const FieldDefAttributes& rhs);
        // Assign to this field-definition-attributes object the value of the
        // specified 'rhs' object, and return a reference to this modifiable
        // object.

    ElemRef defaultValue();
        // Return a 'bdem' element reference to the modifiable default value
        // of this object.  If 'hasDefaultValue()' would have initially
        // returned 'false', a valid element reference to the a default-value
        // object of the appropriate type (having the null value) is
        // returned.  If the element type of this attributes is an array of
        // scalars, then the default value will be of the corresponding scalar
        // type.  The behavior is undefined if the type attribute of this
        // object is an aggregate type -- i.e., 'LIST', 'TABLE', 'CHOICE', or
        // 'CHOICE_ARRAY'.

    void reset(ElemType::Type type,
               bool                nullabilityFlag = false,
               int                 formattingMode  =
                                          bdeat_FormattingMode::BDEAT_DEFAULT);
        // Reset this field definition attributes to have the specified
        // 'type'.  Optionally specify a 'nullabilityFlag' indicating whether
        // a field described by these attributes may be null.  If
        // 'nullabilityFlag' is not specified, 'isNullable()' will default to
        // 'false'.  If 'nullabilityFlag' is specified, optionally specify a
        // 'formattingMode' indicating how a field described by these
        // attributes should be written as text.  If 'formattingMode' is not
        // specified, 'formattingMode()' will default to
        // 'bdeat_FormattingMode::BDEAT_DEFAULT'.  The behavior is undefined
        // unless 'formattingMode' is one of the enumerated modes
        // defined by 'bdeat_FormattingMode'.

    void setIsNullable(bool value);
        // Set the 'isNullable' attribute to the specified 'value'.  Note that
        // this attribute value indicates whether a field characterized by this
        // object may be null.

    void setFormattingMode(int value);
        // Set the formatting-mode of this object to the specified 'value'.
        // The behavior is undefined unless 'formattingMode' is a
        // valid mode value defined by 'bdeat_FormattingMode' (e.g.,
        // 'BDEAT_DEFAULT', 'BDEAT_DEC', 'BDEAT_HEX', etc).  Note that this
        // value is used to determine how a field described by these
        // attributes should be written to text.

    void swap(FieldDefAttributes& other);
        // Swap the value of this object with the value of the specified
        // 'other' object.  This method provides the no-throw guarantee.  The
        // behavior is undefined if the two objects being swapped have
        // non-equal allocators.

    // ACCESSORS
    ConstElemRef defaultValue() const;
        // Return a 'bdem' const-element reference to the default value
        // of this object.  If 'hasDefaultValue()' would have initially
        // returned 'false', a valid element reference to the a default-value
        // object of the appropriate type (having the null value) is returned.
        // If the element type of this attributes is an array of scalars, then
        // the default value will be of the corresponding scalar type.  The
        // behavior is undefined if the type attribute of this object is of an
        // aggregate type -- i.e., 'LIST', 'TABLE', 'CHOICE', or
        // 'CHOICE_ARRAY'.

    ElemType::Type elemType() const;
        // Return the 'bdem' element type attribute value of this object.

    int formattingMode() const;
        // Return the formatting-mode attribute of this object.  The returned
        // value will be one of the mode values defined by
        // 'bdeat_FormattingMode' (e.g., 'BDEAT_DEFAULT', 'BDEAT_DEC',
        // 'BDEAT_HEX').  Note that this value is used to determine how a
        // field described by these attributes should be formatted in
        // text.

    bool hasDefaultValue() const;
        // Return 'true' if this object has a (non-null) default value, and
        // 'false' otherwise.  Note that 'false' is returned if
        // 'elemType()' is an aggregate type -- i.e., 'LIST', 'TABLE',
        // 'CHOICE', or 'CHOICE_ARRAY'.

    bool isNullable() const;
        // Return 'true' if a field characterized by this object may have a
        // null value, and 'false' otherwise'.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the (absolute
        // value of) the optionally specified indentation 'level' and return a
        // reference to 'stream'.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', the number of spaces per indentation level for
        // this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  If 'stream' is
        // not valid on entry, this operation has no effect.
};

// FREE OPERATORS
bool operator==(const FieldDefAttributes& lhs,
                const FieldDefAttributes& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs'
    // field-definition-attribute objects have the same value, and 'false'
    // otherwise.  Two field-definition-attribute objects have the same value
    // if they have the same respective data type, formatting-mode,
    // nullability value, and, in the case of 'bdem' scalar and array types,
    // have the same default value (or both have no default value).

inline
bool operator!=(const FieldDefAttributes& lhs,
                const FieldDefAttributes& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs'
    // field-definition-attribute objects do not have the same value, and
    // 'false' otherwise.  Two field-definition-attribute objects do not have
    // the same value if they do not have the same respective data type,
    // formatting-mode, nullability, or, in the case of 'bdem' scalar and
    // array types, default value.

inline
bsl::ostream& operator<<(bsl::ostream&                  stream,
                         const FieldDefAttributes& attributes);
}  // close package namespace
    // Write the specified 'attributes' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

// FREE FUNCTIONS
void swap(bdlmxxx::FieldDefAttributes& a, bdlmxxx::FieldDefAttributes& b);

namespace bdlmxxx {    // Swap the values of the specified 'a' and 'b' objects.  This method
    // provides the no-throw guarantee.  The behavior is undefined if the two
    // objects being swapped have non-equal allocators.

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                      // -----------------------------
                      // class FieldDefAttributes
                      // -----------------------------

// MANIPULATORS
inline
void FieldDefAttributes::setFormattingMode(int value)
{
    d_formattingMode = value;
}

inline
void FieldDefAttributes::setIsNullable(bool value)
{
    if (value) {
        d_flags |= IS_NULLABLE_FLAG;
    }
    else {
        d_flags &= ~IS_NULLABLE_FLAG;
    }
}

// ACCESSORS
inline
ConstElemRef FieldDefAttributes::defaultValue() const
{
    return const_cast<FieldDefAttributes *>(this)->defaultValue();
}

inline
ElemType::Type FieldDefAttributes::elemType() const
{
    return d_elemType;
}

inline
int FieldDefAttributes::formattingMode() const
{
    return d_formattingMode;
}

inline
bool FieldDefAttributes::hasDefaultValue() const
{
    // 'hasDefaultValue' is 'true' if there is a non-null default value (i.e.,
    // the DEFAULT_VALUE_IS_NULL_FLAG is 'false'), and 'false' otherwise.

    return !(d_flags & DEFAULT_VALUE_IS_NULL_FLAG);
}

inline
bool FieldDefAttributes::isNullable() const
{
    return d_flags & IS_NULLABLE_FLAG;
}
}  // close package namespace

// FREE OPERATORS
inline
bool bdlmxxx::operator!=(const FieldDefAttributes& lhs,
                const FieldDefAttributes& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& bdlmxxx::operator<<(bsl::ostream&                  stream,
                         const FieldDefAttributes& attributes)
{
    return attributes.print(stream, 0, -1);
}

// FREE FUNCTIONS
inline
void swap(bdlmxxx::FieldDefAttributes& a, bdlmxxx::FieldDefAttributes& b)
{
    a.swap(b);
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
