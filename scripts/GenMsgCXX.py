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
## $Id: GenMsgCXX.py,v 1.25 2011/09/08 20:24:06 erk Exp $
## ----------------------------------------------------------------------------

"""
The CERTI Message Generator.
C++ Backend Generator
"""

import logging
import GenMsgAST
import GenMsgBase
import sys
import os


class CXXGenerator(GenMsgBase.CodeGenerator):

    """
    This is a C++ generator for C{MessageAST}.
    
    """

    def generatorName(cls):
        return 'CXX'

    generatorName = classmethod(generatorName)

    def __init__(self, MessageAST):
        super(CXXGenerator, self).__init__(MessageAST, '//')
        self.included = dict()
        self.typedefed = dict()
        self.builtinTypeMap = {
            'onoff': 'bool',
            'bool': 'bool',
            'string': 'std::string',
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
            'onoff': 'write_bool',
            'bool': 'write_bool',
            'string': 'write_string',
            'byte': 'write_byte',
            'int8': 'write_int8',
            'uint8': 'write_uint8',
            'int16': 'write_int16',
            'uint16': 'write_uint16',
            'int32': 'write_int32',
            'uint32': 'write_uint32',
            'int64': 'write_int64',
            'uint64': 'write_uint64',
            'float': 'write_float',
            'double': 'write_double',
            }
        self.deserializeTypeMap = {
            'onoff': 'read_bool',
            'bool': 'read_bool',
            'string': 'read_string',
            'byte': 'read_byte',
            'int8': 'read_int8',
            'uint8': 'read_uint8',
            'int16': 'read_int16',
            'uint16': 'read_uint16',
            'int32': 'read_int32',
            'uint32': 'read_uint32',
            'int64': 'read_int64',
            'uint64': 'read_uint64',
            'float': 'read_float',
            'double': 'read_double',
            }
        self.__languageName = 'C++'
        self.replacePrefix = None
        self.exportPrefix = ''
        self.serializeBufferType = 'libhla::MessageBuffer'
        self.messageTypeGetter = 'getType()'
        self.exception = ['std::string']

    def getTargetTypeName(self, name):
        if name in self.builtinTypeMap.keys():
            return self.builtinTypeMap[name]
        else:
            t = self.AST.getType(name)
        if isinstance(t, GenMsgAST.EnumType):
            prefix = self.AST.name.split('.')[0] + '::'
            return prefix + name
        else:
            return name

    def openNamespaces(self, stream):
        if self.AST.hasPackage():
            self.writeComment(stream, self.AST.package)
            # we may have nested namespace
            nameSpaceList = self.AST.package.name.split('.')
            for ns in nameSpaceList:
                stream.write(self.getIndent() + '''namespace %s {

'''
                             % ns)
                self.indent()

    def closeNamespaces(self, stream):
        if self.AST.hasPackage():

            # we may have nested namespace
            nameSpaceList = self.AST.package.name.split('.')
            nameSpaceList.reverse()
            for ns in nameSpaceList:
                self.unIndent()
                stream.write(self.getIndent() + '} '
                             + self.commentLineBeginWith
                             + ' end of namespace %s \n' % ns)

    def writeOneGetterSetter(self, stream, field):

        targetTypeName = self.getTargetTypeName(field.typeid.name)

        if field.typeid.name == 'onoff':
            if field.qualifier == 'repeated':
                stream.write(self.getIndent())
                stream.write('uint32_t get'
                             + self.upperFirst(field.name)
                             + 'Size() const')
                stream.write(' {return ' + field.name + '.size();}\n')

                stream.write(self.getIndent())
                stream.write('void set' + self.upperFirst(field.name)
                             + 'Size(uint32_t num)')
                stream.write(' {' + field.name + '.resize(num);}\n')

                stream.write(self.getIndent())
                stream.write('const std::vector<' + targetTypeName
                             + '>& get' + self.upperFirst(field.name)
                             + '() const')
                stream.write(' {return ' + field.name + ';}\n')

                stream.write(self.getIndent())
                stream.write('void ' + field.name + 'On(uint32_t rank)')
                stream.write(' {' + field.name + '[rank] = true;}\n')

                stream.write(self.getIndent())
                stream.write('void ' + field.name + 'Off(uint32_t rank)'
                             )
                stream.write(' {' + field.name + '[rank] = false;}\n')

                stream.write(self.getIndent())
                stream.write(targetTypeName + ' is'
                             + self.upperFirst(field.name)
                             + 'On(uint32_t rank) const')
                stream.write(' {return ' + field.name + '[rank];}\n')
            else:
                stream.write(self.getIndent())
                stream.write('void ' + field.name + 'On()')
                stream.write(' {' + field.name + ' = true;}\n')

                stream.write(self.getIndent())
                stream.write('void ' + field.name + 'Off()')
                stream.write(' {' + field.name + ' = false;}\n')

                stream.write(self.getIndent())
                stream.write(targetTypeName + ' is'
                             + self.upperFirst(field.name)
                             + 'On() const')
                stream.write(' {return ' + field.name + ';}\n')
        else:
            if field.qualifier == 'repeated':
                stream.write(self.getIndent())
                stream.write('uint32_t get'
                             + self.upperFirst(field.name)
                             + 'Size() const')
                stream.write(' {return ' + field.name + '.size();}\n')

                stream.write(self.getIndent())
                stream.write('void set' + self.upperFirst(field.name)
                             + 'Size(uint32_t num)')
                stream.write(' {' + field.name + '.resize(num);}\n')

                stream.write(self.getIndent())
                stream.write('const std::vector<' + targetTypeName
                             + '>& get' + self.upperFirst(field.name)
                             + '() const')
                stream.write(' {return ' + field.name + ';}\n')

                stream.write(self.getIndent())
                stream.write('const ' + targetTypeName + '& get'
                             + self.upperFirst(field.name)
                             + '(uint32_t rank) const')
                stream.write(' {return ' + field.name + '[rank];}\n')

                stream.write(self.getIndent())
                stream.write(targetTypeName + '& get'
                             + self.upperFirst(field.name)
                             + '(uint32_t rank)')
                stream.write(' {return ' + field.name + '[rank];}\n')

                stream.write(self.getIndent())
                stream.write('void set' + self.upperFirst(field.name)
                             + '(const ')
                stream.write(targetTypeName + '& new'
                             + self.upperFirst(field.name)
                             + ', uint32_t rank)')
                stream.write(' {' + field.name + '[rank]=new'
                             + self.upperFirst(field.name) + ';}\n')

                stream.write(self.getIndent())
                stream.write('void remove'
                             + self.upperFirst(field.name)
                             + '(uint32_t rank)')
                stream.write(' {' + field.name + '.erase(' + field.name
                             + '.begin() + rank);}\n')
            else:
                stream.write(self.getIndent())
                stream.write('const ' + targetTypeName + '& get'
                             + self.upperFirst(field.name) + '() const')
                stream.write(' {return ' + field.name + ';}\n')

                stream.write(self.getIndent())
                stream.write('void set' + self.upperFirst(field.name)
                             + '(const ')
                stream.write(targetTypeName + '& new'
                             + self.upperFirst(field.name) + ') {')
                if field.qualifier == 'optional':
                    stream.write('\n')
                    self.indent()
                    stream.write(self.getIndent() + '_has%s=true;\n'
                                 % self.upperFirst(field.name))
                    stream.write(self.getIndent() + field.name + '=new'
                                 + self.upperFirst(field.name) + ';\n')
                    self.unIndent()
                    stream.write(self.getIndent())
                else:
                    stream.write(field.name + '=new'
                                 + self.upperFirst(field.name) + ';')
                stream.write('}\n')
                if field.qualifier == 'optional':
                    stream.write(self.getIndent())
                    tmp = self.upperFirst(field.name)
                    stream.write('bool has%s() {return _has%s;}\n'
                                 % (tmp, tmp))

    def writeDeclarationFieldStatement(self, stream, field):
        stream.write(self.getIndent())
        if field.qualifier == 'repeated':
            stream.write('std::vector<%s> %s;'
                         % (self.getTargetTypeName(field.typeid.name),
                         field.name))
        else:
            stream.write('%s %s;'
                         % (self.getTargetTypeName(field.typeid.name),
                         field.name))
        self.writeComment(stream, field)

        # optional field generate another boolean field
        # used to detect whether if the optional field has
        # been given or not.

        if field.qualifier == 'optional':
            stream.write(self.getIndent() + 'bool _has%s;\n'
                         % self.upperFirst(field.name))

    def generateEnum(self, stream, enum):
        self.writeComment(stream, enum)
        stream.write(self.getIndent())
        stream.write('typedef enum %s {\n' % enum.name)
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
        stream.write('} %s_t; ' % enum.name)
        stream.write(self.commentLineBeginWith + 'end of enum %s \n'
                     % enum.name)

    def generateHeader(self, stream, factoryOnly=False):
        self.addGeneratedByLine(stream)
        # write the usual header protecting MACRO
        supposedHeaderName = stream.name
        if supposedHeaderName != '<stdout>':
            supposedHeaderName = os.path.basename(supposedHeaderName)
            supposedHeaderName = os.path.splitext(supposedHeaderName)[0]
            headerProtectMacroName = supposedHeaderName
        else:
            (headerProtectMacroName, ext) = \
                os.path.splitext(self.AST.name)

        headerProtectMacroName = '%s_HH' \
            % headerProtectMacroName.upper()
        stream.write('#ifndef %s\n' % headerProtectMacroName)
        stream.write('#define %s\n' % headerProtectMacroName)

        # add necessary standard and global includes

        stream.write(self.commentLineBeginWith
                     + ' ****-**** Global System includes ****-****\n')
        stream.write('#include <vector>\n')
        self.included['#include <vector>'] = 1
        stream.write('#include <string>\n')
        self.included['#include <string>'] = 1

        # add include coming from native type specification

        stream.write(self.commentLineBeginWith
                     + ' ****-**** Includes coming from native types ****-****\n'
                     )
        for native in self.AST.natives:
            if native.hasLanguage('CXX'):
                for line in native.getLanguageLines('CXX'):
                    # we are only interested in native "include" statement
                    stmt = line.statement
                    if stmt.find('#include') >= 0 and not stmt \
                        in self.included.keys():
                        self.writeComment(stream, native)
                        stream.write(stmt + '\n')
                        self.included[stmt] = 1

        # Generate namespace for specified package package
        # we may have nested namespace

        self.openNamespaces(stream)

        if not factoryOnly:

            # Native type should be defined in included header

            stream.write(self.getIndent() + self.commentLineBeginWith)
            stream.write(' Native types has been defined:\n')
            stream.write(self.getIndent() + self.commentLineBeginWith)
            stream.write('     - by included headers (see above)\n')
            stream.write(self.getIndent() + self.commentLineBeginWith)
            stream.write('     - with typedef (see below [if any])\n')
            for native in self.AST.natives:
                if native.hasLanguage('CXX'):
                    for line in native.getLanguageLines('CXX'):
                        stmt = line.statement

                       # we are only interested in native statement
                       # which are not #include

                        if stmt.find('typedef') >= 0 and not stmt \
                            in self.typedefed.keys():
                            self.writeComment(stream, native)
                            stream.write(self.getIndent() + stmt + '\n')
                            self.typedefed[stmt] = 1

            # Put enum in a namespace in order to avoid conflict

            stream.write(self.getIndent())
            stream.write('namespace %s {\n' % self.AST.name.split('.'
                         )[0])
            self.indent()

            # Generate version

            if self.AST.hasVersion():
                (major, minor) = self.AST.version.number
                stream.write(self.getIndent())
                stream.write('static const uint32_t versionMajor = %d;\n'
                              % major)
                stream.write(self.getIndent())
                stream.write('static const uint32_t versionMinor = %d;\n'
                              % minor)

            # Generate enum

            lastname = ''
            for enum in self.AST.enums:
                self.generateEnum(stream, enum)
            stream.write('\n')

            # close enum namespace

            self.unIndent()
            stream.write(self.getIndent())
            stream.write('}\n')

            # Generate message type

            for msg in self.AST.messages:
                self.writeComment(stream, msg)
                stream.write(self.getIndent())
                stream.write('class %s %s' % (self.exportPrefix,
                             msg.name))
                if msg.hasMerge():
                    stream.write(' : public %s {\n' % msg.merge.name)
                    virtual = 'virtual '
                elif msg.hasHeir():
                    stream.write(' {\n')
                    virtual = 'virtual '
                else:
                    stream.write(' {\n')
                    virtual = ''

                self.indent()

                # begin public

                stream.write(self.getIndent() + 'public:\n')
                self.indent()

                if msg.hasEnum():
                    self.generateEnum(stream, msg.enum)
                    stream.write('\n')

                if msg.hasMerge():
                    stream.write(self.getIndent()
                                 + 'typedef %s Super;\n'
                                 % msg.merge.name)

                # now write constructor/destructor

                stream.write(self.getIndent() + msg.name + '();\n')
                stream.write(self.getIndent() + virtual + '~'
                             + msg.name + '();\n')

                # write virtual serialize and deserialize
                # if we have some specific field

                if len(msg.fields) > 0:

                    # serialize/deserialize

                    stream.write(self.getIndent() + virtual
                                 + 'void serialize(%s& msgBuffer);\n'
                                 % self.serializeBufferType)
                    stream.write(self.getIndent() + virtual
                                 + 'void deserialize(%s& msgBuffer);\n'
                                 % self.serializeBufferType)

                    # specific getter/setter

                    stream.write(self.getIndent()
                                 + self.commentLineBeginWith
                                 + ' specific Getter(s)/Setter(s)\n')
                    for field in msg.fields:
                        if isinstance(field,
                                GenMsgAST.MessageType.CombinedField):
                            for cfield in field.fields:
                                self.writeOneGetterSetter(stream,
                                        cfield)
                        else:
                            self.writeOneGetterSetter(stream, field)

                    # the show method

                    stream.write(self.getIndent()
                                 + self.commentLineBeginWith
                                 + ' the show method\n')
                    stream.write(self.getIndent() + virtual
                                 + 'std::ostream& show(std::ostream& out);\n'
                                 )

                self.unIndent()

                # end public:

                # begin protected

                stream.write(self.getIndent() + 'protected:\n')
                self.indent()
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

                # end protected

                # begin private

                stream.write(self.getIndent() + 'private:\n')
                self.indent()
                self.unIndent()

                # end private

                self.unIndent()
                stream.write(self.getIndent() + '};\n')

        # Generate Factory (if any)
        # @todo

        if self.AST.hasFactory():
            self.writeComment(stream, self.AST.factory)
            stream.write(self.getIndent() + 'class %s %s {\n'
                         % (self.exportPrefix, self.AST.factory.name))
            self.indent()

            # begin public

            stream.write(self.getIndent() + 'public:\n')
            self.indent()

            stream.write(self.getIndent()
                         + 'static %s* %s(%s type) throw ('
                         % self.AST.factory.creator)
            stream.write('%s' % self.exception[0])
            for exception in self.exception[1:]:
                stream.write(' ,%s' % exception)
            stream.write('); \n')

            if self.AST.factory.hasFactoryReceiver():
                stream.write(self.getIndent()
                             + 'static %s* %s(%s stream) throw ('
                             % self.AST.factory.receiver)
                stream.write('%s' % self.exception[0])
                for exception in self.exception[1:]:
                    stream.write(' ,%s' % exception)
                stream.write('); \n')

            self.unIndent()

            # end public
            # begin protected

            stream.write(self.getIndent() + 'protected:\n')
            self.indent()
            self.unIndent()

            # end protected
            # begin private

            stream.write(self.getIndent() + 'private:\n')
            self.indent()
            self.unIndent()

            # end private

            self.unIndent()
            stream.write(self.getIndent() + '''};

''')

        # may close any open namespaces

        self.closeNamespaces(stream)

        # close usual HEADER protecting MACRO

        stream.write(self.commentLineBeginWith + ' %s\n'
                     % headerProtectMacroName)
        stream.write('#endif\n')

    def writeInitFieldStatement(self, stream, field):
        if field.qualifier == 'optional':
            stream.write(self.getIndent())
            stream.write('_has' + self.upperFirst(field.name)
                         + '=false;\n')
        if field.hasDefaultValue():
            stream.write(self.getIndent())
            stream.write(field.name + '=' + str(field.defaultValue)
                         + ';\n')
        else:
            stream.write(self.getIndent())
            stream.write(self.commentLineBeginWith)
            stream.write(field.name
                         + '= <no default value in message spec using builtin>\n'
                         )
            # FIXME find a default value for every type beside natives

    def writeSerializeFieldStatement(self, stream, field):
        indexField = ''
        if field.qualifier == 'optional':
            stream.write(self.getIndent())
            stream.write('msgBuffer.write_bool(_has%s);\n'
                         % self.upperFirst(field.name))
            stream.write(self.getIndent())
            stream.write('if (_has%s) {\n'
                         % self.upperFirst(field.name))
            self.indent()
        elif field.qualifier == 'repeated':
            indexField = '[i]'
            stream.write(self.getIndent())
            stream.write('uint32_t ' + field.name + 'Size = '
                         + field.name + '.size();\n')
            stream.write(self.getIndent())
            stream.write('msgBuffer.write_uint32(' + field.name
                         + 'Size);\n')
            stream.write(self.getIndent())
            stream.write('for (uint32_t i = 0; i < ' + field.name
                         + 'Size; ++i) {\n')
            self.indent()

        stream.write(self.getIndent())
        methodName = self.getSerializeMethodName(field.typeid.name)
        # field has no Serialize Method Name found in the map
        if None == methodName:
            # non native field case
            if field.typeid.name in [m.name for m in self.AST.messages]:
                stream.write(field.name + indexField
                             + '.serialize(msgBuffer);\n')
            # enum type field case (enum are serialized as uint32)
            elif field.typeid.name in [m.name for m in self.AST.enums]:
                methodName = self.getSerializeMethodName('uint32')
                stream.write('msgBuffer.' + methodName)
                stream.write('(' + field.name + indexField + ');\n')
            # native field case
            else:
                repLine = self.getRepresentationFor(field.typeid.name)
                # some native may not have any representation at all
                if repLine:
                    methodName = self.getSerializeMethodName(repLine.representation)
                    # The representation line may have a qualifier
                    # like 'repeated'
                    if methodName != None and repLine.hasQualifier():
                        if repLine.qualifier == 'repeated':
                            stream.write(self.commentLineBeginWith + "serialize native whose representation is 'repeated' %s \n" % repLine.representation)
                            stream.write(self.getIndent() + 'msgBuffer.'+ self.getSerializeMethodName('uint32'))
                            stream.write('('+field.name + indexField + '.size()' +');\n')
                            stream.write(self.getIndent() + 'msgBuffer.'+ self.getSerializeMethodName(repLine.representation)+'s')
                            stream.write('(&('+field.name + indexField +'[0]),')
                            stream.write(field.name + indexField + '.size()' +');\n')
                # we can not handle this native case: no representation given
                else:
                    stream.write(self.commentLineBeginWith + ' FIXME FIXME FIXME\n')
                    stream.write(self.getIndent()
                                 + self.commentLineBeginWith
                                 + "don't know how to serialize native field <%s> of type <%s>\n"
                                 % (field.name, field.typeid.name))
                    stream.write(self.getIndent()
                                 + self.commentLineBeginWith
                                 + "probably no 'representation' given\n")
        else:
            # field has one Serialize Method Name found in the map
            stream.write('msgBuffer.' + methodName)
            stream.write('(' + field.name + indexField + ');\n')

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
            stream.write('out << "(opt) %s =" ' % field.name)
        elif field.qualifier == 'repeated':
            indexField = '[i]'
            stream.write(self.getIndent())
            stream.write('out << "    %s [] =" << std::endl;\n'
                         % field.name)
            stream.write(self.getIndent())
            stream.write('for (uint32_t i = 0; i < get'
                         + self.upperFirst(field.name)
                         + 'Size(); ++i) {\n')
            self.indent()
            stream.write(self.getIndent() + 'out ')
        else:
            stream.write(self.getIndent())
            stream.write('out << " %s = " ' % field.name)

        methodName = self.getSerializeMethodName(field.typeid.name)
        if None == methodName:  # field has no Serialize Method Name found in the map

            # non native field case
            if field.typeid.name in [m.name for m in self.AST.messages]:
                stream.write('<< ' + field.name + indexField
                             + '.show(out)')
            elif field.typeid.name in [m.name for m in self.AST.enums]:

            # enum type field case (enum are serialized as uint32)

                stream.write('<< %s << " " ' % (field.name
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

            stream.write('<< %s << " " ' % (field.name + indexField))

        if field.qualifier == 'optional':
            stream.write(self.getIndent() + '<< std::endl;\n')
        elif field.qualifier == 'repeated':
            stream.write(';\n')
            self.unIndent()
            stream.write(self.getIndent() + '}\n')
            stream.write(self.getIndent() + 'out << std::endl;\n')
        else:
            stream.write(self.getIndent() + '<< std::endl;\n')

    def writeDeSerializeFieldStatement(self, stream, field):
        indexField = ''
        if field.qualifier == 'optional':
            stream.write(self.getIndent())
            stream.write('_has%s = msgBuffer.read_bool();\n'
                         % self.upperFirst(field.name))
            stream.write(self.getIndent())
            stream.write('if (_has%s) {\n'
                         % self.upperFirst(field.name))
            self.indent()
        elif field.qualifier == 'repeated':
            indexField = '[i]'
            stream.write(self.getIndent())
            stream.write('uint32_t ' + field.name
                         + 'Size = msgBuffer.read_uint32();\n')
            stream.write(self.getIndent())
            stream.write(field.name + '.resize(' + field.name
                         + 'Size);\n')
            stream.write(self.getIndent())
            stream.write('for (uint32_t i = 0; i < ' + field.name
                         + 'Size; ++i) {\n')
            self.indent()

        stream.write(self.getIndent())
        methodName = self.getDeSerializeMethodName(field.typeid.name)
        # field has no Deserialize Method Name found in the map
        if None == methodName:
            # non native field case
            if field.typeid.name in [m.name for m in self.AST.messages]:
                stream.write(field.name + indexField
                             + '.deserialize(msgBuffer);\n')
            # enum type field case (enum are deserialized as uint32)
            elif field.typeid.name in [m.name for m in self.AST.enums]:
                targetTypeName = \
                    self.getTargetTypeName(field.typeid.name)
                methodName = self.getDeSerializeMethodName('uint32')
                # We should check if the uint32 value is in enum range before casting it into enumtype
                stream.write(field.name + indexField + ' = static_cast<'
                              + targetTypeName + '>(msgBuffer.'
                             + methodName + ' ());\n')
            # native field case
            else:
                repLine = self.getRepresentationFor(field.typeid.name)
                # some native may not have any representation at all
                if repLine:
                    methodName = self.getDeSerializeMethodName(repLine.representation)
                    # The representation line may have a qualifier
                    # like 'repeated'
                    if methodName != None and repLine.hasQualifier():
                        if repLine.qualifier == 'repeated':
                            stream.write(self.commentLineBeginWith + "deserialize native whose representation is 'repeated' %s \n" % repLine.representation)
                            stream.write(self.getIndent()
                                         + field.name + indexField + '.resize('
                                         + 'msgBuffer.'
                                         + self.getDeSerializeMethodName('uint32')+'()'
                                         + ');\n')
                            stream.write(self.getIndent() + 'msgBuffer.' + self.getDeSerializeMethodName(repLine.representation)+'s')
                            stream.write('(&('+field.name + indexField +'[0]),')
                            stream.write(field.name + indexField + '.size()' +');\n')
                # we can not handle this native case: no representation given
                else:
                    stream.write(self.commentLineBeginWith
                                 + ' FIXME FIXME FIXME\n')
                    stream.write(self.getIndent()
                                 + self.commentLineBeginWith
                                 + "don't know how to deserialize native field <%s> of type <%s>\n"
                                 % (field.name, field.typeid.name))
                    stream.write(self.getIndent()
                                 + self.commentLineBeginWith
                                 + "probably no 'representation' given\n")
        # field has one Deserialize Method Name found in the map
        else:
            if methodName == 'read_string':
                stream.write('msgBuffer.' + methodName + '('
                             + field.name + indexField + ');\n')
            else:
                # We may have to cast in order to enforce conversion
                if isinstance(field.typeid, GenMsgAST.NativeType):
                    stream.write(field.name + indexField
                                 + ' = static_cast<'
                                 + field.typeid.name + '>(msgBuffer.'
                                 + methodName + '());\n')
                else:
                    stream.write(field.name + indexField
                                 + ' = msgBuffer.' + methodName
                                 + '();\n')

        if field.qualifier == 'optional':
            self.unIndent()
            stream.write(self.getIndent() + '}\n')
        elif field.qualifier == 'repeated':
            self.unIndent()
            stream.write(self.getIndent() + '}\n')

    def writeFactoryCreator(self, stream):
        creator = (self.AST.factory.creator[0], self.AST.factory.name) \
            + self.AST.factory.creator[1:]
        stream.write(self.getIndent() + '%s* %s::%s(%s type) throw ('
                     % creator)
        stream.write('%s' % self.exception[0])
        for exception in self.exception[1:]:
            stream.write(' ,%s' % exception)
        stream.write(') { \n')

        self.indent()
        stream.write(self.getIndent() + '''%s* msg = NULL;

'''
                     % creator[0])
        stream.write(self.getIndent() + 'switch (type) {\n')
        self.indent()
        for e in self.AST.eMessageType.values:
            if None != self.replacePrefix:
                stream.write(self.getIndent() + 'case %s::%s:\n'
                             % (creator[0],
                             e.name.replace(self.replacePrefix[0], '',
                             1)))
            else:
                stream.write(self.getIndent() + 'case %s::%s:\n'
                             % (creator[0], e.name))

            self.indent()

            if None == e.type:

        # we throw here the first exception of the list

                stream.write(self.getIndent()
                             + 'throw %s("%s message type should not be used!!");\n'
                              % (self.exception[0], e.name))
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
        stream.write(self.getIndent() + '''} /* end of %s::%s */

'''
                     % (creator[1], creator[2]))

    def writeFactoryReceiver(self, stream):
        receiver = (self.AST.factory.receiver[0],
                    self.AST.factory.name) \
            + self.AST.factory.receiver[1:]
        stream.write(self.getIndent() + '%s* %s::%s(%s stream) throw ('
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
        stream.write(self.AST.factory.name + '::'
                     + self.AST.factory.creator[1] + '(msgGen.%s);\n'
                     % self.messageTypeGetter)

        stream.write(self.getIndent()
                     + 'msgBuffer.assumeSizeFromReservedBytes();\n')
        stream.write(self.getIndent() + 'msg->deserialize(msgBuffer);\n'
                     )
        stream.write(self.getIndent() + 'return msg;\n')
        self.unIndent()
        stream.write(self.getIndent() + '''} /* end of %s::%s */ 

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
        self.addGeneratedByLine(stream)
        # add necessary standard includes
        stream.write('#include <vector>\n')
        stream.write('#include <string>\n')
        # [Try to] add corresponding header include

        supposedHeaderName = stream.name
        if supposedHeaderName != '<stdout>':
            supposedHeaderName = os.path.basename(supposedHeaderName)
            supposedHeaderName = os.path.splitext(supposedHeaderName)[0]
            stream.write('#include "' + supposedHeaderName + '.hh"\n')

        # Generate namespace for specified package package
        # we may have nested namespace

        self.openNamespaces(stream)
        if not factoryOnly:

            # Generate message type

            for msg in self.AST.messages:

                # Generate Constructor

                stream.write(self.getIndent() + '%s::%s() {\n'
                             % (msg.name, msg.name))
                self.indent()
                if msg.hasMerge():

                    # Assign my name.

                    stream.write(self.getIndent()
                                 + 'this->messageName = "' + msg.name
                                 + '";\n')
                    if None != self.replacePrefix:
                        stream.write(self.getIndent() + 'this->type = '
                                + msg.name.upper().replace(self.replacePrefix[0],
                                self.replacePrefix[1], 1) + ';\n')
                    else:
                        stream.write(self.getIndent() + 'this->type = '
                                + msg.name.upper() + ';\n')

                # Write init value if any was provided

                if len(msg.fields) > 0:
                    self.applyToFields(stream, msg.fields,
                            self.writeInitFieldStatement)
                self.unIndent()
                stream.write(self.getIndent() + '''}

''')

                # Generate Destructor

                stream.write(self.getIndent() + '%s::~%s() {\n'
                             % (msg.name, msg.name))
                self.indent()
                self.unIndent()
                stream.write(self.getIndent() + '''}

''')

                # write virtual serialize and deserialize
                # if we have some specific field

                if len(msg.fields) > 0:

                    # begin serialize method

                    stream.write(self.getIndent()
                                 + 'void %s::serialize(%s& msgBuffer) {\n'
                                  % (msg.name,
                                 self.serializeBufferType))
                    self.indent()
                    if msg.hasMerge():
                        stream.write(self.getIndent()
                                + self.commentLineBeginWith)
                        stream.write('Call mother class\n')
                        stream.write(self.getIndent()
                                + 'Super::serialize(msgBuffer);\n')
                    stream.write(self.getIndent()
                                 + self.commentLineBeginWith)
                    stream.write('Specific serialization code\n')
                    self.applyToFields(stream, msg.fields,
                            self.writeSerializeFieldStatement)
                    self.unIndent()
                    stream.write(self.getIndent() + '''}

''')

                    # end serialize method

                    # begin deserialize method

                    stream.write(self.getIndent()
                                 + 'void %s::deserialize(%s& msgBuffer) {\n'
                                  % (msg.name,
                                 self.serializeBufferType))
                    self.indent()
                    if msg.hasMerge():
                        stream.write(self.getIndent()
                                + self.commentLineBeginWith)
                        stream.write('Call mother class\n')
                        stream.write(self.getIndent()
                                + 'Super::deserialize(msgBuffer);\n')
                    stream.write(self.getIndent()
                                 + self.commentLineBeginWith)
                    stream.write('Specific deserialization code\n')
                    self.applyToFields(stream, msg.fields,
                            self.writeDeSerializeFieldStatement)
                    self.unIndent()
                    stream.write(self.getIndent() + '''}

''')

                    # end deserialize method
                    # begin show method

                    stream.write(self.getIndent()
                                 + 'std::ostream& %s::show(std::ostream& out) {\n'
                                  % msg.name)
                    self.indent()
                    stream.write(self.getIndent()
                                 + 'out << "[%s -Begin]" << std::endl;'
                                 % msg.name)
                    if msg.hasMerge():
                        stream.write(self.getIndent()
                                + self.commentLineBeginWith)
                        stream.write('Call mother class\n')
                        stream.write(self.getIndent()
                                + 'Super::show(out);\n')
                    stream.write(self.getIndent()
                                 + self.commentLineBeginWith)
                    stream.write('Specific show code\n')
                    self.applyToFields(stream, msg.fields,
                            self.writeShowFieldStatement)
                    stream.write(self.getIndent()
                                 + 'out << "[%s -End]" << std::endl;\n'
                                 % msg.name)
                    stream.write(self.getIndent() + 'return out;\n')
                    self.unIndent()
                    stream.write(self.getIndent() + '''}

''')

                    # end show method

        # Generate Factory (if any)
        # @todo

        if self.AST.hasFactory():

            # begin creator

            self.writeFactoryCreator(stream)

            # begin receiver

            if self.AST.factory.hasFactoryReceiver():
                self.writeFactoryReceiver(stream)

        self.closeNamespaces(stream)


class CXXCERTIMessageGenerator(CXXGenerator):

    """
    This is a C++ generator for C{MessageAST}.
    
    """

    def generatorName(cls):
        return 'CXXCERTIMessage'

    generatorName = classmethod(generatorName)

    def __init__(self, MessageAST):
        super(CXXCERTIMessageGenerator, self).__init__(MessageAST)
        self.replacePrefix = list()
        self.replacePrefix.append('M_')
        self.replacePrefix.append('Message::')
        self.exportPrefix = 'CERTI_EXPORT'
        self.serializeBufferType = 'libhla::MessageBuffer'
        self.messageTypeGetter = 'getMessageType()'
        self.exception = ['NetworkError', 'NetworkSignal']


class CXXCERTINetworkMessageGenerator(CXXGenerator):

    """
    This is a C++ generator for C{MessageAST}.
    
    """

    def generatorName(cls):
        return 'CXXCERTINetworkMessage'

    generatorName = classmethod(generatorName)

    def __init__(self, MessageAST):
        super(CXXCERTINetworkMessageGenerator,
              self).__init__(MessageAST)
        self.replacePrefix = list()
        self.replacePrefix.append('NM_')
        self.replacePrefix.append('NetworkMessage::')
        self.exportPrefix = 'CERTI_EXPORT'
        self.serializeBufferType = 'libhla::MessageBuffer'
        self.messageTypeGetter = 'getMessageType()'
        self.exception = ['NetworkError', 'NetworkSignal']


