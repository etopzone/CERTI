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
## $Id: GenMsgC.py,v 1.8 2011/07/29 09:08:23 erk Exp $
## ----------------------------------------------------------------------------

"""
The CERTI Message Generator.
C Backend Generator
"""

import logging
import GenMsgAST
import GenMsgBase
import sys
import os

class CGenerator(GenMsgBase.CodeGenerator):
    """
    This is a C generator for C{MessageAST}.
    
    """

    def generatorName(cls):
        return 'C'

    generatorName = classmethod(generatorName)

    def __init__(self, MessageAST):
        super(CGenerator, self).__init__(MessageAST, '/* ',' */')
        self.included = dict()
        self.typedefed = dict()
        self.builtinTypeMap = {
            'onoff': 'uint8_t',
            'bool': 'uint8_t',
            'string': 'char*',
            'byte': 'uint8_t',
            'int8': 'int8_t',
            'uint8': 'uint8_t',
            'int16': 'int16_t',
            'uint16': 'uint16_t',
            'int32': 'int32_t',
            'uint32': 'uint32_t',
            'int64': 'int64_t',
            'uint64': 'uint64_t',
            'float': 'float',
            'double': 'double',
            }
        self.serializeTypeMap = {
            'onoff': 'MB_write_bool',
            'bool': 'MB_write_bool',
            'string': 'MB_write_string',
            'byte': 'MB_write_byte',
            'int8': 'MB_write_int8',
            'uint8': 'MB_write_uint8',
            'int16': 'MB_write_int16',
            'uint16': 'MB_write_uint16',
            'int32': 'MB_write_int32',
            'uint32': 'MB_write_uint32',
            'int64': 'MB_write_int64',
            'uint64': 'MB_write_uint64',
            'float': 'MB_write_float',
            'double': 'MB_write_double',
            }
        self.deserializeTypeMap = {
            'onoff': 'MB_read_bool',
            'bool': 'MB_read_bool',
            'string': 'MB_read_string',
            'byte': 'MB_read_byte',
            'int8': 'MB_read_int8',
            'uint8': 'MB_read_uint8',
            'int16': 'MB_read_int16',
            'uint16': 'MB_read_uint16',
            'int32': 'MB_read_int32',
            'uint32': 'MB_read_uint32',
            'int64': 'MB_read_int64',
            'uint64': 'MB_read_uint64',
            'float': 'MB_read_float',
            'double': 'MB_read_double',
            }
        self.__languageName = 'C'
        self.replacePrefix = None
        self.exportPrefix = ''
        self.serializeBufferType = 'MsgBuffer_t'
        self.messageTypeGetter = 'getType()'
        self.exception = []
        self.commentInsideLineBeginWith = " * "
        if self.AST.hasPackage():
            self.nameprefix=self.AST.package.shortname+"_"
        else:
            self.nameprefix=""

    def getTargetTypeName(self, name):
        if name in self.builtinTypeMap.keys():
            return self.builtinTypeMap[name]
        else:
            return self.nameprefix+name + "_t"

    def openNamespaces(self, stream):
        if self.AST.hasPackage():
            self.writeComment(stream, self.AST.package)
            # we may have nested name space(s)
            nameSpaceList = self.AST.package.name.split('.')

    def closeNamespaces(self, stream):
        if self.AST.hasPackage():
            # we may have nested name space(s)
            nameSpaceList = self.AST.package.name.split('.')
            nameSpaceList.reverse()

    def writeOneGetterSetter(self, stream, field, msg, headerOnly=True):
        targetTypeName = self.getTargetTypeName(field.typeid.name)

        # onoff 
        if field.typeid.name == 'onoff':
            # onoff and repeated
            if field.qualifier == 'repeated':
                stream.write(self.getIndent())
                stream.write('uint32_t ' + self.nameprefix + msg.name + '_get'
                             + self.upperFirst(field.name)
                             + 'Size(%s cthis) '
                             % (self.nameprefix + msg.name))
                if not headerOnly:
                    stream.write(' {return cthis->' + field.name + 'Size;}\n')
                else:
                    stream.write(';\n')
                stream.write(self.getIndent())
                stream.write('void ' + self.nameprefix + msg.name + '_set' + self.upperFirst(field.name)
                             + 'Size(%s_t* cthis, uint32_t sz)' % (self.nameprefix + msg.name))
                if not headerOnly:
                    self.writeSetSizeFunctionBody(stream, field,
                                                  True,
                                                  True,
                                                  False)
                else:
                    stream.write(';\n')

                stream.write(self.getIndent())
                stream.write(' ' + targetTypeName + ' '
                             + self.nameprefix + msg.name +'_get' + self.upperFirst(field.name)
                             + '(' + self.nameprefix + msg.name + '* cthis) ')
                if not headerOnly:
                    stream.write(' {return cthis->' + field.name + ';}\n')
                else:
                    stream.write(';\n')

                stream.write(self.getIndent())
                stream.write('void ' + self.nameprefix + msg.name + '_' + field.name + 'On(%s_t* cthis, uint32_t rank)'
                             % (self.nameprefix + msg.name))
                if not headerOnly:
                    stream.write(' {\n' + self.getIndent() + 'assert(rank<cthis->'+ field.name +'size);\n')
                    stream.write(self.getIndent() + 'cthis->' + field.name + '[rank] = 1;}\n')
                else:
                    stream.write(';\n')

                stream.write(self.getIndent())
                stream.write('void ' + self.nameprefix + msg.name + '_' + field.name + 'Off(%s_t* cthis, uint32_t rank)'
                             % (self.nameprefix + msg.name))
                if not headerOnly:
                    stream.write(' {\n' + self.getIndent() + 'assert(rank<cthis->'+ field.name +'size);\n')
                    stream.write(self.getIndent() + 'cthis->' + field.name + '[rank] = 0;}\n')
                else:
                    stream.write(";\n")

                stream.write(self.getIndent())
                stream.write(targetTypeName + ' is'
                             + self.upperFirst(field.name)
                             + 'On(%s_t* cthis, uint32_t rank)')
                if not headerOnly:
                    stream.write(' {return cthis->' + field.name + '[rank];}\n')
                else:
                    stream.write(';\n')
            # onoff not repeated
            else:
                stream.write(self.getIndent())
                stream.write('void ' + self.nameprefix + msg.name + '_' + field.name + 'On(%s_t* cthis)'
                             % (self.nameprefix + msg.name))
                if not headerOnly:
                    stream.write(' {cthis->' + field.name + ' = 1;}\n')
                else:
                    stream.write(';\n')

                stream.write(self.getIndent())
                stream.write('void ' + self.nameprefix + msg.name + '_' + field.name + 'Off(%s_t* cthis)'
                             % (self.nameprefix + msg.name))
                if not headerOnly:
                    stream.write(' {cthis->' +field.name + ' = 0;}\n')
                else:
                    stream.write(';\n')

                stream.write(self.getIndent())
                stream.write(targetTypeName + ' is'
                             + self.upperFirst(field.name)
                             + 'On(%s_t* cthis) '
                                % (self.nameprefix + msg.name))
                if not headerOnly:
                    stream.write(' {return cthis->' + field.name + ';}\n')
                else:
                    stream.write(';\n')
        # not an onoff
        else:
            # not an onoff and repeated
            if field.qualifier == 'repeated':
                stream.write(self.getIndent())
                stream.write('uint32_t ' + self.nameprefix + msg.name + '_get'
                             + self.upperFirst(field.name)
                             + 'Size(%s_t* cthis)'
                             % (self.nameprefix + msg.name))
                if not headerOnly:
                    stream.write('{ return cthis->' + field.name + 'Size;}\n')
                else:
                    stream.write(';\n')

                stream.write(self.getIndent())
                stream.write('void ' + self.nameprefix + msg.name + '_set' + self.upperFirst(field.name)
                             + 'Size(%s_t* cthis, uint32_t sz)'
                             % (self.nameprefix + msg.name))
                if not headerOnly:
                    self.writeSetSizeFunctionBody(stream, field,
                                                  False,
                                                  True,
                                                  self.isBuiltin(field.typeid.name))
                else:
                    stream.write(';\n')

                stream.write(self.getIndent())
                stream.write(targetTypeName + ' ' + self.nameprefix + msg.name + '_get'
                             + self.upperFirst(field.name)
                             + '(%s_t* cthis, uint32_t rank)'
                             % (self.nameprefix + msg.name))
                if not headerOnly:
                    stream.write(' {return cthis->' + field.name + '[rank];}\n')
                else:
                    stream.write(';\n')

                stream.write(self.getIndent())
                stream.write('void ' + self.nameprefix + msg.name + '_set' + self.upperFirst(field.name)
                             + '(%s_t* cthis, '
                             % (self.nameprefix + msg.name))
                stream.write(targetTypeName + ' new'
                             + self.upperFirst(field.name)
                             + ', uint32_t rank)')
                if not headerOnly:
                    stream.write(' {cthis->' + field.name + '[rank]=new'
                                 + self.upperFirst(field.name) + ';}\n')
                else:
                    stream.write(';\n')
            # not an onoff and not repeated
            else:
                stream.write(self.getIndent())
                stream.write(targetTypeName + ' ' + self.nameprefix + msg.name + '_get'
                             + self.upperFirst(field.name) + '(%s_t* cthis) '
                             % (self.nameprefix + msg.name))
                if not headerOnly:
                    stream.write(' {return cthis->'  + field.name + ';}\n')
                else:
                    stream.write(';\n')

                stream.write(self.getIndent())
                stream.write('void ' + self.nameprefix + msg.name + '_set'+ self.upperFirst(field.name)
                             + '(%s_t* cthis,  '
                            % (self.nameprefix + msg.name))
                stream.write(targetTypeName + ' new'
                             + self.upperFirst(field.name) + ')')
                if not headerOnly:
                    stream.write(' {')
                    if field.qualifier == 'optional':
                        stream.write('\n')
                        self.indent()
                        stream.write(self.getIndent() + 'cthis->_has%s=1;\n'
                                     % self.upperFirst(field.name))
                        stream.write(self.getIndent() + 'cthis->' + field.name + '=new'
                                     + self.upperFirst(field.name) + ';\n')
                        self.unIndent()
                        stream.write(self.getIndent())
                    else:
                        stream.write('cthis->' + field.name + '=new'
                                     + self.upperFirst(field.name) + ';')
                    stream.write('}\n')
                else:
                    stream.write(';\n')

                if field.qualifier == 'optional':
                    stream.write(self.getIndent())
                    tmp = self.upperFirst(field.name)
                    stream.write('uint8_t has%s(%s_t* cthis)'
                                 % (tmp, self.nameprefix + msg.name))
                    if not headerOnly:
                        stream.write(' {return cthis->_has%s;}\n' % tmp)
                    else:
                        stream.write(';\n')

    def writeSetSizeFunctionBody(self, stream, field, isOnoff, isRepeated, isBuiltin):
        stream.write(' {\n')
        self.indent()
        stream.write(self.getIndent() + self.getTargetTypeName(field.typeid.name) + '* temp;\n')
        if isRepeated:
            if isBuiltin:
                stream.write(self.getIndent() + 'temp = calloc(sz,sizeof(' 
                             + self.getTargetTypeName(field.typeid.name) + '));\n')
            else:
                stream.write(self.getIndent() + 'temp = %s_create(sz);\n' % (self.getTargetTypeName(field.typeid.name)))
            stream.write(self.getIndent()+'if (cthis->' + field.name + 'Size>0) {\n')
            self.indent()
            stream.write(self.getIndent() + 'memcpy(temp,cthis->%s, (cthis->%sSize)*sizeof(%s));\n'
                     % (field.name, field.name,self.getTargetTypeName(field.typeid.name)))
            stream.write(self.getIndent() + 'free(cthis->' + field.name + ');\n')
            stream.write(self.getIndent() + 'cthis->' + field.name + ' = temp;\n')
            stream.write(self.getIndent() + 'cthis->' + field.name + 'Size = num;\n')
            stream.write('}\n')
            self.unIndent()
        else:
            self.writeCommentLines("FIXME this case is not handled TODO TODO")
        stream.write('}\n')
        self.unIndent()

    def writeDeclarationFieldStatement(self, stream, field):
        stream.write(self.getIndent())
        if field.qualifier == 'repeated':
            stream.write('%s* %s;\n'
                         % (self.getTargetTypeName(field.typeid.name),
                         field.name))
            stream.write(self.getIndent() + 'uint32_t ' + field.name + 'Size;')
        else:
            stream.write('%s %s;'
                         % (self.getTargetTypeName(field.typeid.name),
                         field.name))
        self.writeComment(stream, field)
        # optional field generate another boolean field
        # used to detect whether if the optional field has
        # been given or not.
        if field.qualifier == 'optional':
            stream.write(self.getIndent() + 'uint8_t _has%s;\n'
                         % self.upperFirst(field.name))

    def generateEnum(self, stream, enum):
        self.writeComment(stream, enum)
        stream.write(self.getIndent())
        stream.write('typedef enum %s {\n' % (self.nameprefix+enum.name))
        self.indent()
        first = True
        lastname = enum.values[len(enum.values) - 1].name
        for enumval in enum.values:
            if first:
                stream.write(self.getIndent())
                stream.write('%s = %d, ' % (enumval.name,
                             enumval.value))
                first = False
                self.writeComment(stream, enumval)
            else:
                stream.write(self.getIndent())
                if enumval.name == lastname:
                    stream.write('%s ' % enumval.name)
                    self.writeComment(stream, enumval)
                    stream.write('\n')
                else:
                    stream.write('%s, ' % enumval.name)
                    self.writeComment(stream, enumval)
        self.unIndent()
        stream.write(self.getIndent())
        stream.write('} %s_t; ' % (self.nameprefix+enum.name))
        self.writeCommentLines(stream,"end of enum %s" % enum.name)

    def generateHeader(self, stream, factoryOnly=False):
        # write the usual header protecting MACRO
        supposedHeaderName = stream.name
        if supposedHeaderName != '<stdout>':
            supposedHeaderName = os.path.basename(supposedHeaderName)
            supposedHeaderName = os.path.splitext(supposedHeaderName)[0]
            headerProtectMacroName = supposedHeaderName                
        else:
            (headerProtectMacroName, ext) = \
                os.path.splitext(self.AST.name)

        headerProtectMacroName = '%s_H' \
            % headerProtectMacroName.upper()
        stream.write('#ifndef %s\n' % headerProtectMacroName)
        stream.write('#define %s\n' % headerProtectMacroName)

        # add necessary standard and global includes
        self.writeCommentLines(stream,"****-**** Global System includes ****-****")
        stream.write('#include <stdio.h>\n')
        self.included['#include <stdio.h>'] = 1
        stream.write('#include <stdlib.h>\n')
        self.included['#include <stdlib.h>'] = 1
        stream.write('#include <stdint.h>\n')
        self.included['#include <stdint.h>'] = 1
        stream.write('#include <assert.h>\n')
        self.included['#include <assert.h>'] = 1
        stream.write('#include <string.h>\n')
        self.included['#include <string.h>'] = 1

        # add include coming from native type specification
        self.writeCommentLines(stream,
                               "****-**** Includes coming from native types ****-****")
        for native in self.AST.natives:
            if native.hasLanguage('C'):
                for line in native.getLanguageLines('C'):
                    # we are only interested in native "include" statement
                    stmt = line.statement
                    if stmt.find('#include') >= 0 and not stmt in self.included.keys():
                        self.included[stmt] = 1
                    self.writeComment(stream, native)
                    stream.write(stmt + '\n')

        # Generate name space for specified package package
        # we may have nested name space
        self.openNamespaces(stream)

        if not factoryOnly:
            # Native type should be defined in included header
            self.writeCommentLines(stream,""" Native types has been defined:
- by included headers (see above)
- with typedef (see below [if any])""")
            for native in self.AST.natives:
                if native.hasLanguage('C'):
                    for line in native.getLanguageLines('C'):
                        stmt = line.statement
                        # we are only interested in native statement
                        # which are not #include
                        if stmt.find('typedef') >= 0 and not stmt \
                            in self.typedefed.keys():
                            self.writeComment(stream, native)
                            stream.write(self.getIndent() + stmt + '\n')
                            self.typedefed[stmt] = 1

            # Put enum in a namespace in order to avoid conflict

            # Generate version
            if self.AST.hasVersion():
                (major, minor) = self.AST.version.number                
                stream.write(self.getIndent())
                stream.write('static const uint32_t %sversionMajor = %d;\n'
                              % (self.nameprefix, major))
                stream.write(self.getIndent())
                stream.write('static const uint32_t %sversionMinor = %d;\n'
                              % (self.nameprefix, minor))

            # Generate enum
            lastname = ''
            for enum in self.AST.enums:
                enum.addNamePrefix(self.nameprefix+self.AST.shortenName(enum.name)+'_')
                self.generateEnum(stream, enum)
            stream.write('\n')

            # close enum namespace

            # Generate message type
            for msg in self.AST.messages:
                self.writeCommentLines(stream,"---------------------- BEGIN <%s> ------------------------" % msg.name)
                self.writeComment(stream, msg)
                if msg.hasEnum():
                    msg.enum.addNamePrefix(self.nameprefix+self.AST.shortenName(msg.enum.name)+'_')
                    self.generateEnum(stream, msg.enum)
                    stream.write('\n')
                stream.write(self.getIndent())
                stream.write('typedef struct %s %s' % (self.exportPrefix,
                             self.nameprefix+msg.name))
                stream.write(' {\n')
                virtual = ''
                self.indent()

                if msg.hasMerge():
                    stream.write(self.getIndent()
                                 + '%s* super;\n'
                                 % self.getTargetTypeName(msg.merge.name))

                # write fields
                for field in msg.fields:
                    if isinstance(field,
                                  GenMsgAST.MessageType.CombinedField):
                        for cfield in field.fields:
                            self.writeDeclarationFieldStatement(stream,
                                    cfield)
                    else:
                        self.writeDeclarationFieldStatement(stream,
                                field)

                self.unIndent()
                stream.write(self.getIndent() + '} %s_t; \n\n' % (self.nameprefix+msg.name))

                # now write constructor/destructor
                self.writeCommentLines(stream,"""Creator function
@param[in] nb>0 is the number of element(s) to create (array)""")
                stream.write(self.getIndent() + '%s_t* %s_create('  % (self.nameprefix+msg.name,self.nameprefix+msg.name)
                             +self.getTargetTypeName("uint32")+' nb);\n')
                self.writeCommentLines(stream,"Destructor function")
                stream.write(self.getIndent() + 'void %s_destroy(%s_t** cthis);\n\n' % (self.nameprefix+msg.name,self.nameprefix+msg.name))

                # write virtual serialize and deserialize
                # if we have some specific field
                if len(msg.fields) > 0:
                    # serialize/deserialize
                    self.writeCommentLines(stream,"Serialize function")
                    stream.write(self.getIndent() + virtual
                                 + 'void %s_serialize(%s_t* cthis, %s* msgBuffer);\n'
                                 % (self.nameprefix+msg.name, self.nameprefix+msg.name, self.serializeBufferType))
                    self.writeCommentLines(stream,"DeSerialize function")
                    stream.write(self.getIndent() + virtual
                                 + 'void %s_deserialize(%s_t* cthis, %s* msgBuffer);\n'
                                 % (self.nameprefix+msg.name, self.nameprefix+msg.name, self.serializeBufferType))

                    # specific getter/setter
                    self.writeCommentLines(stream,"specific Getter(s)/Setter(s)")
                    for field in msg.fields:
                        if isinstance(field,
                                GenMsgAST.MessageType.CombinedField):
                            for cfield in field.fields:
                                self.writeOneGetterSetter(stream,
                                        cfield,msg)
                        else:
                            self.writeOneGetterSetter(stream,field,msg)

                    # the show method
                    self.writeCommentLines(stream,"the show method")
                    stream.write(self.getIndent() + virtual
                                 + "FILE* %s_show(FILE* out);\n" % msg.name
                                 )
                    
                    self.writeCommentLines(stream,"---------------------- END <%s> ------------------------" % msg.name)
                    stream.write("\n")
        # Generate Factory (if any)
        # @todo
        if self.AST.hasFactory():
            self.writeComment(stream, self.AST.factory)
            stream.write(self.getIndent() + '/*\n')

            if self.AST.factory.hasFactoryCreator:
                ccreator = (self.AST.factory.creator[0],
                            self.AST.factory.name+"_"+self.AST.factory.creator[1],
                            self.AST.factory.creator[2])
                stream.write(self.getIndent()
                             + '%s* %s(%s type)'
                             % ccreator)
                stream.write('; \n')

            if self.AST.factory.hasFactoryReceiver():
                creceiver = (self.AST.factory.receiver[0],
                            self.AST.factory.name+"_"+self.AST.factory.receiver[1],
                            self.AST.factory.receiver[2])
                stream.write(self.getIndent()
                             + '%s* %s(%s stream)'
                             % creceiver)
                stream.write('; \n')
        # may close any open namespaces
        self.closeNamespaces(stream)

        # close usual HEADER protecting MACRO
        stream.write(self.commentLineBeginWith + 
                      headerProtectMacroName + self.commentEndWith
                     +"\n")
        stream.write('#endif\n')

    def writeInitFieldStatement(self, stream, field):
        if field.qualifier == 'optional':
            stream.write(self.getIndent()+'newObj->_has' 
                         + self.upperFirst(field.name)
                         + '=0;\n')
        # Sequence are initialized to unallocated pointer
        # they should be allocated with SetSize function
        if field.qualifier== 'repeated':
            stream.write(self.getIndent()+field.name+"Size = 0;\n")
            stream.write(self.getIndent()+field.name+" = NULL;\n")
        # non repeated builtin does not require allocation.
        elif self.isBuiltin(field.typeid.name):
            if field.hasDefaultValue():
                stream.write(self.getIndent())
                if field.typeid.name=='string':
                    stream.write('newObj->' + field.name + '= strndup('
                                 +str(field.defaultValue)+',MAX_CSTRINGSIZE);\n')
                else:
                    stream.write('newObj->' + field.name + '=' + str(field.defaultValue)
                                 + ';\n')
            else:
                if field.typeid.name=='string':
                    self.writeCommentLines(stream,"string without default value are unallocated before they are assigned a value")
                    stream.write(self.getIndent()+'newObj->'+field.name+" = NULL;\n")
                else:
                    self.writeCommentLines(stream,field.name + "= <no default value in message spec using builtin>")
                    # FIXME find a default value for every type beside natives
        # non-builtin - non repeated requires a call to create function
        else:
            stream.write(self.getIndent()+'newObj->'+field.name+' = ')
            if isinstance(field.typeid,GenMsgAST.EnumType):
                stream.write('%s;\n' % (self.nameprefix + self.AST.shortenName(field.typeid.name) +'_'+field.typeid.values[0].name))
            else:
                stream.write(self.getTargetTypeName(field.typeid.name)+'_create(1);\n')

    def writeSerializeFieldStatement(self, stream, field):
        indexField = ''
        if field.qualifier == 'optional':
            stream.write(self.getIndent())
            stream.write('MB_write_bool(msgBuffer,cthis->_has%s);\n'
                         % self.upperFirst(field.name))
            stream.write(self.getIndent())
            stream.write('if (cthis->_has%s) {\n'
                         % self.upperFirst(field.name))
            self.indent()
        elif field.qualifier == 'repeated':
            indexField = '[i]'
            stream.write(self.getIndent())
            stream.write('uint32_t ' + field.name + 'Size = '
                        +'cthis->'+ field.name + 'size;\n')
            stream.write(self.getIndent())
            stream.write('MB_write_uint32(msgBuffer,cthis->' + field.name
                         + 'Size);\n')
            stream.write(self.getIndent())
            stream.write('for (uint32_t i = 0; i <cthis->' + field.name
                         + 'Size; ++i) {\n')
            self.indent()

        stream.write(self.getIndent())
        methodName = self.getSerializeMethodName(field.typeid.name)
        if None == methodName:  # field has no Serialize Method Name found in the map
            # non native field case
            if field.typeid.name in [m.name for m in self.AST.messages]:
                self.writeCommentLines("FIXME call serialize for field %s" % field.name)
                #stream.write(field.name + indexField+ '_serialize(msgBuffer);\n')
            elif field.typeid.name in [m.name for m in self.AST.enums]:
            # enum type field case (enum are serialized as uint32)
                methodName = self.getSerializeMethodName('uint32')
                stream.write(methodName)
                stream.write('(msgBuffer,cthis->' + field.name + indexField + ');\n')
            else:
            # native field case
                stream.write(self.commentLineBeginWith
                             + ' FIXME FIXME FIXME\n')
                stream.write(self.getIndent()
                             + self.commentLineBeginWith
                             + " don't know how to serialize native field <%s> of type <%s>\n"
                              % (field.name, field.typeid.name))
        else:
            # field has one Serialize Method Name found in the map
            stream.write(methodName)
            stream.write('(msgBuffer,cthis->' + field.name + indexField + ');\n')

        if field.qualifier == 'optional':
            self.unIndent()
            stream.write(self.getIndent() + '}\n')
        elif field.qualifier == 'repeated':
            self.unIndent()
            stream.write(self.getIndent() + '}\n')

    def writeShowFieldStatement(self, stream, field):
        indexField = ''
        if field.qualifier == 'optional':
            stream.write(self.getIndent())
            stream.write('fprintf(out,"(opt) %s =" cthis->' % field.name)
        elif field.qualifier == 'repeated':
            indexField = '[i]'
            stream.write(self.getIndent())
            stream.write('fprintf(out,"    %s [] = \n");\n'
                         % field.name)
            stream.write(self.getIndent())
            stream.write('for (uint32_t i = 0; i < get'
                         + self.upperFirst(field.name)
                         + 'Size(); ++i) {\n')
            self.indent()
            stream.write(self.getIndent() + 'out ')
        else:
            stream.write(self.getIndent())
            stream.write('fprintf(out," %s = " cthis->' % field.name)

        methodName = self.getSerializeMethodName(field.typeid.name)
        if None == methodName:  # field has no Serialize Method Name found in the map

            # non native field case

            if field.typeid.name in [m.name for m in self.AST.messages]:
                stream.write(field.name + indexField
                             + '.show(out)')
            elif field.typeid.name in [m.name for m in self.AST.enums]:

            # enum type field case (enum are serialized as uint32)

                stream.write('+ %s + " " ' % (field.name
                             + indexField))
            else:

                # stream.write(self.commentLineBeginWith+" FIXME FIXME FIXME inherited message\n")
                # native field case
                stream.write('<< "')
                stream.write(self.getIndent()
                             + self.commentLineBeginWith
                             + "FIXME FIXME don't know how to serialize native field <%s> of type <%s>"
                              % (field.name, field.typeid.name))
                stream.write('"')
        else:
              # field has one Serialize Method Name found in the map
            stream.write('+ %s + " " ' % (field.name + indexField))

        if field.qualifier == 'optional':
            stream.write(self.getIndent() + '+ "\\n");\n')
            stream.write(self.getIndent() + 'fflush(out)\n')
        elif field.qualifier == 'repeated':
            stream.write(';\n')
            self.unIndent()
            stream.write(self.getIndent() + '}\n')
            stream.write(self.getIndent() + 'fprintf(out,"\\n");\n')
        else:
            stream.write(self.getIndent() + ' + "\\n");\n')

    def writeDeSerializeFieldStatement(self, stream, field):
        indexField = ''
        if field.qualifier == 'optional':
            stream.write(self.getIndent())
            stream.write('MB_read_bool(msgBuffer,&(cthis->_has%s));\n'
                         % self.upperFirst(field.name))
            stream.write(self.getIndent())
            stream.write('if (cthis->_has%s) {\n'
                         % self.upperFirst(field.name))
            self.indent()
        elif field.qualifier == 'repeated':
            indexField = '[i]'
            stream.write(self.getIndent())
            stream.write('uint32_t ' + field.name+ 'Size;\n');
            stream.write('MB_read_uint32(msgBuffer,&'+field.name+'Size);\n')
            stream.write(self.getIndent())
            stream.write('cthis->'+field.name + 'size =' + field.name
                         + 'Size);\n')
            stream.write(self.getIndent())
            stream.write('for (uint32_t i = 0; i < cthis->' + field.name
                         + 'Size; ++i) {\n')
            self.indent()

        stream.write(self.getIndent())
        methodName = self.getDeSerializeMethodName(field.typeid.name)
        if None == methodName:  # field has no Deserialize Method Name found in the map
            # non native field case
            if field.typeid.name in [m.name for m in self.AST.messages]:
                stream.write('cthis->'+field.name + indexField
                             + '_deserialize(msgBuffer);\n')
            elif field.typeid.name in [m.name for m in self.AST.enums]:
                 # enum type field case (enum are deserialized as uint32)
                targetTypeName = \
                    self.getTargetTypeName(field.typeid.name)
                methodName = self.getDeSerializeMethodName('uint32')

                # We should check if the uint32 value is in enum range before casting it into enumtype
                stream.write(methodName +'(msgBuffer,(&cthis->'+field.name + indexField + '));\n')
            else:
            # native field case
                stream.write(self.commentLineBeginWith
                             + ' FIXME FIXME FIXME\n')
                stream.write(self.getIndent()
                             + self.commentLineBeginWith
                             + " don't know how to deserialize native field <%s> of type <%s>\n"
                              % (field.name, field.typeid.name))
        # field has one Deserialize Method Name found in the map
        else:
            if methodName == 'MB_read_string':
                stream.write(methodName + '(msgBuffer,cthis->'
                             + field.name + indexField + ',MAX_CSTRINGSIZE);\n')
            else:
                # We may have to vast in order to enforce conversion
                if isinstance(field.typeid, GenMsgAST.NativeType):
                    stream.write('cthis->'+field.name + indexField
                                 + ' = static_cast<'
                                 + field.typeid.name + '>('
                                 + methodName + '(msgBuffer));\n')
                else:
                    stream.write(methodName
                                 + '(msgBuffer,&(cthis->'+field.name + indexField+'));\n')

        if field.qualifier == 'optional':
            self.unIndent()
            stream.write(self.getIndent() + '}\n')
        elif field.qualifier == 'repeated':
            self.unIndent()
            stream.write(self.getIndent() + '}\n')

    def writeFactoryCreator(self, stream):
        creator = (self.AST.factory.creator[0], self.AST.factory.name) \
            + self.AST.factory.creator[1:]
        stream.write(self.getIndent() + '%s* %s_%s(%s type)'
                     % creator)
        stream.write('{ \n')

        self.indent()
        stream.write(self.getIndent() + '''%s* msg = NULL;

'''
                     % creator[0])
        stream.write(self.getIndent() + 'switch (type) {\n')
        self.indent()
        for e in self.AST.eMessageType.values:
            if None != self.replacePrefix:
                stream.write(self.getIndent() + 'case %s_%s:\n'
                             % (creator[0],
                             e.name.replace(self.replacePrefix[0], '',
                             1)))
            else:
                stream.write(self.getIndent() + 'case %s_%s:\n'
                             % (creator[0], e.name))

            self.indent()
            if None == e.type:
                # we throw here the first exception of the list
                stream.write(self.getIndent()
                             + 'return -1; /* value %s may not be used*/\n' % e.name)
            else:
                stream.write(self.getIndent() + 'msg = new %s();\n'
                             % e.type)
            stream.write(self.getIndent() + 'break;\n')
            self.unIndent()
        self.unIndent()
        stream.write(self.getIndent() + '} '
                     + self.commentLineBeginWith
                     + ' end if switch (type)\n')
        stream.write(self.getIndent() + 'return msg;\n')
        self.unIndent()
        stream.write(self.getIndent() + '''} /* end of %s_%s */

'''
                     % (creator[1], creator[2]))

    def writeFactoryReceiver(self, stream):
        receiver = (self.AST.factory.receiver[0],
                    self.AST.factory.name) \
            + self.AST.factory.receiver[1:]
        stream.write(self.getIndent() + '%s* %s_%s(%s stream) throw ('
                     % receiver)
        stream.write('%s' % self.exception[0])
        for exception in self.exception[1:]:
            stream.write(' ,%s' % exception)
        stream.write(') { \n')

        self.indent()
        stream.write(self.getIndent() + self.commentLineBeginWith
                     + ' FIXME This is not thread safe\n')
        stream.write(self.getIndent() + 'static %s msgBuffer;\n'
                     % self.serializeBufferType)
        stream.write(self.getIndent() + '%s  msgGen;\n' % receiver[0])
        stream.write(self.getIndent() + '''%s* msg;

''' % receiver[0])
        stream.write(self.getIndent() + self.commentLineBeginWith
                     + ' receive generic message \n')
        stream.write(self.getIndent()
                     + 'msgGen.receive(stream,msgBuffer);\n')
        stream.write(self.getIndent() + self.commentLineBeginWith
                     + ' create specific message from type \n')

        stream.write(self.getIndent() + 'msg = ')
        stream.write(self.AST.factory.name + '_'
                     + self.AST.factory.creator[1] + '(msgGen.%s);\n'
                     % self.messageTypeGetter)

        stream.write(self.getIndent()
                     + 'MB_assumeSizeFromReservedBytes();\n')
        stream.write(self.getIndent() + 'msg->deserialize(msgBuffer);\n'
                     )
        stream.write(self.getIndent() + 'return msg;\n')
        self.unIndent()
        stream.write(self.getIndent() + '''} /* end of %s_constructor */ 

'''
                     % (receiver[1], receiver[2]))

    def applyToFields(
        self,
        stream,
        fields,
        applyObject,
        ):
        for field in fields:
            if isinstance(field, GenMsgAST.MessageType.CombinedField):
                for cfield in field.fields:
                    applyObject(stream, cfield)
            else:
                applyObject(stream, field)

    def generateBody(self, stream, factoryOnly=False):
        """
        Generate the body.
        """
        # add necessary standard includes
        # [Try to] add corresponding header include
        supposedHeaderName = stream.name
        if supposedHeaderName != '<stdout>':
            supposedHeaderName = os.path.basename(supposedHeaderName)
            supposedHeaderName = os.path.splitext(supposedHeaderName)[0]
            stream.write('#include "' + supposedHeaderName + '.h"\n')

        # Generate namespace for specified package package
        # we may have nested namespace
        self.openNamespaces(stream)
        if not factoryOnly:

            # Generate message type
            for msg in self.AST.messages:
                self.writeCommentLines(stream,"---------------------- BEGIN <%s> ------------------------" % msg.name)
                # Generate Constructor
                stream.write(self.getIndent() + '%s_t* \n' % (self.nameprefix+msg.name))
                stream.write(self.getIndent() + '%s_create(' % (self.nameprefix+msg.name) 
                             +self.getTargetTypeName("uint32")+' nb) {\n'
                             )
                self.indent()
                stream.write(self.getIndent() +'%s_t* newObj;\n' % (self.nameprefix+msg.name))
                stream.write(self.getIndent() + "assert(nb>0);\n")
                stream.write(self.getIndent() + 'newObj = malloc(nb*sizeof(%s_t));\n' % (self.nameprefix+msg.name))
                stream.write(self.getIndent() + "assert(newObj!=NULL);\n")
                
                if msg.hasMerge():
                    stream.write(self.getIndent()+
                                 'for (%s i=0;i<nb;++i) {\n' % self.getTargetTypeName("uint32"))
                    self.indent()
                    stream.write(self.getIndent()+
                                 'newObj[i].super = %s_create(1);\n' % (self.nameprefix+msg.merge.name))
                    # Assign my name (we can use constant string here)
                    stream.write(self.getIndent()
                                 + 'newObj[i].super->messageName = "%s"; \n' % msg.name)
                    if None != self.replacePrefix:
                        stream.write(self.getIndent() + 'newObj[i].super->type = '
                                     + msg.name.upper().replace(self.replacePrefix[0],
                                     self.replacePrefix[1], 1) + ';\n')
                    else:
                        stream.write(self.getIndent() + 'newObj[i].super->type = '
                                + self.nameprefix + self.AST.shortenName("MessageType")+ '_' + msg.name.upper() + ';\n')
                    self.unIndent()
                    stream.write(self.getIndent()+"}\n")

                # Write allocate non builtin-type field
                # and assigne init value if any was provided
                if len(msg.fields) > 0:
                    self.applyToFields(stream, msg.fields,
                            self.writeInitFieldStatement)
                
                stream.write(self.getIndent() + "return newObj;\n")
                self.unIndent()
                stream.write(self.getIndent() + '}\n\n')

                # Generate Destructor
                stream.write(self.getIndent() + 'void %s_destroy(%s_t** cthis) {\n'
                             % (self.nameprefix+msg.name, self.nameprefix+msg.name))
                self.indent()
                stream.write(self.getIndent() + "assert(cthis!=NULL);\n")
                stream.write(self.getIndent() + 'free(*cthis);\n')
                stream.write(self.getIndent() + '*cthis = NULL;\n')
                self.unIndent()
                stream.write(self.getIndent() + '}\n\n')

                # write virtual serialize and deserialize
                # if we have some specific field

                if len(msg.fields) > 0:
                    # begin serialize method
                    stream.write(self.getIndent()
                                 + 'void %s_serialize(%s_t* cthis, %s* msgBuffer) {\n'
                                  % ((self.nameprefix+msg.name),
                                 (self.nameprefix+msg.name), self.serializeBufferType))
                    self.indent()
                    if msg.hasMerge():
                        self.writeCommentLines(stream,"Call mother class")
                        self.writeCommentLines(stream,"FIXME TBD")
                        #stream.write(self.getIndent()+ 'Super_serialize(msgBuffer);\n')
                    self.writeCommentLines(stream,"Specific serialization code")
                    self.applyToFields(stream, msg.fields,
                            self.writeSerializeFieldStatement)
                    self.unIndent()
                    stream.write(self.getIndent() + '''}

''')
                    # end serialize method

                    # begin deserialize method
                    stream.write(self.getIndent()
                                 + 'void %s_deserialize(%s_t* cthis, %s* msgBuffer) {\n'
                                  % ((self.nameprefix+msg.name), (self.nameprefix+msg.name),
                                 self.serializeBufferType))
                    self.indent()
                    if msg.hasMerge():
                        self.writeCommentLines(stream,"Call mother class")
                        self.writeCommentLines(stream,"FIXME TBD")
                        #stream.write(self.getIndent()+ 'Super_deserialize(msgBuffer);\n')
                    self.writeCommentLines(stream,"Specific deserialization code")
                    self.applyToFields(stream, msg.fields,
                            self.writeDeSerializeFieldStatement)
                    self.unIndent()
                    stream.write(self.getIndent() + '''}

''')

                    # end deserialize method
                    
                    # Generate getter/setter
                    self.writeCommentLines(stream,"specific Getter(s)/Setter(s)")
                    for field in msg.fields:
                        if isinstance(field,
                                GenMsgAST.MessageType.CombinedField):
                            for cfield in field.fields:
                                self.writeOneGetterSetter(stream,
                                        cfield,msg,headerOnly=False)
                        else:
                            self.writeOneGetterSetter(stream,field,msg,headerOnly=False)
                    
                    # begin show method
                    stream.write(self.getIndent()
                                 + 'FILE* %s_show(FILE* out) {\n'
                                  % msg.name)
                    self.indent()
                    stream.write(self.getIndent()
                                 + 'fprintf(out,"[%s -Begin] \\n");\n'
                                 % msg.name)
                    if msg.hasMerge():
                        self.writeCommentLines(stream,"Call mother class")
                        self.writeCommentLines(stream,"FIXME TBD")
                        #stream.write(self.getIndent()+ 'Super_show(out);\n')
                    self.writeCommentLines(stream,"Specific show code")
                    # FIXME this needs a rewrite
                    #self.applyToFields(stream, msg.fields,self.writeShowFieldStatement)
                    stream.write(self.getIndent()
                                 + 'fprintf(out,"[%s -End] \\n");\n'
                                 % msg.name)
                    stream.write(self.getIndent()
                                 + 'fflush(out);\n')
                    stream.write(self.getIndent() + 'return out;\n')
                    self.unIndent()
                    stream.write(self.getIndent() + '''}\n''')
                    # end show method
                    self.writeCommentLines(stream,"---------------------- END <%s> ------------------------" % msg.name)
                    stream.write("\n")

        # Generate Factory (if any)
        # @todo

        if self.AST.hasFactory():

            # begin creator

            self.writeFactoryCreator(stream)

            # begin receiver

            if self.AST.factory.hasFactoryReceiver():
                self.writeFactoryReceiver(stream)

        self.closeNamespaces(stream)


class CCERTIMessageGenerator(CGenerator):

    """
    This is a C generator for C{MessageAST}.
    
    """

    def generatorName(cls):
        return 'CCERTIMessage'

    generatorName = classmethod(generatorName)

    def __init__(self, MessageAST):
        super(CCERTIMessageGenerator, self).__init__(MessageAST)
        self.replacePrefix = list()
        self.replacePrefix.append('M_')
        self.replacePrefix.append('Message::')
        self.exportPrefix = 'CERTI_EXPORT'
        self.serializeBufferType = 'MsgBuffer_t'
        self.messageTypeGetter = 'getMessageType()'
        self.exception = ['NetworkError', 'NetworkSignal']


class CCERTINetworkMessageGenerator(CGenerator):

    """
    This is a C generator for C{MessageAST}.
    
    """

    def generatorName(cls):
        return 'CCERTINetworkMessage'

    generatorName = classmethod(generatorName)

    def __init__(self, MessageAST):
        super(CCERTINetworkMessageGenerator,
              self).__init__(MessageAST)
        self.replacePrefix = list()
        self.replacePrefix.append('NM_')
        self.replacePrefix.append('NetworkMessage::')
        self.exportPrefix = 'CERTI_EXPORT'
        self.serializeBufferType = 'MsgBuffer_t'
        self.messageTypeGetter = 'getMessageType()'
        self.exception = ['NetworkError', 'NetworkSignal']


