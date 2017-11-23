// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003, 2004  ONERA
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

#ifndef _CERTI_AUDIT_FILE_HH
#define _CERTI_AUDIT_FILE_HH

#include "AuditLine.hh"
#include "Exception.hh"
#include <include/certi.hh>

#include <fstream>
#include <string>

namespace certi {

// ----------------------------------------------------------------------------
/** AuditFile class is used to store information into a file for later usage.
 * 
 * First call the startLine method to give background information about the
 * next current audit line. The line is then prepared internally, but not yet
 * written to the audit file. You can then set the Level of the line
 * separately. If the Level is below the fixed Audit level, nothing will be
 * stored or written for this event (except in the case of an exception). The
 * default level is the lowest one. Each call to addToLine or to addToLine
 * adds the parameter string to the current line. Then a last call to EndLine
 * will set the line's status (or Result) and flush the line into the Audit
 * file.
 */
class CERTI_EXPORT AuditFile {
public:
    /** AuditFile constructor to write to file
     * 
     * Audit file is used to store information about actions taken by the RTIG
     */
    AuditFile(const std::string& log_file_name); // Open LogFileName for writing.

    /** delete an AuditFile instance.
     * 
     * if a line is currently being processed, close it. Before closing the file,
     * adds a specific end line.
     */
    ~AuditFile();

    /// start a new line and set with parameters.
    void startLine(const Handle federation_handle, const FederateHandle federate_handle, const AuditLine::Type type);

    /** setLevel change the event level.
     * 
     * event level is used to determine if information has to be inserted into
     * file. Level is only used by endLine module.
     */
    void setLevel(const AuditLine::Level level);

    /** Adds last information about current line and writes it to file.
     * 
     * Completes a line previously initialized by a newLine call. Appends the
     * current status and a comment. Then write line to file.
     */
    void endLine(const AuditLine::Status status, const std::string& reason);

    /** creates a new line with parameters and writes this line to file.
     * 
     * Sometimes, you may want to directly put a line in the audit without
     * calling 3 methods : you can also use the following PutLine method in case
     * of an emergency. The line is written immediatly, even before any currently
     * builded audit line. The federation and federate numbers are set to(0, 0).
     */
    void putLine(const AuditLine::Type type,
                 const AuditLine::Level level,
                 const AuditLine::Status status,
                 const std::string& reason);

    AuditFile& operator<<(const char*);
    AuditFile& operator<<(const std::string& s);
    AuditFile& operator<<(const int n);
    AuditFile& operator<<(const unsigned int n);
    AuditFile& operator<<(const long n);
    AuditFile& operator<<(const unsigned long n);
    AuditFile& operator<<(const double n);

    template <typename T>
    AuditFile& operator<<(const Printable<T>& handle)
    {
        std::stringstream ss;
        handle.print(ss);
        return (*this << ss.str());
    }

protected:
    std::ofstream my_audit_file; /// Stream pointer to output file.
    AuditLine my_current_line; /// Line currently being processed.
};

} // namespace certi

#endif // _CERTI_AUDIT_FILE_HH
