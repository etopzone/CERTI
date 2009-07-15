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
## $Id: GenerateMessages.py,v 1.8 2009/07/15 17:12:02 erk Exp $
## ----------------------------------------------------------------------------

"""
The CERTI Message Generator
"""

import os
import getopt, sys
# We use PLY in order to parse CERTI message specification files 
# PLY is there: http://www.dabeaz.com/ply/
import ply.yacc
import ply.lex
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

def usage():
    print "Usage:\n %s --file=<message> [--language=C++|Java|Python|Text] [--type=header|body|factory] [--output=<filename>] [--verbose] [--help]" % os.path.basename(sys.argv[0])
    
try:
    opts, args = getopt.getopt(sys.argv[1:], "f:l:t:vho:", ["file=","language=","type=","output","verbose","help"])
except getopt.GetoptError, err:
    mainlogger.error("opt = %s, msg = %s" % (err.opt,err.msg))    
    usage()
    sys.exit(2)

if len(opts) < 1:
    usage()
    sys.exit(2)
    
# default value
verbose=False
gentype="header"
language="Text"
output=sys.stdout

# Parse command line options
for o, a in opts:
    if o in ("-f", "--file"):
        messagefile=a
    if o in ("-l", "--language"):
        language=a
    if o in ("-t", "--type"):
        gentype=a
    if o in ("-o", "--output"):        
        output=open(a)
    if o in ("-v", "--verbose"):
        verbose=True
        mainlogger.setLevel(logging.INFO)
    if o in ("-h", "--help"):
        usage()
        sys.exit(0)

mainlogger.info("Reading message specifications from: <%s>" % messagefile)       
mainlogger.info("output send to: <%s>" % repr(output))
mainlogger.info("Generating for language: <%s>" % language)

# Lexer+Parser specification begins here
# reserved keywords
reserved = {
   'package'        : 'PACKAGE',
   'native' : 'NATIVE',            
   'message' : 'MESSAGE',
   'merge' : 'MERGE',
   'enum' : 'ENUM',
   'default' : 'DEFAULT',
   'required' : 'REQUIRED',
   'optional': 'OPTIONAL',
   'repeated': 'REPEATED',
   'bool' : 'BOOL_T',
   'string' : 'STRING_T',
   'byte' : 'BYTE_T',
   'int8' : 'INT8_T',
   'uint8' : 'UINT8_T',
   'int16' : 'INT16_T',
   'uint16' : 'UINT16_T',
   'int32' : 'INT32_T',
   'uint32' : 'UINT32_T',   
   'int64' : 'INT64_T',
   'uint64' : 'UINT64_T',
   'float' : 'FLOAT_T',
   'double' : 'DOUBLE_T',
}

# List of token names.   This is always required
tokens = ['ID',                 
          'COMMENT',
          'INTEGER_VALUE',
          'FLOAT_VALUE',
          'BOOL_VALUE',          
          'RBRACE','LBRACE',
          'RBRACKET','LBRACKET',
          'COMMA',
          'EQUAL',
          'COLON',
          'PERIOD',
          'NEWLINE',
          ] + list(reserved.values())

# This is a message of field or name identifier          
def t_ID(t):
    r'[a-zA-Z_][a-zA-Z_0-9]*'
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
    r'true|false|True|False'     
    if (t.value.lower()=="true"):        
        t.value = True
    elif (t.value.lower()=="false"):
        t.value = False
    else:                
        t.lexer.logger.error("Invalid Boolean value too large", t.value) 
        t.value = False 
    return t
    
t_LBRACE = r'{'
t_RBRACE = r'}'
t_LBRACKET = r'\['
t_RBRACKET = r'\]'
t_COMMA = r','
t_EQUAL = r'='
t_COLON = r':'
t_PERIOD = r'\.'

# Define a rule so we can track line numbers
def t_NEWLINE(t):
    r'\n'
    t.lexer.lineno += 1
    
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
    @param nativeMessages: the set of native messages described in this
                          C{MessageAST}
    @type nativeMessages: C{set} of C{NativeType}
    @param messages: the set of messages described in this C{MessageAST}
    @type messages: C{set} of C{MessageType}          
    
    """
    def __init__(self,name):
        super(MessageAST,self).__init__(name=name)
        self.__package              = None        
        self.__NativeTypeSet = set()
        self.__messageTypeSet       = set()
        self.__enumTypeSet          = set()
        # The types dictionary is initialized with builtin types
        self.__types                = {'bool' : ASTElement("bool"),
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
        
    def __getNativeTypeSet(self):
        return self.__NativeTypeSet  
    # pythonic getter/setter using properties   
    nativeMessages = property(fget=__getNativeTypeSet,fset=None,fdel=None,doc=None)
    
    def __getMessageTypeSet(self):
        return self.__messageTypeSet  
    # pythonic getter/setter using properties      
    messages = property(fget=__getMessageTypeSet,fset=None,fdel=None,doc=None)
    
    def __getEnumTypeSet(self):
        return self.__enumTypeSet
    # pythonic getter/setter using properties   
    enums = property(fget=__getEnumTypeSet,fset=None,fdel=None,doc=None)         
        
        
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
        self.__messageTypeSet.add(message)
        self.__types[message.name] = message
        
    def addEnumType(self,enumType):
        """
        Add an enum type to the AST.
        
        @param enumType: The enum type to be added
        @type enumType: C{EnumType}  
        """        
        self.__enumTypeSet.add(enumType)
        self.__types[enumType.name] = enumType               
    
    def addNativeType(self,native):
        """
        Add a native type to the AST.
        
        @param native: The message type to be added
        @type native: C{NativeType}  
        """
        self.__NativeTypeSet.add(native)
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
        res = "AST with <%d> native message, <%d> enum, <%d> message type(s)" % (len(self.nativeMessages),len(self.enums),len(self.messages))
        res = res + " in package <%s>" % self.package
        return res    
    
class CommentBlock(ASTElement):
    """
    Represents a block of comment
    
    A C{CommentBlock} has lines which is a list of C{string}.
    @param lines: the comments lines
    @type lines: C{list} of C{string}  
    """
    def __init__(self,content,optComment):
        """
        C{CommentBlock} constructor
        """
        super(CommentBlock,self).__init__(name="ANY Comment Block")
        self.lines=[content]
        self.__optComment=optComment
    
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
     
class NativeType(ASTElement):
    """ 
    Represents a native message type.
    
    A C{NaptiveMessageType} is a simple C{ASTElement} whose
    name is the name the native type.
    """
    def __init__(self,name):
        super(NativeType,self).__init__(name=name)        
        
    def __repr__(self):
        return "native %s" % self.name 
                    
class MessageType(ASTElement):
    """ 
    Represents a message type.
    
    @param fields: the fields of this C{MessageType}
    @type fields: C{list} of C{MessageType.MessageField}  
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
        """ Represents a message type
        """
        def __init__(self,qualifier,typeid,name,defaultValue=None):
            super(MessageType.MessageField,self).__init__(name=name)
            self.qualifier    = qualifier
            self.typeid       = typeid
            self.defaultValue = defaultValue
            
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
                 | message                 
                 | native                 
                 | enum'''        
    p[0]=p[1]                            
        
def p_comment_block(p):
    '''comment_block : COMMENT
                     | COMMENT comment_block'''
    if len(p)==2:                 
        p[0]=CommentBlock(p[1].strip('/'),optComment=False)
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
    'native : NATIVE ID'
    p[0]=NativeType(p[2])    
    p[0].linespan = p.linespan(1)
        
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

def p_optional_comment(p):
    '''optional_comment : COMMENT 
                        | empty'''
    # we may store the comment text for future use
    if len(p) > 1 and isinstance(p[1],type("")) :
        p[0] = CommentBlock(p[1].strip('/'),optComment=True)
        p[0].linespan = p.linespan(1)        
    else:
        p[0] = ""         
    
def p_enum_value_list(p):
    '''enum_value_list : enum_val optional_comment  
                       | enum_val COMMA optional_comment
                       | enum_val COMMA optional_comment enum_value_list'''
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
    '''field_list : field_spec optional_comment
                  | field_spec optional_comment field_list'''
    if len(p)==3:
        p[0] = [p[1]]
    else:        
        p[0] = p[3]
        p[0].append(p[1])    

def p_field_spec(p):
    '''field_spec : qualifier typeid ID optional_comment
                  | qualifier typeid ID LBRACKET DEFAULT EQUAL value RBRACKET optional_comment'''
    if len(p)==5:
        p[0] = MessageType.MessageField(p[1],p[2],p[3],None)
        p[0].comment = p[4]
        p[0].linespan = (p.linespan(1)[0],p.linespan(4)[1])
    else:        
        p[0] = MessageType.MessageField(p[1],p[2],p[3],p[7])
        p[0].comment = p[8]   
        p[0].linespan = (p.linespan(1)[0],p.linespan(8)[1])

def p_qualifier(p):
    '''qualifier : REQUIRED
                 | REPEATED
                 | OPTIONAL'''
    p[0] = p[1]
    
def p_typeid(p):
    '''typeid : BOOL_T
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
    # This kind of type should be checked
    # When the AST has been built.
    # We cannot check it now because of the recursive
    # nature of the parser.
    p[0]=p[1]    
    
def p_value(p):
    '''value : INTEGER_VALUE 
             | FLOAT_VALUE 
             | BOOL_VALUE'''    
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
    print "Syntax error at '%s' on line %d column %d (token type is '%s')" % (p.value,p.lineno,find_column(p.lexer.lexdata, p),p.type)

class ASTChecker(object):
    """
    The Purpose of this class is to check AST properties. 
        
    """
    def __init__(self):
        self.logger = logging.Logger("ASTChecker")
        self.logger.setLevel(logging.ERROR)
        self.logger.addHandler(stdoutHandler)
        pass
    
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
        for msg in AST.messages:
            for f in msg.fields:
                if not AST.isDefined(f.typeid):
                    self.logger.fatal("The type <%s> used for field <%s.%s> is unknown (not a builtin, nor native, nor message)" % (f.typeid,msg.name,f.name))
                    self.logger.fatal(" --> Check lines (%d,%d)" % (f.linespan) + " of <%s>" % AST.name)
                    AST.checked = False
                    return
                else:                   
                   f.typeid = AST.getType(f.typeid)
                                
        # check if merger are either native or message
        for msg in AST.messages:
            if msg.hasMerge():
                if not AST.isDefined(msg.merge):
                    self.logger.fatal("The merge target <%s> of message <%s> is unknown (not a builtin, nor native, nor message)" % (msg.merge,msg.name))
                    self.logger.fatal(" --> Check lines (%d,%d)" % (msg.linespan) + " of <%s>" % AST.name )                    
                    AST.checked = False
                    return
                else:
                    msg.merge = AST.getType(msg.merge)
        AST.checked = True                  
    
class TextGenerator(object):
    """
    This is a text generator.
    """
    def __init__(self,MessageAST):
        self.AST = MessageAST
            
    def writeComment(self,stream,ASTElement):
        if ASTElement.hasComment():
            for line in ASTElement.comment.lines:                
                stream.write("// ")
                stream.write(str(line))
                stream.write("\n")
        else:
            stream.write("\n")
            
    def generate(self,stream):
        # Generate package 
        if self.AST.hasPackage():
            self.writeComment(stream, self.AST.package)
            stream.write("package %s\n" % self.AST.package.name)
        # Generate enum
        for enum in self.AST.enums:
            self.writeComment(stream, enum)
            stream.write("enum %s {\n" % enum.name)
            first = True
            for enumval in enum.values:
                if first:
                    stream.write("     %s = %d, " % (enumval.name,enumval.value))                
                    first=False
                else:
                    stream.write("     %s, " % enumval.name)
                self.writeComment(stream, enumval)                
            stream.write("}\n")
        # Generate native message
        for native in self.AST.nativeMessages:            
            self.writeComment(stream, native)
            stream.write("native %s\n" % native.name) 
        # Generate message type
        for msg in self.AST.messages:
            self.writeComment(stream, msg)
            stream.write("message %s"%msg.name)
            if msg.hasMerge():
                stream.write(" : merge %s {\n" % msg.merge.name)
            else:
                stream.write(" {\n")
            
            for field in msg.fields:
                stream.write("        %s %s %s " % (field.qualifier,field.typeid.name,field.name))
                if field.hasDefaultValue():
                    stream.write("[default=%s] " % field.defaultValue)                                    
                self.writeComment(stream, field)                    
            stream.write("}\n")
            
class CXXGenerator(object):
    """This is a C++ generator"""
    def __init__(self,MessageAST):
        self.AST = MessageAST
            
    def writeComment(self,stream,ASTElement):
        if ASTElement.hasComment():
            for line in ASTElement.comment.lines:                
                stream.write("// ")
                stream.write(str(line))
                stream.write("\n")
        else:
            stream.write("\n")
            
    def generate(self,stream):
        stream.write("C++ generator is NOT Implemented\n")

class JavaGenerator(object):
    """This is a Java generator"""
    def __init__(self,MessageAST):
        self.AST = MessageAST
            
    def writeComment(self,stream,ASTElement):
        if ASTElement.hasComment():
            for line in ASTElement.comment.lines:                
                stream.write("// ")
                stream.write(str(line))
                stream.write("\n")
        else:
            stream.write("\n")
            
    def generate(self,stream):
        stream.write("Java generator is NOT Implemented\n")
                     
class PythonGenerator(object):
    """This is a Python generator"""
    def __init__(self,MessageAST):
        self.AST = MessageAST
            
    def writeComment(self,stream,ASTElement):
        if ASTElement.hasComment():
            for line in ASTElement.comment.lines:                
                stream.write("// ")
                stream.write(str(line))
                stream.write("\n")
        else:
            stream.write("\n")
            
    def generate(self,stream):
        stream.write("Python generator is NOT Implemented\n")                     
                     
# Build the PLY parser
parserlogger = logging.Logger("MessageParser")
parserlogger.setLevel(logging.ERROR)
parserlogger.addHandler(stdoutHandler)
parser = ply.yacc.yacc(debug=True)
parser.logger = parserlogger 
    
mainlogger.info("Parsing message file specifications...")    
msgFile =  open(messagefile,'r')
lexer.lineno = 1
parser.AST = MessageAST(messagefile)
parser.parse(msgFile.read(),lexer=lexer)
msgFile.close()
mainlogger.info("Parse succeeded %s" % (parser.AST))

mainlogger.info("Checking AST properties....")
checker = ASTChecker()
checker.check(parser.AST)
if parser.AST.checked:    
    mainlogger.info("AST properties checked Ok.")
else:
    mainlogger.error("AST has error, generation step may produce invalid files!!!")

mainlogger.info("Generate %s from AST,..."%language)
if language=="Text":    
    textGen = TextGenerator(parser.AST)
    textGen.generate(sys.stdout)    
elif language=="C++":
    cxxGen = CXXGenerator(parser.AST)
    cxxGen.generate(sys.stdout)
elif language=="Java":
    cxxGen = JavaGenerator(parser.AST)
    cxxGen.generate(sys.stdout)
elif language=="Python":
    cxxGen = PythonGenerator(parser.AST)
    cxxGen.generate(sys.stdout)

mainlogger.info("Generate %s from AST, Done."%language)

sys.exit()
for l in msgFile:
    cname = l.strip('_ \n')    
    if (gentype.lower()=="header"):
        print "/*<BEGIN>---------- %s ------------<BEGIN>*/" % cname.title()
        print "class CERTI_EXPORT NM_%s : public NetworkMessage {" % cname.title()
        print "  public:"
        print "       NM_%s();"  % cname.title()
        print "       virtual ~NM_%s();"  % cname.title()
        print "       virtual void serialize();"
        print "       virtual void deserialize();"
        print "       /* specific Getter/Setter */"
        print "  protected:"
        print "       /* specific field */"
        print "  private:"       
        print "};\n"
        print "/*<END>---------- %s ------------<END>*/\n" % cname.title()
        

    if (gentype.lower()=="body"):
        print "/*<BEGIN>---------- %s ------------<BEGIN>*/" % cname.title()
        print "NM_%s::NM_%s() {"  % (cname.title(),cname.title())
        print "    this->name = \"%s\";" % cname
        print "    this->type = NetworkMessage::%s;" % cname
        print "    /* specific field init */"
        print "}"
        print "NM_%s::~NM_%s() {"  % (cname.title(), cname.title())
        print "}"       
        print "void NM_%s::serialize() {"  % cname.title()
        print "  /* call mother class */      "
        print "  NetworkMessage::serialize(); "
        print "  /* specific code (if any) goes here */"
        print "} /* end of serialize */ "
        print "void NM_%s::deserialize() {" % cname.title()
        print "  /* call mother class */      "
        print "  NetworkMessage::deserialize(); "
        print "  /* specific code (if any) goes here */"
        print "} /* end of deserialize */"
        print "/*<END>---------- %s ------------<END>*/\n" % cname.title()


    if (gentype.lower()=="factory"):
        print "case NetworkMessage::%s:" % cname
        print "   msg = new NM_%s(); " %cname.title()
        print "   break;"

msgFile.close()
