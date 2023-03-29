#ifndef udCloudProject_h__
#define udCloudProject_h__

//! @file udCloudProject.h
//! The udCloudProject object provide an interface for accessing data of projects hosted in udCloud

#include <stdint.h>

#include "udCloudFile.h"
#include "udCloudScene.h"
#include "udDLLExport.h"
#include "udError.h"

#ifdef __cplusplus
extern "C" {
#endif

struct udContext;

//!
//! @struct udCloudStorageVolume
//! The values for Hot and Archive data in udCloud
//!
struct udCloudStorageVolume
{
  double hot; //!< The amount of stored data in Hot 
  double archive; //!< The amount of stored data in Archive 
};

//!
//! @struct udCloudProject
//! This represents a udCloud Project
//! 
struct udCloudProject
{
  char ID[64]; //!< The project id from udCloud
  char orgID[64]; //!< The workspace it belongs to from udCloud
  char *pName; //!< The name of the project
  char *pRegion; //!< The region the data is hosted on udCloud
  char createdBy[64]; //!< The user id of the user who created this project
  char *pCreated; //!< The time this project was created
  uint64_t optionalPermissions; //!< The permissions of this project
  struct udCloudStorageVolume volume; //!< Hot And Archives volume stored in udCloud
  uint64_t processingtime; //!< The processing time used to convert
};

//!
//! Get a list of available Files owned by a specific Project in udCloud
//!
//! @param pContext The pointer to the udContext of the session to use
//! @param pCloudProject The pointer of the udCloudProject.
//! @param ppCloudFiles A list of Files returned.
//! @param pCount The number of Files in ppCloudFiles.
//! @return A udError value based on the result of the query on udCloud.
//!
UDSDKDLL_API enum udError udCloudProject_GetFileList(struct udContext *pContext, struct udCloudProject *pCloudProject, struct udCloudFile **ppCloudFiles, int *pCount);

//!
//! Destroys the list of Files that was created by udCloudProject_GetFileList
//!
//! @param ppCloudFiles The list of Files to be destroyed.
//! @param count The number of Files in ppCloudFiles.
//! @return A udError value based on the memory been freed.
//!
UDSDKDLL_API enum udError udCloudProject_ReleaseFileList(struct udCloudFile **ppCloudFiles, int count);

//!
//! Get a list of available scenes owned by a specific Project in udCloud
//!
//! @param pContext The pointer to the udContext of the session to use
//! @param pCloudProject The pointer of the udCouldProject.
//! @param ppCloudScenes A list of Scenes returned.
//! @param pCount The number of scenes in ppCloudScenes.
//! @return A udError value based on the result of the query on udCLoud.
//!
UDSDKDLL_API enum udError udCloudProject_GetSceneList(struct udContext *pContext, struct udCloudProject *pCloudProject, struct udCloudScene **ppCloudScenes, int *pCount);

//!
//! Destroys the list of Scenes that was created by udCloudProject_GetSceneList
//!
//! @param ppCloudScenes The list of Scenes to be destroyed.
//! @param count The number of Scenes in ppCloudScenes.
//! @return A udError value based on the memory been freed.
//!
UDSDKDLL_API enum udError udCloudProject_ReleaseSceneList(struct udCloudScene **ppCloudScenes, int count);

#ifdef __cplusplus
}
#endif

#endif //udCloudProject_h__
