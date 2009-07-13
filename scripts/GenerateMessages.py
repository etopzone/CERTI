#!/usr/bin/env python

import os
import getopt, sys
import shutil
# We use PLY in order to parse CERTI message specification files 
import ply.yacc
import ply.lex

def usage():
    print "Usage:\n %s --file=<message> [--language=C++|Java|Python] [--type=header|body|factory] [--verbose] [--help]" % os.path.basename(sys.argv[0])
    
try:
    opts, args = getopt.getopt(sys.argv[1:], "f:l:t:vh", ["file=","language=","type=","verbose","help"])
except getopt.GetoptError, err:
    print >> stderr, "opt = %s, msg = %s" % (err.opt,err.msg)
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
          ] + list(reserved.values())

# This is a message of field identifier          
def t_ID(t):
    r'[a-zA-Z_][a-zA-Z_0-9]*'
    t.type = reserved.get(t.value,'ID')    # Check for reserved words
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

# Message set
class MessageAST(object):
    def __init__(self,name):
        self.name = name
        self.nativeMessageType = set()
        self.messageTypeSet    = set()
        self.enumTypeSet       = set()
        
    def add(self,any):
        if any == None:
            print "<None> given to AST some rule aren't finished"
        elif isinstance(any,type("")):
            pass
        else:         
            if isinstance(any,EnumType):            
                self.addEnumType(any)                            
            elif isinstance(any,NativeMessageType):
                self.addNativeMessageType(any)
            elif isinstance(any,MessageType):
                self.addMessageType(any)
            else:            
                print "<%s> not handle [yet]" % any
            print "%s added to AST" % any
            
    def addMessageType(self,message):
        self.messageTypeSet.add(message)
        
    def addEnumType(self,enumType):
        self.enumTypeSet.add(enumType)
    
    def addNativeMessageType(self,message):
        self.messageTypeSet.add(message)
        
class NativeMessageType(object):
    """ Represents a native message type
    """
    def __init__(self,name):
        self.name = name      
        
    def __repr__(self):
        return "native_message %s" % self.name 
                    
class MessageType(object):
    """ Represents a message type
    """
    def __init__(self,name,field_list,merge):
        self.name       = name
        self.field_list = field_list
        self.merge      = merge
        
    def addField(self,name,type,defaultValue=None):
        self.field[name] = [type,defaultValue]
    
    def __repr__(self):
        res = "message %s " % self.name
        return res
                
class EnumType(object):
    """ Represents an enum type 
    """
    def __init__(self,name,values):
        self.name   = name
        # rebuild dictionary with value from the list                     
        self.values = []
        lastval = -1        
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
            res = res + "  " + str(val[0]) + "=" + str(val[1]) + ", \n"
        res = res + "}"
        return res 
                
        
    def addValue(self,value):
        self.values.append(value)

def p_statement(p):
    '''statement : comment_line
                 | comment_line statement
                 | message
                 | message statement
                 | native_message
                 | native_message statement  
                 | enum
                 | enum statement'''
        
    p.parser.AST.add(p[1])                            
        
def p_comment_line(p):
    '''comment_line : COMMENT'''
    p[0]=p[1].strip('/')    
            
def p_message(p):
    '''message : MESSAGE ID LBRACE optional_comment field_list RBRACE optional_comment
               | MESSAGE ID LBRACE optional_comment RBRACE optional_comment
               | MESSAGE ID COLON MERGE ID LBRACE  optional_comment RBRACE optional_comment
               | MESSAGE ID COLON MERGE ID LBRACE optional_comment field_list RBRACE optional_comment'''
    if len(p)==7:        
        p[0] = MessageType(p[2],[],None)
    elif len(p)==8:
        p[0] = MessageType(p[2],p[5],None)
    elif len(p)==9:
        p[0] = MessageType(p[2],[],p[5])
    elif len(p)==10:
        p[0] = MessageType(p[2],p[7],p[5])                                    

def p_native_message(p): 
    'native_message : NATIVE_MESSAGE ID optional_comment'
    p[0]=NativeMessageType(p[2])    
        
def p_enum(p):
    'enum : ENUM ID LBRACE optional_comment enum_value_list RBRACE optional_comment'
    # we should reverse the enum value list
    # because the parse build it the other way around (recursive way)
    p[5].reverse()
    p[0] = EnumType(p[2],p[5])
    
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
        p[0] = p[3].append(p[1])     

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
              | ID'''
    p[0] = p[1]    
    
def p_value(p):
    '''value : INTEGER_VALUE 
             | FLOAT_VALUE 
             | BOOL_VALUE'''
    p[0]=p[1].value
    
def p_error(p):     
    print "Syntax error at '%s' on line %d (token type is '%s')" % (p.value,p.lineno,p.type)    
         
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
parser.AST = MessageAST(messagefile)
ast = parser.parse(msgFile.read(),lexer=lexer)
msgFile.close()
print "Parse succeeded AST = %s" % ast

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
