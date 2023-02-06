#ifndef udPointBuffer_h__
#define udPointBuffer_h__

//! @file udPointBuffer.h
//! The udPointBuffer object provides an interface to add or get points from udPointClouds

#include "udDLLExport.h"
#include "udAttributes.h"
#include "udError.h"

#ifdef __cplusplus
extern "C" {
#endif

//!
//! @struct udPointBufferIndices
//! Stores a set of indices to be used with a **udPointBuffer**
//!
struct udPointBufferIndices
{
  uint32_t count; //!< Total number of indices currently contained in this object
  uint32_t capacity; //!< Total number of indices that can fit in this object
  uint32_t indices[1]; //!< Array of indices, real length of the array is stored in capacity
};

//!
//! @struct udPointBufferF64
//! Stores a set of points and their attributes that have positions as double (64bit float) values
//!
struct udPointBufferF64
{
  double *pPositions; //!< Flat array of XYZ positions in the format XYZXYZXYZXYZXYZXYZXYZ...
  uint8_t *pAttributes; //!< Byte array of attribute data ordered as specified in `attributes`
  struct udAttributeSet attributes; //!< Information on the attributes that are available in this point buffer
  uint32_t attributeStride; //!< Total number of bytes between the start of the attibutes of one point and the first byte of the next attribute
  uint32_t pointCount; //!< How many points are currently contained in this buffer
  uint32_t pointsAllocated; //!< Total number of points that can fit in this udPointBufferF64
  uint32_t _reserved; //!< Reserved for internal use

#ifdef __cplusplus
  // Convenience helper to return the pointer to a position regardless of presence/absence of indices
  inline const double *GetPosition(uint32_t index, const udPointBufferIndices *pIndices = nullptr) const { return (double *)(((char *)pPositions) + (pIndices ? pIndices->indices[index] : index) * 24); }

  // Convenience helper to return the pointer to attributes regardless of presence/absence of indices
  inline const uint8_t *GetAttributes(uint32_t index, const udPointBufferIndices *pIndices = nullptr) const { return pAttributes + ((pIndices ? pIndices->indices[index] : index) * attributeStride); }

  // Helper to assign a position
  inline void SetPosition(uint32_t index, const double &pos, const udPointBufferIndices *pIndices = nullptr) { *(double *)(((char *)pPositions) + (pIndices ? pIndices->indices[index] : index) * 24) = pos; }

  // Convenience helper to return the point count regardless of presence/absence of indices
  inline uint32_t GetPointCount(const udPointBufferIndices *pIndices = nullptr) const { return pIndices ? pIndices->count : pointCount; }
#endif //__cplusplus
};

//!
//! @struct udPointBufferI64
//! Stores a set of points and their attributes that have positions as Int64 values
//!
struct udPointBufferI64
{
  int64_t *pPositions;  //!< Flat array of XYZ positions in the format XYZXYZXYZXYZXYZXYZXYZ...
  uint8_t *pAttributes; //!< Byte array of attribute data ordered as specified in `attributes`
  struct udAttributeSet attributes; //!< Information on the attributes that are available in this point buffer
  uint32_t attributeStride; //!< Total number of bytes between the start of the attibutes of one point and the first byte of the next attribute
  uint32_t pointCount; //!< How many points are currently contained in this buffer
  uint32_t pointsAllocated; //!< Total number of points that can fit in this udPointBufferF64
  uint32_t _reserved; //!< Reserved for internal use

#ifdef __cplusplus
  // Convenience helper to return the pointer to a position regardless of presence/absence of indices
  inline const int64_t *GetPosition(uint32_t index, const udPointBufferIndices *pIndices = nullptr) const { return (int64_t *)(((char *)pPositions) + (pIndices ? pIndices->indices[index] : index) * 24); }

  // Convenience helper to return the pointer to attributes regardless of presence/absence of indices
  inline const uint8_t *GetAttributes(uint32_t index, const udPointBufferIndices *pIndices = nullptr) const { return pAttributes + ((pIndices ? pIndices->indices[index] : index) * attributeStride); }

  // Helper to assign a position
  inline void SetPosition(uint32_t index, const int64_t &pos, const udPointBufferIndices *pIndices = nullptr) { *(int64_t *)(((char *)pPositions) + (pIndices ? pIndices->indices[index] : index) * 24) = pos; }

  // Convenience helper to return the point count regardless of presence/absence of indices
  inline uint32_t GetPointCount(const udPointBufferIndices *pIndices = nullptr) const { return pIndices ? pIndices->count : pointCount; }
#endif //__cplusplus
};

//!
//! @struct udPointBufferU64
//! Stores a set of points and their attributes that have positions as UInt64 values
//!
struct udPointBufferU64
{
  uint64_t *pPositions;  //!< Flat array of XYZ positions in the format XYZXYZXYZXYZXYZXYZXYZ...
  uint8_t *pAttributes; //!< Byte array of attribute data ordered as specified in `attributes`
  struct udAttributeSet attributes; //!< Information on the attributes that are available in this point buffer
  uint32_t attributeStride; //!< Total number of bytes between the start of the attibutes of one point and the first byte of the next attribute
  uint32_t pointCount; //!< How many points are currently contained in this buffer
  uint32_t pointsAllocated; //!< Total number of points that can fit in this udPointBufferF64
  uint32_t _reserved; //!< Reserved for internal use

#ifdef __cplusplus
  // Convenience helper to return the pointer to a position regardless of presence/absence of indices
  inline const uint64_t *GetPosition(uint32_t index, const udPointBufferIndices *pIndices = nullptr) const { return (uint64_t *)(((char *)pPositions) + (pIndices ? pIndices->indices[index] : index) * 24); }

  // Convenience helper to return the pointer to attributes regardless of presence/absence of indices
  inline const uint8_t *GetAttributes(uint32_t index, const udPointBufferIndices *pIndices = nullptr) const { return pAttributes + ((pIndices ? pIndices->indices[index] : index) * attributeStride); }

  // Helper to assign a position
  inline void SetPosition(uint32_t index, const uint64_t &pos, const udPointBufferIndices *pIndices = nullptr) { *(uint64_t *)(((char *)pPositions) + (pIndices ? pIndices->indices[index] : index) * 24) = pos; }

  // Convenience helper to return the point count regardless of presence/absence of indices
  inline uint32_t GetPointCount(const udPointBufferIndices *pIndices = nullptr) const { return pIndices ? pIndices->count : pointCount; }
#endif //__cplusplus
};

//!
//! Create a point buffer indices object
//!
//! @param ppIndices The pointer pointer of the udPointBufferIndices. This will allocate an instance of `udPointBufferIndices` into `ppIndices`
//! @param numIndices The maximum number of indices that this object will contain (these are preallocated to avoid allocations later)
//! @return A udError value based on creation status.
//! @note The application should call **udPointBufferIndices_Destroy** with `ppIndices` to destroy the point buffer indices object once it's no longer needed.
//! 
UDSDKDLL_API enum udError udPointBufferIndices_Create(struct udPointBufferIndices **ppIndices, uint32_t numIndices);

//!
//! Destroys the udPointBufferIndices.
//!
//! @param ppIndices The pointer pointer of the udPointBufferIndices. This will deallocate any memory used.
//! @return A udError value based on the result of the destruction.
//! @note The value of `ppIndices` will be set to `NULL`.
//!
UDSDKDLL_API enum udError udPointBufferIndices_Destroy(struct udPointBufferIndices **ppIndices);

//!
//! Create a 64bit floating point, point buffer object
//!
//! @param ppBuffer The pointer pointer of the udPointBufferF64. This will allocate an instance of `udPointBufferF64` into `ppBuffer`.
//! @param maxPoints The maximum number of points that this buffer will contain (these are preallocated to avoid allocations later)
//! @param pAttributes The pointer to the udAttributeSet containing information on the attributes that will be available in this point buffer; NULL will have no attributes. An internal copy is made of this attribute set.
//! @return A udError value based on creation status.
//! @note The application should call **udPointBufferF64_Destroy** with `ppBuffer` to destroy the point buffer object once it's no longer needed.
//!
UDSDKDLL_API enum udError udPointBufferF64_Create(struct udPointBufferF64 **ppBuffer, uint32_t maxPoints, struct udAttributeSet *pAttributes);

//!
//! Destroys the udPointBufferF64.
//!
//! @param ppBuffer The pointer pointer of the ppBuffer. This will deallocate any memory used.
//! @return A udError value based on the result of the destruction.
//! @note The value of `ppBuffer` will be set to `NULL`.
//!
UDSDKDLL_API enum udError udPointBufferF64_Destroy(struct udPointBufferF64 **ppBuffer);

//!
//! Create a 64bit integer, point buffer object
//!
//! @param ppBuffer The pointer pointer of the udPointBufferI64. This will allocate an instance of `udPointBufferI64` into `ppBuffer`.
//! @param maxPoints The maximum number of points that this buffer will contain (these are preallocated to avoid allocations later)
//! @param pAttributes The pointer to the udAttributeSet containing information on the attributes that will be available in this point buffer; NULL will have no attributes. An internal copy is made of this attribute set.
//! @return A udError value based on creation status.
//! @note The application should call **udPointBufferI64_Destroy** with `ppBuffer` to destroy the point buffer object once it's no longer needed.
//!
UDSDKDLL_API enum udError udPointBufferI64_Create(struct udPointBufferI64 **ppBuffer, uint32_t maxPoints, struct udAttributeSet *pAttributes);

//!
//! Destroys the udPointBufferI64.
//!
//! @param ppBuffer The pointer pointer of the ppBuffer. This will deallocate any memory used.
//! @return A udError value based on the result of the destruction.
//! @note The value of `ppBuffer` will be set to `NULL`.
//!
UDSDKDLL_API enum udError udPointBufferI64_Destroy(struct udPointBufferI64 **ppBuffer);

//!
//! Create a 64bit unsigned integer, point buffer object
//!
//! @param ppBuffer The pointer pointer of the udPointBufferU64. This will allocate an instance of `udPointBufferU64` into `ppBuffer`.
//! @param maxPoints The maximum number of points that this buffer will contain (these are preallocated to avoid allocations later)
//! @param pAttributes The pointer to the udAttributeSet containing information on the attributes that will be available in this point buffer; NULL will have no attributes. An internal copy is made of this attribute set.
//! @return A udError value based on creation status.
//! @note The application should call **udPointBufferU64_Destroy** with `ppBuffer` to destroy the point buffer object once it's no longer needed.
//!
UDSDKDLL_API enum udError udPointBufferU64_Create(struct udPointBufferU64 **ppBuffer, uint32_t maxPoints, struct udAttributeSet *pAttributes);

//!
//! Destroys the udPointBufferU64.
//!
//! @param ppBuffer The pointer pointer of the ppBuffer. This will deallocate any memory used.
//! @return A udError value based on the result of the destruction.
//! @note The value of `ppBuffer` will be set to `NULL`.
//!
UDSDKDLL_API enum udError udPointBufferU64_Destroy(struct udPointBufferU64 **ppBuffer);

#ifdef __cplusplus
}
#endif

#endif // udPointBuffer_h__
