#ifndef udRenderTarget_h__
#define udRenderTarget_h__

//! @file udRenderTarget.h
//! The **udRenderTarget** object provides an interface to specify a viewport to render to.
//! Once instantiated, the **udRenderTarget** can have its targets set, providing both a colour and depth output of the render which will utilize the matrices provided to the SetMatrix function.

#include <stdint.h>
#include "udDLLExport.h"
#include "udError.h"

#ifdef __cplusplus
extern "C" {
#endif

struct udContext;
struct udRenderContext;

//!
//! @struct udRenderTarget
//! Stores the internal state of a udSDK render target
//!
struct udRenderTarget;

//!
//! These are the various matrix types used within the render target
//!
enum udRenderTargetMatrix
{
  udRTM_Camera,     //!< The local to world-space transform of the camera (View is implicitly set as the inverse)
  udRTM_View,       //!< The view-space transform for the model (does not need to be set explicitly)
  udRTM_Projection, //!< The projection matrix (default is 60 degree LH)
  udRTM_Viewport,   //!< Viewport scaling matrix (default width and height of viewport)

  udRTM_Count       //!< Total number of matrix types. Used internally but can be used as an iterator max when checking matrix information.
};

//!
//! Create a udRenderTarget with a viewport using `width` and `height`.
//!
//! @param pContext The context to be used to create the render target.
//! @param ppRenderTarget The pointer pointer of the udRenderTarget. This will allocate an instance of udRenderTarget into `ppRenderTarget`.
//! @param pRenderer The renderer associated with the render target.
//! @param width The width of the viewport.
//! @param height The height of the viewport.
//! @return A udError value based on the result of the render target creation.
//! @note The application should call **udRenderTarget_Destroy** with `ppRenderTarget` to destroy the object once it's no longer needed.
//!
UDSDKDLL_API enum udError udRenderTarget_Create(struct udContext *pContext, struct udRenderTarget **ppRenderTarget, struct udRenderContext *pRenderer, uint32_t width, uint32_t height);

//!
//! Destroys the instance of `ppRenderTarget`.
//!
//! @param ppRenderTarget The pointer pointer of the udRenderTarget. This will deallocate the instance of udRenderTarget.
//! @return A udError value based on the result of the render target destruction.
//! @note The value of `ppRenderTarget` will be set to `NULL`.
//!
UDSDKDLL_API enum udError udRenderTarget_Destroy(struct udRenderTarget **ppRenderTarget);

//!
//! Set a memory buffers that a render target will write to.
//!
//! @param pRenderTarget The render target to associate a target buffer with.
//! @param pColorBuffer The color buffer, if null the buffer will not be rendered to anymore.
//! @param colorClearValue The clear value to clear the color buffer with.
//! @param pDepthBuffer The depth buffer, required
//! @return A udError value based on the result of setting the targets.
//! @note This internally calls udRenderTarget_SetTargetsWithPitch with both color and depth pitches set to 0.
//!
UDSDKDLL_API enum udError udRenderTarget_SetTargets(struct udRenderTarget *pRenderTarget, void *pColorBuffer, uint32_t colorClearValue, void *pDepthBuffer);

//!
//! Set a memory buffers that a render target will write to (with pitch).
//!
//! @param pRenderTarget The render target to associate a target buffer with.
//! @param pColorBuffer The color buffer, if null the buffer will not be rendered to anymore.
//! @param colorClearValue The clear value to clear the color buffer with.
//! @param pDepthBuffer The depth buffer, required
//! @param colorPitchInBytes The number of bytes that make up a row of the color buffer.
//! @param depthPitchInBytes The number of bytes that make up a row of the depth buffer.
//! @return A udError value based on the result of setting the targets.
//!
UDSDKDLL_API enum udError udRenderTarget_SetTargetsWithPitch(struct udRenderTarget *pRenderTarget, void *pColorBuffer, uint32_t colorClearValue, void *pDepthBuffer, uint32_t colorPitchInBytes, uint32_t depthPitchInBytes);

//!
//! Get the matrix associated with `pRenderTarget` of type `matrixType` and fill it in `cameraMatrix`.
//!
//! @param pRenderTarget The render target to get the matrix from.
//! @param matrixType The type of matrix to get.
//! @param cameraMatrix The array of 16 doubles which gets filled out with the matrix.
//! @return A udError value based on the result of getting the matrix.
//!
UDSDKDLL_API enum udError udRenderTarget_GetMatrix(const struct udRenderTarget *pRenderTarget, enum udRenderTargetMatrix matrixType, double cameraMatrix[16]);

//!
//! Set the matrix associated with `pRenderTarget` of type `matrixType` and get it from `cameraMatrix`.
//!
//! @param pRenderTarget The render target to set the matrix to.
//! @param matrixType The type of matrix to set.
//! @param cameraMatrix The array of 16 doubles to fill out the internal matrix with.
//! @return A udError value based on the result of setting the matrix.
//!
UDSDKDLL_API enum udError udRenderTarget_SetMatrix(struct udRenderTarget *pRenderTarget, enum udRenderTargetMatrix matrixType, const double cameraMatrix[16]);

//!
//! Set the logarithmic depth near and far planes that will be used for logarithmic rendering.
//! Note: These values are only used when the 'udRCF_LogarithmicDepth' rendering flag is set.
//!
//! @param pRenderTarget The render target to set the matrix to.
//! @param nearPlane The value that the near plane will be set to.
//! @param farPlane The value that the far plane will be set to.
//!
UDSDKDLL_API enum udError udRenderTarget_SetLogarithmicDepthPlanes(struct udRenderTarget *pRenderTarget, double nearPlane, double farPlane);

#ifdef __cplusplus
}
#endif

#endif // udRenderTarget_h__
