// --------------------------------------------------------------
//
//                  generated stub for simplefunction.idl
//
//        Author: Derek Benson and Stephen Michel       
//
//       Allows for functions defined in simplefunction.idl to be called on 
//       a remote client.
//
// --------------------------------------------------------------

// Include the idl file
#include "simplefunction.idl"



#include "rpcstubhelper.h"

#include <cstdio>
#include <cstring>
#include "c150debug.h"
#include "base64.h"

using namespace std;
using namespace C150NETWORK;  // for all the comp150 utilities 

void getFunctionNamefromStream();


///////////////Built in///////////////////////////
string to_string64_string(string *val);
void parse_string(string *value, string arg);
string to_string64_int(int *val);
void parse_int(int *value, string arg);
string to_string64_float(float *val);
void parse_float(float *value, string arg);
//////////////////////////////////////////////////


///////////////Built in///////////////////////////
string to_string64_string(string *val) {
    return base64_encode(*val);
}
void parse_string(string *value, string arg) {
    *value = arg;
}
string to_string64_int(int *val) {
    return base64_encode(to_string(*val));
}
void parse_int(int *value, string arg) {
    *value = stoi(arg);
}
string to_string64_float(float *val) {
    return base64_encode(to_string(*val));
}

void parse_float(float *value, string arg) {
    *value = stof(arg);
}
//////////////////////////////////////////////////

// ======================================================================
//                             STUBS
//
//    Each of these is generated to match the signature of the 
//    function for which it is a stub. The names are prefixed with "__"
//    to keep them distinct from the actual routines, which we call!
//
// ======================================================================

void __func3(string args64) {
  istringstream args;
  string arg64;
  args.str(base64_decode(args64));
  string response = "func3";
  c150debug->printf(C150RPCDEBUG,"stub: invoking func3()");
  func3();
  c150debug->printf(C150RPCDEBUG,"stub: func3() has returned");
  RPCSTUBSOCKET->write(response.c_str(), strlen(response.c_str()) + 1);
}
void __func2(string args64) {
  istringstream args;
  string arg64;
  args.str(base64_decode(args64));
  string response = "func2";
  c150debug->printf(C150RPCDEBUG,"stub: invoking func2()");
  func2();
  c150debug->printf(C150RPCDEBUG,"stub: func2() has returned");
  RPCSTUBSOCKET->write(response.c_str(), strlen(response.c_str()) + 1);
}
void __func1(string args64) {
  istringstream args;
  string arg64;
  args.str(base64_decode(args64));
  string response = "func1";
  c150debug->printf(C150RPCDEBUG,"stub: invoking func1()");
  func1();
  c150debug->printf(C150RPCDEBUG,"stub: func1() has returned");
  RPCSTUBSOCKET->write(response.c_str(), strlen(response.c_str()) + 1);
}

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
    if (!strcmp(functionNameBuffer, "func3"))
 __func3(args);
    else if (!strcmp(functionNameBuffer, "func2"))
 __func2(args);
    else if (!strcmp(functionNameBuffer, "func1"))
 __func1(args);
    else
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


