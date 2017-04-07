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

def normalizeType(type):
    if type[0] == '_' and type[1] == '_':
        return type[2:]
    return type

def isArray(type):
    if type[0] == '_' and type[1] == '_':
        return True
    return False

def declareVar(name, type):
    if (isArray(type)):
        index = type.find('[')
        typeName = type[:index]
        brackets = type[index:]
        return "%s %s%s" % (normalizeType(typeName), name, brackets)
    return "%s %s" % (type, name)

def getParseFunc(type):
    if (isArray(type)):
        return 'parse_' + re.sub('[\[\]]+','_',normalizeType(type))
    return 'parse_' + type;

def getEncodeFunc(type):
    if (isArray(type)):
        return 'to_string64_' + re.sub('[\[\]]+','_',normalizeType(type))
    return 'to_string64_' + type;

def proxyString(name, sig):
    # Python Array of all args (each is a hash with keys "name" and "type")
    args = sig["arguments"]

    # Make a string of form:  "type1 arg1, type2 arg2" for use in function sig
    argstring = ', '.join([normalizeType(a["type"]) + ' ' + a["name"] for a in args])

    lines = ["%s %s(%s) {" % (sig["return_type"], name, argstring)]
    lines.append('  c150debug->printf(C150RPCDEBUG,"proxy: %s invoked");' % name)
    lines.append('  RPCPROXYSOCKET->read(readBuffer, sizeof(readBuffer));')
    lines.append('  if (strncmp(readBuffer,"DONE", sizeof(readBuffer))!=0) {')
    lines.append('    throw C150Exception("proxy: %s() received invalid response from the server");' % name)
    lines.append('  }')
    lines.append('  c150debug->printf(C150RPCDEBUG,"proxy: %s() successful return from remote call");' % name)
    lines.append('}')

    return "\n".join(lines);

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
        lines.append('  response = response + " " + %s(retval);' % getEncodeFunc(sig["return_type"]))

    lines.append('  //char doneBuffer[5] = "DONE";')
    lines.append('  //char doneBuffer[5] = "DONE";')
    lines.append('  //char doneBuffer[5] = "DONE";')
    lines.append('  //char doneBuffer[5] = "DONE";')
    lines.append('  //char doneBuffer[5] = "DONE";')
    lines.append('  //char doneBuffer[5] = "DONE";')
    lines.append('  //char doneBuffer[5] = "DONE";')
    lines.append('  //char doneBuffer[5] = "DONE";')
    lines.append('  c150debug->printf(C150RPCDEBUG,"stub: %s() has returned");' % name)
    lines.append('  RPCPROXYSOCKET->write(response.c_str(), strlen(response.c_str()) + 1);')
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
    utilfuncs = [];

    for type, info in decls["types"].iteritems():
         parseDecl = "void %s(*%s, string argstr)" % (getParseFunc(type), type)
         encodeDecl = "string %s(%s x)" % (getEncodeFunc(type), type);
         utildecls.append(parseDecl + ";");
         utildecls.append(encodeDecl + ";");

    for  name, sig in decls["functions"].iteritems():
         proxyfuncs.append(proxyString(name, sig))
         stubconditionals.append("if (strcmp(functionNameBuffer, '%s'))\n      __%s();\n" % (name, name))
         stubfuncs.append(stubString(name, sig))



    proxysub = dict()
    proxysub["functions"] = "\n".join(proxyfuncs)
    proxysub["idlname"] = filename
    proxysub["utildecls"] = "\n".join(utildecls);
    proxysub["utilfuncs"] = "\n".join(utilfuncs);
    print proxytemplate.safe_substitute(proxysub)

    stubsub = dict()
    stubsub["stubs"] = "\n".join(stubfuncs)
    stubsub["idlname"] = filename
    stubsub["conditionals"] = "    else ".join(stubconditionals)
    stubsub["utildecls"] = "\n".join(utildecls);
    stubsub["utilfuncs"] = "\n".join(utilfuncs);
    print stubtemplate.safe_substitute(stubsub)

except Exception as e:
    traceback.print_exc()
    print >> sys.stderr, "Usage: %s <idlfilename>" % sys.argv[0]
