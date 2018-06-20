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
## $Id: GenMsgAST.py,v 1.23 2011/07/29 09:08:23 erk Exp $
## ----------------------------------------------------------------------------

"""
The CERTI Message Generator.
Generator AST classes.
"""

import os
import getopt
import sys
import datetime
import logging
import re

# Build some logger related objects

stdoutHandler = logging.StreamHandler(sys.stdout)


class ASTElement(object):
    """
    The base class for all Abstract Syntax Tree element.
    """

    def __init__(self, name):
        """
        The class constructor

        @param name: the name we want to give to the element
        @type name: C{string}
        """
        
        self.__name = name
        """
        @ivar: the name of the C{ASTElement} object
        @type: C{string}
        """
        self.__comment = None
        """
        @ivar: the comment block attached to the C{ASTElement} object
        @type: C{CommentBlock}
        """
        self.__linespan = None
        """
        @ivar: the line span of this C{ASTElement} in the original file
        """
        self.logger = logging.Logger('ASTElement')
        """
        @ivar: the logger attached to the C{ASTElement} object
        @type: C{Logger} (the C{Logger} class is defined in the C{logging} Python module
        """
        self.logger.setLevel(logging.ERROR)
        self.logger.addHandler(stdoutHandler)

    def __getName(self):
        """
        Getter for __name
        @return: the name of the C{ASTElement}

        """
        return self.__name

    def __setName(self, name):
        """
        Setter for __name
        @param name: the new name
        @type name: C{string}

        """
        self.__name = name

    # pythonic getter/setter using properties

    name = property(fget=__getName, fset=__setName, fdel=None,
                    doc='The name of the C{ASTElement}')

    def hasComment(self):
        """
        Tells if the C{ASTElement} has got a comment block or not
        @return: TRUE if the C{ASTElement} has got a comment block and FALSE if not

        """
        return self.__comment != None

    def __getComment(self):
        """
        Getter for __comment
        @return: the comment block attached to the C{ASTElement}

        """
        return self.__comment

    def __setComment(self, comment):
        """
        Setter for __comment
        @param comment: the new comment block
        @type comment: C{CommentBlock}
    
        """
        if isinstance(comment, type('')):
            pass
        else:
            self.logger.info('Adding comment %s to element %s'
                             % (comment.lines, self.name))
            self.__comment = comment

    # pythonic getter/setter using properties

    comment = property(fget=__getComment, fset=__setComment, fdel=None,
                       doc='The comment block attached to the C{ASTElement}'
                       )

    def __getLinespan(self):
        """
        Getter for __linespan
        @return: The line span of the C{ASTElement} in the original file
        """
        return self.__linespan

    def __setLinespan(self, linespan):
        """
        Setter for __linespan
        @param linespan: the new line span of the C{ASTElement} object
        """
        self.__linespan = linespan

    # pythonic getter/setter using properties

    linespan = property(fget=__getLinespan, fset=__setLinespan,
                        fdel=None,
                        doc='The line span of this C{ASTElement} in the original file'
                        )


class MessageAST(ASTElement):

    """
    Message Abstract Syntax Tree root class.
    
    This class represents the abstraction of the message description
    language including all the features represented by corresponding
    class field. Object instance of C{MessageAST} may be used by a 
    language generator in order to generate code for the target language.
       
    
    """
    def shortenName(cls,somename):
        """
        Shorten the package name in order to use it as a prefix.
        The short name is composed of the upper case letters found
        in the name collated together and transformed to lower case
        """
        all_uppers = re.findall("[A-Z]", somename)
        containsdot = re.findall("\.", somename)
        # build short name with the upper case letters
        if len(all_uppers)>0:
            shortname = ""
            for l in all_uppers:
                shortname = shortname + l
            shortname = shortname.lower()
        # build short name with initial letter of dot-separated names
        elif len(containsdot)>0:
            dotseparated = somename.split(".")
            shortname = ""
            for w in dotseparated:
                shortname = shortname + w[0]
        # build short name with the first 3 letters
        else:
            shortname = somename[0:3]
        return shortname
    
    shortenName = classmethod(shortenName)

    def __init__(self, name):
        """
        The class constructor
        @param name: the name we want to give to the C{ASTElement} object
        @type name: C{string}
        """
        super(MessageAST, self).__init__(name=os.path.basename(name))
        self.__package = None
        """
        @ivar: the package of the C{MessageAST} object
        @type: C{Package}
        """
        self.__version = None
        """
        @ivar: the version of the C{MessageAST} object
        @type: C{Version}
        """
        self.__factory = None
        """
        @ivar: the factory of the C{MessageAST} object
        @type: C{Factory}
        """
        self.__nativeTypes = []
        """
        @ivar: the native types of the C{MessageAST} object
        @type: a C{set} of C{NativeType}
        """
        self.__messageTypes = []
        """
        @ivar: the message types of the C{MessageAST} object
        @type: a C{set} of C{MessageType}
        """
        self.__enumTypes = []
        """
        @ivar: the enum types of the C{MessageAST} object
        @type: a C{set} of C{EnumType}
        """

        # The types dictionary is initialized with builtin types

        self.__types = {
            'onoff': ASTElement('onoff'),
            'bool': ASTElement('bool'),
            'string': ASTElement('string'),
            'byte': ASTElement('byte'),
            'int8': ASTElement('int8'),
            'uint8': ASTElement('uint8'),
            'int16': ASTElement('int16'),
            'uint16': ASTElement('uint16'),
            'int32': ASTElement('int32'),
            'uint32': ASTElement('uint32'),
            'int64': ASTElement('int64'),
            'uint64': ASTElement('uint64'),
            'float': ASTElement('float'),
            'double': ASTElement('double'),
            }
        """
        @ivar: the basic types of the C{MessageAST} object
        @type: dictionary
        """
        self.__ultimateElement = None
        """
        @ivar: the ultimate element of the C{MessageAST} object
        @type: C{ASTElement}
        """
        self.logger = logging.Logger('MessageAST')
        """
        @ivar: the logger attached to the C{MessageAST} object
        @type: C{Logger} (the C{Logger} class is defined in the C{logging} Python module)
        """
        self.logger.setLevel(logging.ERROR)
        self.logger.addHandler(stdoutHandler)

    def hasPackage(self):
        """
        Tells if the C{MessageAST} object has got a package or not
        @return: TRUE if the C{MessageAST} has got a package and FALSE if not
        """
        return self.__package != None

    def __getPackage(self):
        """
        Getter for __package
        @return: the package of the C{MessageAST} object
        """
        return self.__package

    def __setPackage(self, package):
        """
        Setter for __package
        @param package: the new package for this C{MessageAST} object
        @type package: C{Package}

        """
        self.__package = package

    # pythonic getter/setter using properties

    package = property(fget=__getPackage, fset=__setPackage, fdel=None,
                         doc='The package which the generated message will belong to.This will be used by the specific AST generator')

    def hasVersion(self):
        """
        Tells if the C{MessageAST} object has got a version or not
        @return: TRUE if the C{MessageAST} has got a version and FALSE if not
        """
        return self.__version != None

    def __getVersion(self):
        """
        Getter for __version
        @return: the version of the C{MessageAST} object
        """
        return self.__version

    def __setVersion(self, version):
        """
        Setter for __version
        @param version: the new version of the C{MessageAST} object
        @type version: C{Version}
        """
        self.__version = version

    # pythonic getter/setter using properties

    version = property(fget=__getVersion, fset=__setVersion, fdel=None,
                       doc='The version of the C{MessageAST} object')

    def hasFactory(self):
        """
        Tells if the C{MessageAST} object has got a factory or not
        @return: TRUE if the C{MessageAST} has got a factory and FALSE if not
        """
        return self.__factory != None

    def __getFactory(self):
        """
        Getter for __factory
        @return: the factory of the C{MessageAST} object
        """
        return self.__factory

    def __setFactory(self, factory):
        """
        Setter for factory
        @param factory: the new factory of the C{MessageAST} object
        @type factory: C{Factory}
        """
        self.__factory = factory

    # pythonic getter/setter using properties

    factory = property(fget=__getFactory, fset=__setFactory, fdel=None,
                       doc='The factory of the C{MessageAST} object')

    def __getNativeTypes(self):
        """
        Getter for __nativeTypes
        @return: All the native types in the message spécification
        """
        return self.__nativeTypes

    # pythonic getter/setter using properties

    natives = property(fget=__getNativeTypes, fset=None, fdel=None,
                       doc='the set of native types described in this C{MessageAST}')

    def __getMessageTypes(self):
        """
        Getter for __messageTypes
        @return: All the message types in the message specification
        """
        return self.__messageTypes

    # pythonic getter/setter using properties

    messages = property(fget=__getMessageTypes, fset=None, fdel=None,
                        doc='the set of message types described in this C{MessageAST}')

    def __getEnumTypes(self):
        """
        Getter for __enumTypes
        @return: All the enum types in the message specification
        """
        return self.__enumTypes

    # pythonic getter/setter using properties

    enums = property(fget=__getEnumTypes, fset=None, fdel=None,
                     doc='the set of enum types described in this C{MessageAST}')

    def getRootMergeType(self, msg, verbose=0):
        """ 
        Return the root merge type of a message.
    
        The idea is to find the root of the merge chain of
        the provided message. 
        @param msg: the message for which we want to know the root merge type
        @type msg: Any -> more precisely either C{NativeType} or C{MessageType} however
                  only C{MessageType} may lead to a real search of the root.
        @return: the C{MessageType} root merge type or msg if msg wasn't an instance of C{MessageType}
        """

        retval = None

        # msg may be a simple string not a type    ....

        if isinstance(msg, type('')):
            current = self.getType(msg)
        else:
            current = msg

        while retval == None:
            if verbose:
                print 'current = %s, retval = %s' % (current, retval)
                print 'type(current)=', type(current)
            if isinstance(current, MessageType):
                if current.hasMerge():
                    current = self.getType(current.merge)
                else:
                    retval = current
            else:
                retval = current
        return retval

    def add(self, any):
        """ 
        Add an ASTElement to the AST.
        
        The parser will call this method as soons as it has
        built the appropriate ASTElement sub-class.
        
        @param any: the object to be added to the tree
        @type any: some sub-class of C{ASTElement}, see: G{classtree ASTElement}
          
        """

        if any == None:
            self.logger.error("<None> given to AST some rule aren't finished"
                              )
        else:
            self.logger.debug('Add %s %s' % (type(any).__name__,
                              any.name))

            # Typename must be unique (but the package name)
            if not isinstance(any, Package) and self.isDefined(any.name):
                self.logger.error('%s already defined in the AST'
                                  % any.name)
                self.logger.error(' --> Check lines (%d,%d) '
                                  % any.linespan + 'and (%d,%d)'
                                  % self.getType(any.name).linespan
                                  + ' of <%s>' % self.name)
            elif isinstance(any, EnumType):
                self.addEnumType(any)
            elif isinstance(any, NativeType):
                self.addNativeType(any)
            elif isinstance(any, MessageType):
                self.addMessageType(any)
            elif isinstance(any, Package):
                self.package = any
            elif isinstance(any, Version):
                self.version = any
            elif isinstance(any, Factory):
                self.factory = any
            elif isinstance(any, CommentBlock):

            # Handle comment block preceding other AST element

                if self.__ultimateElement != None:

                    # attach the comment block to the preceding
                    # AST element (recursion is backtracking)
                    # The list of comment line should be reversed

                    any.lines.reverse()
                    self.__ultimateElement.comment = any
                else:
                    pass
            else:
                self.logger.error('<%s> not handle [yet]' % any)
            self.__ultimateElement = any

    def addMessageType(self, message):
        """
        Add a message type to the AST.
        
        @param message: The message type to be added
        @type message: C{MessageType}  
        """

        self.__messageTypes.append(message)
        self.__types[message.name] = message

    def addEnumType(self, enumType):
        """
        Add an enum type to the AST.
        
        @param enumType: The enum type to be added
        @type enumType: C{EnumType}  
        """

        self.__enumTypes.append(enumType)
        self.__types[enumType.name] = enumType

    def addNativeType(self, native):
        """
        Add a native type to the AST.
        
        @param native: The message type to be added
        @type native: C{NativeType}  
        """

        self.__nativeTypes.append(native)
        self.__types[native.name] = native

    def isDefined(self, typename):
        """
        Return true if the typename is know in this AST.
        
        @param typename: the name of the type
        @type typename: C{string}  
        """

        return self.getType(typename) != None

    def getType(self, typename):
        """
        Get the known type in the AST corresponding to a given typename. The returned type may be None if the given 
        typename is not found in the AST.

        @param typename: the typename for which we want to know the type
        @type typename: C{string}
        @return: the known type in the AST if found
        """
        if isinstance(typename, type('')):
            if typename in self.__types.keys():
                return self.__types[typename]
            else:
                return None
        else:
            return typename

    def __repr__(self):
        """
        Gives a representation of a C{MessageAST} object
        @return: the representation of the C{MessageAST} object
        """
        res = \
            'AST with <%d> native type(s), <%d> enum, <%d> message type(s)' \
            % (len(self.natives), len(self.enums), len(self.messages))
        if self.hasFactory():
            res = res + ' and factory <%s> ' % self.factory.name
        if self.hasPackage():
            res = res + ' in package <%s>' % self.package
        return res


class CommentBlock(ASTElement):

    """
    Represents a block of comment
    
    A C{CommentBlock} has lines which is a list of C{string}.
 
    """

    def __init__(self, content, isAtEOL):
        """
        The class constructor

        @param content: the content of the C{CommentBock} object
        @type content: C{string}
        @param isAtEOL: this boolean tells if the comment block is at the end of the line or not. If isAtEOL is TRUE that means 
        that the comment block is optional.
        @type isAtEOL: C{boolean}
        """

        super(CommentBlock, self).__init__(name='ANY Comment Block')
        self.lines = [content]
        """
        @ivar: the lines of the C{CommentBlock}
        @type: a C{set} of C{string}
        """
        self.__isAtEOL = isAtEOL
        """
        @ivar: this boolean tells if the comment block is at the end of the line or not. If isAtEOL is TRUE that means 
        that the comment block is optional.
        @type: C{boolean}
        """

    def __getisAtEOL(self):
        """
        Getter for __isAtEOL
        @return: TRUE if the comment block is at the end of the line (e.g. is optional)
        """
        return self.__isAtEOL

    # pythonic getter/setter using properties

    isAtEOL = property(fget=__getisAtEOL, fset=None, fdel=None,
                       doc='True if the comment is optional')


class Package(ASTElement):

    """
    Represents a package.
    
    A C{Package} is a simple C{ASTElement} whose
    name is a C{string} containing a
    dot-separated IDs like: "fr.onera.certi"
    """

    def __init__(self, name):
        """
        The class constructor
        @param name: the name we want to give to the C{Package} object
        @type name: C{string}
        """
        super(Package, self).__init__(name)
        self.shortname = MessageAST.shortenName(self.name)

    def __repr__(self):
        """
        Gives a representation of a C{Package} object
        @return: the representation of the C{Package} object
        """
        return 'package %s' % self.name

class Version(ASTElement):

    """
    Represents a version number.
    
    A C{Version} is a simple C{ASTElement} whose
    name is a C{string}.
    """

    def __init__(self, number):
        """
        The class constructor
        @param number: the number we want to give to the C{Version} object

        """
        super(Version, self).__init__(name='Version')
        self.number = number
        """
        @ivar: the number of the version
        @type: a C{set} of two C{integer} values
        """

    def __repr__(self):
        """
        Gives a representation of a C{Version} object
        @return: the representation of the C{Version} object
        """
        return 'version %d.%d' % self.number


class Factory(ASTElement):

    """
    Represents a factory.
    
    A C{Factory} is a C{ASTElement} whose
    name is a C{string}.
    """

    def __init__(self,name,creator,receiver=None):
        """
        The class constructor

        @param name: the name we want to give to the factory
        @type name: C{string}
        @param creator: the creator of the factory
        @param receiver: the receiver of the factory
        """
        super(Factory, self).__init__(name)
        self.creator = creator
        """
        @ivar: the creator of the factory
        @type: a C{set} of three C{string}
        """
        self.receiver = receiver
        """
        @ivar: the receiver of the factory
        @type: a C{set} of three C{string}
        """

    def hasFactoryCreator(self):
        """
        Tells if the factory has got a factory creator or not
        @return: TRUE if the factory has got a factory creator and FALSE if not
        """
        return self.creator != None

    def hasFactoryReceiver(self):
        """
        Tells if the factory has got a factory creator or not
        @return: TRUE if the factory has got a factory creator and FALSE if not
        """
        return self.receiver != None

    def __repr__(self):
        """
        Gives a representation of a C{Factory} object
        @return: the representation of the C{Factory} object
        """
        res = 'factory %s' % self.name
        res = res + ', creator = %s %s(%s)' % creator
        res = res + ', receiver = %s %s(%s)' % receiver
        return res


class NativeType(ASTElement):

    """ 
    Represents a native message type.
    
    A C{NativeType} is a simple C{ASTElement} whose
    name is the name the native type.
    """

    def __init__(self, name, lines):
        """
        The class constructor
        @param name: the name we want to give to the C{NativeType} object
        @type name: C{string}
        @param lines: the language or representation lines that we want to add to the native type
        """
        super(NativeType, self).__init__(name=name)

        # store language line list in a dictionary
        # in order to ease retrieval
        self.languages = dict()
        """
        @ivar: the language line of the C{NativeType} object
        @type: a C{set} of C{LanuageLine}
        """
        self.representation = None
        """
        @ivar: the representation of the C{NativeType} object. It can be either 'combine' or a basic type with a qualifier.
        @type: C{string} 
        """
        for l in lines:
            # Native line l is a language line
            if isinstance(l, NativeType.LanguageLine):
                if l.name in self.languages.keys():
                    self.languages[l.name].append(l)
                else:
                    self.languages[l.name] = list()
                    self.languages[l.name].append(l)
            # Native line l is a representation line
            else:
                self.representation = l
        # we will keep track of the number of Heir of the native types
        self.nbHeir = 0

    def __repr__(self):
        """
        Gives a representation of a C{NativeType} object
        @return: the representation of the C{NativeType} object
        """
        return 'native %s' % self.name

    def hasLanguage(self, language):
        """
        Tells if a given language is in the available languages
        @param language: the language for which we want to know if it is available
        @type language: C{string}
        @return: TRUE if the given language is available and FALSE if not
        """
        return language in self.languages.keys()

    def getLanguageLines(self, language):
        """
        Get the language lines corresponding to a given language
        @param language: the language for which we want to get the language lines
        @type language: C{string}
        @return: the language lines corresponding to the given language
        """
        if language in self.languages.keys():
            return self.languages[language]

    def hasRepresentation(self):
        """
        Tells if the C{NativeType} object has got a representation or not
        @return: TRUE if the C{NativeType} object has got a representation and FALSE if not
        """
        return self.representation != None

    def getRepresentation(self):
        """
        Get the representation of a C{NativeType} object
        @return: the representation of the C{NativeType} object
        """
        return self.representation

    def hasHeir(self):
        """
        Tells if a C{NativeType} object has got at least one heir or not
        @return: TRUE if the C{NativeType} object has got at least  one heir and FALSE if not
        """
        return self.nbHeir > 0


    class LanguageLine(ASTElement):
        """ 
        Represents a Language Line Value
        """

        def __init__(self, name, value):
            """
            The class constructor
            @param name: the name we want to give to the language lines
            @type name: C{string}
            @param value: the value of the language line, e.g. what specific code has to be generated by the corresponding
            backend
            @type value: C{string}
            """
            super(NativeType.LanguageLine, self).__init__(name=name)
            self.statement = value.strip('[]')
            """
            @ivar: the statement of the language line, e.g. what specific code has to be generated by the corresponding
            backend
            @type: C{string}
            """


    class RepresentationLine(ASTElement):
        """ 
        Represents a Representation Line Value
        """

        def __init__(self, value, qualifier):
            """
            The class constructor
            @param value: the value of the representation line, e.g. how a field of this native type has to be serialized 
            and deserialized. The value can be either 'combine' or a basic type.
            @param qualifier: this is the usual type qualifier (required, repeated or optional) in fact only repeated 
            is currently used.
            """
            super(NativeType.RepresentationLine,self).__init__(name='representation')
            self.representation = value
            """
            @ivar: the representation that has to be used, e.g. how a field of this native type has to be serialized 
            and deserialized. The representation can be either 'combine' or a basic type.
            @type: C{string}
            """
            self.qualifier      = qualifier

        def hasQualifier(self):
            """
            Tells if the C{RepresentationLine} object has got a qualifier or not
            @return: TRUE if the C{RepresentationLine} object has got a qualifier and FALSE if not
            """
            return self.qualifier != None

class MessageType(ASTElement):
    """ 
    Represents a message type.
    
    """

    def __init__(
        self,
        name,
        fields,
        merge,
        ):
        """
        The class constructor
        @param name: the name we want to give to the C{MessageType} object
        @type name: C{string}
        @param merge: the merger of this C{MessageType}
        @type merge: a C{MessageType}  
        @param fields: the fields we want to describe in the C{MessageType} object
        @type fields:  C{list} of C{MessageType.MessageField}
        """
        super(MessageType, self).__init__(name=name)
        self.fields = fields
        """
        @ivar: the fields of this C{MessageType}
        @type: C{list} of C{MessageType.MessageField}
        """
        self.merge = merge
        """ 
        @ivar: the merger of this C{MessageType}
        @type: a C{MessageType}  
        """

        self.enum = None
        """
        @ivar: the enum in which this C{MessageType} is stored. May be None if this C{MessageType} doesn't merge another type
        """
        self.nbHeir = 0 
        """ 
        @ivar: the number of heir of this C{MessageType} object
        @type: C{int}
        """

    def __repr__(self):
        """
        Gives a representation of a C{MessageType} object
        @return: the representation of the C{MessageType} object
        """
        res = 'message %s ' % self.name
        return res

    def hasMerge(self):
        """
        Tells if the C{MessageType} merges another type or not
        @return: TRUE if the C{MessageType} merges another type and FALSE if not
        """
        return self.merge != None

    def hasEnum(self):
        """
        Tells if the C{MessageType} is stored in an enum or not.
        @return: TRUE if the C{MessageType} is stored in an enum and FALSE if not.
        """
        return self.enum != None

    def hasHeir(self):
        """
        Tells if the C{MessageType} has got at least one heir or not
        @return: TRUE if the C{MessageType} has got at least one heir and FALSE if not.
        """
        return self.nbHeir > 0


    class CombinedField(ASTElement):
        """
        Represents a combined field
        """

        def __init__(self, typeid, fields):
            """
            The class constructor
            @param typeid: the type we want for the combined field
            @param fields: the field we want for the combined field
            """
            super(MessageType.CombinedField,
                  self).__init__(name='Combined')
            self.typeid = typeid
            """
            @ivar: the type of the combined field
            @type: a C{NativeType}
            """
            self.fields = fields
            """
            @ivar: the fields of the combined field
            @type: a C{set} of C{MessageField}
            """


    class MessageField(ASTElement):
        """ 
        Represents a message field            
        """

        def __init__(
            self,
            qualifier,
            typeid,
            name,
            defaultValue=None,
            ):
            """
            The class constructor

            @param qualifier: the qualifier (required/repeated/optional) we want for the field
            @type qualifier: C{string}
            @param typeid: the type we want for the field. It may be a basic type or a defined type.
            @type typeid: C{string}
            @param name: the name we want for the field
            @type name: C{string}
            @param defaultValue: the default value we want for the field. May be None.
            @type defaultValue: depends on the type of the field
            """
            super(MessageType.MessageField, self).__init__(name=name)
            self.qualifier = qualifier
            """
            @ivar: the qualifier of the C{MessageField}
            @type: C{string}
            """
            self.typeid = typeid
            """
            @ivar: the type of the C{MessageField}
            @type: C{string}
            """
            self.defaultValue = defaultValue
            """
            @ivar: the default value of the C{MessageField}
            @type: the same type as the C{MessageField}
            """

        def hasDefaultValue(self):
            """
            Tells if the C{MessageField} has got a default value or not
            @return: TRUE if the C{MessageField} has got a default value and FALSE if not
            """
            return self.defaultValue != None


class EnumType(ASTElement):
    """ 
    Represents an enum type 
    """

    def __init__(self, name, values):
        """
        The class constructor

        @param name: the name we want for the enums
        @type name: C{string}
        @param values: the values we want for the enum
        @type values: a C{set} of enum values (e.g. value-key pairs)
        """
        super(EnumType, self).__init__(name=name)

        # rebuild dictionary with value from the list

        self.values = []
        """
        @ivar: the values of the enums
        @type: a C{set} of enum values (e.g. value-key pairs)
        """
        lastval = -1
        for val in values:
            if val.value == None:
                val.value = lastval + 1
                self.values.append(val)
                lastval += 1
            else:
                self.values.append(val)
                lastval = val.value

    def __repr__(self):
        """
        Gives a representation of a C{EnumType} object
        @return: the representation of the C{EnumType} object
        """
        res = 'Enum %s {\n' % self.name
        for val in self.values:
            res = res + '  ' + str(val.name) + ' = ' + str(val.value) \
                + ', \n'
        res = res + '}'
        return res
    
    def addNamePrefix(self,namePrefix):
        """
        Add a name prefix to every enumerated values
        """
        for val in self.values:
            val.name = namePrefix+val.name

    class EnumValue(ASTElement):
        """ 
        Represents an Enum Value
        """

        def __init__(self, name, value):
            """
            The class constructor

            @param name: the name we want for the C{EnumValue}
            @type name: C{string}
            @param value: the value we want for the C{EnumValue}
            @type value: C{int}
            """
            super(EnumType.EnumValue, self).__init__(name=name)
            self.value = value
            """
            @ivar: the value of the C{EnumValue} object
            @type: C{int}
            """


class ASTChecker(object):
    """
    The Purpose of this class is to check AST properties. 
    """

    def __init__(self):
        """
        The class constructor
        """
        self.logger = logging.Logger('ASTChecker')
        """
        @ivar: The logger attached to the C{ASTChecker} object
        @type: C{Logger} (this class is defined in the Python module C{logging})
        """
        self.logger.setLevel(logging.ERROR)
        self.logger.addHandler(stdoutHandler)

    def checkMessageFields(self, msg, AST):
        """
        Checks the fields of a message

        @param msg: the message for which we want to check the fields
        @type msg: C{MessageType}
        @param AST: the AST that contains the given message
        @type AST: C{MessageAST}
        """
        for f in msg.fields:
            if not AST.isDefined(f.typeid):
                self.logger.fatal('The type <%s> used for field <%s.%s> is unknown (not a builtin, nor native, nor message)'
                                   % (f.typeid, msg.name, f.name))
                self.logger.fatal(' --> Check lines (%d,%d)'
                                  % f.linespan + ' of <%s>' % AST.name)
                return False
            else:
                if isinstance(f, MessageType.MessageField):
                    f.typeid = AST.getType(f.typeid)
                elif isinstance(f, MessageType.CombinedField):
                    if not self.checkMessageFields(f, AST):
                        return False
                    else:
                        f.typeid = AST.getType(f.typeid)
                else:
                    self.logger.fatal('Unknown MessageField type %s'
                            % f.str())
                    return False
        return True

    def check(self, AST):
        """
        Checks the AST.
        
        @param AST: the AST to be checked
        @type AST: C{MessageAST}  
        """

        # Consider the AST not checked.

        AST.checked = False

        # check if the supplied object has appropriate super type
        # @todo: note that we may just require to have the appropriate
        #        fields and not being sub-class of MesssageAST.
        #        this could be done with introspection.
        #        see: http://docs.python.org/library/inspect.html
        if not isinstance(AST, MessageAST):
            self.logger.error('The supplied object is not an instance of MessageAST: <%s>'
                               % type(AST))
            return

        # check if all field used in message have known types
        # At the same time build the enum values for MessageTypes
        enumval = EnumType.EnumValue('NOT_USED', None)
        enumval.type = None
        msgTypeEnumVals = [enumval]
        lastMerge = None
        for msg in AST.messages:
            # We do not generate the enum factory entry for a message
            # with no merge there is no possible factory for that
            # kind of message.
            # However some message types may merge from one another
            # as soon as there is a "common" root merge type
            if msg.hasMerge():
                parent = AST.getType(msg.merge)
                parent.nbHeir += 1
                if None != lastMerge:
                    # recurse to find root merge
                    rootMerge = AST.getRootMergeType(msg.merge)
                    if lastMerge != rootMerge:
                        blah = AST.getRootMergeType(msg.merge, 1)
                        self.logger.error('Error: there is more than one root merged type (%s != %s (root of %s)). You should use one root merged type only'
                                 % (lastMerge, rootMerge, msg.merge))
                        self.logger.fatal(' --> Check lines (%d,%d)'
                                % msg.linespan + ' of <%s>' % AST.name)
                        return
                else:
                    lastMerge = AST.getRootMergeType(msg.merge)

                enumval = EnumType.EnumValue(msg.name.upper(), None)
                enumval.type = msg.name
                msgTypeEnumVals.append(enumval)
            if not self.checkMessageFields(msg, AST):
                return

        # only add merger type in the enumeration if the
        # type is not native
        if (not isinstance(lastMerge, NativeType)):
            enumval = EnumType.EnumValue(lastMerge.name.upper(), None)
            enumval.type = lastMerge.name
            msgTypeEnumVals.append(enumval)
        enumval = EnumType.EnumValue('LAST', None)
        enumval.type = None
        msgTypeEnumVals.append(enumval)
        AST.eMessageType = EnumType('MessageType', msgTypeEnumVals)
        if lastMerge != None:
            mergeClass = AST.getType(lastMerge)
            if isinstance(mergeClass, MessageType):
                mergeClass.enum = AST.eMessageType

        # @todo
        # Should check if the default value of a field
        # has the appropriate type (builtin types)
        # and that field with defined type have NO
        # default value

        # check if merger are either native or message
        # @todo should check that merger is not an enum
        for msg in AST.messages:
            if msg.hasMerge():
                if not AST.isDefined(msg.merge):
                    self.logger.fatal('The merge target <%s> of message <%s> is unknown (not a builtin, nor native, nor message)'
                             % (msg.merge, msg.name))
                    self.logger.fatal(' --> Check lines (%d,%d)'
                            % msg.linespan + ' of <%s>' % AST.name)
                    return
                else:
                    msg.merge = AST.getType(msg.merge)

        # check the factory methods if any
        if AST.factory != None:
            if AST.factory.hasFactoryCreator():
                if not AST.isDefined(AST.factory.creator[0]):
                    self.logger.fatal('The return type <%s> of the creator factory method is unknown (not a builtin, nor native, nor message)'
                                       % AST.factory.creator[0])
                    self.logger.fatal(' --> Check lines (%d,%d)'
                                      % AST.factory.linespan + ' of <%s>'
                                      % AST.name)
                    return
                if not AST.isDefined(AST.factory.creator[2]):
                    self.logger.fatal('The parameter type <%s> of the creator factory method is unknown (not a builtin, nor native, nor message)'
                                       % AST.factory.creator[2])
                    self.logger.fatal(' --> Check lines (%d,%d)'
                                      % AST.factory.linespan + ' of <%s>'
                                      % AST.name)
                    return
    
            if AST.factory.hasFactoryReceiver():
                if not AST.isDefined(AST.factory.receiver[0]):
                    self.logger.fatal('The return type <%s> of the receiver factory method is unknown (not a builtin, nor native, nor message)'
                                       % AST.factory.receiver[0])
                    self.logger.fatal(' --> Check lines (%d,%d)'
                                      % AST.factory.linespan + ' of <%s>'
                                      % AST.name)
                    return
                if not AST.isDefined(AST.factory.receiver[2]):
                    self.logger.fatal('The parameter type <%s> of the receiver factory method is unknown (not a builtin, nor native, nor message)'
                                       % AST.factory.receiver[2])
                    self.logger.fatal(' --> Check lines (%d,%d)'
                                      % AST.factory.linespan + ' of <%s>'
                                      % AST.name)
                    return

        # Build short name
        if AST.hasPackage():
            AST.shortname = AST.package.shortname
        else:
            AST.shortname = AST.shortenName(AST.name)
        # Now the AST has been checked successfully

        AST.checked = True

