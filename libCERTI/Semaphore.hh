#ifndef SEMAPHORE_H
#define SEMAPHORE_H

// Systems includes
#include <string>

class Semaphore {

protected:
	/**
	 * Build a proper Semaphore name from a user provided name.
	 * Idea kindly borrowed from TSP (https://savannah.nongnu.org)
	 * bb_utils_build_sem_name
	 */
	std::string buildSemName(std::string& SemName) {
		return "/"+SemName+"_sem";
	}

    public :
    // Semaphore() ;
    virtual ~Semaphore() {};
    // virtual void Sem_create_init(int initval, char * semname) = 0 ;
    // virtual void Sem_create(key_t cle) = 0 ;
    // virtual void Sem_init(int initval) = 0 ;
    virtual void P() = 0 ;
    virtual void V() = 0 ;
    virtual void Delete() = 0 ;

} ;

#endif
