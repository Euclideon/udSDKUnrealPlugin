#ifndef udServerAPI_h__
#define udServerAPI_h__

//! @file udServerAPI.h
//! The **udServerAPI** module provides an interface to communicate with a Euclideon udServer API directly in a simple fashion.

#include "udDLLExport.h"
#include "udError.h"

#ifdef __cplusplus
extern "C" {
#endif

struct udContext;

//!
//! Queries provided API on the specified Euclideon udServer.
//!
//! @param pContext The context to execute the query with.
//! @param pAPIAddress The API address to query, this is the part of the address *after* `/api/`. The rest of the address is constructed from the context provided.
//! @param pJSON The JSON text to POST to the API address.
//! @param ppResult A pointer to a location in which the result data is to be stored.
//! @result A udError value based on the result of the API query.
//! @note The application should call **udServerAPI_ReleaseResult** with `ppResult` to destroy the data once it's no longer needed.
//!
UDSDKDLL_API enum udError udServerAPI_Query(struct udContext *pContext, const char *pAPIAddress, const char *pJSON, const char **ppResult);

//!
//! Destroys the result that was allocated.
//!
//! @param ppResult A pointer to a location in which the result data is stored.
//! @note The value of `ppResult` will be set to `NULL`.
//!
UDSDKDLL_API enum udError udServerAPI_ReleaseResult(const char **ppResult);

#ifdef __cplusplus
}
#endif

#endif // udServerAPI_h__