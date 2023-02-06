#ifndef udMathTypes_h__
#define udMathTypes_h__

//! @file udMathTypes.h
//! udMathTypes.h provides an interface for simple math types use in udSDK functions.

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

  //!
  //! @struct udMathDouble2
  //! A 2D geometric vector with double precision
  //! 
  struct udMathDouble2
  {
    double x; //!< The x coordinate
    double y; //!< The y coordinate
  };

  //!
  //! @struct udMathLong2
  //! A 2D geometric vector with 64bit integers
  //! 
  struct udMathLong2
  {
    int64_t x; //!< The x coordinate
    int64_t y; //!< The y coordinate
  };

  //!
  //! @struct udMathULong2
  //! A 2D geometric vector with 64bit unsigned integers
  //! 
  struct udMathULong2
  {
    uint64_t x; //!< The x coordinate
    uint64_t y; //!< The y coordinate
  };

  //!
  //! @struct udMathDouble3
  //! A 3D geometric vector with double precision
  //! 
  struct udMathDouble3
  {
    double x; //!< The x coordinate
    double y; //!< The y coordinate
    double z; //!< The z coordinate
  };

  //!
  //! @struct udMathLong3
  //! A 3D geometric vector with 64bit integers
  //! 
  struct udMathLong3
  {
    int64_t x; //!< The x coordinate
    int64_t y; //!< The y coordinate
    int64_t z; //!< The z coordinate
  };

  //!
  //! @struct udMathULong3
  //! A 3D geometric vector with 64bit unsigned integers
  //! 
  struct udMathULong3
  {
    uint64_t x; //!< The x coordinate
    uint64_t y; //!< The y coordinate
    uint64_t z; //!< The z coordinate
  };

  //!
  //! @struct udMathDouble4
  //! A 4D geometric vector, or 3D vector for homogeneous coordinates with double precision
  //! 
  struct udMathDouble4
  {
    double x; //!< The x coordinate
    double y; //!< The y coordinate
    double z; //!< The z coordinate
    double w; //!< The w coordinate
  };

  //!
  //! @struct udMathLong4
  //! A 4D geometric vector, or 3D vector for homogeneous coordinates with 64bit integers
  //! 
  struct udMathLong4
  {
    int64_t x; //!< The x coordinate
    int64_t y; //!< The y coordinate
    int64_t z; //!< The z coordinate
    int64_t w; //!< The w coordinate
  };

  //!
  //! @struct udMathULong4
  //! A 4D geometric vector, or 3D vector for homogeneous coordinates with 64bit unsigned integers
  //! 
  struct udMathULong4
  {
    uint64_t x; //!< The x coordinate
    uint64_t y; //!< The y coordinate
    uint64_t z; //!< The z coordinate
    uint64_t w; //!< The w coordinate
  };

  //!
  //! @struct udMathDouble4x4
  //! A 4x4 geometric matrix with double precision
  //! 
  struct udMathDouble4x4
  {
    double array[16]; //!< The matrix elements storing the 4 values for each axis in this order: x axis, y axis, z axis, t axis
  };

#ifdef __cplusplus
}
#endif

#endif // udMathTypes_h__
