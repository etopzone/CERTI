// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
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
// ----------------------------------------------------------------------------

#ifndef _CERTI_AUDIT_LINE_HH
#define _CERTI_AUDIT_LINE_HH

#include "certi.hh"
#include "Exception.hh"

#include <fstream>
#include <string>

namespace certi {

class CERTI_EXPORT AuditLine {

public:
    AuditLine();
    AuditLine(unsigned short, unsigned short, unsigned short, const std::string&);
    ~AuditLine();

    void write(std::ofstream &); //!< Write data to file
    void addComment(const std::string &); //!< Add str at the end of comment.
    void end(unsigned short event_status = e_NO_EXCEPTION,
	     const std::string& reason = "");
    unsigned short getLevel() const { return level ; };
    unsigned short getStatus() const { return status ; };
    bool started() const { return modified ; };
    void setFederation(Handle h);
    void setFederate(FederateHandle h);
    void setLevel(unsigned short l);

private:
    Handle federation ;
    FederateHandle federate ;
    unsigned short type ;
    unsigned short level ;
    unsigned short status ;
    bool modified ;

    time_t date ; //!< date, automatically set at construction time.
    std::string comment ; //!< comment internally managed.
};

}

#endif // _CERTI_AUDIT_LINE_HH
