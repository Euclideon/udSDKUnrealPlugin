#ifndef udAttributes_h__
#define udAttributes_h__

//! @file udAttributes.h
//! udAttributes.h provides an interface to attribute streams of Unlimited Detail models.

#include <stdint.h>
#include "udDLLExport.h"
#include "udError.h"

#ifdef __cplusplus
extern "C" {
#endif

  //!
  //! A list of standard UDS attributes
  //!
  enum udStdAttribute
  {
    udSA_GPSTime, //!< udATI_float64 
    udSA_PrimitiveID, //!< udATI_uint32  
    udSA_ARGB, //!< udATI_color32 
    udSA_Normal, //!< udATI_normal32
    udSA_Red, //!< Legacy 16bit Red channel
    udSA_Green, //!< Legacy 16bit Green channel
    udSA_Blue, //!< Legacy 16bit Blue channel
    udSA_Intensity, //!< udATI_uint16  
    udSA_NIR, //!< udATI_uint16  
    udSA_ScanAngle, //!< udATI_uint16  
    udSA_PointSourceID, //!< udATI_uint16  
    udSA_Classification, //!< udATI_uint8   
    udSA_ReturnNumber, //!< udATI_uint8   
    udSA_NumberOfReturns, //!< udATI_uint8   
    udSA_ClassificationFlags, //!< udATI_uint8   
    udSA_ScannerChannel, //!< udATI_uint8   
    udSA_ScanDirection, //!< udATI_uint8   
    udSA_EdgeOfFlightLine, //!< udATI_uint8   
    udSA_ScanAngleRank, //!< udATI_uint8   
    udSA_LasUserData, //!< Specific LAS User data field (udATI_uint8)

    udSA_Count, //!< Count helper value to iterate this enum
    udSA_AllAttributes = udSA_Count, //!< Internal sentinal value used by some functions to indicate getting start of interleaved attributes
    udSA_First = 0, //!< Generally used to initialise an attribute value for use in loops
  };

  //!
  //! The standard UDS attributes provided as a bitfield
  //! @note udStdAttributeContent enums are guaranteed to be 1 << associated udStdAttribute value
  //!
  enum udStdAttributeContent
  {
    udSAC_None = (0),
    udSAC_GPSTime = (1 << udSA_GPSTime),
    udSAC_PrimitiveID = (1 << udSA_PrimitiveID),
    udSAC_ARGB = (1 << udSA_ARGB),
    udSAC_Normal = (1 << udSA_Normal),
    udSAC_Red = (1 << udSA_Red),
    udSAC_Green = (1 << udSA_Green),
    udSAC_Blue = (1 << udSA_Blue),
    udSAC_Intensity = (1 << udSA_Intensity),
    udSAC_NIR = (1 << udSA_NIR),
    udSAC_ScanAngle = (1 << udSA_ScanAngle),
    udSAC_PointSourceID = (1 << udSA_PointSourceID),
    udSAC_Classification = (1 << udSA_Classification),
    udSAC_ReturnNumber = (1 << udSA_ReturnNumber),
    udSAC_NumberOfReturns = (1 << udSA_NumberOfReturns),
    udSAC_ClassificationFlags = (1 << udSA_ClassificationFlags),
    udSAC_ScannerChannel = (1 << udSA_ScannerChannel),
    udSAC_ScanDirection = (1 << udSA_ScanDirection),
    udSAC_EdgeOfFlightLine = (1 << udSA_EdgeOfFlightLine),
    udSAC_ScanAngleRank = (1 << udSA_ScanAngleRank),
    udSAC_LasUserData = (1 << udSA_LasUserData),

    udSAC_AllAttributes = (1 << udSA_AllAttributes) - 1,
    udSAC_64BitAttributes = udSAC_GPSTime,
    udSAC_32BitAttributes = udSAC_PrimitiveID + udSAC_ARGB + udSAC_Normal,
    udSAC_16BitAttributes = udSAC_Intensity + udSAC_NIR + udSAC_ScanAngle + udSAC_PointSourceID,
  };

  //!
  //! These are the various options for how an attribute is calculated when merging voxels while generating LODs
  //!
  enum udAttributeBlendType
  {
    udABT_Mean, //!< This blend type merges nearby voxels together and finds the mean value for the attribute on those nodes
    udABT_FirstChild, //!< This blend type selects the value from one of the nodes and uses that
    udABT_NoLOD, //!< This blend type has no information in LODs and is only stored in the highest detail level

    udABT_Count //!< Total number of blend types. Used internally but can be used as an iterator max when checking attribute blend modes.
  };

  //!
  //! These are the types that could be contained in attributes
  //!
  enum udAttributeTypeInfo
  {
    udATI_Invalid = 0,
    udATI_SizeMask = 0x000ff,  // Lower 8 bits define the size in bytes - currently the actual maximum is 32
    udATI_SizeShift = 0,
    udATI_ComponentCountMask = 0x0ff00,  // Next 8 bits define the number of components, component size is size/componentCount
    udATI_ComponentCountShift = 8,
    udATI_Signed = 0x10000,  // Set if type is signed (used in blend functions)
    udATI_Float = 0x20000,  // Set if floating point type (signed should always be set)
    udATI_Color = 0x40000,  // Set if type is de-quantized from a color
    udATI_Normal = 0x80000,  // Set if type is encoded normal (32 bit = 16:15:1)

    // Some keys to define standard types
    udATI_uint8 = 1,
    udATI_uint16 = 2,
    udATI_uint32 = 4,
    udATI_uint64 = 8,
    udATI_int8 = 1 | udATI_Signed,
    udATI_int16 = 2 | udATI_Signed,
    udATI_int32 = 4 | udATI_Signed,
    udATI_int64 = 8 | udATI_Signed,
    udATI_float32 = 4 | udATI_Signed | udATI_Float,
    udATI_float64 = 8 | udATI_Signed | udATI_Float,
    udATI_color32 = 4 | udATI_Color,
    udATI_normal32 = 4 | udATI_Normal,
    udATI_vec3f32 = 12 | 0x300 | udATI_Signed | udATI_Float
  };

  //!
  //! @struct udAttributeDescriptor
  //! Describes the contents of an attribute stream including its size, type and how it gets blended in LOD layers
  //!
  struct udAttributeDescriptor
  {
    enum udAttributeTypeInfo typeInfo; //!< This contains information about the type
    enum udAttributeBlendType blendType; //!< Which blend type this attribute is using
    char name[64]; //!< Name of the attibute
    char prefix[16]; //!< Value prefix for display
    char suffix[16]; //!< Value suffix for display
  };

  //!
  //! @struct udAttributeSet
  //! Provides a set a attributes and includes an optimized lookup for standard types
  //!
  struct udAttributeSet
  {
    enum udStdAttributeContent content; //!< Which standard attributes are available (used to optimize lookups internally), they still appear in the descriptors
    uint32_t count; //!< How many udAttributeDescriptor objects are used in pDescriptors
    uint32_t allocated; //!< How many udAttributeDescriptor objects are allocated to be used in pDescriptors
    struct udAttributeDescriptor *pDescriptors; //!< this contains the actual information on the attributes
  };

  //!
  //! Creates a udAttributeSet
  //!
  //! @param pAttributeSet The attribute set to allocate into
  //! @param content The standard attributes that will be created, provided as bitfields
  //! @param additionalCustomAttributes The count of additional attributes to generate, these will be added to the attribute set blank after the standard attributes
  //! @return A udError value based on the result of the creation of the attribute set.
  //! @note The application should call udAttributeSet_Free with pAttributeSet to destroy the object once it's no longer needed.
  //!
  UDSDKDLL_API enum udError udAttributeSet_Create(struct udAttributeSet *pAttributeSet, enum udStdAttributeContent content, uint32_t additionalCustomAttributes);

  //!
  //! Free the memory created by a call to udAttributeSet_Create
  //!
  //! @param pAttributeSet The attribute set to free the resources of
  //! @return A udError value based on the result of the destruction of the attribute set.
  //!
  UDSDKDLL_API enum udError udAttributeSet_Destroy(struct udAttributeSet *pAttributeSet);

  //!
  //! Gets the offset for a standard attribute so that further querying of that attribute can be performed
  //!
  //! @param pAttributeSet The attribute set to get the offset for
  //! @param attribute The enum value of the attribute
  //! @param pOffset This pointer will be written to with the value of the offset if it is found
  //! @return A udError value based on the result of writing the offset to pOffset
  //!
  UDSDKDLL_API enum udError udAttributeSet_GetOffsetOfStandardAttribute(const struct udAttributeSet *pAttributeSet, enum udStdAttribute attribute, uint32_t *pOffset);

  //!
  //! Gets the offset for a named attribute so that further querying of that attribute can be performed
  //!
  //! @param pAttributeSet The attribute set to get the offset for
  //! @param pName The name of the attribute
  //! @param pOffset This pointer will be written to with the value of the offset if it is found
  //! @return A udError value based on the result of writing the offset to pOffset
  //!
  UDSDKDLL_API enum udError udAttributeSet_GetOffsetOfNamedAttribute(const struct udAttributeSet *pAttributeSet, const char *pName, uint32_t *pOffset);

  //!
  //! Gets the descriptor of a named attribute stored in a udAttributeSet
  //!
  //! @param pAttributeSet The attribute set to get the descriptor from
  //! @param pName The name of the attribute
  //! @param pDescriptor This pointer to be written to with the value of the descriptor if it is found
  //! @return A udError value based on the result of writing the descriptor to pDescriptor
  //!
  UDSDKDLL_API enum udError udAttributeSet_GetDescriptorOfNamedAttribute(const struct udAttributeSet *pAttributeSet, const char *pName, struct udAttributeDescriptor *pDescriptor);

  //!
  //! Gets the descriptor of a standard attribute 
  //!
  //! @param attribute The standard attribute to return the descriptor of
  //! @param pDescriptor This pointer to be written to with the value of the descriptor if it is found
  //! @return A udError value based on the result of writing the descriptor to pDescriptor
  //!
  UDSDKDLL_API enum udError udAttribute_GetDescriptorOfStandardAttribute(const enum udStdAttribute attribute, struct udAttributeDescriptor *pDescriptor);

#ifdef __cplusplus
}
#endif //__cplusplus

#ifdef __cplusplus
// These helper operators for the enums are provided when using a C++ compiler
inline enum udStdAttribute &operator++(enum udStdAttribute &attr) { attr = (enum udStdAttribute)(attr + 1); return attr; }

inline enum udStdAttributeContent operator|(enum udStdAttributeContent a, enum udStdAttributeContent b) { return (enum udStdAttributeContent)(int(a) | int(b)); }
inline enum udStdAttributeContent operator&(enum udStdAttributeContent a, enum udStdAttributeContent b) { return (enum udStdAttributeContent)(int(a) & int(b)); }
inline enum udStdAttributeContent operator~(enum udStdAttributeContent a) { return (enum udStdAttributeContent)(~int(a)); }

#endif //__cplusplus

#endif // udAttributes_h__
