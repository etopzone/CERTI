#!/usr/bin/env python

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
## $Id: GenerateMessages.py,v 1.31 2010/03/04 09:28:30 erk Exp $
## ----------------------------------------------------------------------------

"""
The CERTI Message Generator.
CERTI is an HLA RTI (High Level Architecture RunTime Infrastructure)
which uses in its internal a lot of message based communication.
Those messages:
   should be as compact as possible in terms of memory consumption
   should be easy to implement.

The CERTI developer team did develop a message specification language
inspired from Google Protocol buffer. CERTI message have several
specifities which make Google Protocol buffer unusable for CERTI.

The present module is meant to take a message specification language
input and produce various language specific output (C++, Java, Python).
The generator uses the Python PLY module (http://www.dabeaz.com/ply/)
for parsing the message specification language. The definition
of the language itself should be easy to understand from the PLY
grammar but here comes some introduction:

TBD

"""

import os
import getopt, sys
import datetime
# We use PLY in order to parse CERTI message specification files 
# PLY is there: http://www.dabeaz.com/ply/
import ply.yacc
import ply.lex
# We need some regular expression handling 
import re
# We use logging for ... logging :-)
# see http://docs.python.org/library/logging.html
import logging 


# Build some logger related objects
stdoutHandler = logging.StreamHandler(sys.stdout)
# See formatting there:
# http://docs.python.org/library/datetime.html#strftime-behavior
# http://docs.python.org/library/logging.html#formatter-objects
stdoutHandler.setFormatter(logging.Formatter("%(asctime)s.%(msecs)d-[%(name)s::%(levelname)s] %(message)s","%a %H:%M:%S"))
mainlogger = logging.Logger("Main")
mainlogger.setLevel(logging.ERROR)
mainlogger.addHandler(stdoutHandler)

# FIXME TODO: implement automatic loading of additionnal 
# languages
import GenMsgBase
import GenMsgCXX
import GenMsgPython
import GenMsgJava

generatorBackends = dict()
generatorBackends[GenMsgBase.MsgSpecGenerator.generatorName().lower()] = GenMsgBase.MsgSpecGenerator
generatorBackends[GenMsgCXX.CXXGenerator.generatorName().lower()] = GenMsgCXX.CXXGenerator
generatorBackends[GenMsgPython.PythonGenerator.generatorName().lower()] = GenMsgPython.PythonGenerator
generatorBackends[GenMsgJava.JavaGenerator.generatorName().lower()] = GenMsgJava.JavaGenerator

def usage():
    print "Usage:\n%s --input=<message> [--language=<lang>] [--type=header|body] [--factory-only] [--output=<filename>] [--verbose] [--help]" % os.path.basename(sys.argv[0])
    print "   Supported target languages are:"
    for gene in generatorBackends.values():
        print "       - " + gene.generatorName() 
    
try:
    opts, args = getopt.getopt(sys.argv[1:], "i:l:t:o:vh", ["input=","language=","type=","factory-only","output=","verbose","help"])
except getopt.GetoptError, err:
    mainlogger.error("opt = %s, msg = %s" % (err.opt,err.msg))    
    usage()
    sys.exit(2)

if len(opts) < 1:
    usage()
    sys.exit(2)
    
# default value
verbose=False
factoryOnly=False
gentype="header"
language="MsgSpec"
output=sys.stdout

# Parse command line options
for o, a in opts:
    if o in ("-i", "--input"):
        inputFile=a
    if o in ("-l", "--language"):
        language=a
    if o in ("-t", "--type"):
        gentype=a
    if o in ("-f", "--factory-only"):
        factoryOnly=True
    if o in ("-o", "--output"):        
        output=open(a,mode="w")
    if o in ("-v", "--verbose"):
        verbose=True
        mainlogger.setLevel(logging.INFO)
    if o in ("-h", "--help"):
        usage()
        sys.exit(0)

mainlogger.info("Reading message specifications from: <%s>" % inputFile)       
mainlogger.info("output send to: <%s>" % repr(output.name))
mainlogger.info("Generating for language: <%s>" % language)

# Lexer+Parser specification begins here
# reserved keywords
reserved = {
   'package'        : 'PACKAGE',
   'factory'        : 'FACTORY',
   'factoryCreator' : 'FACTORY_CREATOR',
   'factoryReceiver': 'FACTORY_RECEIVER',   
   'native'   : 'NATIVE',
   'language' : 'LANGUAGE',            
   'message'  : 'MESSAGE',
   'merge'    : 'MERGE',
   'enum'     : 'ENUM',
   'default'  : 'DEFAULT',
   'representation' : 'REPRESENTATION',            
   'required' : 'REQUIRED',
   'optional' : 'OPTIONAL',
   'repeated' : 'REPEATED',
   'onoff'    : 'ONOFF_T',
   'bool'     : 'BOOL_T',
   'string'   : 'STRING_T',
   'byte'     : 'BYTE_T',
   'int8'     : 'INT8_T',
   'uint8'    : 'UINT8_T',
   'int16'    : 'INT16_T',
   'uint16'   : 'UINT16_T',
   'int32'    : 'INT32_T',
   'uint32'   : 'UINT32_T',   
   'int64'    : 'INT64_T',
   'uint64'   : 'UINT64_T',
   'float'    : 'FLOAT_T',
   'double'   : 'DOUBLE_T', 
}

# List of token names.   This is always required
tokens = ['ID',                          
          'COMMENT',
          'INTEGER_VALUE',
          'FLOAT_VALUE',
          'BOOL_VALUE',      
          'STRING_VALUE',    
          'RBRACE','LBRACE',
          'RPAREN','LPAREN',
#          'RBRACKET','LBRACKET',
          'COMMA',
          'EQUAL',
          'COLON',
          'PERIOD',
#          'NEWLINE',
          'LANGLINE',
          ] + list(reserved.values())

# This is a message of field or name identifier          
def t_ID(t):
    r'[a-zA-Z_][a-zA-Z_0-9]*'
    if re.match( "^true$|^false$|^True$|^False$|^On$|^on$|^Off$|^off$",t.value):
        t.type = 'BOOL_VALUE'
    else:
        t.type = reserved.get(t.value,'ID') # Check for reserved words  
    return t

# Comment begins with // and ends up at the end of the line
def t_COMMENT(t):
    r'//.*'
    return t
    #pass
    # if pass No return value. Comments are discarded    
def t_INTEGER_VALUE(t): 
    r'\d+'     
    try: 
        t.value = int(t.value) 
    except ValueError: 
        print "Integer value too large", t.value 
        t.value = 0 
    return t
def t_FLOAT_VALUE(t): 
    r'\d+\.\d+' 
    try: 
        t.value = float(t.value) 
    except ValueError: 
        print "Float value too large", t.value 
        t.value = 0.0
    return t

def t_BOOL_VALUE(t): 
    r'true|false|True|False|On|on|Off|off'     
    if t.value.lower()=="true" or t.value.lower()=="on":        
        t.value = True
    elif t.value.lower()=="false" or t.value.lower()=="off" :
        t.value = False
    else:                
        t.lexer.logger.error("Invalid Boolean value too large", t.value) 
        t.value = False 
    return t

def t_STRING_VALUE(t):
    r'\".*\"'
    return t
    
t_LBRACE = r'{'
t_RBRACE = r'}'
#t_LBRACKET = r'\['
#t_RBRACKET = r'\]'
t_LPAREN = r'\('
t_RPAREN = r'\)'
t_COMMA = r','
t_EQUAL = r'='
t_COLON = r':'
t_PERIOD = r'\.'

# Define a rule so we can track line numbers
def t_NEWLINE(t):
    r'\n'
    t.lexer.lineno += 1
    
def t_LANGLINE(t):
    r'\[.*\]'
    return t
    
# A string containing ignored characters (spaces and tabs)
t_ignore  = ' \t'

# Error handling rule
def t_error(t):
    print "Illegal character '%s'" % t.value[0]
    t.lexer.skip(1)

# Build the PLY lexer
lexerlogger = logging.Logger("Lexer")
lexerlogger.setLevel(logging.ERROR)
lexerlogger.addHandler(stdoutHandler)
lexer = ply.lex.lex(debug=False)
lexer.logger = lexerlogger

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
                self.languages[l.name] = l
            else:
                self.representation = l.representation
        
    def __repr__(self):
        return "native %s" % self.name
    
    def getLanguage(self,language):
        if language in self.languages.keys():
            return self.languages[language]

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
    """
    def __init__(self,name,fields,merge):
        super(MessageType,self).__init__(name=name)
        self.fields = fields
        self.merge  = merge                    
    
    def __repr__(self):
        res = "message %s " % self.name
        return res
        
    def hasMerge(self):
        return self.merge != None
    
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
            
def p_statement_list(p):
    '''statement_list : statement 
                      | statement statement_list'''
    p.parser.AST.add(p[1])

def p_statement(p):
    '''statement : comment_block
                 | package
                 | factory
                 | message                 
                 | native                 
                 | enum'''        
    p[0]=p[1]                            
        
def p_comment_block(p):
    '''comment_block : COMMENT
                     | COMMENT comment_block'''
    if len(p)==2:                 
        p[0]=CommentBlock(p[1].strip('/'),isAtEOL=False)
    else:
        p[0]=p[2]
        p[0].lines.append(p[1].strip('/'))        
    
def p_package(p):
    '''package : PACKAGE package_id'''    
    p[0]=Package(p[2])
    p[0].linespan = (p.linespan(1)[0],p.linespan(2)[1])
    
def p_package_id(p):
    '''package_id : ID 
                  | ID PERIOD package_id'''
    if len(p)==2:
        p[0]=p[1]
    else:
        p[0]=p[1]+"."+p[3]

def p_factory(p):
    '''factory : FACTORY ID LBRACE factory_creator factory_receiver RBRACE'''
    p[0] = Factory(p[2],p[4],p[5])
    p[0].linespan = (p.linespan(1)[0],p.linespan(6)[1])

def p_factory_creator(p):
    '''factory_creator : FACTORY_CREATOR ID ID LPAREN ID RPAREN'''
    p[0]=(p[2],p[3],p[5])

def p_factory_receiver(p):
    '''factory_receiver : FACTORY_RECEIVER ID ID LPAREN ID RPAREN'''
    p[0]=(p[2],p[3],p[5])
            
def p_message(p):
    '''message : MESSAGE ID LBRACE RBRACE 
               | MESSAGE ID LBRACE field_list RBRACE 
               | MESSAGE ID COLON MERGE ID LBRACE RBRACE 
               | MESSAGE ID COLON MERGE ID LBRACE field_list RBRACE'''
    if len(p)==5:        
        p[0] = MessageType(p[2],[],None)        
    elif len(p)==6:
        p[4].reverse()
        p[0] = MessageType(p[2],p[4],None)
    elif len(p)==8:
        p[0] = MessageType(p[2],[],p[5])
    elif len(p)==9:
        p[7].reverse()
        p[0] = MessageType(p[2],p[7],p[5])                                    
    p[0].linespan = (p.linespan(1)[0],p.linespan(len(p)-1)[1]) 
    
def p_native(p): 
    'native : NATIVE ID LBRACE native_line_list RBRACE'
    # we should reverse the language list
    # because the parse build it the other way around (recursive way)
    p[4].reverse()
    p[0]=NativeType(p[2],p[4])    
    p[0].linespan = (p.linespan(1)[0],p.linespan(5)[1])
    
def p_native_line_list(p):
    '''native_line_list : native_line eol_comment
                        | native_line eol_comment native_line_list'''
    # Create or append the list (of pair)
    if len(p)==3:
        p[1].comment = p[2]
        p[0]=[p[1]]    
    else:
        p[1].comment = p[2]
        p[3].append(p[1])
        p[0]=p[3]

def p_native_line(p):
    '''native_line : language_line
                   | representation_line'''
    p[0]=p[1]

def p_language_line(p):
    '''language_line : LANGUAGE ID LANGLINE'''
    p[0]=NativeType.LanguageLine(p[2],p[3])
    
def p_representation_line(p):
    '''representation_line : REPRESENTATION typeid'''
    p[0]=NativeType.RepresentationLine(p[2])
        
def p_enum(p):
    'enum : ENUM ID LBRACE enum_value_list RBRACE'
    # we should reverse the enum value list
    # because the parse build it the other way around (recursive way)
    p[4].reverse()
    p[0] = EnumType(p[2],p[4])
    p[0].linespan = (p.linespan(1)[0],p.linespan(5)[1])
    
def p_empty(p):
    'empty :'
    pass

def p_eol_comment(p):
    '''eol_comment : COMMENT 
                        | empty'''
    # we may store the comment text for future use
    if len(p) > 1 and isinstance(p[1],type("")) :
        p[0] = CommentBlock(p[1].strip('/'),isAtEOL=True)
        p[0].linespan = p.linespan(1)        
    else:
        p[0] = ""         
    
def p_enum_value_list(p):
    '''enum_value_list : enum_val eol_comment  
                       | enum_val COMMA eol_comment
                       | enum_val COMMA eol_comment enum_value_list'''
    # Create or append the list (of pair)
    if len(p)==3:
        p[1].comment = p[2]
        p[0]=[p[1]]
    elif len(p)==4:
        p[1].comment = p[3]
        p[0]=[p[1]]              
    else:
        p[1].comment = p[3]
        p[4].append(p[1])
        p[0]=p[4]        
    
def p_enum_val(p):
    '''enum_val : ID 
                | ID EQUAL INTEGER_VALUE'''
    # Build a pair (ID,value)
    # value may be None    
    if len(p)>3:
        p[0] = EnumType.EnumValue(p[1],p[3])
        p[0].linespan = (p.linespan(1)[0],p.linespan(3)[1])
    else:
        p[0] = EnumType.EnumValue(p[1],None)
        p[0].linespan = p.linespan(1)     

def p_field_list(p):
    '''field_list : field_spec eol_comment
                  | field_spec eol_comment field_list'''
    if len(p)==3:
        p[0] = [p[1]]
    else:        
        p[0] = p[3]
        p[0].append(p[1])    

def p_field_spec(p):
    '''field_spec : qualifier typeid ID eol_comment
                  | qualifier typeid ID LBRACE DEFAULT EQUAL value RBRACE eol_comment'''
    
    if len(p)==5:
        p[0] = MessageType.MessageField(p[1],p[2],p[3],None)
        p[0].comment = p[4]
        p[0].linespan = (p.linespan(1)[0],p.linespan(4)[1])
    else:                
        p[0] = MessageType.MessageField(p[1],p[2],p[3],p[7])
        p[0].comment = p[9]   
        p[0].linespan = (p.linespan(1)[0],p.linespan(8)[1])

def p_qualifier(p):
    '''qualifier : REQUIRED
                 | REPEATED
                 | OPTIONAL'''
    p[0] = p[1]
    
def p_typeid(p):
    '''typeid : ONOFF_T
              | BOOL_T
              | STRING_T
              | BYTE_T
              | INT8_T
              | UINT8_T
              | INT16_T
              | UINT16_T
              | INT32_T
              | UINT32_T   
              | INT64_T
              | UINT64_T
              | FLOAT_T
              | DOUBLE_T
              | defined_type'''
    p[0] = p[1]    
    
def p_defined_type(p):
    '''defined_type : ID'''
    # A defined type is either a Message or Native or Enum Type.
    # This kind of type should be checked
    # **after* the AST has been built entirely.
    # We cannot check it now because of the recursive nature of the parser.
    p[0]=p[1]    
    
def p_value(p):
    '''value : INTEGER_VALUE 
             | FLOAT_VALUE 
             | BOOL_VALUE
             | STRING_VALUE'''    
    p[0]=p[1]
    
# Compute column. 
#     input is the input text string
#     token is a token instance
def find_column(input,token):
    last_cr = input.rfind('\n',0,token.lexpos)
    if last_cr < 0:
        last_cr = 0
    column = (token.lexpos - last_cr) + 1
    return column
    
def p_error(p):             
    if lexer in dir(p):
        msg = "Syntax error at '%s' on line %d column %d (token type is '%s')" % (p.value,p.lineno,find_column(p.lexer.lexdata, p),p.type)
    else:
        msg = "Syntax error at '%s' on line %d (token type is '%s')" % (p.value,p.lineno,p.type)
    print msg
    

class ASTChecker(object):
    """
    The Purpose of this class is to check AST properties. 
        
    """
    def __init__(self):
        self.logger = logging.Logger("ASTChecker")
        self.logger.setLevel(logging.ERROR)
        self.logger.addHandler(stdoutHandler)        
    
    def check(self,AST):
        """
        Check the AST.
        
        @param AST: the AST to be checked
        @type AST: C{MessageAST}  
        """
        
        # check if the supplied object has appropriate super type
        # @todo: note that we may just require to have the appropriate
        #        fields and not being sub-class of MesssageAST.
        #        this could be done with introspection.
        #        see: http://docs.python.org/library/inspect.html
        if not isinstance(AST, MessageAST):
           self.logger.error("The supplied object is not an instance of MessageAST: <%s>" % type(AST))
           AST.checked = False 
           return
       
        # check if all field used in message have known types
        # At the same time build the enum values for MessageTypes
        enumval = EnumType.EnumValue("NOT_USED",None)
        enumval.type = None
        msgTypeEnumVals = [enumval]
        for msg in AST.messages:
            enumval      = EnumType.EnumValue(msg.name.upper(),None)
            enumval.type = msg.name
            msgTypeEnumVals.append(enumval)
            for f in msg.fields:
                if not AST.isDefined(f.typeid):
                    self.logger.fatal("The type <%s> used for field <%s.%s> is unknown (not a builtin, nor native, nor message)" % (f.typeid,msg.name,f.name))
                    self.logger.fatal(" --> Check lines (%d,%d)" % (f.linespan) + " of <%s>" % AST.name)
                    AST.checked = False
                    return
                else:                   
                   f.typeid = AST.getType(f.typeid)
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
                    AST.checked = False
                    return
                else:
                    msg.merge = AST.getType(msg.merge)
                    
        # check the factory methods
        if AST.hasFactory():                        
            if not AST.isDefined(AST.factory.creator[0]):
                self.logger.fatal("The return type <%s> of the creator factory method is unknown (not a builtin, nor native, nor message)" % AST.factory.creator[0])
                self.logger.fatal(" --> Check lines (%d,%d)" % (AST.factory.linespan) + " of <%s>" % AST.name )
                AST.checked = False
                return
            if not AST.isDefined(AST.factory.creator[2]):
                self.logger.fatal("The parameter type <%s> of the creator factory method is unknown (not a builtin, nor native, nor message)" % AST.factory.creator[2])
                self.logger.fatal(" --> Check lines (%d,%d)" % (AST.factory.linespan) + " of <%s>" % AST.name )
                AST.checked = False
                return
            if not AST.isDefined(AST.factory.receiver[0]):
                self.logger.fatal("The return type <%s> of the receiver factory method is unknown (not a builtin, nor native, nor message)" % AST.factory.receiver[0])
                self.logger.fatal(" --> Check lines (%d,%d)" % (AST.factory.linespan) + " of <%s>" % AST.name )
                AST.checked = False
                return
            if not AST.isDefined(AST.factory.receiver[2]):
                self.logger.fatal("The parameter type <%s> of the receiver factory method is unknown (not a builtin, nor native, nor message)" % AST.factory.receiver[2])
                self.logger.fatal(" --> Check lines (%d,%d)" % (AST.factory.linespan) + " of <%s>" % AST.name )
                AST.checked = False
                return
                
        AST.checked = True                  
                                                                                 
# Build the PLY parser
parserlogger = logging.Logger("MessageParser")
parserlogger.setLevel(logging.ERROR)
parserlogger.addHandler(stdoutHandler)
parser = ply.yacc.yacc(debug=True)
parser.logger = parserlogger 
    
mainlogger.info("Parsing message file specifications...")
try:    
    msgFile =  open(inputFile,'r')
except IOError, e:        
    mainlogger.error("IOError raised: <"+str(e)+">")
    mainlogger.error("May be input file <%s> is unreadable or mispelled?" % inputFile)
    sys.exit()
    
lexer.lineno = 1
parser.AST = MessageAST(inputFile)
parser.parse(msgFile.read(),lexer=lexer)
parser.AST.messages.reverse()
parser.AST.enums.reverse()
parser.AST.natives.reverse()
msgFile.close()
mainlogger.info("Parse succeeded %s" % (parser.AST))

mainlogger.info("Checking AST properties....")
checker = ASTChecker()
checker.check(parser.AST)
if parser.AST.checked:    
    mainlogger.info("AST properties checked Ok.")
else:
    mainlogger.error("AST has error, generation step may produce invalid files!!!")
    sys.exit()

mainlogger.info("Generate %s from AST,..."%language)
generator = None
if language.lower() in generatorBackends.keys():
    generator = generatorBackends[language.lower()](parser.AST)
elif language.lower()=="none":
    mainlogger.info("Nothing to generate for <%s>." % language)
else:     
    mainlogger.error("Language <%s> is unknown" % language)
    
if generator != None:
    generator.generate(output,gentype,factoryOnly) 
mainlogger.info("Generate %s from AST, Done." % language)
msgFile.close()
