#include <base64.cpp>
#include <sstream>

using namespace C150NETWORK;  // for all the comp150 utilities 

string getNextFromStream();

string* parse_string(char* input, int length);
int*    parse_int   (char* input, int length);
float*  parse_float (char* input, int length);

dispatchFunction() {
    istringstream nameAndArgs;
    string name;
    string args;

    nameAndArgs.str(readFromStream());
    nameAndArgs >> name;
    nameAndArgs >> args;

    if (name == "known_function_that_takes_a_struct_t") {
        parse_struct_t(args)
    }
}

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

//////////////////////////////////////////////////

string to_string64_string(string *val) {
    return base64_encode(*val);
}

void parse_string(string *value, string arg) {
    *value = args;
}

//////////////////////////////////////////////////

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

string to_string64_$TYPE_$LEN(string **val) {
    ostringstream s;

    for (int i = 0; i < $LEN; i++) {
        s << to_string64(*val[i]) << " ";
    }

    return base64_encode(s.str());
}

void parse_strings_$LEN(string *val, string argstr) {
    istringstream args;
    string arg64;
    args.str(argstr);

    for (int i = 0; i < $LEN; i++) {
        args >> arg64;
        parse_string(*val[i], base64_decode(arg64));
    }
}

//////////////////////////////////////////////////

void parse_$TYPE($TYPE *val, string argstr) {
    istringstream args;
    string arg64;
    args.str(argstr);

    args >> arg64;
    parse_string(&((*val).$MEMBER), base64_decode(arg64));
}

string to_string64_$TYPE($TYPE *val) {
    ostringstream s;

    s << $TO_STRING64_TYPE(&((*val).$MEMBER)) << " ";

    return base64_encode(s.str());
}



proxy() {
    // Make the call
    istringstream args;
    args << $TO_STRING_TYPE(&arg) << " ";
    rpcCall("$NAME " + base64_encode(args.str()));

    // Get the response
    ostringstream ret;
    string msg = readFromStream();
    ret.str(msg);
    ret >> msg;
    if (msg != "$NAME") {
        // PANIC
    }
    // if (!FUNCTION_IS_VOID):
    //    ret >> msg;
    //    $RETURN_TYPE retval;
    //    parse_$TYPE(&retval, base64_decode(msg));
    //    return retval;
}


















