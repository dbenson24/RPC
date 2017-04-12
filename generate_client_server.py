#!/bin/env python
#
#          print signatures of all the functions named in supplied IDL file
#

import subprocess
import json
import sys
import os
import re
import traceback
from string import Template


print sys.argv

# Takes a type and removes underscores at the front (dealing with arrays)
def normalizeType(type):
    if type[0] == '_' and type[1] == '_':
        return type[2:]
    return type

# Returns true if the type represents an array
def isArray(type):
    if type[0] == '_' and type[1] == '_':
        return True
    return False

# returns a string that declares a named variable of a specific type
def declareVar(name, type):
    if (isArray(type)):
        index = type.find('[')
        typeName = type[:index]
        brackets = type[index:]
        return "%s %s%s" % (normalizeType(typeName), name, brackets)
    return "%s %s" % (type, name)

# returns the name of the function that should be called to parse a string
# and return that type
def getParseFunc(type):
    if (isArray(type)):
        return 'parse_' + re.sub('[\[\]]+','_',normalizeType(type))
    return 'parse_' + type;

# returns the name of a function that should be called to get a base64 encoding
# of the given type
def getEncodeFunc(type):
    if (isArray(type)):
        return 'to_string64_' + re.sub('[\[\]]+','_',normalizeType(type))
    return 'to_string64_' + type;

# generates the string needed in the proxy template for the given function and
# signature
def proxyString(name, sig):
    # Python Array of all args (each is a hash with keys "name" and "type")
    args = sig["arguments"]

    # Make a string of form:  "type1 arg1, type2 arg2" for use in function sig
    argstring = ', '.join([normalizeType(a["type"]) + ' ' + a["name"] for a in args])

    lines = ["%s %s(%s) {" % (sig["return_type"], name, argstring)]
    lines.append('  istringstream args;')
    for arg in args:
        lines.append('  args << %s(&%s) << " ";' % (getEncodeFunc(arg["type"]), arg["name"]))
    lines.append('  string outgoing = "%s " + base64_encode(args.str());' % name)
    lines.append('  RPCPROXYSOCKET->write(outgoing.c_str(), strlen(outgoing.c_str()) + 1);')
    lines.append('  c150debug->printf(C150RPCDEBUG,"proxy: %s invoked");' % name)
    lines.append('  ostringstream ret;')
    lines.append('  string raw = readFromStream();')
    lines.append('  ret.str(raw);')
    lines.append('  string name;')
    lines.append('  ret >> name;')
    lines.append('  if (name != "%s") {' %name)
    lines.append('    //panic!')
    lines.append('  }')
    if sig["return_type"] != "void":
        lines.append('  string msg;')
        lines.append('  ret >> msg;')
        lines.append('  %s retval;' % normalizeType(sig["return_type"]))
        lines.append('  %s(&retval, base64_decode(msg));' % getParseFunc(sig["return_type"]))
        lines.append('  return retval;')

    else:
        lines.append('  return;')
    lines.append('}')

    return "\n".join(lines);

# generates the string needed in the stub template for the given function and
# signature
def stubString(name, sig):
    # Python Array of all args (each is a hash with keys "name" and "type")
    args = sig["arguments"]

    # Make a string of form:  "type1 arg1, type2 arg2" for use in function sig
    argstring = ', '.join([normalizeType(a["type"]) + ' ' + a["name"] for a in args])

    lines = ['void __%s(string args64) {' % name]
    lines.append('  istringstream args;')
    lines.append('  string arg64;')
    lines.append('  args.str(base64_decode(args64));')
    names = []
    for a in args:
        names.append(a["name"])
        lines.append('  %s;' % declareVar(a["name"], a["type"]))
        lines.append('  args >> arg64;')
        lines.append('  %s(&%s, base64_decode(arg64));' % (getParseFunc(a["type"]), a["name"]))


    lines.append('  string response = "%s";' % name)
    lines.append('  c150debug->printf(C150RPCDEBUG,"stub: invoking %s()");' % name)
    if sig["return_type"] == "void":
        lines.append('  %s(%s);' % (name, ", ".join(names)))
    else:
        lines.append('  %s retval = %s(%s);' % (sig["return_type"], name, ", ".join(names)))
        lines.append('  response = response + " " + %s(&retval);' % getEncodeFunc(sig["return_type"]))

    lines.append('  c150debug->printf(C150RPCDEBUG,"stub: %s() has returned");' % name)
    lines.append('  RPCSTUBSOCKET->write(response.c_str(), strlen(response.c_str()) + 1);')
    lines.append('}')

    return "\n".join(lines);


try:
    #
    #     Make sure invoked properly
    #
    if len(sys.argv) != 2:
        raise "Wrong number of arguments"

    #
    #     Make sure file exists and is readable
    #
    filename = sys.argv[1]
    if (not os.path.isfile(filename)):
        print >> sys.stderr, "Path %s does not designate a file" % filename
        raise "No file named " + filename
    if (not os.access(filename, os.R_OK)):
        print >> sys.stderr, "File %s is not readable" % filename
        raise "File " + filename + " not readable"

    with open("proxy.template.cpp") as f:
        proxytemplate = Template(f.read())

    with open("stub.template.cpp") as f:
        stubtemplate = Template(f.read())

    #
    #     Parse declarations into a Python dictionary
    #
    decls = json.loads(subprocess.check_output(["./idl_to_json", filename]))

    #
    # Loop printing each function signature
    #

    proxyfuncs = []
    stubfuncs = []
    stubconditionals = []
    utildecls = [];

    # Adds the built in function declarations
    utildecls.append("""
///////////////Built in///////////////////////////
string to_string64_string(string *val);
void parse_string(string *value, string arg);
string to_string64_int(int *val);
void parse_int(int *value, string arg);
string to_string64_float(float *val);
void parse_float(float *value, string arg);
//////////////////////////////////////////////////""")
    utilfuncs = [];
    # Adds the built in function implementations
    utilfuncs.append("""
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
//////////////////////////////////////////////////""")


    # loop generates all of encode and parse functions needed for each type
    for type, info in decls["types"].iteritems():
        if info["type_of_type"] == "builtin":
            continue
        parseDecl = "void %s(%s *val, string argstr)" % (getParseFunc(type), type)
        encodeDecl = "string %s(%s *val)" % (getEncodeFunc(type), type);
        utildecls.append(parseDecl + ";");
        utildecls.append(encodeDecl + ";");
        if info["type_of_type"] == "struct":
            parseLines = [parseDecl + " {"]
            parseLines.append("  istringstream args;")
            parseLines.append("  string arg64;")
            parseLines.append("  args.str(argstr);")
            for member in info["members"]:
                parseLines.append("  args >> arg64;")
                parseLines.append("  %s(&((*val).%s), base64_decode(arg64));" % (getParseFunc(member["type"]), member["name"]))
            parseLines.append("}")
            utilfuncs.append("\n".join(parseLines))

            encodeLines = [encodeDecl + " {"]
            encodeLines.append("  ostringstream s;")
            encodeLines.append("  string arg64;")
            encodeLines.append("  args.str(argstr);")
            for member in info["members"]:
                encodeLines.append("  s << %s(&((*val).%s)) << ' '" % (getEncodeFunc(member["type"]), member["name"]))
            encodeLines.append("}")
            utilfuncs.append("\n".join(encodeLines))
        else:
            continue

    for  name, sig in decls["functions"].iteritems():
        proxyfuncs.append(proxyString(name, sig))
        stubconditionals.append("if (strcmp(functionNameBuffer, '%s'))\n      __%s();\n" % (name, name))
        stubfuncs.append(stubString(name, sig))



    proxyFileName = filename.split(".")[0] + ".proxy.cpp.gen"

    proxysub = dict()
    proxysub["functions"] = "\n".join(proxyfuncs)
    proxysub["idlname"] = filename
    proxysub["utildecls"] = "\n".join(utildecls);
    proxysub["utilfuncs"] = "\n".join(utilfuncs);

    with open(proxyFileName, 'w') as f:
        f.write(proxytemplate.safe_substitute(proxysub))
        f.close()


    stubFileName = filename.split(".")[0] + ".stub.cpp.gen"

    stubsub = dict()
    stubsub["stubs"] = "\n".join(stubfuncs)
    stubsub["idlname"] = filename
    stubsub["conditionals"] = "    else ".join(stubconditionals)
    stubsub["utildecls"] = "\n".join(utildecls);
    stubsub["utilfuncs"] = "\n".join(utilfuncs);
    with open(stubFileName, 'w') as f:
        f.write(stubtemplate.safe_substitute(stubsub))
        f.close()





except Exception as e:
    traceback.print_exc()
    print >> sys.stderr, "Usage: %s <idlfilename>" % sys.argv[0]
