#ifndef SHM_H
#define SHM_H

#include <string>
#include "libhla.hh"

namespace libhla {
namespace ipc {


/**
 * A Shared Memory class.
 * This class is an abstract base for representing a shared memory
 * segment. A shared memory segment is identified by its name and
 * has a size and a creator process.
 */
class HLA_EXPORT SHM {

public :
	/**
	 * Constructor.
	 * @param[in] SHMName, the name of the shared memory segment.
	 *            this should be unique on a specified host.
	 * @param[in] SHMSize, the requested size of the SHM
	 * @param[in] creator, true if we are trying to create the SHM, false
	 *            if we want to attach to the SHM.
	 */
	SHM(const std::string& SHMName, const int SHMSize, const bool creator)
	{_Name=SHMName;
	_Size=SHMSize;
	_IsCreator=creator;
	}
	SHM(const std::string& SHMName, const int SHMSize)
	{_Name=SHMName;
	_Size=SHMSize;
	_IsCreator=false;
	}
	virtual ~SHM() {};

	LIBHLA_EXCEPTION(SharedMemoryNotOpen)
	LIBHLA_EXCEPTION(SharedMemoryNotAttached)
	LIBHLA_EXCEPTION(SharedMemoryNotClosed)
	LIBHLA_EXCEPTION(HandleNotClosed)

	const std::string getName() const {return _Name ; } ;
	inline void* GetShm(){return _Shm ; } ;
	inline bool IsCreator(){return _IsCreator ; } ;
	inline int GetSize(){return _Size ; } ;
	virtual void Open() = 0 ;
	virtual void Attach() = 0 ;
	virtual void Close() = 0 ;

	/**
	 * Build a proper SHM name from a user provided name.
	 * Idea kindly borrowed from TSP (https://savannah.nongnu.org)
	 * bb_utils_build_shm_name
	 */
	static std::string buildShmName(const std::string& ShmName) {
		return "/"+ShmName+"_shm";
	}
protected :
	std::string _Name ;
	int  _Size ;
	bool _IsCreator ;
	void *_Shm ;
} ;
} /* end namespace ipc  */
} /* end namespace libhla */
#endif
