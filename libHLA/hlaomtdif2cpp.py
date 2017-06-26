#!/usr/bin/env python

## ----------------------------------------------------------------------------
## hlaomtdif2cpp.py - HLA OMT DIF document to C++ code convertor
## Copyright (C) 2008  Petr Gotthard <petr.gotthard@centrum.cz>
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License version 2.1, as published by the Free Software Foundation.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## $Id: hlaomtdif2cpp.py,v 1.2 2008/08/02 11:44:12 gotthardp Exp $
## ----------------------------------------------------------------------------

import getopt, sys
import re
import xml.sax.handler

# name of the input file
input = None

def usage():
  print "Usage: " + sys.argv[0] + " -i DIF"
  print "Converts HLA OMT DIF document to C++ code."

class CommentFormatter:
  def __init__(self, indent=0, width=80):
    self.lineCount = 0
    self.indent = indent
    self.width = width - indent

  def addComment(self, text):
    stack = [word for word in text.replace("\n", " ").split(" ") if word]
    while stack:
      line = ""
      while stack:
        if len(line) + len(" " + stack[0]) + 3 > self.width: break
        if line: line += " "
        line += stack.pop(0)
      if self.lineCount == 0:
        print " "*self.indent + "/* " + line
      else:
        print " "*self.indent + " * " + line
      self.lineCount += 1

  def addEmptyLine(self):
    print " "*self.indent + " * "

  def finish(self):
    print " "*self.indent + " */"
    self.lineCount = 0

def printComment(text, indent=0, width=80):
  if len(text) == 0 or text == "-NULL-": return

  text = text.replace("\n", " ")
  width = width - indent
  # Single-line comment
  if len(text) + 3 < width:
    print " "*indent + "// " + text
    return
  # Multi-line comment
  formatter = CommentFormatter(indent, width)
  formatter.addComment(text)
  formatter.finish()

def identifier(text):
  return re.sub('[-]', '_', text)

# custom exception to stop the SAX parser
class SAXDone(Exception):
  pass

KnownBasicTypes = (
  "HLAinteger16BE", "HLAinteger32BE", "HLAinteger64BE",
  "HLAfloat32BE", "HLAfloat64BE", "HLAoctetPairBE",
  "HLAinteger16LE", "HLAinteger32LE", "HLAinteger64LE",
  "HLAfloat32LE", "HLAfloat64LE", "HLAoctetPairLE",
  "HLAoctet",
  "Unsignedinteger16BE", "Unsignedinteger32BE", "Unsignedinteger64BE")

KnownArrayEncodings = (
  "HLAfixedArray", "HLAvariableArray")

class DependencyChecker(xml.sax.handler.ContentHandler):
  def __init__(self, deps):
    pass

  def warning(self, string):
    sys.stderr.write("Warning: [line " + str(parser.getLineNumber()) + "] " + string + "\n")

  def startElement(self, name, attributes):
    if name == "basicData":
      self.typeName = attributes["name"]
      if self.typeName in deps:
        return self.warning("Duplicite type " + self.typeName)
      if self.typeName not in KnownBasicTypes:
        return self.warning("Unknown basic type " + self.typeName)
      deps[self.typeName] = set()

    elif name == "simpleData":
      self.typeName = attributes["name"]
      if self.typeName in deps:
        return self.warning("Duplicite type " + self.typeName)
      deps[self.typeName] = set([attributes["representation"]])

    elif name == "enumeratedData":
      self.typeName = attributes["name"]
      if self.typeName in deps:
        return self.warning("Duplicite type " + self.typeName)
      deps[self.typeName] = set([attributes["representation"]])

    elif name == "arrayData":
      self.typeName = attributes["name"]
      if self.typeName in deps:
        return self.warning("Duplicite type " + self.typeName)
      if attributes["encoding"] not in KnownArrayEncodings:
        return self.warning(self.typeName + " uses unknown arrayData encoding " + attributes["encoding"])
      deps[attributes["name"]] = set([attributes["dataType"]])

    elif name == "fixedRecordData":
      self.typeName = attributes["name"]
      if self.typeName in deps:
        return self.warning("Duplicite type " + self.typeName)
      deps[self.typeName] = set()

    elif name == "field":
      deps[self.typeName].add(attributes["dataType"])

    elif name == "variantRecordData":
      self.typeName = attributes["name"]
      if self.typeName in deps:
        return self.warning("Duplicite type " + self.typeName)
      deps[self.typeName] = set([attributes["dataType"]])

    elif name == "alternative":
      deps[self.typeName].add(attributes["dataType"])

class HeaderGenerator(xml.sax.handler.ContentHandler):
  def startElement(self, name, attributes):
    if name == "objectModel":
      print "#line", parser.getLineNumber(), "\"" + input + "\""
      formatter = CommentFormatter()
      formatter.addComment("Name: " + attributes["name"])
      formatter.addComment("Version: " + attributes["version"])
      formatter.addComment("Type: " + attributes["type"])
      formatter.addEmptyLine()
      formatter.addComment("Application domain:")
      formatter.addComment(attributes["appDomain"])
      formatter.addEmptyLine()
      formatter.addComment("Purpose:")
      formatter.addComment(attributes["purpose"])
      formatter.addEmptyLine()
      formatter.addComment("Sponsor:")
      formatter.addComment(attributes["sponsor"])
      formatter.addEmptyLine()
      formatter.addComment("Point of contact:")
      formatter.addComment("Name: " + attributes["pocName"])
      formatter.addComment("Organization: " + attributes["pocOrg"])
      formatter.addComment("Phone: " + attributes["pocPhone"])
      formatter.addComment("E-mail: " + attributes["pocEmail"])
      formatter.finish();
      print
      raise SAXDone

class TypeGenerator(xml.sax.handler.ContentHandler):
  def __init__(self, deps, done):
    self.inEnum = False
    self.inRecord = False

  def warning(self, string):
    sys.stderr.write("Warning: [line " + str(parser.getLineNumber()) + "] " + string + "\n")

  def startElement(self, name, attributes):
    if name == "basicData":
      self.typeName = attributes["name"]
      if self.typeName in done or self.typeName not in deps or len(deps[self.typeName]-done) > 0:
        return

      done.add(self.typeName)

    elif name == "simpleData":
      self.typeName = attributes["name"]
      if self.typeName in done or self.typeName not in deps or len(deps[self.typeName]-done) > 0:
        return

      print "#line", parser.getLineNumber(), "\"" + input + "\""
      self.buffer = ""
      printComment(attributes["semantics"])
      print "typedef " + attributes["representation"] + " " + attributes["name"] + ";\n"

      done.add(self.typeName)

    elif name == "enumeratedData":
      self.typeName = attributes["name"]
      if self.typeName in done or self.typeName not in deps or len(deps[self.typeName]-done) > 0:
        return

      self.inEnum = True
      print "#line", parser.getLineNumber(), "\"" + input + "\""
      self.generatedEnums = set()
      self.enumRepresentation = attributes["representation"]
      printComment(attributes["semantics"])
      # The enumerations must be isolated from other enumerations in an individual namespace
      print "namespace __" + attributes["name"] + " {"
      print "enum __enum {",

    elif name == "enumerator":
      if not self.inEnum:
        return
      if attributes["name"] in self.generatedEnums:
        self.warning("Duplicate enumerator " + attributes["name"] + " in " + self.typeName)
        return

      if len(self.generatedEnums) > 0:
        print ",",
      sys.stdout.write("\n")
      self.generatedEnums.add(attributes["name"])
#     print "#line", parser.getLineNumber(), "\"" + input + "\""
      sys.stdout.write("  " + identifier(attributes["name"]) + " = " + attributes["values"])

    elif name == "arrayData":
      self.typeName = attributes["name"]
      if self.typeName in done or self.typeName not in deps or len(deps[self.typeName]-done) > 0:
        return

      print "#line", parser.getLineNumber(), "\"" + input + "\""
      printComment(attributes["semantics"])
      if attributes["encoding"] == "HLAfixedArray":
        print "typedef HLAfixedArray<" + attributes["dataType"] + ", " + attributes["cardinality"] + "> " + identifier(attributes["name"]) + ";\n"

      elif attributes["encoding"] == "HLAvariableArray":
        print "typedef HLAvariableArray<" + attributes["dataType"] + "> " + identifier(attributes["name"]) + ";\n"

      done.add(self.typeName)

    elif name == "fixedRecordData":
      self.typeName = attributes["name"]
      if self.typeName in done or self.typeName not in deps or len(deps[self.typeName]-done) > 0:
        return

      self.inRecord = True
      print "#line", parser.getLineNumber(), "\"" + input + "\""
      printComment(attributes["semantics"])
      # The enumerations must be isolated from other enumerations in an individual namespace
      print "namespace __" + attributes["name"] + " {"
      print "enum __enum {"
      self.recordFields = []

    elif name == "field":
      if not self.inRecord:
        return

      if len(self.recordFields) > 0:
        sys.stdout.write(",\n")
#     print "#line", parser.getLineNumber(), "\"" + input + "\""
      printComment(attributes["semantics"], 2)
      sys.stdout.write("  " + identifier(attributes["name"]))
      if len(self.recordFields) == 0:
        sys.stdout.write(" = 0")
      tupple = (attributes["dataType"], attributes["name"])
      self.recordFields.append(tupple)

    elif name == "variantRecordData":
      self.typeName = attributes["name"]
      if self.typeName in done or self.typeName not in deps or len(deps[self.typeName]-done) > 0:
        return

      self.inRecord = True
      print "#line", parser.getLineNumber(), "\"" + input + "\""
      printComment(attributes["semantics"])
      # The enumerations must be isolated from other enumerations in an individual namespace
      print "namespace __" + attributes["name"] + " {"
      print "enum __enum {"
      sys.stdout.write("  " + attributes["discriminant"] + " = 0")
      self.variantName = attributes["discriminant"]
      self.variantType = attributes["dataType"]
      self.recordFields = []

    elif name == "alternative":
      if not self.inRecord:
        return

      sys.stdout.write(",\n")
#     print "#line", parser.getLineNumber(), "\"" + input + "\""
      printComment(attributes["semantics"], 2)
      sys.stdout.write("  " + identifier(attributes["name"]))
      tupple = (attributes["dataType"], attributes["enumerator"], attributes["name"])
      self.recordFields.append(tupple)

  def endElement(self, name):
    if name == "enumeratedData":
      if not self.inEnum:
        return

      print "\n};"
      print "}"
      print "typedef HLAenumeratedType<__" + self.typeName + "::__enum, " + self.enumRepresentation + "> " + self.typeName + ";\n"

      done.add(self.typeName)
      self.inEnum = False

    elif name == "fixedRecordData":
      if not self.inRecord:
        return

      print "\n};"
      print "}"
      print "typedef HLAfixedRecord<"
      for index, field in enumerate(self.recordFields):
        if index > 0:
          sys.stdout.write(",\n")
        sys.stdout.write("  HLAfixedField<__" + self.typeName + "::" + identifier(field[1]) + ", " + field[0])
      sys.stdout.write("\n  ")
      for field in self.recordFields:
        sys.stdout.write("> ")
      print "> " + self.typeName + ";\n"

      done.add(self.typeName)
      self.inRecord = False

    elif name == "variantRecordData":
      if not self.inRecord:
        return

      print "\n};"
      print "}"
      print "typedef HLAvariantRecord<"
      sys.stdout.write("  __" + self.typeName + "::" + self.variantName + ", " + self.variantType)
      for field in self.recordFields:
        sys.stdout.write(",\n  HLAvariantField<HLAsetValue<__" + self.variantType + "::" + field[1] + ">")
        sys.stdout.write(",\n    __" + self.typeName + "::" + identifier(field[2]) + ", " + field[0])
      sys.stdout.write("\n  ")
      for field in self.recordFields:
        sys.stdout.write("> ")
      print "> " + self.typeName + ";\n"

      done.add(self.typeName)
      self.inRecord = False

try:
  opts, args = getopt.getopt(sys.argv[1:], "i:h", ["help", "output="])
except getopt.GetoptError, err:
  print sys.argv[0] + ": " + str(err) # will print something like "option -a not recognized"
  print "Try `" + sys.argv[0] + " --help' for more information."
  sys.exit(2)
for o, a in opts:
  if o in ("-i", "--input"):
    input = a
    modulename = "__HLAOMTDIF2CPP_" + re.sub('[-._]', '', input.upper()) + "_HH__"
  elif o in ("-h", "--help"):
    usage()
    sys.exit()
  else:
    assert False, "unhandled option"

if input == None:
  print sys.argv[0] + ": missing input file"
  print "Try `" + sys.argv[0] + " --help' for more information."
  sys.exit(2)

print "// Do not edit! This file was automatically generated by",
for arg in sys.argv:
  print arg,
print "\n"
print "#ifndef " + modulename
print "#define " + modulename
print
print "#include <HLAtypesIEEE1516.hh>\n"
print "namespace libhla {\n"

done = set()

deps = {}
parser = xml.sax.make_parser()

handler = DependencyChecker(deps)
parser.setContentHandler(handler)
parser.parse(input)

handler = HeaderGenerator()
parser.setContentHandler(handler)
try:
  parser.parse(input)
except SAXDone: pass

while True:
  initiallyDone = len(done)

  handler = TypeGenerator(deps, done)
  parser.setContentHandler(handler)
  parser.parse(input)

  if len(done) == initiallyDone:
    break

for datatype in set(deps.keys())-done:
  sys.stderr.write("Warning: " + datatype + " depends on unknown type(s)")
  for dep in deps[datatype]-done:
    sys.stderr.write(" " + dep)
  sys.stderr.write("\n")

print "} // namespace libhla"
print
print "#endif // " + modulename

# $Id: hlaomtdif2cpp.py,v 1.2 2008/08/02 11:44:12 gotthardp Exp $
