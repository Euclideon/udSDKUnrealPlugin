#ifndef udCloudWorkspace_h__
#define udCloudWorkspace_h__

//! @file udCloudWorkspace.h
//! The udCloudWorkspace object provides an interface for accessing data of workspace hosted in udCloud

#include <stdint.h>

#include "udCloudProject.h"
#include "udDLLExport.h"
#include "udError.h"

#ifdef __cplusplus
extern "C" {
#endif

//!
//! @struct udCloudWorkspace
//! This represents a udCloud Workspace
//! 
struct udCloudWorkspace
{
  char ID[64]; //!< The workspace id from udCloud
  char *pName; //!< The name of the workspace
  uint64_t permissions; //!< The permissions of this workspace
  struct udCloudStorageVolume volume; //!< Hot And Archives volume stored in udCloud
  uint32_t isPAYG; //!< A flag for PAYG workspace
  uint32_t isPendingDelete; //!< A flag to know if the workspace will be deleted shortly
};

//!
//! Get a list of available Workspaces hosted in udCloud
//!
//! @param pContext The pointer to the udContext of the session to use.
//! @param ppCloudWorkspaces The pointer pointer of the udCloudWorkspace.
//! @param pCount The number of Workspace in ppCloudWorkspaces.
//! @return A udError value based on the result of the query on udCloud.
//!
UDSDKDLL_API enum udError udCloudWorkspace_GetWorkspaceList(struct udContext *pContext, struct udCloudWorkspace **ppCloudWorkspaces, int *pCount);

//!
//! Destroys the list of Workspaces that was created by udCloudWorkspace_GetWorkspaceList
//!
//! @param ppCloudWorkspaces The list of Workspaces to be destroyed.
//! @param count The number of udCloudWorkspace in ppCloudWorkspaces.
//! @return A udError value based on the memory been freed.
//!
UDSDKDLL_API enum udError udCloudWorkspace_ReleaseWorkspaceList(struct udCloudWorkspace **ppCloudWorkspaces, int count);

//!
//! Get a list of available Projects owned by a specific Workspace in udCloud
//!
//! @param pContext The pointer to the udContext of the session to use.
//! @param pCloudWorkspace The pointer of the udCouldWorkspace.
//! @param ppCloudProjects A list of Projects returned.
//! @param pCount The number of projects in ppCloudProject.
//! @return A udError value based on the result of the query on udCloud.
//!
UDSDKDLL_API enum udError udCloudWorkspace_GetProjectList(struct udContext *pContext, struct udCloudWorkspace *pCloudWorkspace, struct udCloudProject **ppCloudProjects, int *pCount);

//!
//! Destroys the list of Projects that was created by udCloudWorkspace_GetProjectList
//!
//! @param ppCloudProjects The list of Projects to be destroyed.
//! @param count The number of udCloudProject in ppCloudProjects.
//! @return A udError value based on the memory been freed.
//!
UDSDKDLL_API enum udError udCloudWorkspace_ReleaseProjectList(struct udCloudProject **ppCloudProjects, int count);

#ifdef __cplusplus
}
#endif

#endif //udCloudWorkspace_h__
