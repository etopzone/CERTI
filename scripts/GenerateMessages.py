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


stdoutHandler = logging.StreamHandler(sys.stdout)
stdoutHandler.setFormatter(logging.Formatter("%(msecs)d-[%(name)s::%(levelname)s] %(message)s"))
mylogger = logging.Logger("GenerateMessage")
mylogger.setLevel(logging.ERROR)
mylogger.addHandler(stdoutHandler)

def usage():
    print "Usage:\n %s --file=<message> [--language=C++|Java|Python] [--type=header|body|factory] [--verbose] [--help]" % os.path.basename(sys.argv[0])
    
try:
    opts, args = getopt.getopt(sys.argv[1:], "f:l:t:vh", ["file=","language=","type=","verbose","help"])
except getopt.GetoptError, err:
    mylogger.error("opt = %s, msg = %s" % (err.opt,err.msg))    
    usage()
    sys.exit(2)

if len(opts) < 1:
    usage()
    sys.exit(2)
    
# default value
verbose=False
gentype="header"
language="C++"

# Parse command line options
for o, a in opts:
    if o in ("-f", "--file"):
        messagefile=a
    if o in ("-l", "--language"):
        language=a
    if o in ("-t", "--type"):
        gentype=a
    if o in ("-v", "--verbose"):
        verbose=True
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
        print "Invalid Boolean value too large", t.value 
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
def t_newline(t):
    r'\n'
    t.lexer.lineno += 1
    
# A string containing ignored characters (spaces and tabs)
t_ignore  = ' \t'

# Error handling rule
def t_error(t):
    print "Illegal character '%s'" % t.value[0]
    t.lexer.skip(1)

# Build the PLY lexer
lexer = ply.lex.lex()

# Named Class
class Named(object):
    def __init__(self,name):
        self.__name    = name
        self.__comment = None
    
    def __getName(self):
        return self.__name    
    def __setName(self,name):
        self.__name = name
    # pythonic getter/setter using properties    
    name = property(fget=__getName,fset=__setName)
    
    def __getComment(self):
        return self.__comment    
    def __setComment(self,comment):
        self.__comment = comment
    # pythonic getter/setter using properties    
    name = property(fget=__getComment,fset=__setComment)

# Message set
class MessageNotAnAST(Named):
    def __init__(self,name):
        super(MessageNotAnAST,self).__init__(name=name)        
        self.__nativeMessageTypeSet = set()
        self.__messageTypeSet       = set()
        self.__enumTypeSet          = set()
        self.__package              = None
        self.__types                = dict()
        self.logger = logging.Logger("MessageNotAnAST")
        self.logger.setLevel(logging.ERROR)
        self.logger.addHandler(stdoutHandler)
        
    def __getNativeMessageTypeSet(self):
        return self.__nativeMessageTypeSet        
    nativeMessages = property(fget=__getNativeMessageTypeSet)
    
    def __getMessageTypeSet(self):
        return self.__messageTypeSet        
    messages = property(fget=__getMessageTypeSet)
    
    def __getEnumTypeSet(self):
        return self.__enumTypeSet    
    enums = property(fget=__getEnumTypeSet)         
    
    def __getPackage(self):
        return self.__package
    def __setPackage(self,package):
        self.__package = package
    package = property(fget=__getPackage,fset=__setPackage)
        
    def add(self,any):
        if any == None:
            self.logger.error("<None> given to AST some rule aren't finished")
        elif isinstance(any,type("")):
            pass
        else:         
            if isinstance(any,EnumType):            
                self.addEnumType(any)                            
            elif isinstance(any,NativeMessageType):
                self.addNativeMessageType(any)
            elif isinstance(any,MessageType):
                self.addMessageType(any)
            elif isinstance(any,Package):
                self.package = any
            else:            
                self.logger.error("<%s> not handle [yet]" % any)            
            
    def addMessageType(self,message):
        self.__messageTypeSet.add(message)
        self.__types[message.name] = message
        
    def addEnumType(self,enumType):
        self.__enumTypeSet.add(enumType)
        self.__types[enumType.name] = enumType       
        self.logger.debug("Adding enum type %s" % enumType.name) 
    
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
    
    def visit(self):        
        return None
    
class Package(Named):
    """Represents a package"""
    def __init__(self,name):
        super(Package,self).__init__(name=name)
          
    def __repr__(self):
        return "package %s" % self.name
     
class NativeMessageType(Named):
    """ Represents a native message type
    """
    def __init__(self,name):
        super(NativeMessageType,self).__init__(name=name)        
        
    def __repr__(self):
        return "native_message %s" % self.name 
                    
class MessageType(Named):
    """ Represents a message type
    """
    def __init__(self,name,field_list,merge):
        super(MessageType,self).__init__(name=name)
        self.field_list = field_list
        self.merge      = merge
        
    def addField(self,name,type,defaultValue=None):
        self.field[name] = [type,defaultValue]
    
    def __repr__(self):
        res = "message %s " % self.name
        return res
                
class EnumType(Named):
    """ Represents an enum type 
    """
    def __init__(self,name,values):
        super(EnumType,self).__init__(name=name)
        # rebuild dictionary with value from the list                     
        self.values = []
        lastval     = -1        
        for val in values:                                    
            if (val[1]==None):
                self.values.append((val[0],lastval+1))
                lastval += 1
            else:
                self.values.append(val)                           
                lastval = val[1]
        
    def __repr__(self):
        res = "Enum %s {\n" % self.name
        for val in self.values:            
            res = res + "  " + str(val[0]) + " = " + str(val[1]) + ", \n"
        res = res + "}"
        return res                 
        
    def addValue(self,value):
        self.values.append(value)        
        
def p_statement_list(p):
    '''statement_list : statement 
                      | statement statement_list'''
    p.parser.AST.add(p[1])

def p_statement(p):
    '''statement : comment_line
                 | package                 
                 | message                 
                 | native_message                 
                 | enum'''        
    p[0]=p[1]                            
        
def p_comment_line(p):
    '''comment_line : COMMENT'''
    p[0]=p[1].strip('/')
    
def p_package(p):
    '''package : PACKAGE package_id 
               | PACKAGE package_id optional_comment'''    
    p[0]=Package(p[2])
    
def p_package_id(p):
    '''package_id : ID 
                  | ID PERIOD package_id'''
    if len(p)==2:
        p[0]=p[1]
    else:
        p[0]=p[1]+"."+p[3]
            
def p_message(p):
    '''message : MESSAGE ID LBRACE RBRACE optional_comment
               | MESSAGE ID LBRACE field_list RBRACE optional_comment
               | MESSAGE ID COLON MERGE ID LBRACE RBRACE optional_comment
               | MESSAGE ID COLON MERGE ID LBRACE field_list RBRACE optional_comment'''
    if len(p)==6:        
        p[0] = MessageType(p[2],[],None)
    elif len(p)==7:
        p[0] = MessageType(p[2],p[4],None)
    elif len(p)==8:
        p[0] = MessageType(p[2],[],p[4])
    elif len(p)==9:
        p[0] = MessageType(p[2],p[7],p[4])                                    

def p_native_message(p): 
    'native_message : NATIVE_MESSAGE ID'
    p[0]=NativeMessageType(p[2])    
        
def p_enum(p):
    'enum : ENUM ID LBRACE enum_value_list RBRACE optional_comment'
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
        p[0] = p[1].strip('/')
    else:
        p[0] = ""         
    
def p_enum_value_list(p):
    '''enum_value_list : enum_val optional_comment  
                       | enum_val COMMA optional_comment
                       | enum_val COMMA optional_comment enum_value_list'''
    # Create or append the list (of pair)            
    if len(p) > 4:        
        p[4].append(p[1])
        p[0]=p[4]
    else:
        p[0]=[p[1]]    
    
def p_enum_val(p):
    '''enum_val : ID 
                | ID EQUAL INTEGER_VALUE'''
    # Build a pair (ID,value)
    # value may be None    
    if len(p)>3:
        p[0] = (p[1],p[3])
    else:
        p[0] = (p[1],None)     

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
        p[0] = (p[1],p[2],p[3],None)
    else:        
        p[0] = (p[1],p[2],p[3],p[7])   

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
    p[0]=p[1].value
    
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
    
         
# Build the PLY parser
parser = ply.yacc.yacc() 

if False:
    print "Trying to lex..."
    msgFile =  open(messagefile,'r')
    lexer.lineno = 1
    lexer.input(msgFile.read())
    msgFile.close()
    # Tokenize
    while True:
        tok = lexer.token()
        if not tok: break      # No more input
        #print tok
        print "Lex succeeded"
        
    
print "Trying to parse..."    
msgFile =  open(messagefile,'r')
lexer.lineno = 1
parser.AST = MessageNotAnAST(messagefile)
parser.parse(msgFile.read(),lexer=lexer)
msgFile.close()
print "Parse succeeded AST = %s (type=%s)" % (parser.AST,type(parser.AST)) 

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
