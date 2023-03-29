#ifndef udPointCloud_h__
#define udPointCloud_h__

//! @file udPointCloud.h
//! The **udPointCloud** object provides an interface to load a Euclideon Unlimited Detail model.
//! Once instantiated, the **udPointCloud** can be queried for metadata information, and rendered with the udRenderContext functions.
//! Future releases will allow users to also query the pointcloud data itself, providing the ability to export to LAS or render sub-sections of the pointcloud.

#include "udDLLExport.h"
#include "udAttributes.h"
#include "udError.h"

#ifdef __cplusplus
extern "C" {
#endif

struct udContext;
struct udGeometry;

//!
//! @struct udVoxelID
//! Combines the traverse context and node index to uniquely identify a node
//!
struct udVoxelID
{
  uint64_t index; //!< Internal index value
  const void *pTrav; //!< Internal traverse info
  void *pRenderInfo; //!< Internal render info
};

//!
//! @struct udPointCloudHeader
//! Stores basic information about a udPointCloud
//!
struct udPointCloudHeader
{
  double scaledRange; //!< The point cloud's range multiplied by the voxel size
  double unitMeterScale; //!< The scale to apply to convert to/from metres (after scaledRange is applied to the unitCube)
  uint32_t totalLODLayers; //!< The total number of LOD layers in this octree
  double convertedResolution; //!< The resolution this model was converted at
  double storedMatrix[16]; //!< This matrix is the 'default' internal matrix to go from a unit cube to the full size

  struct udAttributeSet attributes;   //!< The attributes contained in this pointcloud

  double baseOffset[3]; //!< The offset to the root of the pointcloud in unit cube space
  double pivot[3]; //!< The pivot point of the model, in unit cube space
  double boundingBoxCenter[3]; //!< The center of the bounding volume, in unit cube space
  double boundingBoxExtents[3]; //!< The extents of the bounding volume, in unit cube space
};

//!
//! @struct udPointCloudLoadOptions
//! Contains additional loading options passed to `udPointCloud_LoadAdv` 
//!
struct udPointCloudLoadOptions
{
  uint32_t numberAttributesLimited; //!< indicates whether to limit attributes uing the limitAttributes array
  uint32_t *pLimitedAttributes; //!< array of booleans corresponding to the pAttributes array in the models original udAttributeSet indicating which attributes to load from the pointcloud
};

//!
//! @struct udPointCloud
//! Stores the internal state of a udSDK pointcloud
//!
struct udPointCloud;

//!
//! Load a udPointCloud from `modelLocation`.
//!
//! @param pContext The context to be used to load the model.
//! @param ppModel The pointer pointer of the udPointCloud. This will allocate an instance of `udPointCloud` into `ppModel`.
//! @param modelLocation The location to load the model from. This may be a file location, or a supported protocol (HTTP, HTTPS, FTP).
//! @param pHeader If non-null, the provided udPointCloudHeader structure will be writen to
//! @return A udError value based on the result of the model load.
//! @note The application should call **udPointCloud_Unload** with `ppModel` to destroy the object once it's no longer needed.
//!
UDSDKDLL_API enum udError udPointCloud_Load(struct udContext *pContext, struct udPointCloud **ppModel, const char *modelLocation, struct udPointCloudHeader *pHeader);

//!
//! Load a udPointCloud from `modelLocation` with additional load options.
//!
//! @param pContext The context to be used to load the model.
//! @param ppModel The pointer pointer of the udPointCloud. This will allocate an instance of `udPointCloud` into `ppModel`.
//! @param modelLocation The location to load the model from. This may be a file location, or a supported protocol (HTTP, HTTPS, FTP).
//! @param pHeader If non-null, the provided udPointCloudHeader structure will be writen to
//! @param pOptions If non-null, the options to be applied when loading the model.
//! @return A udError value based on the result of the model load.
//! @note The application should call **udPointCloud_Unload** with `ppModel` to destroy the object once it's no longer needed.
//!
UDSDKDLL_API enum udError udPointCloud_LoadAdv(struct udContext *pContext, struct udPointCloud **ppModel, const char *modelLocation, struct udPointCloudHeader *pHeader, struct udPointCloudLoadOptions *pOptions);
//!
//! Destroys the udPointCloud.
//!
//! @param ppModel The pointer pointer of the udPointCloud. This will deallocate any internal memory used. It may take a few frames before the streamer releases the internal memory.
//! @return A udError value based on the result of the model unload.
//! @note The value of `ppModel` will be set to `NULL`.
//!
UDSDKDLL_API enum udError udPointCloud_Unload(struct udPointCloud **ppModel);

//!
//! Get the metadata associated with this object.
//!
//! @param pPointCloud The point cloud model to get the metadata from.
//! @param ppJSONMetadata The metadata(in JSON) from the model.
//! @return A udError value based on the result of getting the model metadata.
//!
UDSDKDLL_API enum udError udPointCloud_GetMetadata(struct udPointCloud *pPointCloud, const char **ppJSONMetadata);

//!
//! Get the matrix of this model.
//!
//! @param pPointCloud The point cloud model to get the matrix from.
//! @param pHeader The header structure to fill out
//! @return A udError value based on the result of getting the model header.
//! @note All Unlimited Detail models are assumed to be from { 0, 0, 0 } to { 1, 1, 1 }. Any scaling applied to the model will be in this matrix along with the translation and rotation.
//!
UDSDKDLL_API enum udError udPointCloud_GetHeader(struct udPointCloud *pPointCloud, struct udPointCloudHeader *pHeader);

//!
//! Exports a point cloud
//!
//! @param pModel The loaded pointcloud to export.
//! @param pExportFilename The path and filename to export the point cloud to. This should be a file location with write permissions. Supported formats are .UDS and .LAS.
//! @param pFilter If non-NULL this filter will be applied on the export to export a subsection
//! @param pProgress If non-NULL, this will be updated with a (estimated) progress throughout the export. Ranges between 0.f - 1.f
//! @return A udError value based on the result of the model export
//! @warning If an existing file exists this function will try override it
//!
UDSDKDLL_API enum udError udPointCloud_Export(struct udPointCloud *pModel, const char *pExportFilename, const struct udGeometry *pFilter, float *pProgress);

//!
//! Gets the default colour for a specific voxel in a given point cloud
//!
//! @param pModel The point cloud to get a default colour for.
//! @param pVoxelID The voxelID provided by picking or to voxel shaders
//! @param pColour The address to write the colour of the given voxel to
//! @return A udError value based on the result of getting the colour
//! @warning Calling this with invalid inputs can easily crash the program
//!
UDSDKDLL_API enum udError udPointCloud_GetNodeColour(const struct udPointCloud *pModel, const struct udVoxelID *pVoxelID, uint32_t *pColour);

//!
//! Gets the default colour for a specific voxel in a given point cloud
//!
//! @param pModel The point cloud to get a default colour for.
//! @param pVoxelID The voxelID provided by picking or to voxel shaders
//! @param pColour The address to write the colour of the given voxel to
//! @return A udError value based on the result of getting the colour
//! @warning Calling this with invalid inputs can easily crash the program
//!
UDSDKDLL_API enum udError udPointCloud_GetNodeColour64(const struct udPointCloud *pModel, const struct udVoxelID *pVoxelID, uint64_t *pColour);;

//!
//! Gets the pointer to the attribute offset on a specific voxel in a point cloud
//!
//! @param pModel The point cloud to get an address for.
//! @param pVoxelID The node provided by picking or to voxel shaders
//! @param attributeOffset The attribute offset from udAttributeSet_GetOffsetOfNamedAttribute or udAttributeSet_GetOffsetOfStandardAttribute
//! @param ppAttributeAddress The pointer will be updated with the address to the attribute
//! @return A udError value based on the result of getting the attribute address
//! @warning Calling this with invalid inputs can easily crash the program
//!
UDSDKDLL_API enum udError udPointCloud_GetAttributeAddress(struct udPointCloud *pModel, const struct udVoxelID *pVoxelID, uint32_t attributeOffset, const void **ppAttributeAddress);

//!
//! Gets the streamer status for the model
//!
//! @param pModel The point cloud to get the status of.
//! @return A udError value indicating the internal streamer status for this model.
//! @note udE_Success indicates the model is streaming successfully, other errors indicate other specific issues
//!
UDSDKDLL_API enum udError udPointCloud_GetStreamingStatus(struct udPointCloud *pModel);

//!
//! Gets the udAttributeSet of the model
//!
//! @param pModel The point cloud to get original attributes of.
//! @param pAttributeSet The attributeSet to be populated
//! @return A udError value indicating success of fetching the attributes 
//! @note udAttributeSet_Destroy must be called on pAttributeSet 
//!
UDSDKDLL_API enum udError udPointCloud_GetSourceAttributes(struct udPointCloud *pModel, struct udAttributeSet *pAttributeSet);

#ifdef __cplusplus
}
#endif

#endif // udPointCloud_h__
