// baljsn_encoder.h                                                   -*-C++-*-
#ifndef INCLUDED_BALJSN_ENCODER
#define INCLUDED_BALJSN_ENCODER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a JSON encoder for 'bdeat' compatible types.
//
//@CLASSES:
// baljsn::Encoder: JSON decoder for 'bdeat'-compliant types
//
//@SEE_ALSO: baljsn_decoder, baljsn_printutil
//
//@AUTHOR: Raymond Chiu (schiu49), Rohan Bhindwale (rbhindwa)
//
//@DESCRIPTION: This component provides a class, 'baljsn::Encoder', for encoding
// value-semantic objects in the JSON format.  In particular, the 'class'
// contains a parameterized 'encode' function that encodes an object into a
// specified stream.  There are two overloaded versions of this function:
//..
//: o one that writes to a 'bsl::streambuf'
//: o one that writes to an 'bsl::ostream'
//..
// This component can be used with types that support the 'bdeat' framework
// (see the 'bdeat' package for details), which is a compile-time interface for
// manipulating struct-like and union-like objects.  In particular, types
// generated by the 'bas_codegen.pl' tool, and other dynamic types, can be encoded using this 'class'.  The 'encode' function
// can be invoked on any object that satisfies the requirements of a sequence,
// choice, or array object as defined in the 'bdlat_sequencefunctions',
// 'bdlat_choicefunctions', and 'bdlat_arrayfunctions' components.
//
// Although the JSON format is easy to read and write and is very useful for
// debugging, it is relatively expensive to encode and decode and relatively
// bulky to transmit.  It is more efficient to use a binary encoding (such as
// BER) if the encoding format is under your control (see 'balber_berencoder').
//
///Encoding Format for a Simple Type
///---------------------------------
// The following table describes how various Simple types are encoded.
//..
//  Simple Type          JSON Type  Notes
//  -----------          ---------  -----
//  char                 number     The integer value of the character
//  unsigned char        number     The integer value of the character
//  int                  number
//  unsigned int         number
//  bsls::Types::Int64   number
//  bsls::Types::Uint64  number
//  float                number
//  double               number
//  char *               string
//  bsl::string          string
//  bdlt::Date            string     ISO 8601 format
//  bdlt::DateTz          string     ISO 8601 format
//  bdlt::Time            string     ISO 8601 format
//  bdlt::TimeTz          string     ISO 8601 format
//  bdlt::DatetimeTz      string     ISO 8601 format
//  bdlt::DatetimeTz      string     ISO 8601 format
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Encoding a 'bas_codegen.pl'-generated object into JSON
///-----------------------------------------------------------------
// Consider that we want to exchange an employee's information between two
// processes.  To allow this information exchange we will define the XML schema
// representation for that class, use 'bas_codegen.pl' to create the 'Employee'
// 'class' for storing that information, populate an 'Employee' object, and
// encode that object using the baejsn encoder.
//
// First, we will define the XML schema inside a file called 'employee.xsd':
//..
//  <?xml version='1.0' encoding='UTF-8'?>
//  <xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'
//             xmlns:test='http://bloomberg.com/schemas/test'
//             targetNamespace='http://bloomberg.com/schemas/test'
//             elementFormDefault='unqualified'>
//
//      <xs:complexType name='Address'>
//          <xs:sequence>
//              <xs:element name='street' type='xs:string'/>
//              <xs:element name='city'   type='xs:string'/>
//              <xs:element name='state'  type='xs:string'/>
//          </xs:sequence>
//      </xs:complexType>
//
//      <xs:complexType name='Employee'>
//          <xs:sequence>
//              <xs:element name='name'        type='xs:string'/>
//              <xs:element name='homeAddress' type='test:Address'/>
//              <xs:element name='age'         type='xs:int'/>
//          </xs:sequence>
//      </xs:complexType>
//
//      <xs:element name='Employee' type='test:Employee'/>
//
//  </xs:schema>
//..
// Then, we will use the 'bas_codegen.pl' tool, to generate the C++ classes for
// this schema.  The following command will generate the header and
// implementation files for the all the classes in the 'test_messages'
// components in the current directory:
//..
//  $ bas_codegen.pl -m msg -p test xsdfile.xsd
//..
// Next, we will populate a 'test::Employee' object:
//..
//  test::Employee employee;
//  employee.name()                 = "Bob";
//  employee.homeAddress().street() = "Lexington Ave";
//  employee.homeAddress().city()   = "New York City";
//  employee.homeAddress().state()  = "New York";
//  employee.age()                  = 21;
//..
// Then, we will create a 'baljsn::Encoder' object:
//..
//  baljsn::Encoder encoder;
//..
// Now, we will output this object in the JSON format by invoking the 'encode'
// method of the encoder.  We will also create a 'baljsn::EncoderOptions' object
// that allows us to specify that the encoding should be done in a pretty
// format, and what the initial indent level and spaces per level should be.
// We will then pass that object to the 'encode' method:
//..
//  bsl::ostringstream os;
//
//  baljsn::EncoderOptions options;
//  options.setEncodingStyle(baljsn::EncoderOptions::BAEJSN_PRETTY);
//  options.setInitialIndentLevel(1);
//  options.setSpacesPerLevel(4);
//
//  const int rc = encoder.encode(os, employee, options);
//  assert(!rc);
//  assert(os);
//..
// Finally, we will verify that the output is as expected:
//..
//  const char EXP_OUTPUT[] = "    {\n"
//                            "        \"name\" : \"Bob\",\n"
//                            "        \"homeAddress\" : {\n"
//                            "            \"street\" : \"Lexington Ave\",\n"
//                            "            \"city\" : \"New York City\",\n"
//                            "            \"state\" : \"New York\"\n"
//                            "        },\n"
//                            "        \"age\" : 21\n"
//                            "    }";
//
//  assert(EXP_OUTPUT == os.str());
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALJSN_ENCODEROPTIONS
#include <baljsn_encoderoptions.h>
#endif

#ifndef INCLUDED_BALJSN_PRINTUTIL
#include <baljsn_printutil.h>
#endif

#ifndef INCLUDED_BDLAT_ATTRIBUTEINFO
#include <bdlat_attributeinfo.h>
#endif

#ifndef INCLUDED_BDLAT_CHOICEFUNCTIONS
#include <bdlat_choicefunctions.h>
#endif

#ifndef INCLUDED_BDLAT_CUSTOMIZEDTYPEFUNCTIONS
#include <bdlat_customizedtypefunctions.h>
#endif

#ifndef INCLUDED_BDLAT_CUSTOMIZEDTYPEFUNCTIONS
#include <bdlat_customizedtypefunctions.h>
#endif

#ifndef INCLUDED_BDLAT_ENUMFUNCTIONS
#include <bdlat_enumfunctions.h>
#endif

#ifndef INCLUDED_BDLAT_FORMATTINGMODE
#include <bdlat_formattingmode.h>
#endif

#ifndef INCLUDED_BDLAT_SEQUENCEFUNCTIONS
#include <bdlat_sequencefunctions.h>
#endif

#ifndef INCLUDED_BDLAT_TYPECATEGORY
#include <bdlat_typecategory.h>
#endif

#ifndef INCLUDED_BDLAT_VALUETYPEFUNCTIONS
#include <bdlat_valuetypefunctions.h>
#endif

#ifndef INCLUDED_BDLB_PRINT
#include <bdlb_print.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_SSTREAM
#include <bsl_sstream.h>
#endif

#ifndef INCLUDED_BSL_STREAMBUF
#include <bsl_streambuf.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

namespace baljsn {
                        // ====================
                        // class Encoder
                        // ====================

class Encoder {
    // This class provides a mechanism for encoding value-semantic objects in
    // the JSON format.  The 'encode' methods are function templates that will
    // encode any object that meets the requirements of a sequence, choice, or
    // array object as defined in the 'bdlat_sequencefunctions',
    // 'bdlat_choicefunctions', and 'bdlat_choicefunctions' components
    // respectively.  These generic frameworks provide a common compile-time
    // interface for accessing struct-like and union-like objects.  In
    // particular, the types generated by 'bas_codegen.pl'
    // provide the necessary interface and can be encoded using this component.

    // FRIENDS
    friend class Encoder_EncodeImpl;

  private:
    // DATA
    bsl::ostringstream d_logStream;  // stream used for logging

  private:
    // PRIVATE MANIPULATORS
    bsl::ostream& logStream();
        // Return the stream for logging.

  public:
    // CREATORS
    explicit Encoder(bslma::Allocator *basicAllocator = 0);
        // Create a encoder object.  Optionally specify a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    //! ~Encoder() = default;
        // Destroy this object.

    // MANIPULATORS
    template <class TYPE>
    int encode(bsl::streambuf               *streamBuf,
               const TYPE&                   value,
               const EncoderOptions&  options);
        // Encode the specified 'value', of (template parameter) 'TYPE', in the
        // JSON format using the specified 'options' and output it onto the
        // specified 'streamBuf'.  'TYPE' shall be a 'bdeat'-compatible
        // sequence, choice, or array type, or a 'bdeat'-compatible dynamic
        // type referring to one of those types.  Return 0 on success, and a
        // non-zero value otherwise.

    template <class TYPE>
    int encode(bsl::ostream&                stream,
               const TYPE&                  value,
               const EncoderOptions& options);
        // Encode the specified 'value', of (template parameter) 'TYPE', in the
        // JSON format using the specified 'options' and output it onto the
        // specified 'stream'.  'TYPE' shall be a 'bdeat'-compatible sequence,
        // choice, or array type, or a 'bdeat'-compatible dynamic type
        // referring to one of those types.  Return 0 on success, and a
        // non-zero value otherwise.

    template <class TYPE>
    int encode(bsl::streambuf *streamBuf, const TYPE& value);
        // Encode the specified 'value' of (template parameter) 'TYPE' into the
        // specified 'streamBuf'.  Return 0 on success, and a non-zero value
        // otherwise.
        //
        // DEPRECATED: Use the 'encode' function passed a reference to a
        // non-modifiable 'EncoderOptions' object instead.

    template <class TYPE>
    int encode(bsl::ostream& stream, const TYPE& value);
        // Encode the specified 'value' of (template parameter) 'TYPE' into the
        // specified 'streamBuf'.  Return 0 on success, and a non-zero value
        // otherwise.  Note that 'stream' will be invalidated if the encoding
        // failed.
        //
        // DEPRECATED: Use the 'encode' function passed a reference to a
        // non-modifiable 'EncoderOptions' object instead.

    // ACCESSORS
    bsl::string loggedMessages() const;
        // Return a string containing any error, warning, or trace messages
        // that were logged during the last call to the 'encode' method.  The
        // log is reset each time 'encode' is called.
};

                        // ==============================
                        // class Encoder_Formatter
                        // ==============================

class Encoder_Formatter {
    // This class implements a formatter providing operations for rending JSON
    // text elements to an output stream (supplied at construction) according
    // to a set of formatting options (also supplied at construction).  This is
    // a component-private class and should not be used outside of this
    // component.

    // DATA
    bsl::ostream& d_outputStream;     // stream for output (held, not owned)
    bool          d_usePrettyStyle;   // encoding style
    int           d_indentLevel;      // initial indent level
    int           d_spacesPerLevel;   // spaces per level
    bool          d_isArrayElement;   // is current element part of an array

  public:
    // CREATORS
    Encoder_Formatter(bsl::ostream&                 stream,
                             const EncoderOptions&  options);
        // Create a 'Encoder_Formatter' object using the specified
        // 'stream' and 'options'.

    //! ~Encoder_Formatter() = default;
        // Destroy this object.

    // MANIPULATORS
    void openObject();
        // Print onto the stream supplied at construction the sequence of
        // characters designating the start of an object.

    void closeObject();
        // Print onto the stream supplied at construction the sequence of
        // characters designating the end of an object.

    void openArray(bool formatAsEmptyArrayFlag = false);
        // Print onto the stream supplied at construction the sequence of
        // characters designating the start of an array.  Optionally specify
        // 'formatAsEmptyArrayFlag' denoting if the array being opened should
        // be formatted as an empty array.  If 'formatAsEmptyArrayFlag' is not
        // specified then the array being opened is formatted as an array
        // having elements.  Note that the formatting (and as a consequence the
        // 'formatAsEmptyArrayFlag') is relevant only if this formatter encodes
        // in the pretty style and is ignored otherwise.

    void closeArray(bool formatAsEmptyArrayFlag = false);
        // Print onto the stream supplied at construction the sequence of
        // characters designating the end of an array.  Optionally specify
        // 'formatAsEmptyArrayFlag' denoting if the array being closed should
        // be formatted as an empty array.  If 'formatAsEmptyArrayFlag' is not
        // specified then the array being closed is formatted as an array
        // having elements.  Note that the formatting (and as a consequence the
        // 'formatAsEmptyArrayFlag') is relevant only if this formatter encodes
        // in the pretty style and is ignored otherwise.

    void indent();
        // Print onto the stream supplied at construction the sequence of
        // whitespace characters for the proper indentation of an element
        // given the encoding options supplied at construction.

    int openElement(const bsl::string& name);
        // Print onto the stream supplied at construction the sequence of
        // characters designating the start of an element having the specified
        // 'name'.  Return 0 on success and a non-zero value otherwise.

    void closeElement();
        // Print onto the stream supplied at construction the sequence of
        // characters designating the end of an element.

    void openDocument();
        // Print onto the stream supplied at construction the sequence of
        // characters designating the start of the document.

    void closeDocument();
        // Print onto the stream supplied at construction the sequence of
        // characters designating the end of the document.

    void setIsArrayElement(bool isArrayElement);
        // Set the flag denoting if the current element refers to an array
        // element to the specified 'isArrayElement'.

    // ACCESSORS
    bool isArrayElement() const;
        // Return the value of the flag denoting if the current element refers
        // to an array element.
};

                        // ===============================
                        // class Encoder_EncodeImpl
                        // ===============================

class Encoder_EncodeImpl {
    // This class implements the parameterized 'encode' functions that encode
    // 'bdeat' types in the JSON format.  This is a component-private class and
    // should not be used outside of this component.

    // DATA
    Encoder                       *d_encoder_p;         // encoder
                                                               // (held, not
                                                               // owned)

    bsl::ostream                          d_outputStream;      // stream for
                                                               // output

    Encoder_Formatter              d_formatter;         // formatter

    const EncoderOptions          *d_encoderOptions_p;  // encoder
                                                               // options

    // FRIENDS
    friend struct Encoder_DynamicTypeDispatcher;
    friend struct Encoder_ElementVisitor;
    friend class Encoder_SequenceVisitor;

  private:
    // PRIVATE MANIPULATORS
    bsl::ostream& logStream();
        // Return the stream used for logging.

    int encodeImp(const bsl::vector<char>& value,
                  int,
                  bdeat_TypeCategory::Array);
    template <class TYPE>
    int encodeImp(const TYPE& value,
                  int         mode,
                  bdeat_TypeCategory::Array);
    template <class TYPE>
    int encodeImp(const TYPE& value,
                  int         mode,
                  bdeat_TypeCategory::Choice);
    template <class TYPE>
    int encodeImp(const TYPE& value,
                  int         mode,
                  bdeat_TypeCategory::CustomizedType);
    template <class TYPE>
    int encodeImp(const TYPE& value,
                  int         mode,
                  bdeat_TypeCategory::DynamicType);
    template <class TYPE>
    int encodeImp(const TYPE& value,
                  int         mode,
                  bdeat_TypeCategory::Enumeration);
    template <class TYPE>
    int encodeImp(const TYPE& value,
                  int         mode,
                  bdeat_TypeCategory::NullableValue);
    template <class TYPE>
    int encodeImp(const TYPE& value,
                  int         mode,
                  bdeat_TypeCategory::Sequence);
    template <class TYPE>
    int encodeImp(const TYPE& value,
                  int,
                  bdeat_TypeCategory::Simple);
        // Encode the specified 'value', of a (template parameter) 'TYPE'
        // corresponding to the specified 'bdeat' category, into JSON onto the
        // 'streambuf' supplied at construction, using the specified formatting
        // 'mode'.  Return 0 on success and a non-zero value otherwise.  The
        // behavior is undefined unless 'value' corresponds to the specified
        // 'bdeat' category and 'mode' is a valid formatting mode as specified
        // in 'bdeat_FormattingMode'.

  public:
    // CREATORS
    Encoder_EncodeImpl(Encoder               *encoder,
                              bsl::streambuf               *streambuf,
                              const EncoderOptions&  options);
        // Create a 'Encoder_EncodeImpl' object using the specified
        // 'encoder' and 'options' and writing the encoded output to the
        // specified 'streamBuf'.

    //! ~Encoder_EncodeImpl() = default;
        // Destroy this object.

    // MANIPULATORS
    void openDocument();
        // Print onto the stream supplied at construction the sequence of
        // characters designating the start of the document.

    template <class TYPE>
    int encode(const TYPE& value, int mode);
        // Encode the specified 'value' in the JSON format using the specified
        // formatting 'mode'.  Return 0 on success and a non-zero value
        // otherwise.

    void closeDocument();
        // Print onto the stream supplied at construction the sequence of
        // characters designating the end of the document.

    // ACCESSORS
    const EncoderOptions *encoderOptions() const;
        // Return a reference to the non-modifiable encoder options currently
        // being used by this encoder.
};

                 // ====================================
                 // struct Encoder_ElementVisitor
                 // ====================================

struct Encoder_ElementVisitor {
    // This class encodes elements in an array or a choice in the JSON format.
    // This is a component-private class and should not be used outside of this
    // component.  Note that the operators provided in this 'class' match the
    // function signatures required of visitors encoding elements of compatible
    // types.

    // DATA
    Encoder_EncodeImpl *d_encoder_p; // encoder (held, not owned)
    int                        d_mode;      // formatting mode

    // CREATORS
    // Creators have been omitted to allow simple static initialization of
    // this struct.

    // MANIPULATORS
    template <class TYPE>
    int operator()(const TYPE& value);
        // Encode the specified 'value' in the JSON format.  Return 0 on
        // success and a non-zero value otherwise.

    template <class TYPE, class INFO>
    int operator()(const TYPE& value, const INFO& info);
        // Encode the specified 'value' using the specified 'info' in the JSON
        // format.  Return 0 on success and a non-zero value otherwise.
};

                 // ====================================
                 // class Encoder_SequenceVisitor
                 // ====================================

class Encoder_SequenceVisitor {
    // This functor class encodes element in a sequence.  It should be passed
    // as an argument to the 'bdeat_SequenceFunctions::accessAttributes'
    // function.  Note that the operators provided in this 'class' match the
    // function signatures required of visitors encoding elements of 'bdeat'
    // sequence types.

    // DATA
    Encoder_EncodeImpl *d_encoder_p;        // encoder (held, not owned)
    bool                       d_isFirstElement;   // flag indicating if an
                                                   // current element is the
                                                   // first

    // PRIVATE CLASS METHODS
    template <class TYPE>
    bool skipNullableAttribute(const TYPE&, bslmf::MetaInt<0>);
    template <class TYPE>
    bool skipNullableAttribute(const TYPE& value, bslmf::MetaInt<1>);
    template <class TYPE>
    bool skipNullableAttribute(const TYPE& value);
        // Return 'true' if the specified 'value' represents a
        // 'bdeat_NullableValue' type and should be skipped, and 'false'
        // otherwise.

    template <class TYPE>
    static bool isEmptyArray(const TYPE&, bslmf::MetaInt<0>);
    template <class TYPE>
    static bool isEmptyArray(const TYPE& value, bslmf::MetaInt<1>);
    template <class TYPE>
    static bool isEmptyArray(const TYPE& value);
        // Return 'true' if the specified 'value' represents an empty array and
        // 'false' otherwise.

  public:
    // CREATORS
    explicit Encoder_SequenceVisitor(
                                           Encoder_EncodeImpl *encoder);
        // Create a 'Encoder_SequenceVisitor' object using the
        // specified 'encoder'.

    // MANIPULATORS
    template <class TYPE, class INFO>
    int operator()(const TYPE& value, const INFO &info);
        // Encode the specified 'value' using the specified 'info' in the JSON
        // format.  Return 0 on success and a non-zero value otherwise.
};

                 // ===========================================
                 // struct Encoder_DynamicTypeDispatcher
                 // ===========================================

struct Encoder_DynamicTypeDispatcher {
    // This class is used to dispatch the appropriate 'encodeImp' method for a
    // 'bdeat' Dynamic type.  This is a component-private class and should not
    // be used outside of this component.  Note that the operators provided in
    // this 'class' match the function signatures required of visitors encoding
    // elements of 'bdeat' dynamic types.

    // DATA
    Encoder_EncodeImpl *d_encoder_p; // encoder (held, not owned)
    int                        d_mode;      // formatting mode

    // CREATORS
    // Creators have been omitted to allow simple static initialization of
    // this struct.

    // MANIPULATORS
    template <class TYPE>
    int operator()(const TYPE&, bslmf::Nil);
    template <class TYPE, class ANY_CATEGORY>
    int operator()(const TYPE& value, ANY_CATEGORY category);
        // Encode the specified 'value' of the specified 'bdeat' 'category' in
        // the JSON format.  Return 0 on success and a non-zero value
        // otherwise.
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                            // --------------------
                            // class Encoder
                            // --------------------

// PRIVATE MANIPULATORS
inline
bsl::ostream& Encoder::logStream()
{
    return d_logStream;
}

// CREATORS
inline
Encoder::Encoder(bslma::Allocator *basicAllocator)
: d_logStream(basicAllocator)
{
}

// MANIPULATORS
template <class TYPE>
inline
int Encoder::encode(bsl::streambuf *streamBuf, const TYPE& value)
{
    const EncoderOptions options;
    return encode(streamBuf, value, options);
}

template <class TYPE>
inline
int Encoder::encode(bsl::ostream& stream, const TYPE& value)
{
    const EncoderOptions options;
    return encode(stream, value, options);
}

template <class TYPE>
int Encoder::encode(bsl::streambuf               *streamBuf,
                           const TYPE&                   value,
                           const EncoderOptions&  options)
{
    BSLS_ASSERT(streamBuf);

    bdeat_TypeCategory::Value category =
                                    bdeat_TypeCategoryFunctions::select(value);
    if (bdeat_TypeCategory::BDEAT_SEQUENCE_CATEGORY != category
     && bdeat_TypeCategory::BDEAT_CHOICE_CATEGORY != category
     && bdeat_TypeCategory::BDEAT_ARRAY_CATEGORY != category) {
        logStream()
                  << "Encoded object must be a Sequence, Choice or Array type."
                  << bsl::endl;
        return -1;                                                    // RETURN
    }

    d_logStream.clear();
    d_logStream.str("");

    Encoder_EncodeImpl encoderImpl(this, streamBuf, options);

    encoderImpl.openDocument();

    const int rc = encoderImpl.encode(value, 0);

    if (!rc) {
        encoderImpl.closeDocument();
    }

    streamBuf->pubsync();

    return rc;
}

template <class TYPE>
int Encoder::encode(bsl::ostream&                stream,
                           const TYPE&                  value,
                           const EncoderOptions& options)
{
    if (!stream.good()) {
        logStream() << "Invalid stream." << bsl::endl;
        return -1;                                                    // RETURN
    }

    const int rc = this->encode(stream.rdbuf(), value, options);
    if (rc) {
        stream.setstate(bsl::ios_base::failbit);
        return rc;                                                    // RETURN
    }

    return 0;
}

// ACCESSORS
inline
bsl::string Encoder::loggedMessages() const
{
    return d_logStream.str();
}

                        // ------------------------------
                        // class Encoder_Formatter
                        // ------------------------------
// MANIPULATORS
inline
void Encoder_Formatter::setIsArrayElement(bool isArrayElement)
{
    d_isArrayElement = isArrayElement;
}

// ACCESSORS
inline
bool Encoder_Formatter::isArrayElement() const
{
    return d_isArrayElement;
}


                        // -------------------------------
                        // class Encoder_EncodeImpl
                        // -------------------------------

// PRIVATE MANIPULATORS
inline
bsl::ostream& Encoder_EncodeImpl::logStream()
{
    return d_encoder_p->logStream();
}

template <class TYPE>
inline
int Encoder_EncodeImpl::encodeImp(const TYPE& value,
                                         int         mode,
                                         bdeat_TypeCategory::CustomizedType)
{
    return encode(bdeat_CustomizedTypeFunctions::convertToBaseType(value),
                  mode);
}

template <class TYPE>
inline
int Encoder_EncodeImpl::encodeImp(const TYPE& value,
                                         int         mode,
                                         bdeat_TypeCategory::DynamicType)
{
    Encoder_DynamicTypeDispatcher proxy = { this, mode };
    return bdeat_TypeCategoryUtil::accessByCategory(value, proxy);
}

template <class TYPE>
inline
int Encoder_EncodeImpl::encodeImp(const TYPE& value,
                                         int         mode,
                                         bdeat_TypeCategory::Enumeration)
{
    bsl::string valueString;
    bdeat_EnumFunctions::toString(&valueString, value);
    return encode(valueString, mode);
}

template <class TYPE>
inline
int Encoder_EncodeImpl::encodeImp(const TYPE& value,
                                         int,
                                         bdeat_TypeCategory::Simple)
{
    d_formatter.indent();
    return PrintUtil::printValue(d_outputStream, value);
}

template <class TYPE>
int Encoder_EncodeImpl::encodeImp(const TYPE& value,
                                         int         mode,
                                         bdeat_TypeCategory::Sequence)
{
    if (!(bdeat_FormattingMode::BDEAT_UNTAGGED & mode)) {
        d_formatter.openObject();
    }

    Encoder_SequenceVisitor visitor(this);

    const bool isArrayElement = d_formatter.isArrayElement();

    d_formatter.setIsArrayElement(false);

    const int rc = bdeat_SequenceFunctions::accessAttributes(value, visitor);
    if (rc) {
        return rc;                                                    // RETURN
    }

    d_formatter.setIsArrayElement(isArrayElement);

    if (!(bdeat_FormattingMode::BDEAT_UNTAGGED & mode)) {
        d_formatter.closeObject();
    }

    return 0;
}

template <class TYPE>
int Encoder_EncodeImpl::encodeImp(const TYPE& value,
                                         int         mode,
                                         bdeat_TypeCategory::Choice)
{
    if (bdeat_ChoiceFunctions::BDEAT_UNDEFINED_SELECTION_ID !=
                                   bdeat_ChoiceFunctions::selectionId(value)) {
        if (!(bdeat_FormattingMode::BDEAT_UNTAGGED & mode)) {
            d_formatter.openObject();
        }

        Encoder_ElementVisitor visitor = { this, mode };

        const bool isArrayElement = d_formatter.isArrayElement();

        d_formatter.setIsArrayElement(false);

        const int rc = bdeat_ChoiceFunctions::accessSelection(value, visitor);
        if (rc) {
            return rc;                                                // RETURN
        }

        d_formatter.setIsArrayElement(isArrayElement);

        if (!(bdeat_FormattingMode::BDEAT_UNTAGGED & mode)) {
            d_formatter.closeObject();
        }
    }
    else {
        logStream() << "Undefined selection for Choice object" << bsl::endl;
        return -1;                                                    // RETURN
    }
    return 0;
}

template <class TYPE>
int Encoder_EncodeImpl::encodeImp(const TYPE& value,
                                         int         mode,
                                         bdeat_TypeCategory::Array)
{
    const int size = static_cast<int>(bdeat_ArrayFunctions::size(value));
    if (0 < size) {
        d_formatter.openArray();

        Encoder_ElementVisitor visitor = { this, mode };

        d_formatter.setIsArrayElement(true);

        int rc = bdeat_ArrayFunctions::accessElement(value, visitor, 0);
        if (rc) {
            return rc;                                                // RETURN
        }

        for (int i = 1; i < size; ++i) {
            d_formatter.closeElement();
            rc = bdeat_ArrayFunctions::accessElement(value, visitor, i);
            if (rc) {
                return rc;                                            // RETURN
            }
        }

        d_formatter.setIsArrayElement(false);

        d_formatter.closeArray();
    }
    else if (d_encoderOptions_p->encodeEmptyArrays()) {
        d_formatter.openArray(true);
        d_formatter.closeArray(true);
    }

    return 0;
}

template <class TYPE>
int Encoder_EncodeImpl::encodeImp(const TYPE& value,
                                         int         mode,
                                         bdeat_TypeCategory::NullableValue)
{
    if (bdeat_NullableValueFunctions::isNull(value)) {
        d_formatter.indent();
        d_outputStream << "null";
        return 0;                                                     // RETURN
    }

    Encoder_ElementVisitor visitor = { this, mode };
    return bdeat_NullableValueFunctions::accessValue(value, visitor);
}

template <class TYPE>
inline
int Encoder_EncodeImpl::encode(const TYPE& value, int mode)
{
    typedef typename bdeat_TypeCategory::Select<TYPE>::Type TypeCategory;
    return encodeImp(value, mode, TypeCategory());
}

// CREATORS
inline
Encoder_EncodeImpl::Encoder_EncodeImpl(
                                       Encoder               *encoder,
                                       bsl::streambuf               *streambuf,
                                       const EncoderOptions&  options)
: d_encoder_p(encoder)
, d_outputStream(streambuf)
, d_formatter(d_outputStream, options)
, d_encoderOptions_p(&options)
{
}

inline
void Encoder_EncodeImpl::openDocument()
{
    d_formatter.openDocument();
}

inline
void Encoder_EncodeImpl::closeDocument()
{
    d_formatter.closeDocument();
}

// ACCESSORS
inline
const EncoderOptions *Encoder_EncodeImpl::encoderOptions() const
{
    return d_encoderOptions_p;
}

                    // -------------------------------------
                    // struct Encoder_SequenceVisitor
                    // -------------------------------------

// PRIVATE CLASS METHODS
template <class TYPE>
inline
bool Encoder_SequenceVisitor::skipNullableAttribute(const TYPE&,
                                                           bslmf::MetaInt<0>)
{
    return false;
}

template <class TYPE>
bool Encoder_SequenceVisitor::skipNullableAttribute(const TYPE& value,
                                                           bslmf::MetaInt<1>)
{
    if (bdeat_TypeCategory::BDEAT_NULLABLE_VALUE_CATEGORY ==
                                  bdeat_TypeCategoryFunctions::select(value)) {
        return bdeat_NullableValueFunctions::isNull(value)
            && !d_encoder_p->encoderOptions()->encodeNullElements();  // RETURN
    }
    return false;
}

template <class TYPE>
inline
bool Encoder_SequenceVisitor::skipNullableAttribute(const TYPE& value)
{
    return skipNullableAttribute(value,
                                 bslmf::MetaInt<bdeat_NullableValueFunctions
                                            ::IsNullableValue<TYPE>::VALUE>());
}

template <class TYPE>
bool Encoder_SequenceVisitor::isEmptyArray(const TYPE&,
                                                  bslmf::MetaInt<0>)
{
    return false;
}

template <class TYPE>
inline
bool Encoder_SequenceVisitor::isEmptyArray(const TYPE& value,
                                                  bslmf::MetaInt<1>)
{
    if (bdeat_TypeCategory::BDEAT_ARRAY_CATEGORY ==
                                  bdeat_TypeCategoryFunctions::select(value)) {
        return 0 == bdeat_ArrayFunctions::size(value);                // RETURN
    }
    return false;
}

template <class TYPE>
inline
bool Encoder_SequenceVisitor::isEmptyArray(const TYPE& value)
{
    return isEmptyArray(
                 value,
                 bslmf::MetaInt<bdeat_ArrayFunctions::IsArray<TYPE>::VALUE>());
}

// CREATORS
inline
Encoder_SequenceVisitor::Encoder_SequenceVisitor(
                                            Encoder_EncodeImpl *encoder)
: d_encoder_p(encoder)
, d_isFirstElement(true)
{
}

// MANIPULATORS
template <class TYPE, class INFO>
int Encoder_SequenceVisitor::operator()(const TYPE& value,
                                               const INFO& info)
{
    // Determine if 'value' is null or an empty array where we dont want to
    // encode empty arrays.  In either of those cases, do not encode 'value'.

    if (skipNullableAttribute(value)
     || (isEmptyArray(value)
      && !d_encoder_p->encoderOptions()->encodeEmptyArrays())) {
        return 0;                                                     // RETURN
    }

    if (!d_isFirstElement) {
        d_encoder_p->d_formatter.closeElement();
    }

    d_isFirstElement = false;

    Encoder_ElementVisitor visitor = { d_encoder_p,
                                              info.formattingMode() };
    return visitor(value, info);
}

                    // ------------------------------------
                    // struct Encoder_ElementVisitor
                    // ------------------------------------

template <class TYPE>
inline
int Encoder_ElementVisitor::operator()(const TYPE &value)
{
    return d_encoder_p->encode(value, d_mode);
}

template <class TYPE, class INFO>
int Encoder_ElementVisitor::operator()(const TYPE& value,
                                              const INFO& info)
{
    // Skip encoding of anonymous elements

    const int mode = info.formattingMode();
    if (!(bdeat_FormattingMode::BDEAT_UNTAGGED & mode)) {

        const int rc = d_encoder_p->d_formatter.openElement(info.name());
        if (rc) {
            d_encoder_p->logStream() << "Unable to encode element named: '"
                                     << info.name() << "'." << bsl::endl;
            return rc;                                                // RETURN
        }
    }

    const int rc = d_encoder_p->encode(value, mode);
    if (rc) {
        d_encoder_p->logStream() << "Unable to encode value of element "
                                 << "named: '" << info.name() << "'."
                                 << bsl::endl;
        return rc;                                                    // RETURN
    }
    return 0;
}

                    // -------------------------------------------
                    // struct Encoder_DynamicTypeDispatcher
                    // -------------------------------------------

// MANIPULATORS
template <class TYPE>
inline
int Encoder_DynamicTypeDispatcher::operator()(const TYPE&, bslmf::Nil)
{
    BSLS_ASSERT_OPT(!"Should be unreachable!");

    return -1;
}

template <class TYPE, class ANY_CATEGORY>
inline
int Encoder_DynamicTypeDispatcher::operator()(const TYPE&  value,
                                                     ANY_CATEGORY category)
{
    d_encoder_p->encodeImp(value, d_mode, category);
    return 0;
}
}  // close package namespace


}  // close enterprise namespace

#endif

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
