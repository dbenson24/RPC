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

string to_string64_string(string val) {
    return base64_encode(val);
}

void parse_string(string *value, string arg) {
    *value = args;
}

//////////////////////////////////////////////////

string to_string64_int(int val) {
    return base64_encode(to_string(val));
}

void parse_int(int *value, string arg) {
    *value = stoi(arg);
}



string to_string64_float(float val) {
    return base64_encode(to_string(val));
}

void parse_float(float *value, string arg) {
    *value = stof(arg);
}

//////////////////////////////////////////////////

string to_string64_strings_4(string *val) {
    ostringstream s;

    for (int i = 0; i < 4; i++) {
        s << to_string64(val[i]) << " ";
    }

    return base64_encode(s.str());
}

void parse_strings_4(string *value, string argstr) {
    istringstream args;
    string arg;
    args.str(argstr);

    for (int i = 0; i < 4; i++) {
        args >> arg;
        parse_string(*value[i], base64_decode(arg));
    }
}

//////////////////////////////////////////////////

void parse_person(person *value, string argstr) {
    istringstream args;
    string arg;
    args.str(argstr);

    args >> arg;
    parse_string(*value.name, base64_decode(arg));

    args >> arg;
    parse_int(*value.age, base64_decode(arg));

    args >> arg;
    parse_int(*value.height, base64_decode(arg));
}

string to_string64_person(person *val) {
    ostringstream s;

    s << *val.name << " ";
    s << *val.age << " ";
    s << *val.height << " ";

    return base64_encode(s.str());
}
