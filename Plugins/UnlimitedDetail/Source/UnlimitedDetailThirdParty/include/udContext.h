#ifndef udContext_h__
#define udContext_h__

//! @file udContext.h
//! The **udContext** object provides an interface to connect and communicate with a Euclideon udServer.
//! Once instantiated, the **udContext** can be passed into many udSDK functions to provide a context to operate within.

#include "udDLLExport.h"
#include "udError.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//!
//! @struct udContext
//! Stores the internal state of the udSDK system
//!
struct udContext;

//!
//! @struct udContextPartial
//! Stores the internal state of initial steps towards a full udContext
//!
struct udContextPartial;

//!
//! This structure stores information about the current session
//!
struct udSessionInfo
{
  uint32_t apiVersion; //!< The version of the API of the remote system (0 is offine, 1 is legacy udServer, 2 is udCloud)
  uint32_t isDomain; //!< If this is not 0 then this is a domain session (0 is non-domain session)
  uint32_t isPremium; //!< If this session will have access to premium features

  double expiresTimestamp; //!< The timestamp in UTC when the session will automatically end
  char displayName[1024]; //!< The null terminated display name of the user
};

//!
//! Establishes a connection to a Euclideon udCloud and creates a new udContext object.
//!
//! @param ppPartialContext A pointer to a location in which the new udContextPartial object is stored.
//! @param pServerURL A Server URL to the Euclideon udCloud instance.
//! @param pApplicationName The name of the application using udSDK.
//! @param pApplicationVersion The version of the application using udSDK.
//! @param ppApprovePath The address that needs to be opened in a browser window (if this is nullptr proceed to udContext_ConnectComplete)
//! @param ppApproveCode A code that the user can use to verify their session in the udCloud API on another device (can be NULL)
//! @return A udError value based on the result of the connection creation.
//! @note The application should call udContext_ConnectComplete or udContext_ConnectCancel with ppPartialContext to destroy the object
//!
UDSDKDLL_API enum udError udContext_ConnectStart(struct udContextPartial **ppPartialContext, const char *pServerURL, const char *pApplicationName, const char *pApplicationVersion, const char **ppApprovePath, const char **ppApproveCode);

//!
//! Establishes a connection to a Euclideon udCloud server and creates a new udContext object.
//!
//! @param ppContext A pointer to a location in which the new udContext object is stored.
//! @param ppPartialContext A pointer to the udContextPartial created from udContext_ConnectStart (will be freed on a successful login).
//! @return A udError value based on the result of the connection creation.
//! @note The application should call **udContext_Disconnect** with `ppContext` to destroy the object once it's no longer needed.
//! @warning ppApprovePath from udContext_ConnectStart will be invalid after this call
//!
UDSDKDLL_API enum udError udContext_ConnectComplete(struct udContext **ppContext, struct udContextPartial **ppPartialContext);

//!
//! Cancels a login attempt to a Euclideon udCloud server;
//!
//! @param ppPartialContext A pointer to the udContextPartial created from udContext_ConnectStart (will be freed).
//! @return A udError value based on the result of the connection cleanup.
//!
UDSDKDLL_API enum udError udContext_ConnectCancel(struct udContextPartial **ppPartialContext);

//!
//! Establishes a connection to Euclideon udCloud server and creates a new udContext object.
//!
//! @param ppContext A pointer to a location in which the new udContext object is stored.
//! @param pServerURL A URL to a Euclideon udCloud server to connect to.
//! @param pApplicationName The name of the application using udSDK.
//! @param pApplicationVersion The version of the application using udSDK.
//! @param pKey The provided key that will start the context
//! @return A udError value based on the result of the connection creation.
//! @note The application should call udContext_Disconnect with `ppContext` to destroy the object once it's no longer needed.
//! @warning When used from the Emscripten/WebAssembly builds it will try start a domain session when pKey is NULL
//!
UDSDKDLL_API enum udError udContext_ConnectWithKey(struct udContext **ppContext, const char *pServerURL, const char *pApplicationName, const char *pApplicationVersion, const char *pKey);

//!
//! Establishes a (legacy) connection to a Euclideon udServer and creates a new udContext object.
//!
//! @param ppContext A pointer to a location in which the new udContext object is stored.
//! @param pURL A URL to a Euclideon udServer to connect to.
//! @param pApplicationName The name of the application connecting to the Euclideon udServer.
//! @param pEmail The email address of the user connecting to the Euclideon udServer.
//! @param pPassword The password of the user connecting to the Euclideon udServer.
//! @return A udError value based on the result of the connection creation.
//! @note The application should call **udContext_Disconnect** with `ppContext` to destroy the object once it's no longer needed.
//!
UDSDKDLL_API enum udError udContext_ConnectLegacy(struct udContext **ppContext, const char *pURL, const char *pApplicationName, const char *pEmail, const char *pPassword);

//!
//! Establishes a (legacy) connection to a Euclideon udServer and creates a new udContext object.
//!
//! @param ppContext A pointer to a location in which the new udContext object is stored.
//! @param pServerURL A URL to a Euclideon udServer to connect to.
//! @param pApplicationName The name of the application connecting to the Euclideon udServer.
//! @return A udError value based on the result of the connection creation.
//! @note The application should call **udContext_Disconnect** with `ppContext` to destroy the object once it's no longer needed.
//! @warning This connect function is specific to the Emscripten/WebAssembly builds and will return udE_Unsupported on all other platforms
//!
UDSDKDLL_API enum udError udContext_ConnectFromDomain(struct udContext **ppContext, const char *pServerURL, const char *pApplicationName);

//!
//! Attempts to reestablish a connection to Euclideon udCloud, Euclideon udServer (or run offline with an offline context) and creates a new udContext object.
//!
//! @param ppContext A pointer to a location in which the new udContext object is stored.
//! @param pURL A URL to a Euclideon udServer to connect to.
//! @param pApplicationName The name of the application connecting to the Euclideon udServer.
//! @param pUsername The username of the user connecting to the Euclideon udServer.
//! @param tryDongle If 0, the dongle will not be checked (on platforms that support dongles).
//! @return A udError value based on the result of the connection creation.
//! @warning The application should call udContext_Disconnect with `ppContext` to destroy the object once it's no longer needed.
//! @warning This function can crash with some debuggers attached when trying to read from a dongle. If debugging, ensure that tryDongle is set to 0.
//! @note This function will try use a session from a dongle first if that is supported; passing null to pURL, pApplicationName and pUsername will test the dongle but will otherwise be invalid
//!
UDSDKDLL_API enum udError udContext_TryResume(struct udContext **ppContext, const char *pURL, const char *pApplicationName, const char *pUsername, uint32_t tryDongle);

//!
//! Disconnects and destroys a udContext object that was created using one of the context creation functions.
//!
//! @param ppContext A pointer to a udContext object which is to be disconnected and destroyed.
//! @param endSession Not 0 if the session will be ended (cannot be resumed)
//! @return A udError value based on the result of the connection destruction. 
//! @note The value of `ppContext` will be set to `NULL`.
//! @warning If other resources are still referencing this context this will return udE_NotAllowed until those resources are destroyed first
//!
UDSDKDLL_API enum udError udContext_Disconnect(struct udContext **ppContext, uint32_t endSession);

//!
//! Get the session information from an active udContext.
//!
//! @param pContext The udContext to get the session info for.
//! @param pInfo The preallocated structure to copy the info into.
//! @return A udError value based on the result of copying the structure
//!
UDSDKDLL_API enum udError udContext_GetSessionInfo(struct udContext *pContext, struct udSessionInfo *pInfo);

#ifdef __cplusplus
}
#endif

#endif // udContext_h__
