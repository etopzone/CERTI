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
## $Id: GenMsgBase.py,v 1.12 2011/07/29 09:08:22 erk Exp $
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

    def __init__(self, MessageAST, commentLineBeginWith, commentEndWith=None):
        self.AST = MessageAST
        self.commentLineBeginWith       = commentLineBeginWith
        self.commentEndWith             = commentEndWith
        if commentEndWith==None:
            self.commentInsideLineBeginWith = commentLineBeginWith
        else:
            nbCar = len(commentLineBeginWith)
            self.commentInsideLineBeginWith = nbCar*" "
        self.logger = logging.Logger(self.generatorName() + 'Generator')
        self.logger.setLevel(logging.ERROR)
        #self.logger.addHandler(logging.StreamHandler(sys.stdout))
        self.__indentString = '    '
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

    def isBuiltin(self,typename):
        return typename in self.builtinTypeMap.keys()

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
    
    def getRepresentationFor(self, name):
        for native in self.AST.natives:
            if name == native.name:
                return native.getRepresentation()
        return None

    def getSerializeMethodName(self, name):
        if name in self.serializeTypeMap.keys():
            return self.serializeTypeMap[name]
        else:
            representation = self.getRepresentationFor(name)
            if representation and not representation.hasQualifier():
                return self.getSerializeMethodName(representation.representation)
        
        self.logger.warn("No serialize method for <%s>", name)
        return None

    def getDeSerializeMethodName(self, name):
        if name in self.deserializeTypeMap.keys():
            return self.deserializeTypeMap[name]
        else:
            representation = self.getRepresentationFor(name)
            if representation and not representation.hasQualifier():
                return self.getDeSerializeMethodName(representation.representation)
        self.logger.warn("No serialize method for <%s>", name)
        return None

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
            lineNb = 0
            for line in ASTElement.comment.lines:
                # we should not indent optional comment
                # since they come at the end of a line
                if not ASTElement.comment.isAtEOL:
                    stream.write(self.getIndent())
                if lineNb==0:
                    stream.write(self.commentLineBeginWith)
                else:
                    stream.write('\n')
                    stream.write(self.commentInsideLineBeginWith)
                stream.write(str(line))
                lineNb = lineNb + 1
            if self.commentEndWith!=None:
                stream.write(self.commentEndWith+'\n')
            else:
                stream.write('\n')
        else:
            stream.write('\n')
    
    def writeCommentLines(self,stream, commentLines):
        """
        Write a comment block to the stream.
        
        Multiple lines comment is handled as well.
        """
        lineNb = 0
        if self.commentEndWith!=None:
            stream.write(self.getIndent()+self.commentLineBeginWith)
        for line in commentLines.split('\n'):
            if (lineNb>0):
                stream.write('\n')
                stream.write(self.getIndent()+self.commentInsideLineBeginWith)
            stream.write(str(line))
            lineNb = lineNb + 1
        if lineNb>1:
            stream.write('\n')
        if self.commentEndWith!=None:
            stream.write(self.getIndent()+self.commentEndWith+'\n')

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

    def addGeneratedByLine(self,stream):
        stream.write(self.commentLineBeginWith)
        stream.write(' Generated on %s by the CERTI message generator\n'
                     % datetime.datetime.now().strftime('%Y %B %a, %d at %H:%M:%S'
                     ))

    def generate(
        self,
        stream,
        what,
        factoryOnly=False):
        
        if what.lower() == 'header':
            self.generateHeader(stream, factoryOnly)
        elif what.lower() == 'body':
            self.generateBody(stream, factoryOnly)
        else:
            self.logger.error('What <%s> unknown type??' % what)

