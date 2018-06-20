#!/usr/bin/python
# -*- coding: utf-8 -*-
## ----------------------------------------------------------------------------
## CERTI - HLA RunTime Infrastructure
## Copyright (C) 2002-2018  ISAE-SUPAERO & ONERA
##
## This program is free software ; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public License
## as published by the Free Software Foundation ; either version 2 of
## the License, or (at your option) Any later version.
##
## This program is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY ; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this program ; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
## USA
##
## $Id: GenMsgJava.py,v 1.6 2011/07/28 14:04:21 erk Exp $
## ----------------------------------------------------------------------------

"""
The CERTI Message Generator.
Java Backend Generator
"""

import logging
import GenMsgBase
import GenMsgAST
import sys
import os


class JavaGenerator(GenMsgBase.CodeGenerator):

    """
    This is a Java generator for C{MessageAST}.
    """

    def generatorName(cls):
        return 'Java'

    generatorName = classmethod(generatorName)

    def __init__(self, MessageAST):
        super(JavaGenerator, self).__init__(MessageAST, '//')

        # Message builtin type to Java type
        # Note that java integer type are ALL signed:
        # http://java.sun.com/docs/books/tutorial/java/nutsandbolts/datatypes.html

        self.builtinTypeMap = {  # 'FederateHandleSet': 'FederateHandleSet',
            'onoff': 'boolean',
            'bool': 'boolean',
            'string': 'String',
            'byte': 'byte',
            'int8': 'byte',
            'uint8': 'byte',
            'int16': 'short',
            'uint16': 'short',
            'int32': 'int',
            'uint32': 'int',
            'int64': 'long',
            'uint64': 'long',
            'float': 'float',
            'double': 'double',
            'Tag': 'byte[]',
            'Extents': 'List<CertiExtent>',
            'SuppliedParameters': 'SuppliedParameters',
            'Regions': 'List<Long>',
            }
        self.serializeTypeMap = {
            'onoff': 'messageBuffer.write(%s);',
            'bool': 'messageBuffer.write(%s);',
            'string': 'messageBuffer.write(%s);',
            'byte': 'messageBuffer.write(%s);',
            'int8': 'messageBuffer.write(%s);',
            'uint8': 'messageBuffer.write(%s);',
            'int16': ' messageBuffer.write(%s);',
            'uint16': 'messageBuffer.write(%s);',
            'int32': 'messageBuffer.write(%s);',
            'uint32': 'messageBuffer.write(%s);',
            'int64': 'messageBuffer.write(%s);',
            'uint64': 'messageBuffer.write(%s);',
            'float': 'messageBuffer.write(%s);',
            'double': 'messageBuffer.write(%s);',
            'Tag': 'messageBuffer.writeBytesWithSize(%s);',
            'AttributeHandleSet': 'messageBuffer.write(%s);',
            'SuppliedAttributes': 'messageBuffer.write(%s);',
            'SuppliedParameters': 'messageBuffer.write(%s);',
            'FederateHandleSet': 'messageBuffer.write(%s);',
            'LogicalTimeInterval': 'messageBuffer.write(%s);',
            'Extents': 'messageBuffer.write(%s);',
            'Regions': 'messageBuffer.writeRegions(%s);',
            }
        self.deserializeTypeMap = {
            'onoff': '%s = messageBuffer.readBoolean();',
            'bool': '%s = messageBuffer.readBoolean();',
            'string': '%s = messageBuffer.readString();',
            'byte': '%s = messageBuffer.readByte();',
            'int8': '%s = messageBuffer.read_int8();',
            'uint8': '%s = messageBuffer.read_uint8();',
            'int16': '%s = messageBuffer.readShort();',
            'uint16': '%s = messageBuffer.readShort();',
            'int32': '%s = messageBuffer.readInt();',
            'uint32': '%s = messageBuffer.readInt();',
            'int64': '%s = messageBuffer.readLong();',
            'uint64': '%s = messageBuffer.readLong();',
            'float': '%s = messageBuffer.readFloat();',
            'double': '%s = messageBuffer.readDouble();',
            'Tag': '%s = messageBuffer.readBytesWithSize();',
            'AttributeHandleSet': '%s = messageBuffer.readAttributeHandleSet();',
            'SuppliedAttributes': '%s = messageBuffer.readSuppliedAttributes();',
            'SuppliedParameters': '%s = messageBuffer.readSuppliedParameters();',
            'FederateHandleSet': '%s = messageBuffer.readFederateHandleSet();',
            'LogicalTimeInterval': '%s = messageBuffer.readLogicalTimeInterval();',
            'Extents': '%s = messageBuffer.readExtents();',
            'Regions': '%s = messageBuffer.readRegions();',
            }

    def writeFieldStatement(self, stream, field):
        if field.hasDefaultValue():
            stream.write(self.getTargetTypeName(field.typeid.name) + ' '
                          + field.name + '=' + str(field.defaultValue)
                         + ';\n')
        else:
            stream.write(self.getTargetTypeName(field.typeid.name) + ' '
                          + field.name + ';\n')

    def generateIncludes(self, stream):
        stream.write("""import certi.communication.CertiException;
import certi.communication.MessageBuffer;
import certi.communication.CertiMessageType;
import certi.communication.CertiMessage;
import certi.rti.impl.CertiAttributeHandleSet;
import certi.rti.impl.CertiSuppliedAttributes;
import hla.rti.AttributeHandleSet;
import hla.rti.SuppliedAttributes;
import certi.rti.impl.CertiExtent;
import java.util.List;
import hla.rti.Region;
import hla.rti.FederateHandleSet;
import hla.rti.SuppliedParameters;
import certi.rti.impl.CertiLogicalTime;
import certi.rti.impl.CertiLogicalTimeInterval;
import hla.rti.LogicalTime;
import hla.rti.LogicalTimeInterval;

""")

    def generateHeader(self, stream, factoryOnly=False):
        self.logger.warn("There is no 'header' for Java, only body.")
        self.logger.warn("Header generation does sanity checks only")
        for native in self.AST.natives:
            if native.hasLanguage('Java'):
                # This an example may be it would be worth to check whether if the
                # referred native types are valids?
                for line in native.getLanguageLines('Java'):
                    self.logger.info('Found native **' + line.statement+ '**')

    def prepareName(self, name):
        upperAfterScore = lambda x: __import__('re').sub(r'_(.)',
                lambda y: y.group(0).upper(), x)
        if (name.find('_')>0):
            return upperAfterScore(name.lower()).replace('_', '')[1:]
        else:
            return name

    def generateBody(self, stream, factoryOnly=False):
        """
        Generate the body.
        """
        # Java generator generates multiple files so close
        # and delete the previously opened javaFile and create
        # a directory with the same name instead
        shouldCreateDir=False
        if (isinstance(stream,file)):
            mydirname = stream.name
            if os.path.exists(mydirname):
                os.remove(mydirname)
                shouldCreateDir=True
        elif (isinstance(stream,"".__class__)):
            mydirname = stream
            if not os.path.exists(mydirname):
                shouldCreateDir = True
        else:
            self.logger.error("Unknown type of stream <%>" % type(stream))
        
        if shouldCreateDir:
            os.makedirs(mydirname)
            self.logger.info("Generates java javaFile in %s directory" % mydirname)
        for msg in self.AST.messages:
            if os.path.exists(mydirname):
                javaFile = open(mydirname +os.sep+ self.prepareName(msg.name)
                            + '.java', 'w')
            else:
                javaFile = sys.stdout
                javaFile.write("// ----------    %s ----------\n" % (self.prepareName(msg.name)+'.java'))

            self.addGeneratedByLine(javaFile)
            javaFile.write("""// ----------------------------------------------------------------------------
// CERTI - HLA Run Time Infrastructure
// Copyright (C) 2010 Andrej Pancik
//
// This program is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// ----------------------------------------------------------------------------
""")

            if self.AST.hasPackage():
                javaFile.write('package ' + self.AST.package.name
                           + ''';

''')
            self.generateIncludes(javaFile)
            javaFile.write(self.getIndent() + 'public class '
                       + self.prepareName(msg.name)
                       + ' extends CertiMessage {\n')
            self.indent()

            if len(msg.fields) > 0:
                for field in msg.fields:
                    javaFile.write(self.getIndent() + 'private ')
                    # Combined field and "plain" field must not be treated the same way
                    if not isinstance(field,
                            GenMsgAST.MessageType.CombinedField):
                        self.writeFieldStatement(javaFile, field)
                    else:

                        # combined field may be written in a specific way
                        # or the field contained in the combined field may
                        # be written as usual and Getter/Setter for the combined may be
                        # generated

                        for cfield in field.fields:
                            self.writeFieldStatement(javaFile, cfield)

            javaFile.write('\n')

            # constructor

            javaFile.write(self.getIndent() + 'public '
                       + self.prepareName(msg.name) + '() {\n')
            self.indent()
            javaFile.write(self.getIndent() + 'super(CertiMessageType.'
                       + msg.name.upper()[2:] + ');\n')
            self.unIndent()
            javaFile.write(self.getIndent() + '''}

''')

            javaFile.write(self.getIndent() + 'public '
                       + self.prepareName(msg.name)
                       + '(CertiLogicalTime federationTime) {\n')
            self.indent()
            javaFile.write(self.getIndent() + 'super(CertiMessageType.'
                       + msg.name.upper()[2:] + ', federationTime);\n')
            self.unIndent()
            javaFile.write(self.getIndent() + '''}

''')

            # javaFile.write(self.getIndent() + "public " + self.prepareName(msg.name) + "(double federationTime, attributes) {\n")
            # TODO attributes in constructor
            # self.indent()
            # javaFile.write(self.getIndent() + "super(CertiMessageType." + msg.name.upper()[2:] + ", federationTime);\n")

            # TODO initialize local attributes
            # self.unIndent()
            # javaFile.write(self.getIndent() + "}\n\n");

            if len(msg.fields) > 0:
                javaFile.write(self.getIndent() + '@Override\n')
                javaFile.write(self.getIndent()
                           + 'public void writeMessage(MessageBuffer messageBuffer) {\n'
                           )
                self.indent()
                javaFile.write(self.getIndent()
                           + '''super.writeMessage(messageBuffer); //Header

''')

                for field in msg.fields:
                    methodName = self.getSerializeMethodName(field.typeid.name)
                    # Combined field and "plain" field must not be treated the same way
                    if not isinstance(field,
                            GenMsgAST.MessageType.CombinedField) and methodName:
                        javaFile.write(self.getIndent()
                                   + methodName
                                   % field.name + '\n')
                    elif methodName:
                        # FIXME TODO
                        for cfield in field.fields:
                            pass

                self.unIndent()
                javaFile.write(self.getIndent() + '''}

''')

                javaFile.write(self.getIndent() + '@Override\n')
                javaFile.write(self.getIndent()
                           + 'public void readMessage(MessageBuffer messageBuffer) throws CertiException {\n'
                           )
                self.indent()
                javaFile.write(self.getIndent()
                           + '''super.readMessage(messageBuffer); //Header 

''')

                for field in msg.fields:
                    methodName = self.getSerializeMethodName(field.typeid.name)
                    # Combined field and "plain" field must not be treated the same way
                    if not isinstance(field,
                            GenMsgAST.MessageType.CombinedField) and methodName:
                        javaFile.write(self.getIndent()
                                   + methodName
                                   % field.name + '\n')
                    elif methodName:
                        # FIXME TODO
                        for cfield in field.fields:
                            pass

                self.unIndent()
                javaFile.write(self.getIndent() + '''}

''')

                javaFile.write(self.getIndent() + '@Override\n')
                javaFile.write(self.getIndent()
                           + 'public String toString() {\n')
                self.indent()
                javaFile.write(self.getIndent() + 'return (super.toString()'
                           )
                for field in msg.fields:
                    javaFile.write(' + ", ' + field.name + ': " + '
                               + field.name)
                javaFile.write(');\n')
                self.unIndent()
                javaFile.write(self.getIndent() + '''}

''')

                # GETTERS

                for field in msg.fields:

                    # Combined field and "plain" field must not be treated the same way

                    if not isinstance(field,
                            GenMsgAST.MessageType.CombinedField):
                        javaFile.write(self.getIndent() + 'public '
                                   + self.getTargetTypeName(field.typeid.name)
                                   + ' get'
                                   + field.name[0].capitalize()
                                   + field.name[1:] + '() {\n')
                        self.indent()
                        javaFile.write(self.getIndent() + 'return '
                                   + field.name + ';\n')
                        self.unIndent()
                        javaFile.write(self.getIndent() + '''}

''')

                # SETTERS

                for field in msg.fields:

                    # Combined field and "plain" field must not be treated the same way

                    if not isinstance(field,
                            GenMsgAST.MessageType.CombinedField):
                        javaFile.write(self.getIndent() + 'public void set'
                                   + field.name[0].capitalize()
                                   + field.name[1:] + '('
                                   + self.getTargetTypeName(field.typeid.name)
                                   + ' new'
                                   + field.name[0].capitalize()
                                   + field.name[1:] + ') {\n')
                        self.indent()
                        javaFile.write(self.getIndent() + 'this.'
                                   + field.name + ' = new'
                                   + field.name[0].capitalize()
                                   + field.name[1:] + ';\n')
                        self.unIndent()
                        javaFile.write(self.getIndent() + '''}

''')

            javaFile.write('''}

''')
            self.unIndent()


