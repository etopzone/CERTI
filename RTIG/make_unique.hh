#ifndef MAKE_UNIQUE_FOR_OLDER_COMPILERS
#define MAKE_UNIQUE_FOR_OLDER_COMPILERS

#if defined(__clang__)

using std::make_unique;

#elif __GNUC__ < 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ < 9))

#include <memory>

template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

#else

using std::make_unique;

#endif

#endif
