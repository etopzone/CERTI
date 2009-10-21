#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include "certi.hh"
#include "Exception.hh"
// Systems includes
#include <string>

class CERTI_EXPORT Semaphore {

protected:

    public :
    // Semaphore() ;
    virtual ~Semaphore() {};
    virtual void Create_Init(const int initval, const std::string& New_Semname) = 0 ;
    virtual void Attach(const std::string& New_Semname ) = 0 ;
    virtual void P() = 0 ;
    virtual void V() = 0 ;
    virtual void Delete() = 0 ;


   	/**
	 * Build a proper Semaphore name from a user provided name.
	 * Idea kindly borrowed from TSP (https://savannah.nongnu.org)
	 * bb_utils_build_sem_name
	 */
	static std::string buildSemName(const std::string& SemName) {
		return "/"+SemName+"_sem";
	}

} ;

#endif
