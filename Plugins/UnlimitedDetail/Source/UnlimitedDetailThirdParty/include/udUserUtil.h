#ifndef udUserUtil_h__
#define udUserUtil_h__

//! @file udUserUtil.h
//! Helper functions related to user management

#include <stdint.h>

#include "udDLLExport.h"
#include "udError.h"

struct udContext;

#ifdef __cplusplus
extern "C" {
#endif

  //!
  //! Changes the password of a logged in session
  //!
  //! @param pContext The context for the user to change the password of
  //! @param pNewPassword The password the user wants to have
  //! @param pOldPassword The current password for the user
  //! @return A udError value based on the result of changing the users password
  //! @warning It is best practice get the user to enter the new password twice to confirm they typed it correctly
  //!
  UDSDKDLL_API enum udError udUserUtil_ChangePassword(struct udContext *pContext, const char *pNewPassword, const char *pOldPassword);

  //!
  //! Attempts to register a user using the simple registration system
  //!
  //! @param pServerURL The address of the server to register the user on
  //! @param pName The users name
  //! @param pEmail The users email address
  //! @param pApplicationName The name of this application (analytical to help us improve user experience; can be NULL)
  //! @param marketingEmailOptIn Not 0 if the user ACCEPTS being sent marketing material via email
  //! @return A udError value based on the result of registering the user
  //! @note This will return success in a lot of 'failure' cases and alert the user using the email address
  //! @warning The user must actively click a check box that is clearly marked as consent to receive marketing materials via email 
  //!
  UDSDKDLL_API enum udError udUserUtil_Register(const char *pServerURL, const char *pName, const char *pEmail, const char *pApplicationName, uint32_t marketingEmailOptIn);

  //!
  //! Sends an email with password reset instructions to the provided email
  //!
  //! @param pServerURL The address of the server to register the user on
  //! @param pEmail The users email address
  //! @return A udError value based on the result of registering the password reset
  //! @note This will return success in a lot of 'failure' cases and alert the user using the email address
  //!
  UDSDKDLL_API enum udError udUserUtil_ForgotPassword(const char *pServerURL, const char *pEmail);

#ifdef __cplusplus
}
#endif

#endif // udUserUtil_h__
