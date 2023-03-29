#ifndef udDLLExport_h__
#define udDLLExport_h__

// Generic helper definitions for shared library support
#if defined(_WIN32)
# define UDSDKDLL_IMPORT __declspec(dllimport)
# define UDSDKDLL_EXPORT __declspec(dllexport)
#elif defined(EMSCRIPTEN)
# include <emscripten.h>
# define UDSDKDLL_IMPORT EMSCRIPTEN_KEEPALIVE
# define UDSDKDLL_EXPORT EMSCRIPTEN_KEEPALIVE
#else
# define UDSDKDLL_IMPORT __attribute__ ((visibility ("default")))
# define UDSDKDLL_EXPORT __attribute__ ((visibility ("default")))
#endif

#if defined(UDSDKSTATIC) // defined if udSDK was compiled, or is currently compiling, as a static library
# define UDSDKDLL_API
#elif defined(UDSDKDLL) // defined if udSDK is currently compiling as a shared library
# define UDSDKDLL_API UDSDKDLL_EXPORT
#else
# define UDSDKDLL_API UDSDKDLL_IMPORT
#endif // UDSDKDLL

#endif // udDLLExport_h__
