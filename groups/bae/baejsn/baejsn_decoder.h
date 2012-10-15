#ifndef INCLUDED_BAEJSN_DECODER
#define INCLUDED_BAEJSN_DECODER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a JSON decoder class
//
//@CLASSES:
//
//@SEE_ALSO:
//
//@AUTHOR: Raymond Chiu (schiu49)
//
//@DESCRIPTION: This component provides utility functions for
// decoding a JSON string.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BAEJSN_PARSERUTIL
#include <baejsn_parserutil.h>
#endif

#include <stdint.h>

#include <iomanip>
#include <sstream>
#include <limits>
#include <set>
#include <vector>

#include <bslmf_if.h>
#include <bdeut_nullablevalue.h>
#include <bdeat_attributeinfo.h>
#include <bdeat_sequencefunctions.h>
#include <bdeat_choicefunctions.h>
#include <bdeat_enumfunctions.h>
#include <bdeat_typecategory.h>
#include <bdeat_formattingmode.h>
#include <bdeat_valuetypefunctions.h>
#include <bsl_sstream.h>

#ifndef INCLUDED_BDESB_MEMOUTSTREAMBUF
#include <bdesb_memoutstreambuf.h>
#endif

#ifndef INCLUDED_BSL_STREAMBUF
#include <bsl_streambuf.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#include <iostream>  // TBD: Remove later

namespace BloombergLP {

// TBD: Remove this macro
//!
//! Use this macro to throw a new exception of a type that accepts a single string argument in constructor
//!
//! For example BAEJSN_THROW(mobcmn::TransactionalException, "Some message: " << additionalInfo)
//!
#define BAEJSN_THROW(ExceptionClassName, Message) do { \
    logStream() << Message; \
    throw d_logStream->str(); \
  } while(false)

                        // ====================
                        // class baejsn_Decoder
                        // ====================

class baejsn_Decoder
{
    // DATA
    //bslma_Allocator                 *d_allocator;    // held, not owned

    bsls_ObjectBuffer<bsl::ostringstream>  d_logArea;      // placeholder for
                                                     // MemOutStream

    bsl::ostringstream              *d_logStream;    // if not zero,
                                                     // log stream was
                                                     // created at the
                                                     // moment of first
                                                     // logging and must
                                                     // be destroyed

    bsl::streambuf                  *d_streamBuf;    // held, not owned


    friend struct baejsn_Decoder_DecodeImpProxy;
    friend struct baejsn_Decoder_ElementVisitor;

  private:
    // PRIVATE MANIPULATORS
    bsl::ostream& logStream();
        // Return the stream for logging.  Note the if stream has not
        // been created yet, it will be created during this call.

    template <typename TYPE>
    int decodeChooser(TYPE *value, bdeat_TypeCategory::DynamicType);

    template <typename TYPE>
    int decodeChooser(TYPE *value, bdeat_TypeCategory::Sequence);

    template <typename TYPE>
    int decodeChooser(TYPE *value, bdeat_TypeCategory::Choice);

    template <typename TYPE>
    int decodeChooser(TYPE *value, bdeat_TypeCategory::Enumeration);

    template <typename TYPE>
    int decodeChooser(TYPE *value, bdeat_TypeCategory::CustomizedType);

    template <typename TYPE>
    int decodeChooser(TYPE *value, bdeat_TypeCategory::Simple);

    template <typename TYPE>
    int decodeChooser(TYPE *value, bdeat_TypeCategory::Array);

    template <typename TYPE>
    int decodeChooser(TYPE *value, bdeat_TypeCategory::NullableValue);

    template <typename TYPE, typename TYPE_CATEGORY>
    int decodeChooser(TYPE *, const TYPE_CATEGORY&);

    template <typename TYPE>
    int decodeNumber(TYPE *value);

    int decodeSimple(bool *value);

    int decodeSimple(char *value);

    int decodeSimple(short *value);
    int decodeSimple(int *value);
    int decodeSimple(bsls::Types::Int64 *value);
    int decodeSimple(unsigned short *value);
    int decodeSimple(unsigned int *value);
    int decodeSimple(bsls::Types::Uint64 *value);
    int decodeSimple(float *value);
    int decodeSimple(double *value);
    int decodeSimple(unsigned char *value);


    int decodeSimple(bdet_Time *value);
    int decodeSimple(bdet_Date *value);
    int decodeSimple(bdet_Datetime *value);
    int decodeSimple(bdet_TimeTz *value);
    int decodeSimple(bdet_DateTz *value);
    int decodeSimple(bdet_DatetimeTz *value);

    template <typename TYPE>
    int decodeIso8601(TYPE *value);

    template <typename TYPE>
    int decodeNullable(TYPE *value);

    int decodeSimple(std::string *value);

    int decodeArray(std::vector<char> *value);;

    template <typename TYPE>
    int decodeArray(TYPE *value);

    template <typename TYPE>
    int decodeSequence(TYPE *value);

    template <typename TYPE>
    int decodeChoice(TYPE *value);

    template <typename TYPE>
    int decodeEnumeration(TYPE *value);

    template <typename TYPE>
    int decodeCustom(TYPE *value);

    template <typename TYPE>
    int decode(TYPE *value);

  public:
    // CREATORS
    baejsn_Decoder();
        // Create a decoder object.

    template <typename TYPE>
    int decode(bsl::streambuf *streamBuf, TYPE *variable);
        // Decode an object of parameterized 'TYPE' from the specified
        // 'streamBuf' and load the result into the specified modifiable
        // 'variable'.  Return 0 on success, and a non-zero value otherwise.

    template <typename TYPE>
    int decode(bsl::istream& stream, TYPE *variable);
        // Decode an object of parameterized 'TYPE' from the specified 'stream'
        // and load the result into the specified modifiable 'variable'.
        // Return 0 on success, and a non-zero value otherwise.  Note that
        // 'stream' will be invalidated if the decoding fails.

    bsl::string loggedMessages() const;
        // Return a string containing any error, warning, or trace messages
        // that were logged during the last call to the 'decode' method.  The
        // log is reset each time 'decode' is called.
};

                 // ====================================
                 // struct baejsn_Decoder_ElementVisitor
                 // ====================================

struct baejsn_Decoder_ElementVisitor {
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.

    // DATA
    baejsn_Decoder *d_decoder;

    // CREATORS
    // Creators have been omitted to allow simple static initialization of
    // this struct.

    // MANIPULATORS
    template <typename TYPE>
    int operator()(TYPE value);

    template <typename TYPE, typename ATTRIBUTE_OR_SELECTION>
    int operator()(TYPE value, const ATTRIBUTE_OR_SELECTION &);
};

                 // ====================================
                 // struct baejsn_Decoder_DecodeImpProxy
                 // ====================================

struct baejsn_Decoder_DecodeImpProxy {
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.

    // DATA
    baejsn_Decoder *d_decoder;

    // CREATORS
    // Creators have been omitted to allow simple static initialization of
    // this struct.

    // MANIPULATORS
    template <typename TYPE>
    int operator()(TYPE *, bslmf_Nil);

    template <typename TYPE, typename ANY_CATEGORY>
    int operator()(TYPE *object, ANY_CATEGORY category);
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // --------------------
                        // class baejsn_Decoder
                        // --------------------

// PRIVATE MANIPULATORS
template <typename TYPE>
inline
int baejsn_Decoder::decodeChooser(TYPE *value, bdeat_TypeCategory::DynamicType)
{
    baejsn_Decoder_DecodeImpProxy proxy = { this };
    return bdeat_TypeCategoryUtil::manipulateByCategory(value, proxy);
}

template <typename TYPE>
inline
int baejsn_Decoder::decodeChooser(TYPE *value, bdeat_TypeCategory::Sequence)
{
    // TBD: need to specialize for empty sequence.
    //typedef typename bslmf_If< TYPE::NUM_ATTRIBUTES != 0, IsSequence, IsSequenceEmpty >::Type TypeTag;
    //decodeChooserSequence(value, TypeTag());
    return decodeSequence(value);
}

template <typename TYPE>
inline
int baejsn_Decoder::decodeChooser(TYPE *value, bdeat_TypeCategory::Choice)
{
    return decodeChoice(value);
}

template <typename TYPE>
inline
int baejsn_Decoder::decodeChooser(TYPE *value, bdeat_TypeCategory::Enumeration)
{
    return decodeEnumeration(value);
}

template <typename TYPE>
inline
int baejsn_Decoder::decodeChooser(TYPE *value, bdeat_TypeCategory::CustomizedType)
{
    return decodeCustom(value);
}

template <typename TYPE>
inline
int baejsn_Decoder::decodeChooser(TYPE *value, bdeat_TypeCategory::Simple)
{
    return decodeSimple(value);
}

template <typename TYPE>
inline
int baejsn_Decoder::decodeChooser(TYPE *value, bdeat_TypeCategory::Array)
{
    return decodeArray(value);
}

template <typename TYPE>
inline
int baejsn_Decoder::decodeChooser(TYPE *value, bdeat_TypeCategory::NullableValue)
{
    return decodeNullable(value);
}

template <typename TYPE, typename TYPE_CATEGORY>
inline
int baejsn_Decoder::decodeChooser(TYPE *, const TYPE_CATEGORY&)
{
    BSLS_ASSERT(0);
    return 1;
}

inline
int baejsn_Decoder::decodeSimple(short *value)
{
    return decodeNumber(value);
}

inline
int baejsn_Decoder::decodeSimple(int *value)
{
    return decodeNumber(value);
}

inline
int baejsn_Decoder::decodeSimple(bsls::Types::Int64 *value)
{
    return decodeNumber(value);
}

inline
int baejsn_Decoder::decodeSimple(unsigned short *value)
{
    return decodeNumber(value);
}

inline
int baejsn_Decoder::decodeSimple(unsigned int *value)
{
    return decodeNumber(value);
}

inline
int baejsn_Decoder::decodeSimple(bsls::Types::Uint64 *value)
{
    return decodeNumber(value);
}

inline
int baejsn_Decoder::decodeSimple(unsigned char *value)
{
    return decodeNumber(value);
}

inline
int baejsn_Decoder::decodeSimple(float *value)
{
    return decodeNumber(value);
}

inline
int baejsn_Decoder::decodeSimple(double *value)
{
    return decodeNumber(value);
}


inline
int baejsn_Decoder::decodeSimple(bdet_Time *value)
{
    return decodeIso8601(value);
}

inline
int baejsn_Decoder::decodeSimple(bdet_Date *value)
{
    return decodeIso8601(value);
}

inline
int baejsn_Decoder::decodeSimple(bdet_Datetime *value)
{
    return decodeIso8601(value);
}

inline
int baejsn_Decoder::decodeSimple(bdet_TimeTz *value)
{
    return decodeIso8601(value);
}

inline
int baejsn_Decoder::decodeSimple(bdet_DateTz *value)
{
    return decodeIso8601(value);
}

inline
int baejsn_Decoder::decodeSimple(bdet_DatetimeTz *value)
{
    return decodeIso8601(value);
}

template <typename TYPE>
int baejsn_Decoder::decodeIso8601(TYPE *value)
{
    baejsn_ParserUtil::skipSpaces(d_streamBuf);

    bsl::string temp;
    if (0 != baejsn_ParserUtil::getString(d_streamBuf, &temp)) {
        BAEJSN_THROW(mobcmn::ExBadArg, "Could not decode string");
        return 1;
    }
    return bdepu_Iso8601::parse(value, temp.c_str(), temp.length());
}

template <typename TYPE>
int baejsn_Decoder::decodeNumber(TYPE *value)
{
    baejsn_ParserUtil::skipSpaces(d_streamBuf);

    if (0 != baejsn_ParserUtil::getNumber(d_streamBuf, value)) {
        BAEJSN_THROW(mobcmn::ExBadArg,
                     "Could not decode integer @ " << d_streamBuf->sgetc());
        return 1;
    }
    return 0;
}


inline
int baejsn_Decoder::decodeSimple(bool *value)
{
    baejsn_ParserUtil::skipSpaces(d_streamBuf);

    if (0 == baejsn_ParserUtil::eatToken(d_streamBuf, "true")) {
        *value = true;
    }
    else if (0 == baejsn_ParserUtil::eatToken(d_streamBuf, "false")) {
        *value = false;
    }
    else {
        BAEJSN_THROW(mobcmn::ExBadArg, "Could not decode boolean");
        return 1;
    }
    return 0;
}

template <typename TYPE>
int baejsn_Decoder::decodeNullable(TYPE *value)
{
    baejsn_ParserUtil::skipSpaces(d_streamBuf);

    if (0 == baejsn_ParserUtil::eatToken(d_streamBuf, "null")) {
        return 0;
    }

    bdeat_NullableValueFunctions::makeValue(value);

    baejsn_Decoder_ElementVisitor visitor = { this };
    return bdeat_NullableValueFunctions::manipulateValue(value, visitor);
}

inline
int baejsn_Decoder::decodeSimple(char *value)
{
    baejsn_ParserUtil::skipSpaces(d_streamBuf);

    std::string valueString;

    if (0 == baejsn_ParserUtil::getString(d_streamBuf, &valueString)
     && valueString.length() == 1) {
        *value = valueString[0];
    }
    else {
        BAEJSN_THROW(mobcmn::ExBadArg, "Could not decode char");
        return 1;
    }
    return 0;
}

inline
int baejsn_Decoder::decodeSimple(std::string *value)
{
    baejsn_ParserUtil::skipSpaces(d_streamBuf);

    if (0 != baejsn_ParserUtil::getString(d_streamBuf, value)) {
        BAEJSN_THROW(mobcmn::ExBadArg, "Could not decode string");
        return 1;
    }
    return 0;
}

template <typename TYPE>
int baejsn_Decoder::decodeArray(TYPE *value)
{
    baejsn_ParserUtil::skipSpaces(d_streamBuf);

    if (0 != baejsn_ParserUtil::eatToken(d_streamBuf, "[")) {
        BAEJSN_THROW(mobcmn::ExBadArg, "Could not decode vector, missing start [");
        return 1;
    }

    baejsn_ParserUtil::skipSpaces(d_streamBuf);

    if (0 != baejsn_ParserUtil::eatToken(d_streamBuf, "]"))
    {
        do
        {
            const int i = static_cast<int>(bdeat_ArrayFunctions::size(*value));

            bdeat_ArrayFunctions::resize(value, i + 1);

            baejsn_Decoder_ElementVisitor visitor = { this };
            if (0 != bdeat_ArrayFunctions::manipulateElement(value,
                                                             visitor,
                                                             i)) {
                BAEJSN_THROW(mobcmn::ExBadArg, "Could not element '" << i << "\'");
                return 1;
            }

            baejsn_ParserUtil::skipSpaces(d_streamBuf);
        } while (0 == baejsn_ParserUtil::eatToken(d_streamBuf, ","));

        if (0 != baejsn_ParserUtil::eatToken(d_streamBuf, "]")) {
            BAEJSN_THROW(mobcmn::ExBadArg, "Could not decode vector, missing end ]");
            return 1;
        }
    }
    return 0;
}

template <typename TYPE>
int baejsn_Decoder::decodeSequence(TYPE *value)
{
    baejsn_ParserUtil::skipSpaces(d_streamBuf);

    if (0 != baejsn_ParserUtil::eatToken(d_streamBuf, "{")) {
        BAEJSN_THROW(mobcmn::ExBadArg, "Could not decode sequence, missing start {");
        return 1;
    }

    baejsn_ParserUtil::skipSpaces(d_streamBuf);

    //std::set<int> hasValue;

    if (0 != baejsn_ParserUtil::eatToken(d_streamBuf, "}"))
    {
        do {
            baejsn_ParserUtil::skipSpaces(d_streamBuf);

            std::string attributeName;
            if (0 != baejsn_ParserUtil::getString(d_streamBuf, &attributeName)) {
                BAEJSN_THROW(mobcmn::ExBadArg, "Could not decode sequence, missing element string after ',' or '{'");
                return 1;
            }

            if (attributeName.empty()) {
                BAEJSN_THROW(mobcmn::ExBadArg, "Could not decode sequence, empty attribute names are not permitted");
                return 1;
            }

            //const bdeat_AttributeInfo * attribute = TYPE::lookupAttributeInfo(&attributeName[0], attributeName.length());

            //if (attribute == 0)
            //    BAEJSN_THROW(mobcmn::ExBadArg, "Could not decode sequence, \"" << attributeName << "\" is not valid in sequence");

            baejsn_ParserUtil::skipSpaces(d_streamBuf);

            if (0 != baejsn_ParserUtil::eatToken(d_streamBuf, ":")) {
                BAEJSN_THROW(mobcmn::ExBadArg, "Could not decode sequence, missing ':' after attribute name");
                return 1;
            }

            baejsn_Decoder_ElementVisitor visitor = { this };
            if (0 != bdeat_SequenceFunctions::manipulateAttribute(value, visitor, attributeName.c_str(), attributeName.length())) {
                BAEJSN_THROW(mobcmn::ExBadArg, "Could not decode sequence, unknown attribute id " << attributeName);
                return 1;
            }

            baejsn_ParserUtil::skipSpaces(d_streamBuf);

            //hasValue.insert(attribute->id());
        } while (0 == baejsn_ParserUtil::eatToken(d_streamBuf, ","));

        if (0 != baejsn_ParserUtil::eatToken(d_streamBuf, "}")) {
            BAEJSN_THROW(mobcmn::ExBadArg, "Could not decode sequence, missing terminator '}' or seperator ','");
            return 1;
        }
    }

    return 0;
}

template <typename TYPE>
int baejsn_Decoder::decodeChoice(TYPE *value)
{
    baejsn_ParserUtil::skipSpaces(d_streamBuf);

    if (0 != baejsn_ParserUtil::eatToken(d_streamBuf, "{")) {
        BAEJSN_THROW(mobcmn::ExBadArg, "Could not decode choice, missing start {");
        return 1;
    }

    baejsn_ParserUtil::skipSpaces(d_streamBuf);

    std::string attributeName;

    if (0 != baejsn_ParserUtil::getString(d_streamBuf, &attributeName)) {
        BAEJSN_THROW(mobcmn::ExBadArg, "Could not decode choice, missing attribute name");
        return 1;
    }

    if (0 != bdeat_ChoiceFunctions::makeSelection(value,
                                                  attributeName.c_str(),
                                                  attributeName.length())) {
        BAEJSN_THROW(mobcmn::ExBadArg, "Could not deocde choice, bad attribute name '" << attributeName << "'");
        return 1;
    }

    baejsn_ParserUtil::skipSpaces(d_streamBuf);

    if (0 != baejsn_ParserUtil::eatToken(d_streamBuf, ":")) {
        BAEJSN_THROW(mobcmn::ExBadArg, "Could not decode choice, missing :");
        return 1;
    }

    baejsn_Decoder_ElementVisitor visitor = { this };
    if (0 != bdeat_ChoiceFunctions::manipulateSelection(value, visitor)) {
        BAEJSN_THROW(mobcmn::ExBadArg, "Could not decode choice, attribute '" << attributeName << "' was not decoded");
        return 1;
    }

    baejsn_ParserUtil::skipSpaces(d_streamBuf);

    if (0 != baejsn_ParserUtil::eatToken(d_streamBuf, "}")) {
        BAEJSN_THROW(mobcmn::ExBadArg, "Could not decode choice, missing }");
        return 1;
    }
    return 0;
}

template <typename TYPE>
inline
int baejsn_Decoder::decodeEnumeration(TYPE *value)
{
    std::string valueString;
    decodeSimple(&valueString);
    if (bdeat_EnumFunctions::fromString(value, valueString.data(), valueString.length()) != 0) {
        BAEJSN_THROW(mobcmn::ExBadArg, "Could not decode Enum String, value not allowed \"" << valueString << "\"");
        return 1;
    }
    return 0;
}

template <typename TYPE>
inline
int baejsn_Decoder::decodeCustom(TYPE *value)
{
    typename bdeat_CustomizedTypeFunctions::BaseType<TYPE>::Type valueBaseType;
    decode(&valueBaseType);
    if (bdeat_CustomizedTypeFunctions::convertFromBaseType(value, valueBaseType) != 0) {
        BAEJSN_THROW(mobcmn::ExBadArg, "Could not decode Enum Customized, value not allowed \"" << valueBaseType << "\"");
        return 1;
    }
    return 0;
}

template <typename TYPE>
inline
int baejsn_Decoder::decode(TYPE *value)
{
    // TBD: A JSON string must contain an object.  i.e., the 'TYPE' *must*
    // be a Sequence or Choice on the first invocation of 'decode'.  This
    // 'decode' method is currently used to decode other JSON values.
    // Consequently, the decoder will allow a JSON string that contains
    // other JSON value instead of an object and should be fixed.

    typedef typename
    bdeat_TypeCategory::Select<TYPE>::Type TypeCategory;
    return decodeChooser(value, TypeCategory());
}

// CREATORS
inline
baejsn_Decoder::baejsn_Decoder()
: d_streamBuf(0)
{
}


// MANIPULATORS
template <typename TYPE>
int baejsn_Decoder::decode(bsl::streambuf *streamBuf, TYPE *variable)
{
    BSLS_ASSERT(0 == d_streamBuf);
    BSLS_ASSERT(streamBuf);

    d_streamBuf = streamBuf;

    //if (d_logStream != 0) {
    //    d_logStream->reset();
    //}

    bdeat_ValueTypeFunctions::reset(variable);

    int rc = decode(variable);

    d_streamBuf = 0;
    return rc;
}

template <typename TYPE>
int baejsn_Decoder::decode(bsl::istream& stream, TYPE *value)
{
    if (!stream.good()) {
        return -1;
    }

    if (0 != this->decode(stream.rdbuf(), value)) {
        stream.setstate(bsl::ios_base::failbit);
        return -1;
    }

    return 0;
}

                    // ------------------------------------
                    // struct baejsn_Decoder_ElementVisitor
                    // ------------------------------------

template <typename TYPE>
inline
int baejsn_Decoder_ElementVisitor::operator()(TYPE value)
{
    d_decoder->decode(value);
    return 0;
}

template <typename TYPE, typename ATTRIBUTE_OR_SELECTION>
inline
int baejsn_Decoder_ElementVisitor::operator()(TYPE value,
                                              const ATTRIBUTE_OR_SELECTION &)
{
    d_decoder->decode(value);

    return 0;
}

                    // ------------------------------------
                    // struct baejsn_Decoder_DecodeImpProxy
                    // ------------------------------------

// MANIPULATORS
template <typename TYPE>
inline
int baejsn_Decoder_DecodeImpProxy::operator()(TYPE *, bslmf_Nil)
{
    BSLS_ASSERT_SAFE(0);
    return -1;
}

template <typename TYPE, typename ANY_CATEGORY>
inline
int baejsn_Decoder_DecodeImpProxy::operator()(TYPE         *object,
                                              ANY_CATEGORY  category)
{
    d_decoder->decodeChooser(object, category);
    return 0;
}


}  // close namespace BloombergLP

#endif
