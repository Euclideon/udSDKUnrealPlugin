#ifndef udConvert_h__
#define udConvert_h__

//! @file udConvert.h
//! The **udConvertContext** object provides an interface to create a Euclideon Unlimited Detail model from a number of supported pointcloud formats.
//! Once instantiated, the **udConvertContext** object can be populated with input files and various conversion settings, before initiating the conversion process.

#include <stddef.h>
#include <stdint.h>

#include "udDLLExport.h"
#include "udError.h"
#include "udAttributes.h"

#include "udPointBuffer.h"

#ifdef __cplusplus
extern "C" {
#endif

  struct udContext;
  struct udPointCloud;

  //!
  //! @struct udConvertContext
  //! Stores the internal state of the convert system
  //!
  struct udConvertContext;

  //!
  //! @struct udConvertInfo
  //! Provides a copy of a subset of the convert state
  //!
  struct udConvertInfo
  {
    const char *pOutputName; //!< The output filename
    const char *pTempFilesPrefix; //!< The file prefix for temp files

    const char *pMetadata; //!< The metadata that will be added to this model (in JSON format)

    struct udAttributeSet attributes; //!< The attributes in this model

    int32_t ignoredAttributesLength; //!< The length of the ignored attributes list
    const char **ppIgnoredAttributes; //!< The list of ignored attributes

    double globalOffset[3]; //!< This amount is added to every point during conversion. Useful for moving the origin of the entire scene to geolocate

    double minPointResolution; //!< The native resolution of the highest resolution file
    double maxPointResolution; //!< The native resolution of the lowest resolution file
    uint32_t skipErrorsWherePossible; //!< If not 0 it will continue processing other files if a file is detected as corrupt or incorrect

    uint32_t everyNth; //!< If this value is >1, only every Nth point is included in the model. e.g. 4 means only every 4th point will be included, skipping 3/4 of the points
    uint32_t polygonVerticesOnly; //!< If not 0 it will skip rasterization of polygons in favour of just processing the vertices
    uint32_t retainPrimitives; //!< If not 0 rasterised primitives such as triangles/lines/etc are retained to be rendered at finer resolution if required at runtime
    uint32_t bakeLighting; //!< if not 0 bake the normals into the colour channel in the output UDS file
    uint32_t exportOtherEmbeddedAssets; //!< if not 0 export images contained in e57 files

    uint32_t overrideResolution; //!< Set to not 0 to stop the resolution from being recalculated
    double pointResolution; //!< The scale to be used in the conversion (either calculated or overriden)

    uint32_t overrideSRID; //!< Set to not 0 to prevent the SRID being recalculated
    int srid; //!< The geospatial reference ID (either calculated or overriden)
    const char *pWKT; //!< The geospatial WKT string

    uint64_t totalPointsRead; //!< How many points have been read in this model
    uint64_t totalItems; //!< How many items are in the list

    // These are quick stats while converting
    uint64_t currentInputItem; //!< The index of the item that is currently being read
    uint64_t outputFileSize; //!< Size of the result UDS file
    uint64_t sourcePointCount; //!< Number of points added (may include duplicates or out of range points)
    uint64_t uniquePointCount; //!< Number of unique points in the final model
    uint64_t discardedPointCount; //!< Number of duplicate or ignored out of range points
    uint64_t outputPointCount; //!< Number of points written to UDS (can be used for progress)
    uint64_t peakDiskUsage; //!< Peak amount of disk space used including both temp files and the actual output file
    uint64_t peakTempFileUsage; //!< Peak amount of disk space that contained temp files
    uint32_t peakTempFileCount; //!< Peak number of temporary files written
  };

  //!
  //! @struct udConvertItemInfo
  //! Provides a copy of a subset of a convert item state
  //!
  struct udConvertItemInfo
  {
    const char *pFilename; //!< Name of the input file
    int64_t pointsCount; //!< This might be an estimate, -1 is no estimate is available
    uint64_t pointsRead; //!< Once conversation begins, this will give an indication of progress
    double estimatedResolution; //!< The estimated scale of the item
    int srid; //!< The calculated geospatial reference ID of the item
  };

  //!
  //! Create a udConvertContext to convert models to the Euclideon file format.
  //!
  //! @param pContext The context to be used to create the convert context.
  //! @param ppConvertContext The pointer pointer of the udConvertContext. This will allocate an instance of `udConvertContext` into `ppConvertContext`.
  //! @return A udError value based on the result of the convert context creation.
  //! @note The application should call **udConvert_DestroyContext** with `ppConvertContext` to destroy the object once it's no longer needed.
  //!
  UDSDKDLL_API enum udError udConvert_CreateContext(struct udContext *pContext, struct udConvertContext **ppConvertContext);

  //!
  //! Destroys the instance of `ppConvertContext`.
  //!
  //! @param ppConvertContext The pointer pointer of the udConvertContext. This will deallocate the instance of `udConvertContext`.
  //! @return A udError value based on the result of the convert context destruction.
  //! @note The value of `ppConvertContext` will be set to `NULL`.
  //!
  UDSDKDLL_API enum udError udConvert_DestroyContext(struct udConvertContext **ppConvertContext);

  //!
  //! Sets the filename of the output UDS.
  //!
  //! @param pConvertContext The convert context to use to set the output filename.
  //! @param pFilename The filename to set for the output.
  //! @return A udError value based on the result of setting the output filename.
  //! @warning If the .UDS extension isn't set, this function will add the extension.
  //! @note If the extension is .UDG the converter will generate a UDG file
  //!
  UDSDKDLL_API enum udError udConvert_SetOutputFilename(struct udConvertContext *pConvertContext, const char *pFilename);

  //!
  //! Sets the temporary output directory for the conversion.
  //!
  //! @param pConvertContext The convert context to use to set the temporary directory.
  //! @param pFolder The folder path to set for the temporary directory.
  //! @return A udError value based on the result of setting the temporary directory.
  //! @note A trailing slash is not automatically added, this is to allow for a prefix for the temporary files instead of, or as well as, folders.
  //!
  UDSDKDLL_API enum udError udConvert_SetTempDirectory(struct udConvertContext *pConvertContext, const char *pFolder);

  //!
  //! Sets the bounds and point resolution for the conversion from a previously converted point cloud.
  //!
  //! @param pConvertContext The convert context to use to set the point resolution.
  //! @param pPointCloudBoundsPath The UDS file path to be used to set the bounds and point resolution.
  //! @return A udError value based on the result of setting the point resolution and min ,max bounds.
  //!
  UDSDKDLL_API enum udError udConvert_SetUseBounds(struct udConvertContext *pConvertContext, const char *pPointCloudBoundsPath);

  //!
  //! Sets the point resolution for the conversion.
  //!
  //! @param pConvertContext The convert context to use to set the point resolution.
  //! @param override A boolean value (0 is false) to indicate whether to override the point resolution or use the auto-detected value.
  //! @param pointResolutionMeters The point resolution in meters.
  //! @return A udError value based on the result of setting the point resolution.
  //!
  UDSDKDLL_API enum udError udConvert_SetPointResolution(struct udConvertContext *pConvertContext, uint32_t override, double pointResolutionMeters);

  //!
  //! Flags an attribute to be ignored for the conversion.
  //!
  //! @param pConvertContext The convert context to use to set the ignored attribute.
  //! @param pAttributeName The name of the attribute to be ignored.
  //!
  UDSDKDLL_API enum udError udConvert_IgnoreAttribute(struct udConvertContext *pConvertContext, const char *pAttributeName);

  //!
  //! Includes an attribute in the conversion if the attribute has previously been ignored.
  //!
  //! @param pConvertContext The convert context to restore attribute.
  //! @param pAttributeName The name of the attribute to be restored.
  //!
  UDSDKDLL_API enum udError udConvert_RestoreAttribute(struct udConvertContext *pConvertContext, const char *pAttributeName);

  //!
  //! Sets the prefix of the attribute, used when displaying values to users.
  //!
  //! @param pConvertContext The convert context to use to set the attribute prefix.
  //! @param pAttributeName The name of the attribute to set the prefix for.
  //! @param pPrefix The prefix to use for the attribute.
  //! @note The prefix has a limit of 15 characters, not including the null terminator.
  //!
  UDSDKDLL_API enum udError udConvert_SetAttributePrefix(struct udConvertContext *pConvertContext, const char *pAttributeName, const char *pPrefix);

  //!
  //! Sets the suffix of the attribute, used when displaying values to users.
  //!
  //! @param pConvertContext The convert context to use to set the attribute suffix.
  //! @param pAttributeName The name of the attribute to set the suffix for.
  //! @param pSuffix The suffix to use for the attribute.
  //! @note The suffix has a limit of 15 characters, not including the null terminator.
  //!
  UDSDKDLL_API enum udError udConvert_SetAttributeSuffix(struct udConvertContext *pConvertContext, const char *pAttributeName, const char *pSuffix);

  //!
  //! Sets the SRID for the conversion.
  //!
  //! @param pConvertContext The convert context to use to set the SRID and WKT using SRID.
  //! @param override A boolean value (0 is false) to indicate whether to override the SRID or use the auto-detected value.
  //! @param srid The SRID value to use.
  //! @return A udError value based on the result of setting the SRID & WKT.
  //!
  UDSDKDLL_API enum udError udConvert_SetSRID(struct udConvertContext *pConvertContext, uint32_t override, int srid);

  //!
  //! Sets the WKT for the conversion.
  //!
  //! @param pConvertContext The convert context to use to set the SRID and WKT using WKT.
  //! @param pWKT The WKT string to use.
  //! @note This will also enable the override on the SRID field (will not use the auto-detected value)
  //! @return A udError value based on the result of setting the SRID & WKT.
  //!
  UDSDKDLL_API enum udError udConvert_SetWKT(struct udConvertContext *pConvertContext, const char *pWKT);

  //!
  //! This function adds the supplied global offset to each point in the model.
  //!
  //! @param pConvertContext The convert context to set the offset within.
  //! @param globalOffset An array of 3 Doubles representing the desired offset in X, Y and then Z.
  //! @return A udError value based on the result of setting the global offset.
  //! @note This is most useful for moving the origin of a model (or set of models) to the false easting and northing of an alternative geozone.
  //!
  UDSDKDLL_API enum udError udConvert_SetGlobalOffset(struct udConvertContext *pConvertContext, const double globalOffset[3]);

  //!
  //! This function sets the convert context up to attempt to skip errors where it can.
  //!
  //! @param pConvertContext The convert context to use to set the skip errors where possible option.
  //! @param ignoreParseErrorsWherePossible A boolean value (0 is false) to indicate whether to skip errors where possible.
  //! @return A udError value based on the result of setting the skip errors where possible option.
  //! @note In most situations this will mean that an input that is corrupt, malformed or not completely supported will be parsed as far as possible and if an error occurs it will skip the rest of this input and begin on the next.
  //! @note Some importers may be able to skip to a later section in the file and continue conversion but this is up to the specific implementation of the importer.
  //!
  UDSDKDLL_API enum udError udConvert_SetSkipErrorsWherePossible(struct udConvertContext *pConvertContext, uint32_t ignoreParseErrorsWherePossible);

  //!
  //! `EveryNth` lets the importers know to only include every *_n_*th point. If this is set to 0 or 1, every point will be included.
  //!
  //! @param pConvertContext The convert context to set the everyNth param on
  //! @param everyNth How many _n_th points to include. Alternatively, how many (n - 1) points to skip for every point included in the export. _See the example below for a bit more context on what this number means_.
  //! @return A udError value based on the result of setting the every Nth option.
  //! @note For many file formats this will be significantly faster to process making this valuable as a tool to test if the resolution and geolocation settings are correct before doing a full conversion.
  //! @note The first (0th) point is always included regardless of this value.
  //!       Example:
  //!         Setting this to `50` would:
  //!           1. Include the first point(point 0)
  //!           2. Skip 49 points
  //!           3. Include the 50th point
  //!           4. Skip another 49 points
  //!           5. Include the 100th point
  //!           n. ...and so on skipping 49 points and including the 50th until all points from this input are processed.
  //!           The next input would then reset the count and include the 0th, skipping 49 etc.as before.
  //!
  UDSDKDLL_API enum udError udConvert_SetEveryNth(struct udConvertContext *pConvertContext, uint32_t everyNth);

  //!
  //! This function sets the convert context up to skip rasterization of the polygons, leaving only the vertices.
  //!
  //! @param pConvertContext The convert context to use to set the polygonVerticesOnly param on.
  //! @param polygonVerticesOnly A boolean value (0 is false) to indicate whether to skip rasterization of the polygons being converted, leaving only the vertices.
  //! @return A udError value based on the result of setting the polygon vertices only option.
  //!
  UDSDKDLL_API enum udError udConvert_SetPolygonVerticesOnly(struct udConvertContext *pConvertContext, uint32_t polygonVerticesOnly);

  //!
  //! This function sets the convert context up to retain rasterised primitives such as lines/triangles to be rendered at finer resolutions at runtime
  //!
  //! @param pConvertContext The convert context to use to set the polygonVerticesOnly param on.
  //! @param retainPrimitives A boolean value (0 is false) to indicate whether to retain the primitives in the output UDS file
  //! @return A udError value based on the result of setting the retainPrimitives option.
  //!
  UDSDKDLL_API enum udError udConvert_SetRetainPrimitives(struct udConvertContext *pConvertContext, uint32_t retainPrimitives);

  //!
  //! This function sets the convert context up to set the udCIF_BakeLightning flag allowing the read point function to bake normals into the colour channel
  //!
  //! @param pConvertContext The convert context to use to set the polygonVerticesOnly param on.
  //! @param bakeLighting A boolean value (0 is false) to indicate whether to bake the normals into the colour channel in the output UDS file
  //! @return A udError value based on the result of setting the retainPrimitives option.
  //!
  UDSDKDLL_API enum udError udConvert_SetBakeLighting(struct udConvertContext *pConvertContext, uint32_t bakeLighting);

  //!
  //! This function sets the convert context up to set the udCIF_ExportImages flag allowing the open function to export images to png or jpg files
  //!
  //! @param pConvertContext The convert context to use to set the polygonVerticesOnly param on.
  //! @param exportImages A boolean value (0 is false) to indicate whether to export or not images contained in e57 files
  //! @return A udError value based on the result of setting the exportImages option.
  //!
  UDSDKDLL_API enum udError udConvert_SetExportOtherEmbeddedAssets(struct udConvertContext *pConvertContext, uint32_t exportImages);

  //!
  //! This adds a metadata key to the output UDS file. There are no restrictions on the key.
  //!
  //! @param pConvertContext The convert context to use to set the metadata key.
  //! @param pMetadataKey The name of the key.This is parsed as a JSON address.
  //! @param pMetadataValue The contents of the key, settings this as `NULL` will remove the key from the system (if it exists). This value is handled internal as a string (won't be parsed as JSON).
  //! @return A udError value based on the result of setting the metadata key and value.
  //! @note There are a number of 'standard' keys that are recommended to support.
  //!       - `Author`: The name of the company that owns or captured the data
  //!       - `Comment`: A miscellaneous information section
  //!       - `Copyright`: The copyright information
  //!       - `License`: The general license information
  //!
  UDSDKDLL_API enum udError udConvert_SetMetadata(struct udConvertContext *pConvertContext, const char *pMetadataKey, const char *pMetadataValue);

  //!
  //! This adds an item to be converted in the convert context.
  //!
  //! @param pConvertContext The convert context to add the item to.
  //! @param pFilename The file to add to the convert context.
  //! @return A udError value based on the result of adding the item.
  //!
  UDSDKDLL_API enum udError udConvert_AddItem(struct udConvertContext *pConvertContext, const char *pFilename);

  //!
  //! This removes an item to be converted from the convert context.
  //!
  //! @param pConvertContext The convert context to remove the item from.
  //! @param index The index of the item to remove from the convert context.
  //! @return A udError value based on the result of removing the item.
  //!
  UDSDKDLL_API enum udError udConvert_RemoveItem(struct udConvertContext *pConvertContext, uint64_t index);

  //!
  //! This specifies the projection of the source data.
  //!
  //! @param pConvertContext The convert context to set the input source projection on.
  //! @param index The index of the item to set the source project on.
  //! @param srid The SRID to use for the specified item.
  //! @return A udError value based on the result of setting the source projection.
  //!
  UDSDKDLL_API enum udError udConvert_SetInputSourceProjection(struct udConvertContext *pConvertContext, uint64_t index, int srid);

  //!
  //! This provides a way to get the information of the convert context.
  //!
  //! @param pConvertContext The convert context to retrieve the information from.
  //! @param ppInfo The pointer pointer of the udConvertInfo. This will be managed by the convert context and does not need to be deallocated.
  //! @return A udError value based on the result of getting the information of the convert context.
  //!
  UDSDKDLL_API enum udError udConvert_GetInfo(struct udConvertContext *pConvertContext, const struct udConvertInfo **ppInfo);

  //!
  //! This provides a way to get the information of a specific item in the convert context.
  //!
  //! @param pConvertContext The convert context to retrieve the item information from.
  //! @param index The index of the item to retrieve the information for from the convert context.
  //! @param pInfo The pointer of the udConvertItemInfo. The will be populated by the convert context from an internal representation.
  //! @return A udError value based on the result of getting the information of the specified item.
  //!
  UDSDKDLL_API enum udError udConvert_GetItemInfo(struct udConvertContext *pConvertContext, uint64_t index, struct udConvertItemInfo *pInfo);

  //!
  //! This begins the conversion process for the provided convert context.
  //!
  //! @param pConvertContext The convert context on which to start the conversion.
  //! @return A udError value based on the result of starting the conversion.
  //!
  UDSDKDLL_API enum udError udConvert_DoConvert(struct udConvertContext *pConvertContext);

  //!
  //! This cancels the running conversion for the provided convert context.
  //!
  //! @param pConvertContext The convert context on which to cancel the conversion.
  //! @return A udError value based on the result of cancelling the conversion.
  //!
  UDSDKDLL_API enum udError udConvert_Cancel(struct udConvertContext *pConvertContext);

  //!
  //! This resets the statis for the provided convert context, for example to re-run a previously completed conversion.
  //!
  //! @param pConvertContext The convert context on which to reset the status.
  //! @return A udError value based on the result of resetting the status.
  //!
  UDSDKDLL_API enum udError udConvert_Reset(struct udConvertContext *pConvertContext);

  //!
  //! This generates a preview of the provided convert context.
  //!
  //! @param pConvertContext The convert context to generate the preview for.
  //! @param ppCloud The pointer pointer of the udPointCloud. This will allocate an instance of `udPointCloud` into `ppCloud`.
  //! @return A udError value based on the result of genearting the preview.
  //! @note The application should call **udPointCloud_Unload** with `ppCloud` to destroy the object once it's no longer needed.
  //!
  UDSDKDLL_API enum udError udConvert_GeneratePreview(struct udConvertContext *pConvertContext, struct udPointCloud **ppCloud);

  //!
  //! Callback invoked on pointbuffers after being read in during the convert process to conditionally modify points based on buffer contents
  //! 
  //! @param pConvertInput The convert info associated with the current item being processed at the time the callback is invoked
  //! @param pBuffer The point buffer containing the points currently being read in prior to any processing done by convert (e.g. reprojection)
  //! @param pUserData Pointer to a struct containing user data used by this function- this may be freed by pCleanUpUserData on completion of processing of convert inputs if necessary
  //! @return A udError to indicate the success of the postprocessing - returning anything other than udE_Success will cause the conversion to fail
  //! 
  typedef enum udError (udConvertPostProcessCallback)(struct udConvertItemInfo *pConvertInput, struct udPointBufferF64 *pBuffer, void *pUserData);

  //!
  //! Postprocessing to perform on points as they are read in
  //! 
  //! @param pContext The convert context 
  //! @param callback takes the convertInput, a point buffer, a pointer to user data (which must point to memory that is valid for the duration of the convert process); returns udError
  //! This can be used to modify the points and their attributes as well as modify the contents of the userData Structure
  //! @param pUserData a pointer to any data used by the callback
  //! @param pCleanUpUserData a function called with pUserData as the argument once the input has finished processing
  //!
  //! @return A udError value based on the result of setting the callback
  //!
  UDSDKDLL_API enum udError udConvert_SetPostProcessCallback(struct udConvertContext *pContext, udConvertPostProcessCallback *pCallback, void *pUserData, void(*pCleanUpUserData)(void*));

  //!
  //! Forces the produced UDS to include the specified attribute despite not being present in any input file. This is useful when these attributes are calculated using a postprocess callback
  //! 
  //! @param pContext The convert context 
  //! @param pAttribute descriptor of the attribute to be added. This is copied by the function
  //!
  //! @return A udError value based on the result of setting the forced attribute
  //! @return udE_CountExceeded if the number of attributes exceeds the limit in a UDS
  //!
  UDSDKDLL_API enum udError udConvert_AddOutputAttribute(struct udConvertContext *pContext, struct udAttributeDescriptor *pAttribute);

  //!
  //! Removes the forced attribute at the index specified from the list
  //! 
  //! @param pContext The convert context 
  //! @param index the index of from the array of forced attributes to remove
  //!
  //! @return A udError value based on the result of removing the forced attribute
  //!
  UDSDKDLL_API enum udError udConvert_RemoveOutputAttribute(struct udConvertContext *pContext, uint32_t index);

#ifdef __cplusplus
}
#endif

#endif // udConvert_h__
