#ifndef udCompare_h__
#define udCompare_h__

//! @file udCompare.h
//! udCompare.h provides an interface to compare Unlimited Detail models.

#include <stdint.h>
#include "udDLLExport.h"
#include "udError.h"
#include "udMeshing.h"

#ifdef __cplusplus
extern "C" {
#endif
  struct udConvertContext;

  struct udCompareBPAOptions
  {
    const char *pBaseModelPath;       //! The path to the UDS model that pComparisonModel will be compared against
    const char *pComparisonModelPath; //! The path to the UDS model for comparison
    const char *pOutputName;          //! The path that the displacement result UDS will be written to
    struct udMeshingBPAOptions meshingOptions; //! The meshing options associated with this comparison operation
  };

  //!
  //! Compares input models using the Ball-Pivot Algorithm (BPA). A meshing operation is first performed using BPA followed by a cloud to model (C2M) comparison
  //!
  //! @param pConvertContext The convert context to be used to compare the models.
  //! @param pOptions The parameters passed to the BPA algorithm
  //! @result A udError value based on the result of setting up BPA on the input models.
  //! @note This function does not start the conversion process, this allows the user to make additional changes to the convert job.
  //!
  UDSDKDLL_API enum udError udCompare_BPA(struct udConvertContext *pConvertContext, struct udCompareBPAOptions *pOptions);

#ifdef __cplusplus
}
#endif

#endif // udCompare_h__
