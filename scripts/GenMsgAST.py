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
## $Id: GenMsgAST.py,v 1.6 2010/03/14 15:35:54 gotthardp Exp $
## ----------------------------------------------------------------------------

"""
The CERTI Message Generator.
Generator AST classes.
"""

import os
import getopt, sys
import datetime
import logging
# Build some logger related objects
stdoutHandler = logging.StreamHandler(sys.stdout)
                                      
class ASTElement(object):
    """
    The base class for all Abstract Syntax Tree element.

    @param name: the name of the element
    @type name: C{string}
    @param comment: the comment attached to this element.
                    may be C{None}
    @type comment: C{CommentBlock}   
    """
    
    def __init__(self,name):        
        self.__name     = name
        self.__comment  = None
        self.__linespan = None
        self.logger = logging.Logger("ASTElement")
        self.logger.setLevel(logging.ERROR)
        self.logger.addHandler(stdoutHandler)
    
    def __getName(self):
        return self.__name    
    def __setName(self,name):        
        self.__name = name
    # pythonic getter/setter using properties    
    name = property(fget=__getName,fset=__setName,fdel=None, doc="The name of the C{ASTElement}")    
    
    def hasComment(self):
        return self.__comment!=None
    
    def __getComment(self):
        return self.__comment    
    def __setComment(self,comment):
        if isinstance(comment,type("")):
            pass
        else:
            self.logger.info("Adding comment %s to element %s" % (comment.lines,self.name))
            self.__comment = comment
    # pythonic getter/setter using properties    
    comment = property(fget=__getComment,fset=__setComment,fdel=None, doc="The comment block attached to the C{ASTElement}")
    
    def __getLinespan(self):
        return self.__linespan    
    def __setLinespan(self,linespan):        
        self.__linespan = linespan
    # pythonic getter/setter using properties    
    linespan = property(fget=__getLinespan,fset=__setLinespan,fdel=None, doc="The line span of this C{ASTElement} in the original file")    
    
class MessageAST(ASTElement):
    """
    Message Abstract Syntax Tree root class.
    
    This class represents the abstraction of the message description
    language including all the features represented by corresponding
    class field. Object instance of C{MessageAST} may be used by a 
    language generator in order to generate code for the target language.
    
    @param package: the package which the generated message will belong
                    this will be used by the specific AST generator
    @type package:  C{Package}                     
    @param natives: the set of native types described in this
                          C{MessageAST}
    @type natives: C{set} of C{NativeType}
    @param messages: the set of messages described in this C{MessageAST}
    @type messages: C{set} of C{MessageType}          
    
    """
    def __init__(self,name):
        super(MessageAST,self).__init__(name=name)
        self.__package            = None
        self.__version            = None
        self.__factory            = None
        self.__nativeTypes        = []
        self.__messageTypes       = []
        self.__enumTypes          = []
        # The types dictionary is initialized with builtin types
        self.__types                = {'onoff' : ASTElement("onoff"),
                                       'bool' : ASTElement("bool"),
                                       'string' : ASTElement("string"),
                                       'byte' : ASTElement("byte"),
                                       'int8' : ASTElement("int8"),
                                       'uint8' : ASTElement("uint8"),
                                       'int16' : ASTElement("int16"),
                                       'uint16' : ASTElement("uint16"),
                                       'int32' : ASTElement("int32"),
                                       'uint32' : ASTElement("uint32"),   
                                       'int64' : ASTElement("int64"),
                                       'uint64' : ASTElement("uint64"),
                                       'float' : ASTElement("float"),
                                       'double' : ASTElement("double")}
        self.__ultimateElement      = None                
        self.logger = logging.Logger("MessageAST")
        self.logger.setLevel(logging.ERROR)
        self.logger.addHandler(stdoutHandler)
        
    def hasPackage(self):
        return self.__package != None
    def __getPackage(self):
        return self.__package
    def __setPackage(self,package):
        self.__package = package
    # pythonic getter/setter using properties   
    package = property(fget=__getPackage,fset=__setPackage,fdel=None,doc=None)

    def hasVersion(self):
        return self.__version != None
    def __getVersion(self):
        return self.__version
    def __setVersion(self,version):
        self.__version = version
    # pythonic getter/setter using properties   
    version = property(fget=__getVersion,fset=__setVersion,fdel=None,doc=None)

    def hasFactory(self):
        return self.__factory != None
    def __getFactory(self):
        return self.__factory
    def __setFactory(self,factory):
        self.__factory = factory
    # pythonic getter/setter using properties   
    factory = property(fget=__getFactory,fset=__setFactory,fdel=None,doc=None)
        
    def __getNativeTypes(self):
        return self.__nativeTypes  
    # pythonic getter/setter using properties   
    natives = property(fget=__getNativeTypes,fset=None,fdel=None,doc=None)
    
    def __getMessageTypes(self):
        return self.__messageTypes  
    # pythonic getter/setter using properties      
    messages = property(fget=__getMessageTypes,fset=None,fdel=None,doc=None)
    
    def __getEnumTypes(self):
        return self.__enumTypes
    # pythonic getter/setter using properties   
    enums = property(fget=__getEnumTypes,fset=None,fdel=None,doc=None)         
        
        
    def add(self,any):
        """ 
        Add an ASTElement to the AST.
        
        The parser will call this method as soons as it has
        built the appropriate ASTElement sub-class.
        
        @param any: the object to be added to the tree
        @type any: some sub-class of C{ASTElement}, see: G{ASTElement}
          
        """
        if any == None:
            self.logger.error("<None> given to AST some rule aren't finished")    
        else:                     
            self.logger.debug("Add %s %s" % (type(any).__name__,any.name))
            # Typename must be unique
            if self.isDefined(any.name):                
                self.logger.error("%s already defined in the AST" % any.name)
                self.logger.error(" --> Check lines (%d,%d) " % any.linespan + "and (%d,%d)" % self.getType(any.name).linespan+ " of <%s>" % self.name)                          
            elif isinstance(any,EnumType):            
                self.addEnumType(any)                            
            elif isinstance(any,NativeType):
                self.addNativeType(any)
            elif isinstance(any,MessageType):
                self.addMessageType(any)
            elif isinstance(any,Package):
                self.package = any
            elif isinstance(any,Version):
                self.version = any
            elif isinstance(any,Factory):
                self.factory = any
            # Handle comment block preceding other AST element
            elif isinstance(any,CommentBlock):                
                if self.__ultimateElement != None:
                    # attach the comment block to the preceding
                    # AST element (recursion is backtracking)
                    # The list of comment line should be reversed
                    any.lines.reverse()
                    self.__ultimateElement.comment = any
                else:
                    pass               
            else:            
                self.logger.error("<%s> not handle [yet]" % any)                   
            self.__ultimateElement    = any            
            
    def addMessageType(self,message): 
        """
        Add a message type to the AST.
        
        @param message: The message type to be added
        @type message: C{MessageType}  
        """       
        self.__messageTypes.append(message)
        self.__types[message.name] = message
        
    def addEnumType(self,enumType):
        """
        Add an enum type to the AST.
        
        @param enumType: The enum type to be added
        @type enumType: C{EnumType}  
        """        
        self.__enumTypes.append(enumType)
        self.__types[enumType.name] = enumType               
    
    def addNativeType(self,native):
        """
        Add a native type to the AST.
        
        @param native: The message type to be added
        @type native: C{NativeType}  
        """
        self.__nativeTypes.append(native)
        self.__types[native.name] = native
        
    def isDefined(self,typename):
        """
        Return true if the typename is know in this AST.
        
        @param typename: the name of the type
        @type typename: C{string}  
        """
        return self.getType(typename)!=None        
    
    def getType(self,typename):
        if isinstance(typename,type("")):
            if typename in self.__types.keys():
                return self.__types[typename]
            else:
                return None
        else:
            return typename        
    
    def __repr__(self):
        res = "AST with <%d> native type(s), <%d> enum, <%d> message type(s)" % (len(self.natives),len(self.enums),len(self.messages))
        if (self.hasFactory()):
            res = res + " and factory <%s> "% self.factory.name
        if (self.hasPackage()):
            res = res + " in package <%s>" % self.package
        return res    
    
class CommentBlock(ASTElement):
    """
    Represents a block of comment
    
    A C{CommentBlock} has lines which is a list of C{string}.
    @param lines: the comments lines
    @type lines: C{list} of C{string}  
    """
    
    def __init__(self,content,isAtEOL):
        """
        C{CommentBlock} constructor
        """
        super(CommentBlock,self).__init__(name="ANY Comment Block")
        self.lines=[content]
        self.__isAtEOL=isAtEOL
        
    def __getisAtEOL(self):
        return self.__isAtEOL
    # pythonic getter/setter using properties   
    isAtEOL = property(fget=__getisAtEOL,fset=None,fdel=None,doc="True if the comment is optional")   
    
class Package(ASTElement):
    """
    Represents a package.
    
    A C{Package} is a simple C{ASTElement} whose
    name is a C{string} containing a
    dot-separated IDs like: "fr.onera.certi"
    """
    def __init__(self,name):        
        super(Package,self).__init__(name)                        
          
    def __repr__(self):
        return "package %s" % self.name

class Version(ASTElement):
    """
    Represents a version number.
    
    A C{Version} is a simple C{ASTElement} whose
    name is a C{string}.
    """
    def __init__(self,number):
        super(Version,self).__init__(name="Version")
        self.number = number
          
    def __repr__(self):
        return "version %d.%d" % self.number

class Factory(ASTElement):
    """
    Represents a factory.
    
    A C{Factory} is anC{ASTElement} whose
    name is a C{string}.
    """
    def __init__(self,name,creator,receiver):        
        super(Factory,self).__init__(name)
        self.creator  = creator
        self.receiver = receiver
          
    def __repr__(self):
        res="factory %s" % self.name
        res=res+ ", creator = %s %s(%s)" % creator
        res=res+ ", receiver = %s %s(%s)" % receiver
        return res    
     
class NativeType(ASTElement):
    """ 
    Represents a native message type.
    
    A C{NaptiveType} is a simple C{ASTElement} whose
    name is the name the native type.
    """
    
    def __init__(self,name,lines):
        super(NativeType,self).__init__(name=name)
        # store language line list in a dictionnary
        # in order to ease retrieval
        self.languages = dict()
        self.representation = None
        for l in lines:
            if isinstance(l,NativeType.LanguageLine):
                if l.name in self.languages.keys():
                    self.languages[l.name].append(l)
                else:
                    self.languages[l.name] = list()
                    self.languages[l.name].append(l)
            else:
                self.representation = l.representation
        
    def __repr__(self):
        return "native %s" % self.name
    
    def hasLanguage(self,language):
        return (language in self.languages.keys())
    
    def getLanguageLines(self,language):
        if language in self.languages.keys():
            return self.languages[language]
        
    def hasRepresentation(self):
        return self.representation != None

    def getRepresentation(self):
        return self.representation
     
    class LanguageLine(ASTElement):
        """ Represents a Language Line Value
        """
        def __init__(self,name,value):
            super(NativeType.LanguageLine,self).__init__(name=name)    
            self.statement = value.strip("[]")                

    class RepresentationLine(ASTElement):
        """ Represents a Representation Line Value
        """
        def __init__(self,value):
            super(NativeType.RepresentationLine,self).__init__(name='representation')
            self.representation = value
                    
class MessageType(ASTElement):
    """ 
    Represents a message type.
    
    @param fields: the fields of this C{MessageType}
    @type fields: C{list} of C{MessageType.MessageField}
    @param merge: the merger of this C{MessageType}
    @type merge: a C{MessageType}  
    @param combine: a combined set of fields
    """
    def __init__(self,name,fields,merge):
        super(MessageType,self).__init__(name=name)
        self.fields        = fields
        self.merge         = merge                 
    
    def __repr__(self):
        res = "message %s " % self.name
        return res
        
    def hasMerge(self):
        return self.merge != None
    
    class CombinedField(ASTElement):
        def __init__(self,typeid,fields):
            super(MessageType.CombinedField,self).__init__(name="Combined")
            self.typeid = typeid
            self.fields = fields            
    
    class MessageField(ASTElement):
        """ Represents a message field            
        """
        def __init__(self,qualifier,typeid,name,defaultValue=None):
            super(MessageType.MessageField,self).__init__(name=name)
            self.qualifier    = qualifier
            """ The field qualifier, which may be
                 - repeated
                 - optional
                 - required 
            """
            self.typeid       = typeid
            """ The type of the field
            """
            self.defaultValue = defaultValue
            """ The default value for this field
            """
            
        def hasDefaultValue(self):
            return self.defaultValue != None
                
class EnumType(ASTElement):
    """ Represents an enum type 
    """
    def __init__(self,name,values):
        super(EnumType,self).__init__(name=name)
        # rebuild dictionary with value from the list                     
        self.values = []
        lastval     = -1        
        for val in values:                                    
            if (val.value==None):
                val.value = lastval+1
                self.values.append(val)
                lastval += 1
            else:
                self.values.append(val)                           
                lastval = val.value
        
    def __repr__(self):
        res = "Enum %s {\n" % self.name
        for val in self.values:            
            res = res + "  " + str(val[0]) + " = " + str(val[1]) + ", \n"
        res = res + "}"
        return res                 
               
    class EnumValue(ASTElement):
        """ Represents an Enum Value
        """
        def __init__(self,name,value):
            super(EnumType.EnumValue,self).__init__(name=name)    
            self.value = value
                
class ASTChecker(object):
    """
    The Purpose of this class is to check AST properties. 
        
    """
    def __init__(self):
        self.logger = logging.Logger("ASTChecker")
        self.logger.setLevel(logging.ERROR)
        self.logger.addHandler(stdoutHandler)      
    
    def checkMessageFields(self,msg,AST):  
        for f in msg.fields:
            if not AST.isDefined(f.typeid):
                self.logger.fatal("The type <%s> used for field <%s.%s> is unknown (not a builtin, nor native, nor message)" % (f.typeid,msg.name,f.name))
                self.logger.fatal(" --> Check lines (%d,%d)" % (f.linespan) + " of <%s>" % AST.name)                
                return
            else:
                if (isinstance(f,MessageType.MessageField)):                   
                    f.typeid = AST.getType(f.typeid)
                elif (isinstance(f,MessageType.CombinedField)):
                    self.checkMessageFields(f,AST)
                else:
                    self.logger.fatal("Unknown MessageField type %s" % f.str())
                    
                
    def check(self,AST):
        """
        Check the AST.
        
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
           self.logger.error("The supplied object is not an instance of MessageAST: <%s>" % type(AST))        
           return
       
        # check if all field used in message have known types
        # At the same time build the enum values for MessageTypes
        enumval = EnumType.EnumValue("NOT_USED",None)
        enumval.type = None
        msgTypeEnumVals = [enumval]
        for msg in AST.messages:
            # We do not generate the enum factory entry for a message
            # with no merge there is no possible factory for that
            # kind of message. 
            if msg.hasMerge():
                enumval      = EnumType.EnumValue(msg.name.upper(),None)
                enumval.type = msg.name
                msgTypeEnumVals.append(enumval)
            self.checkMessageFields(msg,AST)            
        enumval      = EnumType.EnumValue("LAST",None)
        enumval.type = None                                    
        msgTypeEnumVals.append(enumval)
        AST.eMessageType = EnumType(AST.name.split(".")[0]+"_MessageType",msgTypeEnumVals) 
        AST.add(AST.eMessageType)
         

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
                    self.logger.fatal("The merge target <%s> of message <%s> is unknown (not a builtin, nor native, nor message)" % (msg.merge,msg.name))
                    self.logger.fatal(" --> Check lines (%d,%d)" % (msg.linespan) + " of <%s>" % AST.name )                           
                    return
                else:
                    msg.merge = AST.getType(msg.merge)
                    
        # check the factory methods
        if AST.hasFactory():                        
            if not AST.isDefined(AST.factory.creator[0]):
                self.logger.fatal("The return type <%s> of the creator factory method is unknown (not a builtin, nor native, nor message)" % AST.factory.creator[0])
                self.logger.fatal(" --> Check lines (%d,%d)" % (AST.factory.linespan) + " of <%s>" % AST.name )                
                return
            if not AST.isDefined(AST.factory.creator[2]):
                self.logger.fatal("The parameter type <%s> of the creator factory method is unknown (not a builtin, nor native, nor message)" % AST.factory.creator[2])
                self.logger.fatal(" --> Check lines (%d,%d)" % (AST.factory.linespan) + " of <%s>" % AST.name )                
                return
            if not AST.isDefined(AST.factory.receiver[0]):
                self.logger.fatal("The return type <%s> of the receiver factory method is unknown (not a builtin, nor native, nor message)" % AST.factory.receiver[0])
                self.logger.fatal(" --> Check lines (%d,%d)" % (AST.factory.linespan) + " of <%s>" % AST.name )                
                return
            if not AST.isDefined(AST.factory.receiver[2]):
                self.logger.fatal("The parameter type <%s> of the receiver factory method is unknown (not a builtin, nor native, nor message)" % AST.factory.receiver[2])
                self.logger.fatal(" --> Check lines (%d,%d)" % (AST.factory.linespan) + " of <%s>" % AST.name )                
                return
        # Now the AST has been checked successfully
        AST.checked = True                  
                                                                                 