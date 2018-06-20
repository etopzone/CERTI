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
## $Id: GenerateMessages.py,v 1.46 2011/07/18 11:54:10 erk Exp $
## ----------------------------------------------------------------------------

"""
The CERTI Message Generator.
CERTI is an HLA RTI (High Level Architecture RunTime Infrastructure)
which uses in its internal a lot of message based communication.
Those messages:
   - should be as compact as possible in terms of memory consumption
   - should be easy to implement.

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
import getopt
import sys
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

stdoutHandler.setFormatter(logging.Formatter('%(asctime)s.%(msecs)d-[%(name)s::%(levelname)s] %(message)s'
                           , '%a %H:%M:%S'))
mainlogger = logging.Logger('Main')
mainlogger.setLevel(logging.ERROR)
mainlogger.addHandler(stdoutHandler)

# Load AST related classes

import GenMsgAST

# Load backend specific language generator
# FIXME TODO: implement automatic loading of additionnal languages

import GenMsgBase
import GenMsgSpec
import GenMsgCXX
import GenMsgPython
import GenMsgJava
import GenMsgAST
import GenMsgC

generatorBackends = dict()
generatorBackends[GenMsgSpec.MsgSpecGenerator.generatorName().lower()] = \
    GenMsgSpec.MsgSpecGenerator
generatorBackends[GenMsgCXX.CXXGenerator.generatorName().lower()] = \
    GenMsgCXX.CXXGenerator
generatorBackends[GenMsgCXX.CXXCERTIMessageGenerator.generatorName().lower()] = \
    GenMsgCXX.CXXCERTIMessageGenerator
generatorBackends[GenMsgCXX.CXXCERTINetworkMessageGenerator.generatorName().lower()] = \
    GenMsgCXX.CXXCERTINetworkMessageGenerator
generatorBackends[GenMsgPython.PythonGenerator.generatorName().lower()] = \
    GenMsgPython.PythonGenerator
generatorBackends[GenMsgJava.JavaGenerator.generatorName().lower()] = \
    GenMsgJava.JavaGenerator
generatorBackends[GenMsgC.CGenerator.generatorName().lower()] = \
    GenMsgC.CGenerator

def usage():
    print 'Usage:\n%s --input=<message> [--language=<lang>] [--type=header|body] [--factory-only] [--output=<filename>|<output directory>] [--verbose] [--help]' \
        % os.path.basename(sys.argv[0])
    print '   Supported target languages are:'
    for gene in generatorBackends.values():
        print '       - ' + gene.generatorName()


try:
    (opts, args) = getopt.getopt(sys.argv[1:], 'i:l:t:o:vh', [
        'input=',
        'language=',
        'type=',
        'factory-only',
        'output=',
        'verbose',
        'help',
        ])
except getopt.GetoptError, err:
    mainlogger.error('opt = %s, msg = %s' % (err.opt, err.msg))
    usage()
    sys.exit(2)

if len(opts) < 1:
    usage()
    sys.exit(2)

# default values for command line options

verbose = False
factoryOnly = False
gentype = 'header'
language = GenMsgSpec.MsgSpecGenerator.generatorName()
output = sys.stdout
oname  = 'stdout'
inputFile = None

# Parse command line options

for (o, a) in opts:
    if o in ('-i', '--input'):
        inputFile = a
    if o in ('-l', '--language'):
        language = a
    if o in ('-t', '--type'):
        gentype = a
    if o in ('-f', '--factory-only'):
        factoryOnly = True
    if o in ('-o', '--output'):
        try:
            output = open(a, mode='w')
            oname  = a
        except IOError, err:
            if (os.path.isdir(a)):
                mainlogger.warn('output argument <%s> is a directory, passing it as-is to the generator'%a)
                output = a
                oname  = a
            else:
                mainlogger.error('unable to create output file <%s> [err=%s]' % (a,err))
    if o in ('-v', '--verbose'):
        verbose = True
        mainlogger.setLevel(logging.INFO)
    if o in ('-h', '--help'):
        usage()
        sys.exit(0)

mainlogger.info('Reading message specifications from: <%s>' % inputFile)
mainlogger.info('output send to: <%s>' % oname)
mainlogger.info('Generating for language: <%s>' % language)

# Lexer+Parser specification begins here
# reserved keywords

reserved = {
    'package': 'PACKAGE',
    'version': 'VERSION',
    'factory': 'FACTORY',
    'factoryCreator': 'FACTORY_CREATOR',
    'factoryReceiver': 'FACTORY_RECEIVER',
    'native': 'NATIVE',
    'language': 'LANGUAGE',
    'message': 'MESSAGE',
    'merge': 'MERGE',
    'enum': 'ENUM',
    'default': 'DEFAULT',
    'representation': 'REPRESENTATION',
    'required': 'REQUIRED',
    'optional': 'OPTIONAL',
    'repeated': 'REPEATED',
    'combine': 'COMBINE',
    'onoff': 'ONOFF_T',
    'bool': 'BOOL_T',
    'string': 'STRING_T',
    'byte': 'BYTE_T',
    'int8': 'INT8_T',
    'uint8': 'UINT8_T',
    'int16': 'INT16_T',
    'uint16': 'UINT16_T',
    'int32': 'INT32_T',
    'uint32': 'UINT32_T',
    'int64': 'INT64_T',
    'uint64': 'UINT64_T',
    'float': 'FLOAT_T',
    'double': 'DOUBLE_T',
    }

# List of token names.   This is always required

tokens = [  #          'RBRACKET','LBRACKET',
            #          'NEWLINE',
    'ID',
    'COMMENT',
    'INTEGER_VALUE',
    'FLOAT_VALUE',
    'BOOL_VALUE',
    'STRING_VALUE',
    'RBRACE',
    'LBRACE',
    'RPAREN',
    'LPAREN',
    'COMMA',
    'EQUAL',
    'COLON',
    'PERIOD',
    'LANGLINE',
    ] + list(reserved.values())

# This is a message of field or name identifier


def t_ID(t):
    r'''[a-zA-Z_][a-zA-Z_0-9]*'''

    if re.match('^true$|^false$|^True$|^False$|^On$|^on$|^Off$|^off$',
                t.value):
        t.type = 'BOOL_VALUE'
    else:
        t.type = reserved.get(t.value, 'ID')  # Check for reserved words
    return t


# Comment begins with // and ends up at the end of the line


def t_COMMENT(t):
    r'''//.*'''

    return t


    # pass
    # if pass No return value. Comments are discarded


def t_INTEGER_VALUE(t):
    r'''\d+'''

    try:
        t.value = int(t.value)
    except ValueError:
        print 'Integer value too large', t.value
        t.value = 0
    return t


def t_FLOAT_VALUE(t):
    r'''\d+\.\d+'''

    try:
        t.value = float(t.value)
    except ValueError:
        print 'Float value too large', t.value
        t.value = 0.0
    return t


def t_BOOL_VALUE(t):
    r'''true|false|True|False|On|on|Off|off'''

    if t.value.lower() == 'true' or t.value.lower() == 'on':
        t.value = True
    elif t.value.lower() == 'false' or t.value.lower() == 'off':
        t.value = False
    else:
        t.lexer.logger.error('Invalid Boolean value too large', t.value)
        t.value = False
    return t


def t_STRING_VALUE(t):
    r'''\".*\"'''

    return t


t_LBRACE = r'{'
t_RBRACE = r'}'

# t_LBRACKET = r'\['
# t_RBRACKET = r'\]'

t_LPAREN = r'\('
t_RPAREN = r'\)'
t_COMMA = r','
t_EQUAL = r'='
t_COLON = r':'
t_PERIOD = r'\.'

# Define a rule so we can track line numbers


def t_NEWLINE(t):
    r'''\n'''

    t.lexer.lineno += 1


def t_LANGLINE(t):
    r'''\[.*\]'''

    return t


# A string containing ignored characters (spaces and tabs)

t_ignore = ' \t'

# Error handling rule


def t_error(t):
    t.lexer.logger.error("Illegal character '%s'" % t.value[0])
    t.lexer.skip(1)


# Build the PLY lexer

lexerlogger = logging.Logger('Lexer')
lexerlogger.setLevel(logging.ERROR)
lexerlogger.addHandler(stdoutHandler)
lexer = ply.lex.lex(debug=False)
lexer.logger = lexerlogger


def p_statement_list(p):
    '''statement_list : statement 
                      | statement statement_list'''

    p.parser.AST.add(p[1])


def p_statement(p):
    '''statement : comment_block
                 | package
                 | version
                 | factory
                 | message                 
                 | native                 
                 | enum'''

    p[0] = p[1]


def p_comment_block(p):
    '''comment_block : COMMENT
                     | COMMENT comment_block'''

    if len(p) == 2:
        p[0] = GenMsgAST.CommentBlock(p[1].strip('/'), isAtEOL=False)
    else:
        p[0] = p[2]
        p[0].lines.append(p[1].strip('/'))


def p_package(p):
    '''package : PACKAGE package_id'''

    p[0] = GenMsgAST.Package(p[2])
    p[0].linespan = (p.linespan(1)[0], p.linespan(2)[1])


def p_package_id(p):
    '''package_id : ID 
                  | ID PERIOD package_id'''

    if len(p) == 2:
        p[0] = p[1]
    else:
        p[0] = p[1] + '.' + p[3]


def p_version(p):
    '''version : VERSION INTEGER_VALUE PERIOD INTEGER_VALUE'''

    p[0] = GenMsgAST.Version((p[2], p[4]))


def p_factory(p):
    '''factory : FACTORY ID LBRACE factory_creator factory_receiver RBRACE
               | FACTORY ID LBRACE factory_creator RBRACE'''

    if len(p) == 7:
        p[0] = GenMsgAST.Factory(p[2], p[4], p[5])
        p[0].linespan = (p.linespan(1)[0], p.linespan(6)[1])
    elif len(p) == 6:

        p[0] = GenMsgAST.Factory(p[2], p[4])
        p[0].linespan = (p.linespan(1)[0], p.linespan(5)[1])


def p_factory_creator(p):
    '''factory_creator : FACTORY_CREATOR ID ID LPAREN ID RPAREN'''

    p[0] = (p[2], p[3], p[5])


def p_factory_receiver(p):
    '''factory_receiver : FACTORY_RECEIVER ID ID LPAREN ID RPAREN'''

    p[0] = (p[2], p[3], p[5])


def p_message(p):
    '''message : MESSAGE ID LBRACE RBRACE 
               | MESSAGE ID LBRACE field_list RBRACE 
               | MESSAGE ID COLON MERGE ID LBRACE RBRACE 
               | MESSAGE ID COLON MERGE ID LBRACE field_list RBRACE'''

    if len(p) == 5:
        p[0] = GenMsgAST.MessageType(p[2], [], None)
    elif len(p) == 6:
        p[4].reverse()
        p[0] = GenMsgAST.MessageType(p[2], p[4], None)
    elif len(p) == 8:
        p[0] = GenMsgAST.MessageType(p[2], [], p[5])
    elif len(p) == 9:
        p[7].reverse()
        p[0] = GenMsgAST.MessageType(p[2], p[7], p[5])
    p[0].linespan = (p.linespan(1)[0], p.linespan(len(p) - 1)[1])


def p_native(p):
    '''native : NATIVE ID LBRACE native_line_list RBRACE'''

    # we should reverse the language list
    # because the parse build it the other way around (recursive way)
    p[4].reverse()
    p[0] = GenMsgAST.NativeType(p[2], p[4])
    p[0].linespan = (p.linespan(1)[0], p.linespan(5)[1])


def p_native_line_list(p):
    '''native_line_list : native_line eol_comment
                        | native_line eol_comment native_line_list'''

    # Create or append the list (of pair)
    if len(p) == 3:
        p[1].comment = p[2]
        p[0] = [p[1]]
    else:
        p[1].comment = p[2]
        p[3].append(p[1])
        p[0] = p[3]


def p_native_line(p):
    '''native_line : language_line
                   | representation_line'''

    p[0] = p[1]


def p_language_line(p):
    '''language_line : LANGUAGE ID LANGLINE'''

    p[0] = GenMsgAST.NativeType.LanguageLine(p[2], p[3])


def p_representation_line(p):
    '''representation_line : REPRESENTATION qualifier typeid
                        | REPRESENTATION typeid
                        | REPRESENTATION COMBINE'''

    if len(p)==3:
        p[0] = GenMsgAST.NativeType.RepresentationLine(p[2],None)
    else:
        p[0] = GenMsgAST.NativeType.RepresentationLine(p[3],p[2])


def p_enum(p):
    '''enum : ENUM ID LBRACE enum_value_list RBRACE'''

    # we should reverse the enum value list
    # because the parse build it the other way around (recursive way)

    p[4].reverse()
    p[0] = GenMsgAST.EnumType(p[2], p[4])
    p[0].linespan = (p.linespan(1)[0], p.linespan(5)[1])


def p_empty(p):
    '''empty :'''

    pass


def p_eol_comment(p):
    '''eol_comment : COMMENT 
                        | empty'''

    # we may store the comment text for future use

    if len(p) > 1 and isinstance(p[1], type('')):
        p[0] = GenMsgAST.CommentBlock(p[1].strip('/'), isAtEOL=True)
        p[0].linespan = p.linespan(1)
    else:
        p[0] = ''


def p_enum_value_list(p):
    '''enum_value_list : enum_val eol_comment  
                       | enum_val COMMA eol_comment
                       | enum_val COMMA eol_comment enum_value_list'''

    # Create or append the list (of pair)

    if len(p) == 3:
        p[1].comment = p[2]
        p[0] = [p[1]]
    elif len(p) == 4:
        p[1].comment = p[3]
        p[0] = [p[1]]
    else:
        p[1].comment = p[3]
        p[4].append(p[1])
        p[0] = p[4]


def p_enum_val(p):
    '''enum_val : ID 
                | ID EQUAL INTEGER_VALUE'''

    # Build a pair (ID,value)
    # value may be None

    if len(p) > 3:
        p[0] = GenMsgAST.EnumType.EnumValue(p[1], p[3])
        p[0].linespan = (p.linespan(1)[0], p.linespan(3)[1])
    else:
        p[0] = GenMsgAST.EnumType.EnumValue(p[1], None)
        p[0].linespan = p.linespan(1)


def p_field_list(p):
    '''field_list : field_spec eol_comment
                  | field_spec eol_comment field_list'''

    if len(p) == 3:
        p[0] = [p[1]]
    else:
        p[0] = p[3]
        p[0].append(p[1])


def p_field_spec(p):
    '''field_spec : qualifier typeid ID eol_comment
                  | qualifier typeid ID LBRACE DEFAULT EQUAL value RBRACE eol_comment
                  | COMBINE typeid LBRACE field_list RBRACE eol_comment'''

    if len(p) == 5:
        p[0] = GenMsgAST.MessageType.MessageField(p[1], p[2], p[3],
                None)
        p[0].comment = p[4]
        p[0].linespan = (p.linespan(1)[0], p.linespan(4)[1])
    elif len(p) == 7:
        p[4].reverse()
        p[0] = GenMsgAST.MessageType.CombinedField(p[2], p[4])
        p[0].comment = p[5]
        p[0].linespan = (p.linespan(1)[0], p.linespan(4)[1])
    else:
        p[0] = GenMsgAST.MessageType.MessageField(p[1], p[2], p[3],
                p[7])
        p[0].comment = p[9]
        p[0].linespan = (p.linespan(1)[0], p.linespan(8)[1])


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

    p[0] = p[1]


def p_value(p):
    '''value : INTEGER_VALUE 
             | FLOAT_VALUE 
             | BOOL_VALUE
             | STRING_VALUE'''

    p[0] = p[1]


def find_column(input, token):
    '''input is the input text string
    token is a token instance'''

    last_cr = input.rfind('\n', 0, token.lexpos)
    if last_cr < 0:
        last_cr = 0
    column = token.lexpos - last_cr + 1
    return column


def p_error(p):
    if lexer in dir(p):
        msg = \
            "Syntax error at '%s' on line %d column %d (token type is '%s')" \
            % (p.value, p.lineno, find_column(p.lexer.lexdata, p),
               p.type)
    else:
        msg = "Syntax error at '%s' on line %d (token type is '%s')" \
            % (p.value, p.lineno, p.type)
    parserlogger.fatal(msg)
    lexer.syntax = False


# Build the PLY parser

lexer.syntax = True

parserlogger = logging.Logger('MessageParser')
parserlogger.setLevel(logging.ERROR)
parserlogger.addHandler(stdoutHandler)
parser = ply.yacc.yacc(debug=True)
parser.logger = parserlogger

if inputFile != None:
    mainlogger.info('Parsing message file specifications...')
    try:
        msgFile = open(inputFile, 'r')
    except IOError, e:
        mainlogger.error('IOError raised: <' + str(e) + '>')
        mainlogger.error('May be input file <%s> is unreadable or mispelled?'
                          % inputFile)
        sys.exit()
else:
    mainlogger.error('No input file given!!')
    sys.exit()

lexer.lineno = 1
parser.AST = GenMsgAST.MessageAST(inputFile)
parser.parse(msgFile.read(), lexer=lexer, tracking=True)
parser.AST.messages.reverse()
parser.AST.enums.reverse()
parser.AST.natives.reverse()
if lexer.syntax:
  msgFile.close()
  mainlogger.info('Parse succeeded %s' % parser.AST)

  mainlogger.info('Checking AST properties....')
  checker = GenMsgAST.ASTChecker()
  checker.check(parser.AST)
  if parser.AST.checked:
      mainlogger.info('AST properties checked Ok.')
  else:
      mainlogger.error('AST has error, generation step may produce invalid files!!!'
                      )
      sys.exit()

  mainlogger.info('Generate %s from AST,...' % language)
  generator = None
  if language.lower() in generatorBackends.keys():
      generator = generatorBackends[language.lower()](parser.AST)
  elif language.lower() == 'none':
      mainlogger.info('Nothing to generate for <%s>.' % language)
      usage()
  else:
      mainlogger.error('Language <%s> is unknown' % language)
      usage()

  if generator != None:
      generator.logger.addHandler(stdoutHandler)
      if verbose:
          generator.logger.setLevel(logging.INFO)
      generator.generate(output, gentype, factoryOnly)
  mainlogger.info('Generate %s from AST, Done.' % language)
  msgFile.close()

else:
  mainlogger.error('Syntax error: code cannot be generated')
