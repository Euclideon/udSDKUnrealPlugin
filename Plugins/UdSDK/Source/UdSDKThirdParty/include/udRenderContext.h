#ifndef udRenderContext_h__
#define udRenderContext_h__

//! @file udRenderContext.h
//! The **udRenderContext** object provides an interface to render Euclideon Unlimited Detail models.
//! It provides the ability to render by colour, intensity or classification; additionally allowing the user to query a specific pixel for information about the pointcloud data.

#include "udDLLExport.h"
#include "udError.h"
#include "udPointCloud.h"

#ifdef __cplusplus
extern "C" {
#endif

struct udContext;
struct udRenderTarget;

//!
//! @struct udRenderContext
//! Stores the internal state of the udSDK render system
//!
struct udRenderContext;

//!
//! These are the various point modes available in udSDK
//!
enum udRenderContextPointMode
{
  udRCPM_Rectangles, //!< This is the default, renders the voxels expanded as screen space rectangles
  udRCPM_Cubes, //!< Renders the voxels as cubes
  udRCPM_Points, //!< Renders voxels as a single point (Note: does not accurately reflect the 'size' of voxels)

  udRCPM_Count //!< Total number of point modes. Used internally but can be used as an iterator max when displaying different point modes.
};

//!
//! These are the various render flags available in udSDK
//!
enum udRenderContextFlags
{
  udRCF_None = 0, //!< Render the points using the default configuration.

  udRCF_PreserveBuffers = 1 << 0, //!< The colour and depth buffers won't be cleared before drawing and existing depth will be respected
  udRCF_ComplexIntersections = 1 << 1, //!< This flag is required in some scenes where there is a very large amount of intersecting point clouds
                                       //!< It will internally batch rendering with the udRCF_PreserveBuffers flag after the first render.
  udRCF_BlockingStreaming = 1 << 2, //!< This forces the streamer to load as much of the pointcloud as required to give an accurate representation in the current view. A small amount of further refinement may still occur.
  udRCF_LogarithmicDepth = 1 << 3, //!< Calculate the depth as a logarithmic distribution.
  udRCF_ManualStreamerUpdate = 1 << 4, //!< The streamer won't be updated internally but a render call without this flag or a manual streamer update will be required
  udRCF_ZeroAlphaSkip = 1 << 5, //!< If the voxel has 0 alpha (upper 8 bits) after the voxel shader then the voxel will not be drawn to the buffers (effectively skipped)

  udRCF_2PixelOpt = 1 << 6, //!< Optimisation that allows the renderer to resolve the last 2 pixels simulataneously, this can result in slight inaccuracies (generally a few pixels) in the final image for a huge performance improvement.
  udRCF_DisableOrthographic = 1 << 7, //!< Disables the renderer entering high-performance orthographic mode

  udRCF_NoTraversal = 1 << 8, //!< Skip octree traversal, instead reuse existing data from previous traversal
  udRCF_DisableDrawCallFiltering = 1 << 9, //!< Disables the splitting of blocks into multiple draw calls, when a filter is active
};

//!
//! @struct udRenderPicking
//! Stores both the input and output of the udSDK picking system
//!
struct udRenderPicking
{
  // Input variables
  unsigned int x; //!< Mouse X position in udRenderTarget space
  unsigned int y; //!< Mouse Y position in udRenderTarget space

  // Output variables
  uint32_t hit; //!< Not 0 if a voxel was hit by this pick
  uint32_t isHighestLOD; //!< Not 0 if this voxel that was hit is as precise as possible
  unsigned int modelIndex; //!< Index of the model in the ppModels list
  double pointCenter[3]; //!< The center of the hit voxel in world space
  struct udVoxelID voxelID; //!< The ID of the voxel that was hit by the pick; this ID is only valid for a very short period of time- Do any additional work using this ID this frame.
};

//!
//! @struct udRenderSettings
//! Stores the render settings used per render
//!
struct udRenderSettings
{
  enum udRenderContextFlags flags; //!< Optional flags providing information about how to perform this render
  struct udRenderPicking *pPick; //!< Optional This provides information about the voxel under the mouse
  enum udRenderContextPointMode pointMode; //!< The point mode for this render
  struct udGeometry *pFilter; //!< Optional This filter is applied to all models in the scene

  uint32_t pointCount; //!< Optional (GPU Renderer) A hint to the renderer at the upper limit of voxels that are to be rendered.
  float pointThreshold; //!< Optional (GPU Renderer) A hint of the minimum size (in screen space) of a voxel that the renderer will produce.

  double forwardAxis[3]; //!< Optional (GPU Renderer) Override the forward axis of this render.
};

//!
//! @struct udRenderInstance
//! Stores the instance settings of a model to be rendered
//!
struct udRenderInstance
{
  struct udPointCloud *pPointCloud; //!< This is the point cloud to display
  double matrix[16]; //!< The world space matrix for this point cloud instance (this does not to be the default matrix)
                     //!< @note The default matrix for a model can be accessed from the associated udPointCloudHeader

  const struct udGeometry *pFilter; //!< Filter to override for this model, this one will be used instead of the global one applied in udRenderSettings

  uint32_t (*pVoxelShader)(struct udPointCloud *pPointCloud, const struct udVoxelID *pVoxelID, const void *pVoxelUserData); //!< When the renderer goes to select a colour, it calls this function instead
  void *pVoxelUserData; //!< If pVoxelShader is set, this parameter is passed to that function

  double opacity; //!< If this is a value between 0 and 1 this model will be rendered blended with the rest of the scene. If the alpha from pVoxelShader is 0, the alpha provided will be written to the colourBuffer otherwise it will be calculated using this opacity value
  uint32_t skipRender; //!< If set not 0 the model will not be rendered
};

//!
//! Create an instance of `udRenderContext` for rendering.
//!
//! @param pContext The context to be used to create the render context.
//! @param ppRenderer The pointer pointer of the udRenderContext. This will allocate an instance of udRenderContext into `ppRenderer`.
//! @return A udError value based on the result of the render context creation.
//! @note It is not recommended to have more than one instance of udRenderContext.
//!
UDSDKDLL_API enum udError udRenderContext_Create(struct udContext *pContext, struct udRenderContext **ppRenderer);

//!
//! Destroy the instance of the renderer.
//!
//! @param ppRenderer The pointer pointer of the udRenderContext. This will deallocate the instance of udRenderContext.
//! @return A udError value based on the result of the render context destruction.
//!
UDSDKDLL_API enum udError udRenderContext_Destroy(struct udRenderContext **ppRenderer);

//!
//! Render the models from the perspective of `pRenderView`.
//!
//! @param pRenderer The renderer to render the scene.
//! @param pRenderView The view to render from with the render buffers associated with it being filled out.
//! @param pModels The array of models to use in render.
//! @param modelCount The amount of models in pModels.
//! @param pRenderOptions Additional render options.
//! @return A udError value based on the result of the render.
//!
UDSDKDLL_API enum udError udRenderContext_Render(struct udRenderContext *pRenderer, struct udRenderTarget *pRenderView, struct udRenderInstance *pModels, int modelCount, struct udRenderSettings *pRenderOptions);

#ifdef __cplusplus
}
#endif

#endif // udRenderContext_h__
