#ifndef udCloudFile_h__
#define udCloudFile_h__

//! @file udCloudFile.h
//! The udCloudFile object provide an interface for accessing data of files hosted in udCloud

#include <stdint.h>

//!
//! @struct udCloudFile
//! This represents a udCloud File
//! 
struct udCloudFile
{
  char ID[64]; //!< The id from udCloud
  char projID[64]; //!< The project id it belongs to from udCloud
  char *pPath; //!< The path of the udCloud File
  char *pCreated; //!< The time this udCloud File was created
  char *pTier; //!< The tier where this udCloud File is stored in udCloud
  char *pTimeToLive;//!< If the udCloud File will be auto-deleted this is the timestamp for when that will occur
  uint64_t size; //!< The size of the udCloud File
  uint32_t isFolder; //!< != 0 if the udCloud File is a folder
  uint32_t isShared; //!< != 0 if the udCloud File is shared
};

#endif //udCloudFile_h__
