#ifndef udTriangleVoxelizer_h__
#define udTriangleVoxelizer_h__

//! @file udTriangleVoxelizer.h
//! The **udTriangleVoxelizer** object provides an interface to convert triangles to voxels to be added to a convert job.

#include "udDLLExport.h"
#include "udError.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

  //!
  //! @struct udTriangleVoxelizer
  //! Stores the internal state of the triangle voxelizer system
  //!
  struct udTriangleVoxelizer;

  //!
  //! Creates a udTriangleVoxelizer object at the specified grid resolution.
  //!
  //! @param ppVoxelizer A pointer to a location in which the new udTriangleVoxelizer object is stored.
  //! @param gridRes The intended grid resolution of the convert job.
  //! @return A udError value based on the result of the triangle voxelizer creation.
  //! @note The application should call **udTriangleVoxelizer_Destroy** with `ppTriRaster` to destroy the object once it's no longer needed.
  //!
  UDSDKDLL_API enum udError udTriangleVoxelizer_Create(struct udTriangleVoxelizer **ppVoxelizer, double gridRes);

  //!
  //! Destroys a udTriangleVoxelizer object that was created using **udTriangleVoxelizer_Create**.
  //!
  //! @param ppVoxelizer A pointer to a udTriangleVoxelizer object which is to be destroyed.
  //! @return A udError value based on the result of the object destruction.
  //! @note The value of `ppTriRaster` will be set to `NULL`.
  //!
  UDSDKDLL_API enum udError udTriangleVoxelizer_Destroy(struct udTriangleVoxelizer **ppVoxelizer);

  //!
  //! Set the vertices of the triangle, this primes a new triangle.
  //!
  //! @param pVoxelizer The voxelizer to be used to set the triangle.
  //! @param pV0 The first vertex in the triangle.
  //! @param pV1 The second vertex in the triangle.
  //! @param pV2 The third vertex in the triangle.
  //! @return A udError value based on the result of setting the triangle.
  //! @warning This will overwrite any existing unread points from a previous triangle
  //! @note Triangle will be treated as a line if v1 == v2 (v2 is degenerate)
  //!
  UDSDKDLL_API enum udError udTriangleVoxelizer_SetTriangle(struct udTriangleVoxelizer *pVoxelizer, const double *pV0, const double *pV1, const double *pV2);

  //!
  //! Get the voxelized points from the current triangle.
  //!
  //! @param pVoxelizer The voxelizer to be used to get the points.
  //! @param ppPositions A pointer to be populated with an array of positions.
  //! @param ppBarycentricWeights A pointer to be populated with an array of Barycentric weights.
  //! @param pPointCount A pointer to be populated with the number of poisitions and weights.
  //! @param maxPoints The maximum number of points to be returned.
  //! @return A udError value based on the result of getting the points.
  //! @note Returns a portion of voxelized points from current triangle, call repeatedly until *pPointCount is zero meaning the triangle is fully voxelized.
  //! @note The udTriangleVoxelizer object owns the arrays returned via ppPositions and ppBarycentricWeights.
  //!
  UDSDKDLL_API enum udError udTriangleVoxelizer_GetPoints(struct udTriangleVoxelizer *pVoxelizer, double **ppPositions, double **ppBarycentricWeights, uint32_t *pPointCount, uint32_t maxPoints);

#ifdef __cplusplus
}
#endif

#endif // udTriangleVoxelizer_h__
