// --------------------------------------------------------------
//
//                        proxy.cpp
//
//        Author: Derek Benson and Stephen Michel
//   
//       This is a generated file for $idlname
//
//     
// --------------------------------------------------------------

#include "rpcproxyhelper.h"

#include <cstdio>
#include <cstring>
#include "c150debug.h"
#include "base64.h"

using namespace std;
using namespace C150NETWORK;  // for all the comp150 utilities 

#include "$idlname"

string readFromStream();
$utildecls
$utilfuncs

$functions

string readFromStream() {
    std::ostringstream name;    // name to build
    char bufc;                  // next char to read
    ssize_t readlen;            // amount of data read from socket

    while(1) {
        readlen = RPCPROXYSOCKET-> read(&bufc, 1);  // read a byte

        //
        // With TCP streams, we should never get a 0 length read except with
        // timeouts (which we're not setting in pingstreamserver) or EOF
        //
        if (readlen == 0) {
            c150debug->printf(C150RPCDEBUG,"simplefunction.proxy: read zero length message, checking EOF");
            if (RPCPROXYSOCKET-> eof()) {
                c150debug->printf(C150RPCDEBUG,"simplefunction.proxy: EOF signaled on input");
                return name.str();
            } else {
                throw C150Exception("simplefunction.proxy: unexpected zero length read without eof");
            }
        }

        // check for null or space
        if (bufc == '\0') {
            return name.str();
        }
        name << bufc;
    }
    throw C150Exception("getFunctionNameFromString: This should never be thrown.");
}
