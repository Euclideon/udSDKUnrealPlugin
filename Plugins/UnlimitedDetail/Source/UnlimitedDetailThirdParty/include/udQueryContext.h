#ifndef udQueryContext_h__
#define udQueryContext_h__

//! @file udQueryContext.h
//! The udQueryContext object provides an interface to query or filter pointclouds.

#include <stdint.h>

#include "udDLLExport.h"
#include "udError.h"
#include "udPointCloud.h"
#include "udPointBuffer.h"

#ifdef __cplusplus
extern "C" {
#endif

  struct udQueryContext;

  struct udRenderInstance; // From udRenderContext.h
  struct udGeometry; // From udGeometry.h

  //!
  //! Create an instance of a udQueryContext with a specific model.
  //!
  //! @param pContext The context to be used to create the query context.
  //! @param ppQueryCtx The pointer pointer of the udQueryContext. This will allocate an instance of udQueryContext into `ppQuery`.
  //! @param pPointCloud The point cloud to run the query on, it is located at its storedMatrix location (this can be changed using udQueryContext_ChangePointCloud).
  //! @param pFilter The filter to use in this query (this can be changed using udQueryContext_ChangeFilter).
  //! @return A udError value based on the result of the udQueryContext creation.
  //! @note A future release will add multiple model support and non-storedMatrix locations.
  //!
  UDSDKDLL_API enum udError udQueryContext_Create(struct udContext *pContext, struct udQueryContext **ppQueryCtx, struct udPointCloud *pPointCloud, const struct udGeometry *pFilter);

  //!
  //! Resets the udQueryContext and uses a new filter.
  //!
  //! @param pQueryCtx The udQueryContext item previously created using udQueryContext_Create.
  //! @param pFilter The new filter to use in this query.
  //! @return A udError value based on the result of the operation.
  //! @note This will reset the query, any existing progress will be lost.
  //!
  UDSDKDLL_API enum udError udQueryContext_ChangeFilter(struct udQueryContext *pQueryCtx, const struct udGeometry *pFilter);

  //!
  //! Resets the udQueryContext and uses a different model.
  //!
  //! @param pQueryCtx The udQueryContext item previously created using udQueryContext_Create.
  //! @param pPointCloud The new model to use in this query.
  //! @return A udError value based on the result of the operation.
  //! @note This will reset the query, any existing progress will be lost.
  //!
  UDSDKDLL_API enum udError udQueryContext_ChangePointCloud(struct udQueryContext *pQueryCtx, const struct udPointCloud *pPointCloud);

  //!
  //! Gets the next set of points from an existing udQueryContext.
  //!
  //! @param pQueryCtx The udQueryContext to execute.
  //! @param pPointBuffer The point buffer to write found points to.
  //! @return A udError value based on the result of the operation. 
  //! @note This should continue to be called until it returns udE_NotFound indicating the query has completed.
  //!
  UDSDKDLL_API enum udError udQueryContext_ExecuteF64(struct udQueryContext *pQueryCtx, struct udPointBufferF64 *pPointBuffer);

  //!
  //! Gets the next set of points from an existing udQueryContext.
  //!
  //! @param pQueryCtx The udQueryContext to execute.
  //! @param pPointBuffer The point buffer to write found points to.
  //! @return A udError value based on the result of the operation.
  //! @note This should continue to be called until it returns udE_NotFound indicating the query has completed.
  //!
  UDSDKDLL_API enum udError udQueryContext_ExecuteI64(struct udQueryContext *pQueryCtx, struct udPointBufferI64 *pPointBuffer);

  //!
  //! Destroy the instance of udQueryContext.
  //!
  //! @param ppQueryCtx The pointer pointer of the udQueryContext. This will destroy the instance of udQueryContext in `ppQuery` and set it to NULL.
  //! @return A udError value based on the result of the udQueryContext destruction.
  //!
  UDSDKDLL_API enum udError udQueryContext_Destroy(struct udQueryContext **ppQueryCtx);

  //!
  //! Test the specified voxel against the given udQueryFilter.
  //!
  //! @param pQueryFilter The udQueryFilter to test.
  //! @param matrix The scene matrix for the model that contains the voxel being rendered.
  //! @param pRenderInfo The render info of the voxel being rendered.
  //! @return A udError value based on the result of the test.
  //!
  UDSDKDLL_API enum udError udQuery_TestRenderVoxel(struct udGeometry *pQueryFilter, double matrix[16], void *pRenderInfo);

#ifdef __cplusplus
}
#endif

#endif // udQueryContext_h__
