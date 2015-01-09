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
// $Id: syntax.yy,v 3.10 2011/12/29 17:59:28 erk Exp $
// ----------------------------------------------------------------------------

#include "fed.hh"
#include <iostream>

using std::cout ;
using std::cerr ;
using std::endl ;

namespace certi {
namespace fedparser {

extern std::string arg ;
extern const char *fed_filename ;
extern int line_number ;
extern std::string federationname_arg;
extern std::string federatename_arg;
extern std::string timestamp_arg;
extern std::string spacename_arg ;
extern std::string dimensionname_arg;
extern std::string objectclassname_arg;
extern std::string attributename_arg;
extern std::string interactionclassname_arg;
extern std::string parametername_arg;

}}

int yylex();
int yyerror(const char *);

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
%token TIMESTAMP_TOKEN

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
	FEDERATION STRING { certi::fedparser::federationname_arg =  certi::fedparser::arg;
	                    certi::fedparser::addFederation(); }
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
	FEDERATE STRING { certi::fedparser::federatename_arg =  certi::fedparser::arg;
	                  certi::fedparser::startFederate(); }
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
	SPACE STRING { certi::fedparser::spacename_arg =  certi::fedparser::arg;
	               certi::fedparser::startSpace(); }
	opt_dimension_list { certi::fedparser::endSpace(); }
	R_PAR ;

opt_space_name:
	STRING { certi::fedparser::spacename_arg =  certi::fedparser::arg;
	         certi::fedparser::checkSpaceName(); }
	| ;

opt_dimension_list:
	dimension_list
	| ;

dimension_list:
	dimension
	| dimension_list dimension ;

dimension:
	DIMENSION STRING { certi::fedparser::dimensionname_arg =  certi::fedparser::arg;
	                   certi::fedparser::addDimension(); }
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
	CLASS STRING { certi::fedparser::objectclassname_arg =  certi::fedparser::arg;
	               certi::fedparser::startObject(); }
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
      attribute_named_ts
    | attribute_prefix attribute_ro
    | attribute_prefix attribute_ts;
    
attribute_named_ts:
	ATTRIBUTE TIMESTAMP_TOKEN
	{ certi::fedparser::attributename_arg = certi::fedparser::timestamp_arg;} 
	TRANSPORT ORDER opt_space_name 	                                            
	{ certi::fedparser::addAttribute(); }
	R_PAR ;

attribute_prefix:
	ATTRIBUTE STRING
	{ certi::fedparser::attributename_arg = certi::fedparser::arg;}
	TRANSPORT;
	
attribute_ts:
	TIMESTAMP_TOKEN opt_space_name 
	{ certi::fedparser::addAttribute(); }	
	R_PAR ;

attribute_ro:
    ORDER opt_space_name 
	{ certi::fedparser::addAttribute(); }
	R_PAR ;

interactions:
	INTERACTIONS 
	{ certi::fedparser::startInteractions(); }
	opt_interaction_class_list 
	{ certi::fedparser::end(); }
	R_PAR ;

opt_interaction_class_list:
	interaction_class_list 
	| ;

interaction_class_list:
	interaction_class
	| interaction_class_list interaction_class ;
	
interaction_class:
    interaction_class_prefix interaction_class_ts
    | interaction_class_prefix interaction_class_ro;

interaction_class_prefix:
	CLASS STRING
	{ certi::fedparser::interactionclassname_arg = certi::fedparser::arg;}
	TRANSPORT;

interaction_class_ts:	
	TIMESTAMP_TOKEN opt_space_name 
	{ certi::fedparser::startInteraction(); }
	interaction_class_items 
	{ certi::fedparser::endInteraction(); }
	R_PAR ;

interaction_class_ro:
	ORDER opt_space_name 
	{ certi::fedparser::startInteraction(); }
	interaction_class_items 
	{ certi::fedparser::endInteraction(); }
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
	PARAMETER TIMESTAMP_TOKEN 
	{certi::fedparser::parametername_arg = certi::fedparser::timestamp_arg; 
	 certi::fedparser::addParameter(); }
	R_PAR ;

parameter:
	PARAMETER STRING 
	{ certi::fedparser::parametername_arg = certi::fedparser::arg;
	  certi::fedparser::addParameter(); }
	R_PAR ;

interaction_security_level:
	SEC_LEVEL STRING { certi::fedparser::addInteractionSecurityLevel(); }
	R_PAR ;

object_security_level:
	SEC_LEVEL STRING { certi::fedparser::addObjectSecurityLevel(); }
	R_PAR ;

%%

int yyerror(const char *s) {
    cerr << endl << certi::fedparser::fed_filename << ":" 
         << certi::fedparser::line_number << ": " << s << endl ;
    return 0 ;
}
