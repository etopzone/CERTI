// HLA 1.3 Header "baseTypes.hh"

typedef unsigned short UShort ;
typedef short Short ;
#if defined(__alpha) || (defined(__sgi) && _MIPS_SZLONG == 64)
typedef unsigned int ULong ;
typedef int Long ;
#else
typedef unsigned long ULong ;
typedef long Long ;
#endif
typedef double Double ;
typedef float Float ;

enum Boolean {
    RTI_FALSE = 0,
    RTI_TRUE
};

class RTI_EXPORT Exception
{
public:
    ULong _serial ;
    char *_reason ;
    const char *_name ;
    Exception(const char *reason);
    Exception(ULong serial, const char *reason);
    Exception(const Exception &toCopy);
    virtual ~Exception();
    Exception &operator=(const Exception &);
    virtual Exception *cloneSelf() const throw() = 0 ;
    virtual void throwSelf() const = 0 ;
    const std::string displayMe() const;
};

#define RTI_EXCEPT(A) \
class A : public Exception { \
public: \
    static RTI_EXPORT const char *_ex ; \
    static RTI_EXPORT long type ; \
    A (const char *reason) : Exception(reason) { _name = #A ; this->displayMe();} \
    A (ULong serial, const char *reason = 0) \
        : Exception(serial, reason) { _name = #A ; this->displayMe(); } \
    A (A const &toCopy) : Exception(toCopy) { _name = #A ; this->displayMe();} \
    Exception *cloneSelf() const throw() { return (new A(_reason)); } \
    void throwSelf() const { throw *this ; } \
};
