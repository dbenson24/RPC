// --------------------------------------------------------------
//
//                        simplefunction.proxy.cpp
//
//        Author: Noah Mendelsohn         
//   
//       This is a hand-crafted demonstration proxy.
//
//       For your project, your "rpcgen" program will generate
//       proxies like this one automatically from the idl
//       file. Note that this proxy also #includes the 
//       simplefunctions.idl file. Of course, your rpcgen
//       program will change that, as well as the number
//       of functions generated. More importantly, it will
//       generate code to handle function arguments and
//       return values.
//
//
//
//       Copyright: 2012 Noah Mendelsohn
//     
// --------------------------------------------------------------

// IMPORTANT! WE INCLUDE THE IDL FILE AS IT DEFINES THE INTERFACES
// TO THE FUNCTIONS WE'RE IMPLEMENTING. THIS MAKES SURE THE
// CODE HERE ACTUALLY MATCHES THE REMOTED INTERFACE



#include "$idlname"

#include "rpcproxyhelper.h"

#include <cstdio>
#include <cstring>
#include "c150debug.h"

using namespace std;
using namespace C150NETWORK;  // for all the comp150 utilities 

string readFromStream();
$utildecls
$utilfuncs

$functions

string readFromStream() {
    std::ostringstream name;    // name to build
    char bufc;                  // next char to read
    ssize_t readlen;            // amount of data read from socket

    while(1) {
        readlen = RPCSTUBSOCKET-> read(&bufc, 1);  // read a byte

        //
        // With TCP streams, we should never get a 0 length read except with
        // timeouts (which we're not setting in pingstreamserver) or EOF
        //
        if (readlen == 0) {
            c150debug->printf(C150RPCDEBUG,"simplefunction.stub: read zero length message, checking EOF");
            if (RPCSTUBSOCKET-> eof()) {
                c150debug->printf(C150RPCDEBUG,"simplefunction.stub: EOF signaled on input");
                return name.str();
            } else {
                throw C150Exception("simplefunction.stub: unexpected zero length read without eof");
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
