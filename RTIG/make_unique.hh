#if __GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ > 9))

#include <memory>

namespace std {

template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

}

#endif
