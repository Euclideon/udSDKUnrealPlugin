#ifndef udVersion_h__
#define udVersion_h__

//! @file udVersion.h
//! The **udVersion** object provides an interface to query the version of the loaded udSDK library.

#include <stdint.h>
#include "udDLLExport.h"
#include "udError.h"

#ifdef __cplusplus
extern "C" {
#endif

//! The major version part of the library version
# define UDSDK_MAJOR_VERSION 2
//! The minor version part of the library version
# define UDSDK_MINOR_VERSION 3
//! The patch version part of the library version
# define UDSDK_PATCH_VERSION 1

  //!
  //! Stores the version information for the loaded udSDK library.
  //!
  struct udVersion
  {
    uint8_t major; //!< The major version part of the library version
    uint8_t minor; //!< The minor version part of the library version
    uint8_t patch; //!< The patch version part of the library version
  };

  //!
  //! Populates the provided structure with the version information for the loaded udSDK library.
  //!
  //! @param pVersion The version structure which will be populated with the version information.
  //! @return A udError value based on the result of getting the version information.
  //!
  UDSDKDLL_API enum udError udVersion_GetVersion(struct udVersion *pVersion);

#ifdef __cplusplus
}
#endif

#endif // udVersion_h__
