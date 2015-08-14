// btlso_streamsocket.t.cpp                                           -*-C++-*-

#include <btlso_streamsocket.h>

#include <btlso_ipv4address.h>
#include <btlso_sockethandle.h>
#include <btlso_socketoptutil.h>

#include <bsls_timeinterval.h>

#include <bsl_cstring.h>
#include <bsl_cstdlib.h>     // atoi()
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  OVERVIEW
//
// The component under test is a protocol component.  Therefore, the test plan
// consists of validating the protocol via the standard protocol test, and
// verifying the syntactic correctness of the associated usage examples.
//-----------------------------------------------------------------------------
// [1] PROTOCOL TEST
//-----------------------------------------------------------------------------

//=============================================================================
//                       STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
static int verbose;

//=============================================================================
//                             PROTOCOL TEST CLASS
//-----------------------------------------------------------------------------
struct TestAddress {
    int d_address;
};

class my_StreamSocket : public btlso::StreamSocket<TestAddress> {
    // This class tests the btlso_streamsocket protocol by assigning
    // a different function code to the value pointed to by d_fcode_p
    // for each virtual member function call.

    int *d_fcode_p;    // code describing last function called:
                       //  1: ~my_StreamSocket
                       //  2: accept(result)
                       //  3: accept(socket, peerAddress)
                       //  4: bind
                       //  5: connect
                       //  6: listen
                       //  7: read
                       //  8: readv
                       //  9: write
                       // 10: writev(btes::IOvec)
                       // 11: writev(btls::Ovec)
                       // 12: setBlockingMode
                       // 13: shutdown
                       // 14: waitForConnect
                       // 15: waitForAccept
                       // 16: waitForIO(timeout)
                       // 17: waitForIO
                       // 18: setLingerOption
                       // 19: setOption
                       // 20: blockingMode
                       // 21: connectionStatus
                       // 22: handle
                       // 23: localAddress
                       // 24: peerAddress
                       // 25: lingerOption
                       // 26: socketOption

    int *d_shutdownMode_p;

public:
    // CREATORS
    my_StreamSocket(int *fcode, int *shutdownMode)
        : d_fcode_p(fcode), d_shutdownMode_p(shutdownMode) {}

    virtual ~my_StreamSocket()
        { *d_fcode_p = 1; }

    // MANIPULATORS
    virtual int accept(btlso::StreamSocket<TestAddress> **result)
        { *d_fcode_p = 2; return 0; }

    virtual int accept(btlso::StreamSocket<TestAddress> **socket,
                       TestAddress                      *peerAddress)
        { *d_fcode_p = 3; return 0; }

    virtual int bind(const TestAddress& address)
        { *d_fcode_p = 4; return 0; }

    virtual int connect(const TestAddress& address)
        { *d_fcode_p = 5; return 0; }

    virtual int listen(int backlog)
        { *d_fcode_p = 6; return 0; }

    virtual int read(char *buffer, int length)
        { *d_fcode_p = 7; return 0; }

    virtual int readv(const btls::Iovec *buffers, int numBuffers)
        { *d_fcode_p = 8; return 0; }

    virtual int write(const char *buffer, int length)
        { *d_fcode_p = 9; return 0; }

    virtual int writev(const btls::Iovec *buffers, int numBuffers)
        { *d_fcode_p = 10; return 0; }

    virtual int writev(const btls::Ovec *buffers, int numBuffers)
        { *d_fcode_p = 11; return 0; }

    virtual int setBlockingMode(bteso_Flag::BlockingMode mode)
        { *d_fcode_p = 12; return 0; }

    virtual int shutdown(bteso_Flag::ShutdownType type)
        { *d_fcode_p = 13; *d_shutdownMode_p = type; return 0; }

    virtual int waitForConnect(const bsls::TimeInterval& timeout)
        { *d_fcode_p = 14; return 0; }

    virtual int waitForAccept(const bsls::TimeInterval& timeout)
        { *d_fcode_p = 15; return 0; }

    virtual int waitForIO(bteso_Flag::IOWaitType     type,
                          const bsls::TimeInterval&   timeout)
        { *d_fcode_p = 16; return 0; }

    virtual int waitForIO(bteso_Flag::IOWaitType type)
        { *d_fcode_p = 17; return 0; }

    virtual int setLingerOption(const btlso::SocketOptUtil::LingerData& options)
        { *d_fcode_p = 18; return 0; }

    virtual int setOption(int level, int option, int value)
        { *d_fcode_p = 19; return 0; }

    // ACCESSORS
    virtual int blockingMode(bteso_Flag::BlockingMode *result)
            const
        { *d_fcode_p = 20; return 0; }

    virtual int connectionStatus() const
        { *d_fcode_p = 21; return 0; }

    virtual btlso::SocketHandle::Handle handle() const
        { *d_fcode_p = 22; return static_cast<btlso::SocketHandle::Handle>(0); }

    virtual int localAddress(TestAddress *result) const
        { *d_fcode_p = 23; return 0; }

    virtual int peerAddress(TestAddress *result) const
        { *d_fcode_p = 24; return 0; }

    virtual int lingerOption(btlso::SocketOptUtil::LingerData *result) const
        { *d_fcode_p = 25; return 0; }

    virtual int socketOption(int *result, int level, int option) const
        { *d_fcode_p = 26; return 0; }
};

//=============================================================================
//                               USAGE EXAMPLES
//-----------------------------------------------------------------------------

// Example 1
//
void dictionary_client(btlso::StreamSocket<btlso::IPv4Address> *client)
{
    enum {
        MAX_DEF_LENGTH    = 512,    // maximum length of definition
        SERVER_PORT       = 2698    // arbitrary port number
    };

    const char *SERVER_ADDRESS = "127.0.0.1";
    btlso::IPv4Address address(SERVER_ADDRESS, SERVER_PORT);
    int rc;

    rc = client->connect(address);
    if (rc) {
        if (verbose)
            bsl::cout << "Failed to connect to server: " << rc << bsl::endl;
        return;                                                       // RETURN
    }

    const char word[] = "socket";
    char definition[MAX_DEF_LENGTH];
    int length;

    length = htonl(sizeof word); // includes the trailing null character
    rc = client->write((char*)&length, sizeof length);
    if (sizeof length != rc) {
        if (verbose)
            bsl::cout << "Error writing request header to server: "
                      << rc << bsl::endl;
        client->shutdown(bteso_Flag::e_SHUTDOWN_BOTH);
        return;                                                       // RETURN
    }
    rc = client->write(word, sizeof word);
    if (rc != length) {
        if (verbose)
            bsl::cout << "Error writing request body to server: "
                      << rc << bsl::endl;
        client->shutdown(bteso_Flag::e_SHUTDOWN_BOTH);
        return;                                                       // RETURN
    }

    rc = client->read((char*)&length, sizeof length);
    if (rc != sizeof length) {
        if (verbose)
            bsl::cout << "Error reading from server: " << rc << bsl::endl;
        client->shutdown(bteso_Flag::e_SHUTDOWN_BOTH);
        return;                                                       // RETURN
    }

    length = ntohl(length);
    ASSERT(0 < length);
    ASSERT((int)sizeof definition >= length);
    rc = client->read(definition, length);
    if (rc != length) {
        if (verbose)
            bsl::cout << "Error reading from server: " << rc << bsl::endl;
        client->shutdown(bteso_Flag::e_SHUTDOWN_BOTH);
        return;                                                       // RETURN
    }
    if (verbose)
        bsl::cout << definition << bsl::endl;
    client->shutdown(bteso_Flag::e_SHUTDOWN_BOTH);
}

// Example 2
//
const char *lookupWord(const char *word)
{
    return 0;
}

void dictionary_server(btlso::StreamSocket<btlso::IPv4Address> *server)
{
    enum {
        MAX_WORD_LENGTH        = 512,  // maximum length of word to look up
        MAX_LISTEN_QUEUE_DEPTH = 20,   // maximum number of pending requests
        SERVER_PORT            = 2698  // arbitrary port number
    };

    btlso::IPv4Address addr(btlso::IPv4Address::k_ANY_ADDRESS, SERVER_PORT);
    int rc;
    rc = server->bind(addr);
    if (rc != 0) {
        if (verbose)
            bsl::cout << "Failed to bind socket: " << rc << bsl::endl;
        return;                                                       // RETURN
    }

    rc = server->listen(MAX_LISTEN_QUEUE_DEPTH);
    if (rc != 0) {
        if (verbose)
            bsl::cout << "Failed to listen on socket: " << rc << bsl::endl;
        return;                                                       // RETURN
    }
    do {
        btlso::StreamSocket<btlso::IPv4Address> *client = 0;
        rc = server->accept(&client);
        ASSERT(0 == rc);
        ASSERT(client);

        char word[MAX_WORD_LENGTH];
        int length;

        rc = client->read((char*)&length, sizeof length);
        if (rc != sizeof length) {
            if (verbose)
                bsl::cout << "Error reading from client: " << rc << bsl::endl;
            client->shutdown(bteso_Flag::e_SHUTDOWN_BOTH);
            continue;
        }

        length = ntohl(length);
        if (length < 0 || length > (int)sizeof word) {
            if (verbose)
                bsl::cout << "Invalid request length: " << length << bsl::endl;
            client->shutdown(bteso_Flag::e_SHUTDOWN_BOTH);
            continue;
        }

        rc = client->read(word, length);
        if (length != rc) {
            if (verbose)
                bsl::cout << "Error reading from client: " << rc << bsl::endl;
            client->shutdown(bteso_Flag::e_SHUTDOWN_BOTH);
            continue;
        }
        const char *definition = lookupWord(word);
        if (definition == 0) {
            definition = "UNKNOWN!";
        }
        length = strlen(definition) + 1;
        int tmp = htonl(length);
        rc = client->write((char*)&tmp, sizeof tmp);
        if (sizeof tmp != rc) {
            if (verbose)
                bsl::cout << "Error writing to client: " << rc << bsl::endl;
            client->shutdown(bteso_Flag::e_SHUTDOWN_BOTH);
            continue;
        }
        rc = client->write(definition, length);
        if (rc != length) {
            if (verbose)
                bsl::cout << "Error writing to client: " << rc << bsl::endl;
            client->shutdown(bteso_Flag::e_SHUTDOWN_BOTH);
            continue;
        }

        client->shutdown(bteso_Flag::e_SHUTDOWN_BOTH);
    } while(1);
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2; // this one is global
    // int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 1: {
        // -----------------------------------------------------------------
        // PROTOCOL TEST:
        //   Make sure that a subclass of 'btlso::StreamSocket<ADDRESS>'
        //   compiles and links when all virtual functions are defined.
        // Testing:
        //   ~btlso::StreamSocket();
        //   int accept(btlso::StreamSocket<ADDRESS> **result);
        //   int accept(btlso::StreamSocket<ADDRESS> **socket,
        //              ADDRESS                      *peerAddress );
        //   int bind(const ADDRESS& address);
        //   int connect(const ADDRESS& address);
        //   int listen(int backlog);
        //   int read(char *buffer, int length);
        //   int readv(const btls::Iovec *buffers, int numBuffers);
        //   int write(const char *buffer, int length);
        //   int writev(const btls::Iovec *buffers, int numBuffers);
        //   int writev(const btls::Ovec  *buffers, int numBuffers);
        //   int setBlockingMode(bteso_Flag::BlockingMode mode);
        //   int shutdown(bteso_Flag::ShutdownType streamOption);
        //   int waitForConnect(const bsls::TimeInterval& timeout);
        //   int waitForAccept(const bsls::TimeInterval& timeout);
        //   int waitForIO(bteso_Flag::IOWaitType     type,
        //                 const bsls::TimeInterval&   timeout);
        //   int waitForIO(bteso_Flag::IOWaitType type);
        //   int setLingerOption(
        //           const btlso::SocketOptUtil::LingerData& options);
        //   int setOption(int level, int option, int value);
        //   int blockingMode(bteso_Flag::BlockingMode *result) const;
        //   int connectionStatus() const;
        //   btlso::SocketHandle::Handle handle() const;
        //   int localAddress(ADDRESS *result) const;
        //   int peerAddress(ADDRESS *result) const;
        //   int lingerOption(btlso::SocketOptUtil::LingerData *result) const;
        //   int socketOption(int *result, int level, int option) const;
        // -----------------------------------------------------------------

        if (verbose) cout << endl << "PROTOCOL TEST" << endl
                                  << "=============" << endl;

         int                                c;
         int                                f;
         int                                r;
         my_StreamSocket                   *m = new my_StreamSocket(&c, &f);
         my_StreamSocket&                   t = *m;
         char                              *b = 0;
         btlso::StreamSocket<TestAddress>   *s = 0;
         TestAddress                        a;
         btls::Iovec                        *iov = 0;
         btls::Ovec                         *ov = 0;
         bsls::TimeInterval                  to;
         btlso::SocketOptUtil::LingerData    ld;
         bteso_Flag::BlockingMode          *bm = 0;

         if (verbose) cout << "Testing manipulators." << endl;

         t.accept(&s);
         ASSERT(2 == c);

         t.accept(&s, &a);
         ASSERT(3 == c);

         t.bind(a);
         ASSERT(4 == c);

         t.connect(a);
         ASSERT(5 == c);

         t.listen(1);
         ASSERT(6 == c);

         t.read(b, 1);
         ASSERT(7 == c);

         t.readv(iov, 1);
         ASSERT(8 == c);

         t.write(b, 1);
         ASSERT(9 == c);

         t.writev(iov, 1);
         ASSERT(10 == c);

         t.writev(ov, 1);
         ASSERT(11 == c);

         t.setBlockingMode(bteso_Flag::e_NONBLOCKING_MODE);
         ASSERT(12 == c);

         t.shutdown(bteso_Flag::e_SHUTDOWN_BOTH);
         ASSERT(13 == c);

         t.waitForConnect(to);
         ASSERT(14 == c);

         t.waitForAccept(to);
         ASSERT(15 == c);

         t.waitForIO(bteso_Flag::e_IO_RW, to);
         ASSERT(16 == c);

         t.waitForIO(bteso_Flag::e_IO_WRITE);
         ASSERT(17 == c);

         t.setLingerOption(ld);
         ASSERT(18 == c);

         t.setOption(1, 2, 3);
         ASSERT(19 == c);

         if (verbose) cout << "Testing accessors." << endl;

         t.blockingMode(bm);
         ASSERT(20 == c);

         t.connectionStatus();
         ASSERT(21 == c);

         t.handle();
         ASSERT(22 == c);

         t.localAddress(&a);
         ASSERT(23 == c);

         t.peerAddress(&a);
         ASSERT(24 == c);

         t.lingerOption(&ld);
         ASSERT(25 == c);

         t.socketOption(&r, 1, 2);
         ASSERT(26 == c);

         delete m;
         ASSERT(1 == c);

         if (verbose) cout << "Testing close guard." << endl;

         c = -1;
         f = -1;
         m = new my_StreamSocket(&c, &f);
         {
             btlso::StreamSocketAutoClose<TestAddress>
                                     guard(m, bteso_Flag::e_SHUTDOWN_BOTH);

             guard.release();
         }
         ASSERT(-1 == c);

         {
             btlso::StreamSocketAutoClose<TestAddress>
                                     guard(m, bteso_Flag::e_SHUTDOWN_BOTH);
         }
         ASSERT(13 == c);
         ASSERT(bteso_Flag::e_SHUTDOWN_BOTH == f);

         {
             btlso::StreamSocketAutoClose<TestAddress>
                                     guard(m, bteso_Flag::e_SHUTDOWN_SEND);
         }
         ASSERT(13 == c);
         ASSERT(bteso_Flag::e_SHUTDOWN_SEND == f);

         {
             btlso::StreamSocketAutoClose<TestAddress>
                                  guard(m, bteso_Flag::e_SHUTDOWN_RECEIVE);
         }
         ASSERT(13 == c);
         ASSERT(bteso_Flag::e_SHUTDOWN_RECEIVE == f);

         delete m;
         ASSERT(1 == c);

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

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
