#ifndef udConvertCustom_h__
#define udConvertCustom_h__

//! @file udConvertCustom.h
//! udConvertCustomItem provides a way to convert proprietary or unsupported file formats to Unlimited Detail format

#include "udConvert.h"
#include "udAttributes.h"
#include "udPointCloud.h"
#include "udPointBuffer.h"

#ifdef __cplusplus
extern "C" {
#endif

  //!
  //! Settings the custom converts need to be aware of that are set by the user
  //!
  enum udConvertCustomItemFlags
  {
    udCCIF_None = 0, //!< No additional flags specified
    udCCIF_SkipErrorsWherePossible = 1, //!< If its possible to continue parsing, that is perferable to failing
    udCCIF_PolygonVerticesOnly = 2, //!< Do not rasterise the polygons, just use the vertices as points
    udCCIF_BakeLighting = 8, //!< Bake normals into color channel for polygons conversion
    udCCIF_ExportImages = 16, //!< Export images contained in e57 files
  };

  //!
  //! @struct udConvertCustomItem
  //! Allows for conversion of custom data formats to UDS
  //!
  struct udConvertCustomItem
  {
    enum udError(*pOpen)(struct udConvertCustomItem *pConvertInput, uint32_t everyNth, double pointResolution, enum udConvertCustomItemFlags flags); //!< Open the file and provide information on the file (bounds, point count, etc.)
    enum udError(*pReadPointsFloat)(struct udConvertCustomItem *pConvertInput, struct udPointBufferF64 *pBuffer); //!< Provide position and attribute data to convert to UDS
    void(*pDestroy)(struct udConvertCustomItem *pConvertInput); //!< Cleanup all memory related to this custom convert item
    void(*pClose)(struct udConvertCustomItem *pConvertInput); //!< This function will be called when 

    void *pData; //!< Private user data relevant to the specific geomtype, must be freed by the pClose function

    const char *pName; //!< Filename or other identifier
    double boundMin[3]; //!< Optional (see boundsKnown) source space minimum values
    double boundMax[3]; //!< Optional (see boundsKnown) source space maximum values
    double sourceResolution; //!< Source resolution (eg 0.01 if points are 1cm apart). 0 indicates unknown
    int64_t pointCount; //!< Number of points coming, -1 if unknown
    int32_t srid; //!< If non-zero, this input is considered to be within the given srid code (useful mainly as a default value for other files in the conversion)
    struct udAttributeSet attributes; //!< Content of the input; this might not match the output
    uint32_t boundsKnown; //!< If not 0, boundMin and boundMax are valid, if 0 they will be calculated later
    uint32_t pointCountIsEstimate; //!< If not 0, the point count is an estimate and may be different
  };

  //!
  //! Adds a prefilled udConvertCustomItem to a udConvertContext
  //!
  //! @param pConvertContext The convert context to add the item to
  //! @param pCustomItem The custom convert item to add
  //! @return A udError value based on the result of adding the item
  //!
  UDSDKDLL_API enum udError udConvert_AddCustomItem(struct udConvertContext *pConvertContext, struct udConvertCustomItem *pCustomItem);

  //!
  //! Registers a format in the udConvertContext to allow users to just call udConvert_AddItem
  //!
  //! @param pConvertContext The convert context to register the format with
  //! @param pExtensionCheck The callback used to determine if the format should be used, returns 1 when a match, 0 otherwise
  //! @param pTryAddItem The callback used when calling udConvert_AddItem needs to populate the pCustomItem parameter, returns udE_Success on success
  //! @return A udError value based on the result of registering the format
  //! @note pTryAddItem needs to handle being called if a file doesn't match any of the extensions
  //!
  UDSDKDLL_API enum udError udConvert_AddCustomItemFormat(struct udConvertContext *pConvertContext, int (*pExtensionCheck)(const char *pFilename), enum udError (*pTryAddItem)(struct udConvertContext *pConvertContext, const char *pFilename, struct udConvertCustomItem *pCustomItem));

#ifdef __cplusplus
}
#endif

#endif // udConvertCustom_h__
