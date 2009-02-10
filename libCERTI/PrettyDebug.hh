// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
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
// $Id: PrettyDebug.hh,v 4.4.8.1 2009/02/10 18:25:15 erk Exp $
// ----------------------------------------------------------------------------

#ifndef PRETTYDEBUG_HH
#define PRETTYDEBUG_HH

#ifdef NO_PRETTYDEBUG ///< Deprecated, use NDEBUG
#define NDEBUG
#endif

#include "DebugOStream.hh"
#include <string>
#include "RTI.hh"

#define pdSEmptyMessage "Pretty Debug Server empty Message."

// Pretty Debug Basic types
typedef unsigned TDebugLevel;


// Pretty Debug Constants
#define pdMaxMessageSize 255  // greater then pdTooLongInitMessage length!

/** Do not use the pdUnused and pdLast Levels!!! Do not also specify
 *  any value for the elements, because order is used and missing
 *  value would cause crash.  pdLast must always be the last in the
 *  enum.  Key(see pdDebugKeys) */
enum pdDebugLevel  {pdUnused, /**< Do not use! : */
                    pdAnswer, /**< Server answer A */
                    pdCom, /**< Communication C */
                    pdDebug, /**< Debug D */
                    pdError, /**< Error E */
                    pdGendoc, /**< Gendoc G */
                    pdInit, /**< Initialization I */
                    pdMessage, /**< Message Type M */
                    pdProtocol, /**< Protocol P */
                    pdRegister, /**< Object Registration R */
                    pdRequest, /**< Client Request S */
                    pdTerm, /**< Terminate T */
                    pdWarning, /**< Warning W */
                    pdExcept, /**< Exceptions X */
                    pdTrace, /**< Trace Z */
                    pdLast}; /**< Do not use \0 */

// The following keys are used in the environment variable to enable
// debug level.  For example, if ENVVAR=D:C, the pdDebug and the pdCom
// debug level are enabled, and all others are not enabled.

// KEEP THE SAME ORDER AS IN THE pdDebugLevel ENUM!
#define pdDebugKeysString ":ACDEGIMPRSTWXZ\0"

//---------------------------------------------------------------------------
class CERTI_EXPORT PrettyDebug {
    
    public :
       static void setFederateName( const std::string &inName )
          { federateName_ = inName ; }
       static void setFederateName( const char *inName )
          { federateName_ = inName ; }
protected :
       static std::string federateName_ ;
private:
    char* LEnvVar;			/**< Name of the environment variable to look for. */
    char* LMessage;			/**< The container of all printed debug messages. 
					Start with the Header specified at construction time */
    char* HeaderMessage;	/**< The header message specified at construction time. */
 
    static DebugOStream defaultOutputStream;
    static DebugOStream* nullOutputStreamPtr;
    static DebugOStream& nullOutputStream;
    
    #ifdef _WIN32				//Mutex : Print Control
		 static HANDLE	g_hMutex;
    #endif
 
    /** If Level_Map[Level] != &PrettyDebug::nullOutputStream, then the
       debug message must be printed in the ostream addressed. */
    DebugOStream* Level_Map [pdLast + 1];

    void Print(DebugOStream& theOutputStream, 
                      const char* theHeaderMessage, 
                      const char * Message);

    // Parse the environment variable value to find debug keys and
    // enable debug levels
    void ParseEnvString(const char * Name);

public:  
    PrettyDebug(const char * Name, const char * Header);
    ~PrettyDebug();
  
  
    bool Mode(pdDebugLevel Level);

    void enableDebugLevel(pdDebugLevel Level, 
                          DebugOStream& theOutputStream = PrettyDebug::defaultOutputStream);
    void disableDebugLevel(pdDebugLevel Level);
  
    DebugOStream& operator[](pdDebugLevel Level) //Inline method
    {
        DebugOStream* theReturnedOutputStreamPtr = (Level_Map[Level]);
        if (theReturnedOutputStreamPtr == PrettyDebug::nullOutputStreamPtr) {
            return(PrettyDebug::nullOutputStream);
        }
        Print( *theReturnedOutputStreamPtr, HeaderMessage, "" );
        return(*theReturnedOutputStreamPtr);
    }
 
#ifdef NDEBUG
    inline void Out(pdDebugLevel Level, const char *Format, ...) {};
#else
	void Out(pdDebugLevel Level, const char *Format, ...);
	void Mes(pdDebugLevel Level, const char type, const short testMess, const char *context);

#endif

};


// Alias
typedef PrettyDebug pdCDebug ; ///< \deprecated pdCDebug replaced by PrettyDebug

#endif // PRETTYDEBUG_HH

// $Id: PrettyDebug.hh,v 4.4.8.1 2009/02/10 18:25:15 erk Exp $
