// bdes_platform.h                 -*-C++-*-
#ifndef INCLUDED_BDES_PLATFORM
#define INCLUDED_BDES_PLATFORM

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide preprocessor support for platform identification.
//
//@DEPRECATED: Use 'bsls_platform' instead.
//
//@CLASSES:
//   bdes_Platform: a namespace containing platform identification structs
//
//@AUTHOR: John Lakos (jlakos)
//
//@DESCRIPTION: This component defines preprocessor macros that identify and
// describe platform-specific attributes (at compile time).  These attributes
// consist of the types and versions of
//..
//   - the operating system
//   - the processor(s)
//   - the compiler
//..
// that comprise the platform.  By including this header file, most, but not
// all, of the following macros are configured automatically at compile time;
// the remainder can be set using compile-time switches.
//
// For the sake of brevity in the following table, the '*' character will be
// used to represent the characters 'BDES_PLATFORM'
//..
//      =============================================================
//                        OPERATING SYSTEM
//      -------------------------------------------------------------
//      Type                Subtype               Version
//      -----------------   -------------------   -------------------
//      *__OS_UNIX          *__OS_AIX             *__OS_VER_MAJOR
//                          *__OS_HPUX            *__OS_VER_MINOR
//                          *__OS_LINUX
//                          *__OS_SOLARIS
//                          *__OS_SUNOS
//
//      *__OS_WINDOWS       *__OS_WIN9X
//                          *__OS_WINNT
//                          *__OS_WIN2K
//                          *__OS_WINXP
//      =============================================================
//                          PROCESSOR
//      -------------------------------------------------------------
//      Instruction Set          Width                 Version
//      -----------------   -------------------   -------------------
//      *__CPU_88000        *__CPU_32_BIT         *__CPU_VER_MAJOR
//      *__CPU_ALPHA        *__CPU_64_BIT         *__CPU_VER_MINOR
//      *__CPU_HPPA
//      *__CPU_INTEL^
//      *__CPU_X86
//      *__CPU_IA64
//      *__CPU_X86_64
//      *__CPU_MIPS
//      *__CPU_POWERPC
//      *__CPU_SPARC
//        ^ - DEPRECATED
//      =============================================================
//                          COMPILER
//      -------------------------------------------------------------
//      Vendor                    Version
//      -----------------   -------------------
//      *__CMP_AIX          *__CMP_VER_MAJOR
//      *__CMP_GNU
//      *__CMP_HP
//      *__CMP_MSVC
//      *__CMP_SUN
//      ==========================================================
//..
// Where possible, the macros described above are configured automatically.
// At a minimum, the generic operating system type (i.e., either
// 'BDES_PLATFORM_OS_UNIX' or 'BDES_PLATFORM_OS_WINDOWS') is defined along
// with exactly one processor macro (e.g., 'BDES_PLATFORM_CPU_SPARC') and
// exactly one compiler macro (e.g., 'BDES_PLATFORM_CMP_SUN').  Clients may be
// required to supply user-defined macros (typically specified via the '-D'
// option of a compiler) if the client software needs to discriminate further
// based on operating system subtypes or individual versions of the specific
// operating system, processor, or compiler.  Note that supplying a minor
// version number implies that the major version is also defined.
//
///Usage
///-----
// Writing portable software at times involves specializing implementations to
// work with system-specific features.  For example, a socket-level
// communications framework would undoubtedly need to differentiate Windows
// Handle objects and Unix socket descriptors, but specialization of the
// different versions of Unix or the different versions of Windows is probably
// not needed.
//..
//    // my_socket.h
//    #include <bdes_platform.h>
//
//    #ifdef BDES_PLATFORM_OS_WINDOWS
//    #  ifndef INCLUDED_WINSOCK2
//    #  include <winsock2.h>
//    #  define INCLUDED_WINSOCK2
//    #  endif
//    #endif
//
//    class my_Socket {
//    #ifdef BDES_PLATFORM_OS_WINDOWS
//        SOCKET d_sockfd;   // Windows SOCKET handler
//    #else
//        int d_sockfd;      // UNIX socket descriptor
//    #endif
//
//    // ...
//
//    };
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>  // required by 'bdes'
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif


// Check using legacy macros only if 'BDE_OMIT_INTERNAL_DEPRECATED' is not
// defined.

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

#ifdef BSLS_PLATFORM_OS_UNIX
#ifndef BDES_PLATFORM_OS_UNIX
#define BDES_PLATFORM_OS_UNIX        BSLS_PLATFORM_OS_UNIX
#endif
#endif

#ifdef BSLS_PLATFORM_OS_WINDOWS
#ifndef BDES_PLATFORM_OS_WINDOWS
#define BDES_PLATFORM_OS_WINDOWS     BSLS_PLATFORM_OS_WINDOWS
#endif
#endif

#ifdef BSLS_PLATFORM_OS_VER_MAJOR
#ifndef BDES_PLATFORM_OS_VER_MAJOR
#define BDES_PLATFORM_OS_VER_MAJOR   BSLS_PLATFORM_OS_VER_MAJOR
#endif
#endif

#ifdef BSLS_PLATFORM_OS_VER_MINOR
#ifndef BDES_PLATFORM_OS_VER_MINOR
#define BDES_PLATFORM_OS_VER_MINOR   BSLS_PLATFORM_OS_VER_MINOR
#endif
#endif

#ifdef BSLS_PLATFORM_OS_AIX
#ifndef BDES_PLATFORM_OS_AIX
#define BDES_PLATFORM_OS_AIX         BSLS_PLATFORM_OS_AIX
#endif
#endif

#ifdef BSLS_PLATFORM_OS_HPUX
#ifndef BDES_PLATFORM_OS_HPUX
#define BDES_PLATFORM_OS_HPUX        BSLS_PLATFORM_OS_HPUX
#endif
#endif

#ifdef BSLS_PLATFORM_OS_LINUX
#ifndef BDES_PLATFORM_OS_LINUX
#define BDES_PLATFORM_OS_LINUX       BSLS_PLATFORM_OS_LINUX
#endif
#endif

#ifdef BSLS_PLATFORM_OS_SOLARIS
#ifndef BDES_PLATFORM_OS_SOLARIS
#define BDES_PLATFORM_OS_SOLARIS     BSLS_PLATFORM_OS_SOLARIS
#endif
#endif

#ifdef BSLS_PLATFORM_OS_SUNOS
#ifndef BDES_PLATFORM_OS_SUNOS
#define BDES_PLATFORM_OS_SUNOS       BSLS_PLATFORM_OS_SUNOS
#endif
#endif

#ifdef BSLS_PLATFORM_OS_WIN2K
#ifndef BDES_PLATFORM_OS_WIN2K
#define BDES_PLATFORM_OS_WIN2K       BSLS_PLATFORM_OS_WIN2K
#endif
#endif

#ifdef BSLS_PLATFORM_OS_WIN9X
#ifndef BDES_PLATFORM_OS_WIN9X
#define BDES_PLATFORM_OS_WIN9X       BSLS_PLATFORM_OS_WIN9X
#endif
#endif

#ifdef BSLS_PLATFORM_OS_WINNT
#ifndef BDES_PLATFORM_OS_WINNT
#define BDES_PLATFORM_OS_WINNT       BSLS_PLATFORM_OS_WINNT
#endif
#endif

#ifdef BSLS_PLATFORM_OS_WINXP
#ifndef BDES_PLATFORM_OS_WINXP
#define BDES_PLATFORM_OS_WINXP       BSLS_PLATFORM_OS_WINXP
#endif
#endif

#ifdef BSLS_PLATFORM_CMP_IBM
#ifndef BDES_PLATFORM_CMP_AIX
#define BDES_PLATFORM_CMP_AIX        BSLS_PLATFORM_CMP_IBM
#endif
#endif

#ifdef BSLS_PLATFORM_CMP_GNU
#ifndef BDES_PLATFORM_CMP_GNU
#define BDES_PLATFORM_CMP_GNU        BSLS_PLATFORM_CMP_GNU
#endif
#endif

#ifdef BSLS_PLATFORM_CMP_HP
#ifndef BDES_PLATFORM_CMP_HP
#define BDES_PLATFORM_CMP_HP         BSLS_PLATFORM_CMP_HP
#endif
#endif

#ifdef BSLS_PLATFORM_CMP_MSVC
#ifndef BDES_PLATFORM_CMP_MSVC
#define BDES_PLATFORM_CMP_MSVC       BSLS_PLATFORM_CMP_MSVC
#endif
#endif

#ifdef BSLS_PLATFORM_CMP_SUN
#ifndef BDES_PLATFORM_CMP_SUN
#define BDES_PLATFORM_CMP_SUN        BSLS_PLATFORM_CMP_SUN
#endif
#endif

#ifdef BSLS_PLATFORM_CPU_VER_MAJOR
#ifndef BDES_PLATFORM_CPU_VER_MAJOR
#define BDES_PLATFORM_CPU_VER_MAJOR  BSLS_PLATFORM_CPU_VER_MAJOR
#endif
#endif

#ifdef BSLS_PLATFORM_CPU_VER_MINOR
#ifndef BDES_PLATFORM_CPU_VER_MINOR
#define BDES_PLATFORM_CPU_VER_MINOR  BSLS_PLATFORM_CPU_VER_MINOR
#endif
#endif

#ifdef BSLS_PLATFORM_CPU_88000
#ifndef BDES_PLATFORM_CPU_88000
#define BDES_PLATFORM_CPU_88000      BSLS_PLATFORM_CPU_88000
#endif
#endif

#ifdef BSLS_PLATFORM_CPU_ALPHA
#ifndef BDES_PLATFORM_CPU_ALPHA
#define BDES_PLATFORM_CPU_ALPHA      BSLS_PLATFORM_CPU_ALPHA
#endif
#endif

#ifdef BSLS_PLATFORM_CPU_HPPA
#ifndef BDES_PLATFORM_CPU_HPPA
#define BDES_PLATFORM_CPU_HPPA       BSLS_PLATFORM_CPU_HPPA
#endif
#endif

#ifdef BSLS_PLATFORM_CPU_IA64
#ifndef BDES_PLATFORM_CPU_IA64
#define BDES_PLATFORM_CPU_IA64       BSLS_PLATFORM_CPU_IA64
#endif
#endif

#ifdef BSLS_PLATFORM_CPU_MIPS
#ifndef BDES_PLATFORM_CPU_MIPS
#define BDES_PLATFORM_CPU_MIPS       BSLS_PLATFORM_CPU_MIPS
#endif
#endif

#ifdef BSLS_PLATFORM_CPU_POWERPC
#ifndef BDES_PLATFORM_CPU_POWERPC
#define BDES_PLATFORM_CPU_POWERPC    BSLS_PLATFORM_CPU_POWERPC
#endif
#endif

#ifdef BSLS_PLATFORM_CPU_SPARC
#ifndef BDES_PLATFORM_CPU_SPARC
#define BDES_PLATFORM_CPU_SPARC      BSLS_PLATFORM_CPU_SPARC
#endif
#endif

#ifdef BSLS_PLATFORM_CPU_X86
#ifndef BDES_PLATFORM_CPU_X86
#define BDES_PLATFORM_CPU_X86        BSLS_PLATFORM_CPU_X86
#endif
#endif

#ifdef BSLS_PLATFORM_CPU_X86_64
#ifndef BDES_PLATFORM_CPU_X86_64
#define BDES_PLATFORM_CPU_X86_64     BSLS_PLATFORM_CPU_X86_64
#endif
#endif

                                 // ==========
                                 // Validation
                                 // ==========

// Unix flag must be set by the compiler if Unix detected (except for AIX).
#if defined(BDES_PLATFORM_OS_UNIX) && !defined(BDES_PLATFORM_OS_AIX)
    #if !defined(unix) && !defined(__unix__) && !defined(__unix)
        #error "Unix platform assumed, but unix flag not set by compiler"
        char assertion[0];                     // stop non-compliant compilers
    #endif
#endif

// Exactly one CMP type.
#if BDES_PLATFORM_CMP_AIX \
  + BDES_PLATFORM_CMP_GNU \
  + BDES_PLATFORM_CMP_HP \
  + BDES_PLATFORM_CMP_MSVC \
  + BDES_PLATFORM_CMP_SUN != 1
    #error "Exactly one compiler must be set."
    char assertion[0];                         // stop non-compliant compilers
#endif

// Exactly one OS type.
#if BDES_PLATFORM_OS_UNIX \
  + BDES_PLATFORM_OS_WINDOWS != 1
    #error "Exactly one operating system must be set."
    char assertion[0];                         // stop non-compliant compilers
#endif

// At most one OS subtype.
#define BDES_PLATFORM_OS_SUBTYPE_COUNT \
    BDES_PLATFORM_OS_AIX \
  + BDES_PLATFORM_OS_HPUX \
  + BDES_PLATFORM_OS_LINUX \
  + BDES_PLATFORM_OS_SOLARIS \
  + BDES_PLATFORM_OS_SUNOS \
  + BDES_PLATFORM_OS_WIN9X \
  + BDES_PLATFORM_OS_WINNT \
  + BDES_PLATFORM_OS_WIN2K \
  + BDES_PLATFORM_OS_WINXP
#if BDES_PLATFORM_OS_SUBTYPE_COUNT > 1
    #error "At most one operating system subtype must be set."
    char assertion[0];                         // stop non-compliant compilers
#endif

// Exactly one CPU type.
#if BDES_PLATFORM_CPU_88000 \
  + BDES_PLATFORM_CPU_ALPHA \
  + BDES_PLATFORM_CPU_HPPA \
  + BDES_PLATFORM_CPU_IA64 \
  + BDES_PLATFORM_CPU_X86 \
  + BDES_PLATFORM_CPU_X86_64 \
  + BDES_PLATFORM_CPU_MIPS \
  + BDES_PLATFORM_CPU_POWERPC \
  + BDES_PLATFORM_CPU_SPARC != 1
    #error "Exactly one processor must be set."
    char assertion[0];                         // stop non-compliant compilers
#endif


#if defined(BDES_PLATFORM_OS_VER_MAJOR) \
         && BDES_PLATFORM_OS_SUBTYPE_COUNT != 1
        // For OS, MAJOR VERSION implies SUBTYPE.
    #error "Operating system major version but not subtype defined."
    char assertion[0];                         // stop non-compliant compilers
#endif

#undef BDES_PLATFORM_OS_SUBTYPE_COUNT

#if defined(BDES_PLATFORM_OS_VER_MINOR) && \
   !defined(BDES_PLATFORM_OS_VER_MAJOR)
    #error "Operating System minor but not major version defined."
    char assertion[0];                         // stop non-compliant compilers
#endif

#if defined(BDES_PLATFORM_CPU_VER_MINOR) && \
   !defined(BDES_PLATFORM_CPU_VER_MAJOR)
    #error "Processor minor but not major version defined."
    char assertion[0];                         // stop non-compliant compilers
#endif

#endif // BDE_OMIT_INTERNAL_DEPRECATED

namespace BloombergLP {

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
