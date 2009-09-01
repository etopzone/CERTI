#ifndef SHM_H
#define SHM_H

#include <string>

/**
 * A Shared Memory class.
 * This class is an abstract base for representing a shared memory
 * segment. A shared memory segment is identified by its name and
 * has a size and a creator process.
 */
class SHM {
protected :
	std::string _Name ;
	int  _Size ;
	bool _IsCreator ;
	void *_Shm ;

	/**
	 * Build a proper SHM name from a user provided name.
	 * Idea kindly borrowed from TSP (https://savannah.nongnu.org)
	 * bb_utils_build_shm_name
	 */
	std::string buildShmName(std::string& ShmName) {
	  return "/"+ShmName+"_shm";
	}

public :
	/**
	 * Constructor.
	 * @param[in] SHNName, the name of the shared memory segment.
	 *            this should be unique on a specified host.
	 * @param[in] SHMSize, the requested size of the SHM
	 * @param[in] isCreator,
	 */
	SHM(const std::string& SHMName, const int SHMSize, const bool isCreator)
		{_Name=SHMName;
		 _Size=SHMSize;
		 _IsCreator=isCreator;
		 }
	virtual ~SHM() {};
	const std::string getName() const {return _Name ; } ;
	inline void* GetShm(){return _Shm ; } ;
	inline bool IsCreator(){return _IsCreator ; } ;
	inline int GetSize(){return _Size ; } ;
	virtual void Open() = 0 ;
	virtual void Attach() = 0 ;
	virtual void Close() = 0 ;
} ;

#endif
