#!/usr/bin/env python

import os
import getopt, sys
import shutil
# We use PLY in order to parse CERTI message specification files 
# PLY is there: http://www.dabeaz.com/ply/
import ply.yacc
import ply.lex
# We use logging for ... logging :-)
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
        mainlogger.info("output send to file: <%s>" % a)
        output=open(a)
    if o in ("-v", "--verbose"):
        verbose=True
        mainlogger.setLevel(logging.INFO)
    if o in ("-h", "--help"):
        usage()
        sys.exit(0)                

# Lexer+Parser specification begins here
# reserved keywords
reserved = {
   'package'        : 'PACKAGE',
   'native_message' : 'NATIVE_MESSAGE',            
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

# Named Class
class ASTElement(object):
    def __init__(self,name):        
        self.__name    = name
        self.__comment = None
        self.logger = logging.Logger("ASTElement")
        self.logger.setLevel(logging.ERROR)
        self.logger.addHandler(stdoutHandler)
    
    def getName(self):
        return self.__name    
    def setName(self,name):        
        self.__name = name
    # pythonic getter/setter using properties    
    name = property(fget=getName,fset=setName,fdel=None, doc=None)    
    
    def hasComment(self):
        return self.__comment!=None
    
    def getComment(self):
        return self.__comment    
    def setComment(self,comment):
        if isinstance(comment,type("")):
            pass
        else:
            self.logger.info("Adding comment %s to element %s" % (comment.lines,self.name))
            self.__comment = comment
    # pythonic getter/setter using properties    
    comment = property(fget=getComment,fset=setComment,fdel=None, doc=None)

# Message set
class MessageAST(ASTElement):
    def __init__(self,name):
        super(MessageAST,self).__init__(name=name)        
        self.__nativeMessageTypeSet = set()
        self.__messageTypeSet       = set()
        self.__enumTypeSet          = set()
        self.__package              = None
        self.__types                = dict()
        self.__ultimateElement      = None                
        self.logger = logging.Logger("MessageAST")
        self.logger.setLevel(logging.ERROR)
        self.logger.addHandler(stdoutHandler)        
        
    def __getNativeMessageTypeSet(self):
        return self.__nativeMessageTypeSet        
    nativeMessages = property(fget=__getNativeMessageTypeSet,fset=None,fdel=None,doc=None)
    
    def __getMessageTypeSet(self):
        return self.__messageTypeSet        
    messages = property(fget=__getMessageTypeSet,fset=None,fdel=None,doc=None)
    
    def __getEnumTypeSet(self):
        return self.__enumTypeSet    
    enums = property(fget=__getEnumTypeSet,fset=None,fdel=None,doc=None)         
    
    def hasPackage(self):
        return self.__package != None
    def __getPackage(self):
        return self.__package
    def __setPackage(self,package):
        self.__package = package
    package = property(fget=__getPackage,fset=__setPackage,fdel=None,doc=None) 
        
    def add(self,any):
        """ Add an ASTElement to the AST """
        if any == None:
            self.logger.error("<None> given to AST some rule aren't finished")    
        else:                     
            self.logger.debug("Add %s %s" % (type(any).__name__,any.name))            
            if isinstance(any,EnumType):            
                self.addEnumType(any)                            
            elif isinstance(any,NativeMessageType):
                self.addNativeMessageType(any)
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
        self.__messageTypeSet.add(message)
        self.__types[message.name] = message
        
    def addEnumType(self,enumType):        
        self.__enumTypeSet.add(enumType)
        self.__types[enumType.name] = enumType               
    
    def addNativeMessageType(self,message):
        self.__nativeMessageTypeSet.add(message)
        self.__types[message.name] = message
        
    def isDefined(self,typename):
        self.logger.debug("%s" % self.__types.keys())
        if typename in self.__types.keys():
            return True
        else:
            return False 
    
    def __repr__(self):
        res = "AST with:\n <%d> native messages type,\n <%d> message type,\n <%d> enum type\n" % (len(self.nativeMessages),len(self.messages),len(self.enums))
        res = res + " will be in package <%s>\n" % self.package
        return res    
    
class CommentBlock(ASTElement):
    def __init__(self,content,optComment):
        super(CommentBlock,self).__init__(name="ANY Comment Block")
        self.lines=[content]
        self.__optComment=optComment
    
class Package(ASTElement):
    """Represents a package"""
    def __init__(self,name):        
        super(Package,self).__init__(name)                        
          
    def __repr__(self):
        return "package %s" % self.name
     
class NativeMessageType(ASTElement):
    """ Represents a native message type
    """
    def __init__(self,name):
        super(NativeMessageType,self).__init__(name=name)        
        
    def __repr__(self):
        return "native_message %s" % self.name 
                    
class MessageType(ASTElement):
    """ Represents a message type
    """
    def __init__(self,name,field_list,merge):
        super(MessageType,self).__init__(name=name)
        self.field_list = field_list
        self.merge      = merge                    
    
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
                 | native_message                 
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

def p_native_message(p): 
    'native_message : NATIVE_MESSAGE ID'
    p[0]=NativeMessageType(p[2])    
        
def p_enum(p):
    'enum : ENUM ID LBRACE enum_value_list RBRACE'
    # we should reverse the enum value list
    # because the parse build it the other way around (recursive way)
    p[4].reverse()
    p[0] = EnumType(p[2],p[4])
    
def p_empty(p):
    'empty :'
    pass

def p_optional_comment(p):
    '''optional_comment : COMMENT 
                        | empty'''
    # we may store the comment text for future use
    if len(p) > 1 and isinstance(p[1],type("")) :
        p[0] = CommentBlock(p[1].strip('/'),optComment=True)        
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
    else:
        p[0] = EnumType.EnumValue(p[1],None)     

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
    else:        
        p[0] = MessageType.MessageField(p[1],p[2],p[3],p[7])
        p[0].comment = p[8]   

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
    
class TextGenerator(object):
    """This is a text generator"""
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
            stream.write("native_message %s\n" % native.name) 
        # Generate message type
        for msg in self.AST.messages:
            self.writeComment(stream, msg)
            stream.write("message %s"%msg.name)
            if msg.hasMerge():
                stream.write(" : merge %s {\n" % msg.merge)
            else:
                stream.write(" {\n")
            
            for field in msg.field_list:
                stream.write("        %s %s %s " % (field.qualifier,field.typeid,field.name))
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
    
mainlogger.info("Trying to parse...")    
msgFile =  open(messagefile,'r')
lexer.lineno = 1
parser.AST = MessageAST(messagefile)
parser.parse(msgFile.read(),lexer=lexer)
msgFile.close()
mainlogger.info("Parse succeeded AST = %s" % (parser.AST))

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
