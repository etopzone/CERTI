%{
// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2003  Benoît Bréholée
//
// This file is part of CERTI-libCERTI
//
// CERTI-libCERTI is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libCERTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: tokens.ll,v 3.1 2003/10/27 10:21:08 breholee Exp $
// ----------------------------------------------------------------------------

#include "syntax.h"
#include "RTItypes.hh"
#include <string>
#include <iostream>

using std::cout ;
using std::endl ;

namespace certi {
namespace fedparser {

std::string arg ;
OrderType order ;
TransportType transport ;
int line_number ;

}}

int yyerror(char *);
%}

ident   [A-Za-z_][A-Za-z0-9_\.]*

%%

;;.*\n		certi::fedparser::line_number++ ; /* comments */
[ \t]+		/* whitespaces */
\n		certi::fedparser::line_number++ ;
"("		return L_PAR ;
")"		return R_PAR ;
"("[Aa][Tt][Tt][Rr][Ii][Bb][Uu][Tt][Ee]	return ATTRIBUTE ;
"("[Cc][Ll][Aa][Ss][Ss]	return CLASS ;
"("[Dd][Ii][Mm][Ee][Nn][Ss][Ii][Oo][Nn]	return DIMENSION ;
"("[Ff][Ee][Dd]	return FED ;
"("[Ff][Ee][Dd][Vv][Ee][Rr][Ss][Ii][Oo][Nn]	return FED_VERSION ;
"("[Ff][Ee][Dd][Ee][Rr][Aa][Tt][Ii][Oo][Nn]	return FEDERATION ;
"("[Ff][Ee][Dd][Ee][Rr][Aa][Tt][Ee]	return FEDERATE ;
"("[Ii][Nn][Tt][Ee][Rr][Aa][Cc][Tt][Ii][Oo][Nn][Ss]	return INTERACTIONS ;
"("[Oo][Bb][Jj][Ee][Cc][Tt][Ss]	return OBJECTS ;
[Rr][Ee][Cc][Ee][Ii][Vv][Ee] {
	certi::fedparser::order = certi::RECEIVE ;
	return ORDER ;
}
[Tt][Ii][Mm][Ee][Ss][Tt][Aa][Mm][Pp] {
	certi::fedparser::order = certi::TIMESTAMP ;
	return ORDER ;
}
"("[Pp][Aa][Rr][Aa][Mm][Ee][Tt][Ee][Rr]	return PARAMETER ;
"("[Ss][Ee][Cc]_[Ll][Ee][Vv][Ee][Ll]	return SEC_LEVEL ;
"("[Ss][Pp][Aa][Cc][Ee]	return SPACE ;
"("[Ss][Pp][Aa][Cc][Ee][Ss]	return SPACES ;
[Bb][Ee][Ss][Tt]_[Ee][Ff][Ff][Oo][Rr][Tt] {
	certi::fedparser::transport = certi::BEST_EFFORT ;	
	return TRANSPORT ;
}
[Rr][Ee][Ll][Ii][Aa][Bb][Ll][Ee] { 
	certi::fedparser::transport = certi::RELIABLE ; 
 	return TRANSPORT ; 
}
{ident}		{ certi::fedparser::arg = yytext ; return STRING ; }
\"[^"\n]*["]	{ 			
			certi::fedparser::arg = std::string(&yytext[1], 
				&yytext[yyleng - 1]) ;			
			return STRING ; 
		}
.		yyerror("invalid character");

%%

int yywrap() { 
	return 1 ;
}
