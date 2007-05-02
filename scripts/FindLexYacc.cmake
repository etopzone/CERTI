
SET(PATH_DIR /usr/local/bin /usr/bin)

MESSAGE(STATUS "Looking for lex...")
FIND_PROGRAM(LEX_PROGRAM 
  NAMES lex flex 
  PATHS ${PATH_DIR}
  DOC "A lex-compatible lexer generator")
IF (LEX_PROGRAM) 
  MESSAGE(STATUS "Looking for lex... - found lex is ${LEX_PROGRAM}")
  SET(LEX_FOUND "YES")
ELSE (LEX_PROGRAM) 
  SET(LEX_FOUND "NO")
  MESSAGE(STATUS "Looking for lex... - NOT found")
ENDIF (LEX_PROGRAM) 

MESSAGE(STATUS "Looking for yacc...")
FIND_PROGRAM(YACC_PROGRAM 
  NAMES yacc yacc.bison bison
  PATHS ${PATH_DIR}
  DOC "A yacc-compatible parser generator")
IF (YACC_PROGRAM) 
  SET(YACC_FOUND "YES")
  MESSAGE(STATUS "Looking for yacc... - found yacc is ${YACC_PROGRAM}")
  IF ("${YACC_PROGRAM}" MATCHES ".*bison.*")
    SET(YACC_COMPAT_ARG -y)
  ELSE ("${YACC_PROGRAM}" MATCHES ".*bison.*")
    SET(YACC_COMPAT_ARG  )
  ENDIF ("${YACC_PROGRAM}" MATCHES ".*bison.*")
ELSE (YACC_PROGRAM)
  SET(YACC_FOUND "NO")
  MESSAGE(STATUS "Looking for yacc... - NOT found")
ENDIF (YACC_PROGRAM)