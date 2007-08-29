%{
// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2003  ONERA
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
// $Id: syntax.yy,v 3.5 2007/08/29 13:07:58 erk Exp $
// ----------------------------------------------------------------------------

#include "fed.hh"
#include <iostream>

using std::cout ;
using std::endl ;

namespace certi {
namespace fedparser {

extern std::string arg ;
extern const char *fed_filename ;
extern int line_number ;
}}

int yylex();
int yyerror(char *);

%}

%token IDENT
%token STRING
%token L_PAR
%token R_PAR

%token ATTRIBUTE
%token CLASS
%token DIMENSION
%token FED
%token FED_VERSION
%token FEDERATION
%token FEDERATE
%token INTERACTIONS
%token OBJECTS
%token ORDER
%token PARAMETER
%token SEC_LEVEL
%token SPACE
%token SPACES
%token TRANSPORT

%start fed

%%

fed:
	FED { certi::fedparser::startFed(); }
	federation 
	fed_version 
	federates 
	spaces 
	objects 
	interactions { certi::fedparser::endFed(); }
	R_PAR ;

federation:
	FEDERATION STRING { certi::fedparser::addFederation(); }
	R_PAR ;

fed_version:
	FED_VERSION STRING { certi::fedparser::addFedVersion(); }
	R_PAR ;

federates:
	federate_list
	| ;

federate_list:
	federate
	| federate_list federate ;

federate:
	FEDERATE STRING { certi::fedparser::startFederate(); }
	STRING { certi::fedparser::endFederate(); }
	R_PAR ;

spaces:
	SPACES { certi::fedparser::startSpaces(); }
	opt_space_list { certi::fedparser::end(); }
	R_PAR ;

opt_space_list:
	space_list
	| ;

space_list:
	space 
	| space_list space ;

space:
	SPACE STRING { certi::fedparser::startSpace(); }
	opt_dimension_list { certi::fedparser::endSpace(); }
	R_PAR ;

opt_dimension_list:
	dimension_list
	| ;

dimension_list:
	dimension
	| dimension_list dimension ;

dimension:
	DIMENSION STRING { certi::fedparser::addDimension(); }
	R_PAR ;

objects:
	OBJECTS { certi::fedparser::startObjects(); }
	opt_object_class_list { certi::fedparser::end(); }
	R_PAR ;

opt_object_class_list:
	object_class_list 
	| ;

object_class_list:
	object_class
	| object_class_list object_class ;

object_class:
	CLASS STRING { certi::fedparser::startObject(); }
	object_class_items { certi::fedparser::endObject(); }
	R_PAR ;

object_class_items:
	attribute_list
	| object_security_level attribute_list
	| object_class_list
	| object_security_level object_class_list
	| attribute_list object_class_list
	| object_security_level attribute_list object_class_list
	| ;

attribute_list:
	attribute
	| attribute_list attribute ;

attribute:
	ATTRIBUTE STRING TRANSPORT ORDER { certi::fedparser::addAttribute(); }
	R_PAR ;

interactions:
	INTERACTIONS { certi::fedparser::startInteractions(); }
	opt_interaction_class_list { certi::fedparser::end(); }
	R_PAR ;

opt_interaction_class_list:
	interaction_class_list 
	| ;

interaction_class_list:
	interaction_class
	| interaction_class_list interaction_class ;

interaction_class:
	CLASS STRING TRANSPORT ORDER { certi::fedparser::startInteraction(); }
	interaction_class_items { certi::fedparser::endInteraction(); }
	R_PAR ;

interaction_class_items:	
	parameter_list
	| interaction_security_level parameter_list
	| interaction_class_list
	| interaction_security_level interaction_class_list
	| parameter_list interaction_class_list
	| interaction_security_level parameter_list interaction_class_list
	| ;

parameter_list:
	parameter
	| parameter_list parameter ;

parameter:
	PARAMETER STRING { certi::fedparser::addParameter(); }
	R_PAR ;

interaction_security_level:
	SEC_LEVEL STRING { certi::fedparser::addInteractionSecurityLevel(); }
	R_PAR ;

object_security_level:
	SEC_LEVEL STRING { certi::fedparser::addObjectSecurityLevel(); }
	R_PAR ;

%%

int yyerror(char *s) {
    cout << endl << certi::fedparser::fed_filename << ":" 
	 << certi::fedparser::line_number << ": " << s << endl ;
    return 0 ;
}
