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
## $Id: GenMsgSpec.py,v 1.2 2011/07/18 11:57:34 erk Exp $
## ----------------------------------------------------------------------------

# We use logging for ... logging :-)
# see http://docs.python.org/library/logging.html

import logging
import GenMsgAST
import GenMsgBase
import sys

class MsgSpecGenerator(GenMsgBase.CodeGenerator):
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
            if self.AST.factory.hasFactoryCreator():
                stream.write(self.getIndent() + 'factoryCreator %s %s(%s)\n'
                             % self.AST.factory.creator)
            if self.AST.factory.hasFactoryReceiver():
                stream.write(self.getIndent()
                             + 'factoryReceiver %s %s(%s)\n'
                             % self.AST.factory.receiver)
            self.unIndent()
            stream.write('''}

''')
