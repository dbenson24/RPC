#!/bin/env python
#
#          print signatures of all the functions named in supplied IDL file
#

import subprocess
import json
import sys
import os
from string import Template


print sys.argv

def proxyString(name, sig):
    # Python Array of all args (each is a hash with keys "name" and "type")
    args = sig["arguments"]

    # Make a string of form:  "type1 arg1, type2 arg2" for use in function sig
    argstring = ', '.join([a["type"] + ' ' + a["name"] for a in args])

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
    argstring = ', '.join([a["type"] + ' ' + a["name"] for a in args])

    lines = ["%s __%s(%s) {" % (sig["return_type"], name, argstring)]
    lines.append('  char doneBuffer[5] = "DONE";')
    lines.append('  c150debug->printf(C150RPCDEBUG,"stub: invoking %s()");' % name)
    lines.append('  %s();' % name)
    lines.append('  c150debug->printf(C150RPCDEBUG,"stub: %s() has returned");' % name)
    lines.append('  RPCPROXYSOCKET->write(doneBuffer, sizeof(doneBuffer));')
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


    for  name, sig in decls["functions"].iteritems():
         proxyfuncs.append(proxyString(name, sig))
         stubconditionals.append("if (strcmp(functionNameBuffer, '%s'))\n      __%s()\n" % (name, name))
         stubfuncs.append(stubString(name, sig))



    proxysub = dict()
    proxysub["functions"] = "\n".join(proxyfuncs)
    proxysub["idlname"] = filename
    print proxytemplate.safe_substitute(proxysub)

    stubsub = dict()
    stubsub["stubs"] = "\n".join(stubfuncs)
    stubsub["idlname"] = filename
    stubsub["conditionals"] = "    else ".join(stubconditionals)
    print stubtemplate.safe_substitute(stubsub)

except Exception as e:
    print e
    print >> sys.stderr, "Usage: %s <idlfilename>" % sys.argv[0]
