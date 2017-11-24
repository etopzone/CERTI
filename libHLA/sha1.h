/*
 * This code has been taken from RFC 3174 on 
 * http://ww.rfc-editors.org/
 * File has been renamed to sha1.[hc] to avoid name clash.
 * 
 *  sha1.h
 *
 *  Description:
 *      This is the header file for code which implements the Secure
 *      Hashing Algorithm 1 as defined in FIPS PUB 180-1 published
 *      April 17, 1995.
 *
 *      Many of the variable names in this code, especially the
 *      single character names, were used because those were the names
 *      used in the publication.
 *
 *      Please read the file sha1.c for more information.
 *
 */
#ifndef _SHA1_H_
#define _SHA1_H_

#ifdef _MSC_VER
    typedef unsigned __int64 uint64_t;
    typedef signed __int64 int64_t;
    typedef unsigned __int32 uint32_t;
    typedef signed __int32 int32_t;
    typedef unsigned __int16 uint16_t;
    typedef signed __int16 int16_t;
    typedef unsigned __int8 uint8_t;
    typedef signed __int8 int8_t;
    typedef short int int_least16_t;
#else
    #include <stdint.h>
#endif

#if defined(_WIN32) || defined(__CYGWIN__)
    #if defined(_MSC_VER)
        #pragma warning(disable : 4251)
    #endif
    #define ANY_DLL_EXPORT __declspec(dllexport)
    #define ANY_DLL_IMPORT __declspec(dllimport)
    #define ANY_DLL_LOCAL
#else
    #if (__GNUC__ >= 4)
        #define ANY_DLL_EXPORT __attribute__((visibility("default")))
        #define ANY_DLL_IMPORT __attribute__((visibility("default")))
        #define ANY_DLL_LOCAL __attribute__((visibility("hidden")))
    #else
        #define ANY_DLL_EXPORT
        #define ANY_DLL_IMPORT
        #define ANY_DLL_LOCAL
    #endif
#endif

#if defined(HLA_EXPORTS)
    #define HLA_EXPORT ANY_DLL_EXPORT
#else
    #define HLA_EXPORT ANY_DLL_IMPORT
#endif

/* BEGIN_C_DECLS should be used at the beginning of your declarations,
   so that C++ compilers don't mangle their names.  Use END_C_DECLS at
   the end of C declarations. */
#undef BEGIN_C_DECLS
#undef END_C_DECLS
#ifdef __cplusplus
    #define BEGIN_C_DECLS extern "C" {
    #define END_C_DECLS }
#else
    #define BEGIN_C_DECLS /* empty */
    #define END_C_DECLS /* empty */
#endif

/*
 * If you do not have the ISO standard stdint.h header file, then you
 * must typedef the following:
 *    name              meaning
 *  uint32_t         unsigned 32 bit integer
 *  uint8_t          unsigned 8 bit integer (i.e., unsigned char)
 *  int_least16_t    integer of >= 16 bits
 *
 */

#ifndef _SHA_enum_
#define _SHA_enum_
enum {
    shaSuccess = 0,
    shaNull, /* Null pointer parameter */
    shaInputTooLong, /* input data too long */
    shaStateError /* called Input after Result */
};
#endif
#define SHA1HashSize 20

/*
 *  This structure will hold context information for the SHA-1
 *  hashing operation
 */
typedef struct SHA1Context {
    uint32_t Intermediate_Hash[SHA1HashSize / 4]; /* Message Digest  */

    uint32_t Length_Low; /* Message length in bits      */
    uint32_t Length_High; /* Message length in bits      */

    /* Index into message block array   */
    int_least16_t Message_Block_Index;
    uint8_t Message_Block[64]; /* 512-bit message blocks      */

    int Computed; /* Is the digest computed?         */
    int Corrupted; /* Is the message digest corrupted? */
} SHA1Context;

BEGIN_C_DECLS

/*
 *  Function Prototypes
 */
HLA_EXPORT int SHA1Reset(SHA1Context*);
HLA_EXPORT int SHA1Input(SHA1Context*, const uint8_t*, unsigned int);
HLA_EXPORT int SHA1Result(SHA1Context*, uint8_t Message_Digest[SHA1HashSize]);

END_C_DECLS

#endif
