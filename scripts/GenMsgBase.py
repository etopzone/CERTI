#!/usr/bin/python
# -*- coding: utf-8 -*-
## ----------------------------------------------------------------------------
## CERTI - HLA RunTime Infrastructure
## Copyright (C) 2002-2005  ONERA
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
## $Id: GenMsgBase.py,v 1.6 2010/06/11 12:43:12 erk Exp $
## ----------------------------------------------------------------------------

# We use logging for ... logging :-)
# see http://docs.python.org/library/logging.html

"""
The CERTI Message Generator.
Generator Base class
"""

import logging
import sys
import datetime
import GenMsgAST


class CodeGenerator(object):

    """
    This is a base class generator for C{MessageAST}.
    
    This is not a working generator it should be subclassed. 
    """

    def generatorName(cls):
        raise Exception('This class method should be overloaded by daughter class <'
                         + cls.__name__ + '>')

    generatorName = classmethod(generatorName)

    def __init__(self, MessageAST, commentLineBeginWith):
        self.AST = MessageAST
        self.commentLineBeginWith = commentLineBeginWith
        self.logger = logging.Logger(self.generatorName() + 'Generator')
        self.logger.setLevel(logging.ERROR)
        self.logger.addHandler(logging.StreamHandler(sys.stdout))
        self.__indentString = '   '
        self.__indentLevel = 0
        self.builtinTypeMap = {
            'onoff': 'onoff',
            'bool': 'bool',
            'string': 'string',
            'byte': 'byte',
            'int8': 'int8',
            'uint8': 'uint8',
            'int16': 'int16',
            'uint16': 'uint16',
            'int32': 'int32',
            'uint32': 'uint32',
            'int64': 'int64',
            'uint64': 'uint64',
            'float': 'float',
            'double': 'double',
            }

    def setIndentString(self, indentString):
        self.__indentString = indentString

    def indent(self):
        self.__indentLevel += 1

    def unIndent(self):
        if self.__indentLevel > 0:
            self.__indentLevel -= 1
        else:
            self.logger.error('Trying to unIndent lower than 0!??!')

    def getIndent(self):
        res = ''
        i = self.__indentLevel
        while i > 0:
            res = res + self.__indentString
            i -= 1
        return res

    def getTargetTypeName(self, name):
        if name in self.builtinTypeMap.keys():
            return self.builtinTypeMap[name]
        else:
            return name

    def lowerFirst(self, str):
        res = str[0].lower() + str[1:]
        return res

    def upperFirst(self, str):
        res = str[0].upper() + str[1:]
        return res

    def writeComment(self, stream, ASTElement):
        """
        Write a comment block to the stream.
        
        This function may be generic if the target
        language has whole line comment support
        with some beginning characters.
        """

        if ASTElement.hasComment():
            for line in ASTElement.comment.lines:

                # we should not indent optional comment
                # since they come at the end of a line

                if not ASTElement.comment.isAtEOL:
                    stream.write(self.getIndent())
                stream.write(self.commentLineBeginWith)
                stream.write(str(line))
                stream.write('\n')
        else:
            stream.write('\n')

    def generateHeader(self, stream, factoryOnly=False):
        """
        Generate the header.
        """

        self.logger.error('generateHeader not IMPLEMENTED')

    def generateBody(self, stream, factoryOnly=False):
        """
        Generate the body.
        """

        self.logger.error('generateBody not IMPLEMENTED')

    def generate(
        self,
        stream,
        what,
        factoryOnly=False,
        ):
        stream.write(self.commentLineBeginWith)
        stream.write(' Generated on %s by the CERTI message generator\n'

                     % datetime.datetime.now().strftime('%Y %B %a, %d at %H:%M:%S'
                     ))
        if what.lower() == 'header':
            self.generateHeader(stream, factoryOnly)
        elif what.lower() == 'body':
            self.generateBody(stream, factoryOnly)
        else:
            self.logger.error('What <%s> unknown type??' % what)


class MsgSpecGenerator(CodeGenerator):

    """
    This is a text generator for C{MessageAST}.
    
    This generator should produce almost the same output
    as the input message specification file. 
    """

    def generatorName(cls):
        return 'MsgSpec'

    generatorName = classmethod(generatorName)

    def __init__(self, MessageAST):
        super(MsgSpecGenerator, self).__init__(MessageAST, '//')
        self.logger = logging.Logger('MsgSpecGenerator')
        self.logger.setLevel(logging.ERROR)
        self.logger.addHandler(logging.StreamHandler(sys.stdout))
        self.__languageName = 'MsgSpec'

    def generate(
        self,
        stream,
        what,
        factoryOnly=False,
        ):
        """
        Redefine super.generate.
        
        what is not important in this case.
        """

        # Generate package

        if self.AST.hasPackage():
            self.writeComment(stream, self.AST.package)
            stream.write('package %s\n' % self.AST.package.name)

    # Generate version

        if self.AST.hasVersion():
            self.writeComment(stream, self.AST.version)
            stream.write('version %d.%d\n' % self.AST.version.number)

        if not factoryOnly:

            # Generate native type

            for native in self.AST.natives:
                self.writeComment(stream, native)
                stream.write('native %s {\n' % native.name)
                self.indent()
                if native.hasRepresentation():
                    stream.write(self.getIndent() + 'representation '
                                 + native.getRepresentation() + '\n')
                for (k, v) in native.languages.items():
                    for l in v:
                        stream.write(self.getIndent() + 'language ' + k
                                + '     [' + l.statement + ']\n')
                self.unIndent()
                stream.write('}\n')

            # Generate enum

            for enum in self.AST.enums:
                self.writeComment(stream, enum)
                stream.write('enum %s {\n' % enum.name)
                first = True
                self.indent()
                for enumval in enum.values:
                    if first:
                        stream.write(self.getIndent() + '%s = %d, '
                                % (enumval.name, enumval.value))
                        first = False
                    else:
                        stream.write(self.getIndent() + '%s, '
                                % enumval.name)
                    self.writeComment(stream, enumval)
                self.unIndent()
                stream.write('''}

''')

            # Generate message type

            for msg in self.AST.messages:
                self.writeComment(stream, msg)
                stream.write('message %s' % msg.name)
                if msg.hasMerge():
                    stream.write(' : merge %s {\n' % msg.merge.name)
                else:
                    stream.write(' {\n')

                for field in msg.fields:
                    if isinstance(field,
                                  GenMsgAST.MessageType.CombinedField):
                        stream.write('        combined %s {'
                                % field.typeid)
                        self.writeComment(stream, field)
                        for cfield in field.fields:
                            stream.write('            %s %s %s '
                                    % (cfield.qualifier,
                                    cfield.typeid.name, cfield.name))
                            if cfield.hasDefaultValue():
                                stream.write('[default=%s] '
                                        % cfield.defaultValue)
                            self.writeComment(stream, cfield)
                        stream.write('        }\n')
                    else:
                        stream.write('        %s %s %s '
                                % (field.qualifier, field.typeid.name,
                                field.name))
                        if field.hasDefaultValue():
                            stream.write('[default=%s] '
                                    % field.defaultValue)
                        self.writeComment(stream, field)
                stream.write('''}

''')

        # Generate Factory

        if self.AST.hasFactory():
            self.writeComment(stream, self.AST.factory)
            stream.write('factory %s {\n' % self.AST.factory.name)
            self.indent()
            stream.write(self.getIndent() + 'factoryCreator %s %s(%s)\n'
                          % self.AST.factory.creator)
            stream.write(self.getIndent()
                         + 'factoryReceiver %s %s(%s)\n'
                         % self.AST.factory.receiver)
            self.unIndent()
            stream.write('''}

''')


