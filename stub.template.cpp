// --------------------------------------------------------------
//
//                  generated stub for $idlname
//
//        Author: Derek Benson and Stephen Michel       
//
//       Allows for functions defined in $idlname to be called on 
//       a remote client.
//
// --------------------------------------------------------------


#include "rpcstubhelper.h"

#include <cstdio>
#include <cstring>
#include "c150debug.h"
#include "base64.h"

using namespace std;
using namespace C150NETWORK;  // for all the comp150 utilities 

// Include the idl file
#include "$idlname"

void getFunctionNamefromStream();

$utildecls

$utilfuncs

// ======================================================================
//                             STUBS
//
//    Each of these is generated to match the signature of the 
//    function for which it is a stub. The names are prefixed with "__"
//    to keep them distinct from the actual routines, which we call!
//
// ======================================================================

$stubs

//
//     __badFunction
//
//   Pseudo-stub for missing functions.
//

void __badFunction(const char *functionName) {
  char doneBuffer[5] = "BAD";  // to write magic value DONE + null


  //
  // Send the response to the client indicating bad function
  //

  c150debug->printf(C150RPCDEBUG,"stub: received call for nonexistent function %s()",functionName);
  RPCSTUBSOCKET->write(doneBuffer, strlen(doneBuffer)+1);
}



// ======================================================================
//
//                        COMMON SUPPORT FUNCTIONS
//
// ======================================================================

// forward declaration
string readFromStream();



//
//                         dispatchFunction()
//
//   Called when we're ready to read a new invocation request from the stream
//

void dispatchFunction() {

  istringstream nameAndArgs;
  string name;
  string args;

  nameAndArgs.str(readFromStream());
  nameAndArgs >> name;
  nameAndArgs >> args;

  const char *functionNameBuffer = name.c_str();

  if (!RPCSTUBSOCKET-> eof()) {
    $conditionals    else
      __badFunction(functionNameBuffer);
  }
}

 
//
//                   readFromStream
//
//   Important: this routine must leave the sock open but at EOF
//   when eof is read from client.
//
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
            c150debug->printf(C150RPCDEBUG,"stub: read zero length message, checking EOF");
            if (RPCSTUBSOCKET-> eof()) {
                c150debug->printf(C150RPCDEBUG,"stub: EOF signaled on input");
                return name.str();
            } else {
                throw C150Exception("stub: unexpected zero length read without eof");
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


