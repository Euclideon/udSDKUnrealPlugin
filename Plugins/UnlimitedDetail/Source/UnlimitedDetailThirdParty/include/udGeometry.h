#ifndef udGeometry_h__
#define udGeometry_h__

//! @file udGeometry.h
//! The **udGeometry** object provides an interface to filter Euclideon Unlimited Detail models using geometric filters.

#include <stdint.h>
#include "udDLLExport.h"
#include "udError.h"
#include "udMathTypes.h"
#include "udAttributes.h"

#ifdef __cplusplus
extern "C" {
#endif

  //!
  //! The currently supported geometry types
  //! 
  enum udGeometryType
  {
    udGT_Inverse, //!< An inversion filter; flips the udGeometryTestResult of the child udGeometry node
    udGT_CircleXY, //!< A 2D Circle with an infinite Z value
    udGT_RectangleXY, //!< A 2D Rectangle with an infinite Z value
    udGT_PolygonXY, //!< A 2D Polygon with rotation (quaternion) to define the up of the polygon
    udGT_PolygonPerspective, //!< A 2D polygon with a perspective projection to the screen plane
    udGT_Capsule, //!< A line with a radius from the line; forming hemispherical caps at the end of the line
    udGT_Sphere, //!< A radius from a point
    udGT_HalfSpace, //!< A binary space partition allowing 1 side of a plane
    udGT_AABB, //!< An axis aligned box; Use with caution. OBB while less performant correctly handles transforms
    udGT_OBB, //!< An oriented bounding box using half size and orientation
    udGT_CSG, //!< A constructed solid geometry that uses a udGeometryCSGOperation to join to child udGeometry nodes
    udGT_Attribute, //!< Filter based on attribute values (automatic pass if no attribute information passed to test function)

    udGT_Count //!< Count helper value to iterate this enum
  };

  //!
  //! @typedef udGeometryDouble2
  //! A 2D geometric vector with double precision
  //! 
  typedef struct udMathDouble2 udGeometryDouble2;

  //!
  //! @typedef udGeometryDouble3
  //! A 3D geometric vector with double precision
  //! 
  typedef struct udMathDouble3 udGeometryDouble3;

  //!
  //! @typedef udGeometryDouble4
  //! A 4D geometric vector, or 3D vector for homogeneous coordinates with double precision
  //! 
  typedef struct udMathDouble4 udGeometryDouble4;

  //!
  //! @typedef udGeometryDouble4x4
  //! A 4x4 geometric matrix with double precision
  //! 
  typedef struct udMathDouble4x4 udGeometryDouble4x4;

  //!
  //! @struct udGeometryVoxelNode
  //! The geometric representation of a Node in a Unlimited Detail Model.
  //! 
  struct udGeometryVoxelNode
  {
    udGeometryDouble3 minPos; //!< The Bottom, Left, Front corner of the voxel (closest to the origin)
    double childSize; //!< The half size of the voxel (which is the same size as this voxels children)
    struct udNode *pNode; //!< Optional pointer to the node for testing attributes
    uint8_t childDepth; //!< Optional depth down the tree value for additional context (0=unpecified, 1=root etc)
  };

  //!
  //! @struct udGeometryCircleXY
  //! The geometric representation of a Circle.
  //! 
  struct udGeometryCircleXY
  {
    udGeometryDouble2 centre; //!< The centre of the circle
    double radius; //!< The radius of the circle
  };

  //!
  //! @struct udGeometryRectangleXY
  //! The geometric representation of a Rectangle.
  //! 
  struct udGeometryRectangleXY
  {
    udGeometryDouble2 minPoint; //!< The lowest point of the rectangle
    udGeometryDouble2 maxPoint; //!< The highest point of the rectangle
  };

  //!
  //! @struct udGeometryPolygonXYZ
  //! The geometric representation of a Polygon.
  //! 
  struct udGeometryPolygonXYZ
  {
    uint32_t pointCount; //!< THe number of points defining the polygon
    udGeometryDouble3 *pointsList; //!< The list of points defining the polygon
    udGeometryDouble4 rotationQuat; //!< The rotation of the polygon
  };

  //!
  //! @struct udGeometryPolygonPerspective
  //! The geometric representation of a Polygon with a perspective projection.
  //! 
  struct udGeometryPolygonPerspective
  {
    uint32_t pointCount; //!< The number of points defining the polygon
    udGeometryDouble3 *pointsList; //!< The list of points defining the polygon
    udGeometryDouble4 rotationQuat; //!< The rotation of the polygon
    udGeometryDouble4x4 worldToScreen; //!< The matrix to project from World space to Screen space
    udGeometryDouble4x4 projectionMatrix; //!< The matrix to project the points of the polygon
    udGeometryDouble4x4 cameraMatrix; //!< The camera matrix
    udGeometryDouble3 normRight; //!< The normal on the right of the plane
    udGeometryDouble3 normLeft; //!< The normal on the left of the plane
    udGeometryDouble3 normTop; //!< The normal on the top of the plane
    udGeometryDouble3 normBottom; //!< The normal on the bottom of the plane
    double nearPlane; //!< the near plane distance
    double farPlane; //!< The far plane distance
  };

  //!
  //! @struct udGeometryCapsule
  //! Stores the properties of a geometric capsule
  //! 
  struct udGeometryCapsule
  {
    udGeometryDouble3 point1; //!< One end of the line
    udGeometryDouble3 point2; //!< The other end of the line
    double radius; //!< The radius around the line

    // Derived values
    udGeometryDouble3 axisVector; //!< The vector of the line
    double length; //!< The length of the line
  };

  //!
  //! @struct udGeometrySphere
  //! Stores the properties of a geometric sphere
  //! 
  struct udGeometrySphere
  {
    udGeometryDouble3 center; //!< The center of the sphere
    double radius; //!< The radius of the sphere
  };

  //!
  //! @struct udGeometryHalfSpace
  //! Stores the properties of a geometric half space
  //! 
  struct udGeometryHalfSpace
  {
    udGeometryDouble4 plane; //!< The parameters to define the plane (normal XYZ and offset from origin)
  };

  //!
  //! @struct udGeometryAABB
  //! Stores the properties of a geometric axis aligned bounding box
  //!
  struct udGeometryAABB
  {
    udGeometryDouble3 center; //!< The point at the center of the AABB
    udGeometryDouble3 extents; //!< The half space size of the AABB
  };

  //!
  //! @struct udGeometryOBB
  //! Stores the properties of a geometric axis aligned bounding box (extending AABB)
  //! 
  struct udGeometryOBB
  {
    udGeometryDouble3 center; //!< The point at the center of the AABB
    udGeometryDouble3 extents; //!< The half space size of the AABB
    udGeometryDouble4x4 rotationMatrix; //!< The transform that represents the rotation
  };

  //!
  //! The Constructive Solid Geometry operations
  //! 
  enum udGeometryCSGOperation
  {
    udGCSGO_Union = 0,   //!< A union CSG operation; any point matching the one or the other geometry (OR operation)
    udGCSGO_Difference,  //!< A subtractive CSG operation; any point in the first geometry but not matching the second geometry (XOR operation)
    udGCSGO_Intersection //!< An intersection CSG operation; any point that matches both geometries (AND operation)
  };

  struct udGeometry; // This is defined further down this header but is needed to be predeclared for udGeometryCSG

  //!
  //! @struct udGeometryInverse
  //! Stores the properties of an inversed udGeometry node
  //! 
  struct udGeometryInverse
  {
    const struct udGeometry *pInverseOf; //!< The inverse geometry
    int owns; //!< If non-zero pInverseOf is owned by this need (and will need to be cleaned up)
  };

  //!
  //! @struct udGeometryCSG
  //! Stores the properties of a CSG udGeometry node
  //! 
  struct udGeometryCSG
  {
    const struct udGeometry *pFirst; //!< The first geometry
    const struct udGeometry *pSecond; //!< The second geometry
    enum udGeometryCSGOperation operation; //!< The operation applied to the 2 geometries
    int owns; //!< non zero if it owns both children
  };

  //!
  //! @struct udGeometryAttribute
  //! Stores the properties of a attribute udGeometry node
  //! 
  struct udGeometryAttribute
  {
    enum udStdAttribute attr;
    union
    {
      int64_t iValue;
      double fValue;
    } data;
  };

  //!
  //! The results of a geometry test
  //! 
  enum udGeometryTestResult
  {
    udGTR_Reject = 0,   //!< The node is either totally outside of the geometry filter OR fails attribute filter
    udGTR_Accept = 1,   //!< The node is both totally inside of the geometry AND fully matches attribute filter (and no further tests are required)
    udGTR_Partial = 2,  //!< The node is partiall filtered, should be accepted but child nodes must be re-tested (eg overlapping the boundary of the geometry)

    // Deprecated:
    udGTR_CompletelyOutside = udGTR_Reject,
    udGTR_CompletelyInside = udGTR_Accept,
    udGTR_PartiallyOverlap = udGTR_Partial
  };

  //! The function that will be called for each node until the node is either inside or outside of the geometry
  typedef enum udGeometryTestResult udGeometryTestFunc(const struct udGeometry *pGeometry, const struct udGeometryVoxelNode *pVoxel);

  //! This sets up pFilterOut by doing the pMatrix transform on pFilterIn (which isn't modified)- pFilterOut needs to be deinited after this
  typedef enum udError udGeometryTransform(const struct udGeometry *pFilterIn, struct udGeometry *pFilterOut, const udGeometryDouble4x4 *pMatrix);

  //! Optional function to return a crc of ADDITIONAL data outside the structure, such as pointers in a CSG/Inverse type etc.
  typedef uint32_t udGeometryCrc(const struct udGeometry *pGeometry);

  //! This sets up pFilterOut by doing the pMatrix transform on pFilterIn (which isn't modified)- pFilterOut needs to be deinited after this
  typedef void udGeometryDeinit(struct udGeometry *pGeometry);

  //!
  //! @struct udGeometry
  //! Stores the information required for all udGeometry shapes
  //! @warning This struct will change drastically as udSDK shifts to programable query filters
  //!
  struct udGeometry
  {
    udGeometryTestFunc *pTestFunc; //!< The function to call to test the geometry
    udGeometryTransform *pTransformFunc; //!< The function to transform this geometry using a linear matrix
    udGeometryCrc *pExtraCRCFunc; //!< When non-null, the function to perform crc calculation on ADDITIONAL data outside the base structure that represents the geometry (0 is valid for no extra data)
    udGeometryDeinit *pDeinitFunc; //!< The function that is called when this is cleaned up

    union
    {
      struct udGeometryInverse inverse;
      struct udGeometryCircleXY circleXY;
      struct udGeometryRectangleXY rectangleXY;
      struct udGeometryPolygonXYZ polygonXYZ;
      struct udGeometryPolygonPerspective polygonPerspective;
      struct udGeometryCapsule capsule;
      struct udGeometrySphere sphere;
      struct udGeometryHalfSpace halfSpace;
      struct udGeometryAABB aabb;
      struct udGeometryOBB obb;
      struct udGeometryCSG csg;
      struct udGeometryAttribute attribute;
    } data; //!< The geometry used

    enum udGeometryType type; //!< The type of the geometry for internal verification
  };

  //!
  //! Helper to initialise a geometry that simply inverts another primitive (CompletelyOutside <-> CompletelyInside)
  //! @param pGeometry The preallocated udGeometry to init
  //! @param pSource The Geometry to inverse
  //!
  UDSDKDLL_API enum udError udGeometry_InitInverse(struct udGeometry *pGeometry, const struct udGeometry *pSource);

  //!
  //! Helper to initialise a 2D circle extended to infinity in Z (elevation)
  //! @param pGeometry The preallocated udGeometry to init
  //! @param centre THe centre fo the Circle
  //! @param radius The radius of the circle
  //!
  UDSDKDLL_API enum udError udGeometry_InitCircleXY(struct udGeometry *pGeometry, const udGeometryDouble2 centre, double radius);

  //!
  //! Helper to initialise a 2D rectangle extended to infinity in Z (elevation)
  //! @param pGeometry The preallocated udGeometry to init
  //! @param point1 The first point to define a rectangle
  //! @param point2 The second point to define a rectangle
  //!
  UDSDKDLL_API enum udError udGeometry_InitRectangleXY(struct udGeometry *pGeometry, const udGeometryDouble2 point1, const udGeometryDouble2 point2);

  //!
  //! Helper to initialise a Polygon shape extened to infinity along a direction
  //! @param pGeometry The preallocated udGeometry to init
  //! @param pXYCoords the list of 2D positions defining the polygon
  //! @param count the number of points in pXYCoords list
  //! @param rotationQuat The rotation quaternion between Up and the direction of the polygon
  //!
  UDSDKDLL_API enum udError udGeometry_InitPolygonXY(struct udGeometry *pGeometry, const udGeometryDouble3 *pXYCoords, uint32_t count, const udGeometryDouble4 rotationQuat);

  //!
  //! Helper to initialise a Polygon shape extended to infinity with a perspective projection
  //! @param pGeometry The preallocated udGeometry to init
  //! @param pXYCoords the list of 2D positions defining the polygon
  //! @param count the number of points in pXYCoords list
  //! @param projectionMatrix The projection matrix of model to world
  //! @param cameraMatrix The projection matrix of world to screen
  //! @param nearPlaneOffset The offset off the near plane to detect if voxel is visible by the camera
  //! @param farPlaneOffset The offset off the far plane to detect if voxel is visible by the camera
  //! 
  UDSDKDLL_API enum udError udGeometry_InitPolygonPerspective(struct udGeometry *pGeometry, const udGeometryDouble2 *pXYCoords, uint32_t count, udGeometryDouble4x4 projectionMatrix, udGeometryDouble4x4 cameraMatrix, double nearPlaneOffset, double farPlaneOffset);

  //!
  //! Helper to initialise a CSG shape
  //! @param pGeometry The preallocated udGeometry to init
  //! @param pGeometry1 First Geometry
  //! @param pGeometry2 Second Geometry
  //! @param function the function to apply to these 2 geometries (Union, Difference, Intersection)
  //!
  UDSDKDLL_API enum udError udGeometry_InitCSG(struct udGeometry *pGeometry, const struct udGeometry *pGeometry1, const struct udGeometry *pGeometry2, enum udGeometryCSGOperation function);

  //!
  //! Helper to initialise a plane extended to infinity in the direction of its normal
  //! @param pGeometry The preallocated udGeometry to init
  //! @param point The first point to define the space
  //! @param normal The normal vector of the defined space
  //!
  UDSDKDLL_API enum udError udGeometry_InitHalfSpace(struct udGeometry *pGeometry, const udGeometryDouble3 point, const udGeometryDouble3 normal);

  //!
  //! Helper to initialise a Capsule
  //! @param pGeometry The preallocated udGeometry to init
  //! @param point1 First point to define a capsule at this position
  //! @param point2 Second point to define a capsule
  //! @param radius Radius of the capsule
  //!
  UDSDKDLL_API enum udError udGeometry_InitCapsule(struct udGeometry *pGeometry, const udGeometryDouble3 point1, const udGeometryDouble3 point2, double radius);

  //!
  //! Helper to initialise a Sphere
  //! @param pGeometry The preallocated udGeometry to init
  //! @param center The center of the Sphere
  //! @param radius The radius of the sphere
  //!
  UDSDKDLL_API enum udError udGeometry_InitSphere(struct udGeometry *pGeometry, const udGeometryDouble3 center, double radius);

  //!
  //! Helper to initialise an Axis Aligned Box using the min and max point
  //! @param pGeometry The preallocated udGeometry to init
  //! @param point1 First point to define a box starting at this position
  //! @param point2 Last point to define a box ending at this position
  //!
  UDSDKDLL_API enum udError udGeometry_InitAABBFromMinMax(struct udGeometry *pGeometry, const udGeometryDouble3 point1, const udGeometryDouble3 point2);

  //!
  //! Helper to initialise an Axis Aligned Box using center and extents
  //! @param pGeometry The preallocated udGeometry to init
  //! @param centre The centre of the Axis Aligned Box
  //! @param extents The dimension of the Axis Aligned Box
  //!
  UDSDKDLL_API enum udError udGeometry_InitAABBFromCentreExtents(struct udGeometry *pGeometry, const udGeometryDouble3 centre, const udGeometryDouble3 extents);

  //!
  //! Helper to initialise an arbitrarily aligned three dimensional box, centered at centerPoint, rotated about the center
  //!
  //! @param pGeometry The preallocated udGeometry to init
  //! @param centerPoint The centre of the box
  //! @param extents The distances from the center to the sides (half the size of the dimensions of the box)
  //! @param rotations The rotations of the box in radians about x,y,z
  UDSDKDLL_API enum udError udGeometry_InitOBB(struct udGeometry *pGeometry, const udGeometryDouble3 centerPoint, const udGeometryDouble3 extents, const udGeometryDouble3 rotations);

  //!
  //! Helper to initialise an attribute equality filter, testing the attribute for equality with integer based comparitors
  //!
  //! @param pGeometry The preallocated udGeometry to init
  //! @param attr Attribute from standard attribute set
  //! @param value Value to compare node's attribute with (integer version)
  UDSDKDLL_API enum udError udGeometry_InitAttributeEqInt(struct udGeometry *pGeometry, enum udStdAttribute attr, int64_t value);

  //!
  //! This cleans up internal allocations
  //!
  UDSDKDLL_API void udGeometry_Deinit(struct udGeometry *pGeometry);

  //!
  //! Return a crc of the geometry (and sub-geometry in the case of CSG etc)
  //! 
  //! @param pGeometry Pointer to geometry
  //! @return Crc {number} of the geometry, zero if pGeometry is passed as null
  //! 
  UDSDKDLL_API uint32_t udGeometry_CRC(const struct udGeometry *pGeometry);

  //!
  //! Helper to create a pointer to an allocated udGeometry struct. This is a conveneince for wrapping libraries that do not need or have a concept of the underlying object
  //! It is NOT recommended to use this function in applications where creating a udGeometry struct directly is possible (either using an allocation or on stack)
  //! 
  //! @param ppGeometry Pointer to memory location with which to return the allocated struct
  //! 
  UDSDKDLL_API enum udError udGeometry_Create(struct udGeometry **ppGeometry);

  //!
  //! Free the struct allocated by udGeometry_Create
  //! udGeometry_DeInit should be called on the struct first before calling this function.
  //! Calling this function on a udGeometry struct that was not allocated using udGeometry_Create may result in a crash
  //! 
  //! @param ppGeometry Pointer to memory location of struct to free
  //! 
  UDSDKDLL_API void udGeometry_Destroy(struct udGeometry **ppGeometry);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // udGeometry_h__