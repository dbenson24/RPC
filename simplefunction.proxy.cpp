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



#include "simplefunction.idl"

#include "rpcproxyhelper.h"

#include <cstdio>
#include <cstring>
#include "c150debug.h"
#include "base64.h"

using namespace std;
using namespace C150NETWORK;  // for all the comp150 utilities 

string readFromStream();

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

void func3() {
  ostringstream args;
  args << " ";
  string outgoing = "func3 " + base64_encode(args.str());
  RPCPROXYSOCKET->write(outgoing.c_str(), strlen(outgoing.c_str()) + 1);
  c150debug->printf(C150RPCDEBUG,"proxy: func3 invoked");
  istringstream ret;
  string raw = readFromStream();
  ret.str(raw);
  string name;
  ret >> name;
  if (name != "func3") {
    //panic!
  }
  return;
}
void func2() {
  ostringstream args;
  args << " ";
  string outgoing = "func2 " + base64_encode(args.str());
  RPCPROXYSOCKET->write(outgoing.c_str(), strlen(outgoing.c_str()) + 1);
  c150debug->printf(C150RPCDEBUG,"proxy: func2 invoked");
  istringstream ret;
  string raw = readFromStream();
  ret.str(raw);
  string name;
  ret >> name;
  if (name != "func2") {
    //panic!
  }
  return;
}
void func1() {
  ostringstream args;
  args << " ";
  string outgoing = "func1 " + base64_encode(args.str());
  RPCPROXYSOCKET->write(outgoing.c_str(), strlen(outgoing.c_str()) + 1);
  c150debug->printf(C150RPCDEBUG,"proxy: func1 invoked");
  istringstream ret;
  string raw = readFromStream();
  ret.str(raw);
  string name;
  ret >> name;
  if (name != "func1") {
    //panic!
  }
  return;
}

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
