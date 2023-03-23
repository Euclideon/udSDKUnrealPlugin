#ifndef udStreamer_h__
#define udStreamer_h__

#include "udError.h"
#include "udDLLExport.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//!
//! @struct udStreamerInfo
//! Stores returned information from a streamer update
//!
struct udStreamerInfo
{
  uint32_t active; //!< Not 0 if streamer has blocked to load, or models are awaiting destruction
  int64_t memoryInUse; //!< Total (approximate) memory in use by the streamer (in bytes)
  int modelsActive; //!< Number of models actively requesting data
  int starvedTimeMsSinceLastUpdate; //!< Number of milliseconds spent waiting with no work to do since the previous update (ideally should be 0)
};

//!
//! Initialises the UDS streamer
//!
//! @param memoryThresholdBytes Sets the threshold for how much memory the streaming system should *attempt* to stay below in bytes. Set as 0 to use the default amount of memory for the current platform.
//! @warning If memoryThresholdBytes is non-zero (which will use the default amount of memory) the return code will be udE_CalledMoreThanOnce if the streamer has already been started
//! @note If the streamer hasn't been initialised before loading a UDS or creating a udRenderContext it will be initialised during those using default values
//!
UDSDKDLL_API enum udError udStreamer_Init(uint64_t memoryThresholdBytes);

//!
//! Deinitialises the UDS streamer (reference counted). This must be called once for every call to udStreamer_Init regardless of return code of that function to decrease the reference count.
//!
UDSDKDLL_API enum udError udStreamer_Deinit();

//!
//! Updates the UDS streamer manually (used in conjuction with udRCF_ManualStreamerUpdate)
//!
//! @param pStatus A structure to write streaming information to; Use NULL if the information isn't required
//! @note The application should call this immediately after the last render for the frame
//!
UDSDKDLL_API enum udError udStreamer_Update(struct udStreamerInfo *pStatus);

#ifdef __cplusplus
}
#endif

#endif // udStreamer_h__
