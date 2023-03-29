#ifndef udCloudScene_h__
#define udCloudScene_h__

//! @file udCloudScene.h
//! The udCloudScene object provide an interface for accessing data of Scenes hosted in udCloud

#include <stdint.h>

//!
//! @struct udCloudScene
//! This represents a udCloud Scene
//! 
struct udCloudScene
{
  char *pName; //!< The name of the Scene
  char ID[64]; //!< The sceneid from udCloud
  char orgID[64]; //!< The workspace it belongs to from udCloud
  char projID[64]; //!< The project it belongs to from udCloud
  char *pRegion; //!< The region the data is hosted on udCloud
  char *pCreated; //!< The time this scene was created
  char *pLastUpdated; //!< The time this scene was last updated
  char *pDeletedTime; //!< The time this scene was deleted
  uint32_t isShared; //!< Is the scene shared
  char *pShortcode; //!< The short code for this scene
};

#endif //udCloudScene_h__
