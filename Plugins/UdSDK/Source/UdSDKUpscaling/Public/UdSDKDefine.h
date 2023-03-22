// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"

#include "udError.h"
#include "udPointCloud.h"

#include "Containers/UnrealString.h"
#include "Core/udMath.h"

#include <cstdint>
#include <string>
#include <vector>



struct FUdAsset
{
	int64_t id = -1;
	FString name = "";
	FString description = "";
	FString attribution = "";
	FString type = "";
	int64_t bytes = 0;
	FString dateAdded = "";
	FString status = "";
	int8_t percentComplete = 0;

	FString folder = "";
	FString url = "";
	FVector pivot = FVector::ZeroVector;
	void* pPointCloud = nullptr;
};

const TMap<udError, FString> g_udSDKErrorInfo = {
	{ udE_Success,TEXT("Indicates the operation was successful.") },
	{ udE_Failure,TEXT("A catch-all value that is rarely used, internally the below values are favored.") },
	{ udE_NothingToDo,TEXT("The operation didn't specifically fail but it also didn't do anything.") },
	{ udE_InternalError,TEXT("There was an internal error that could not be handled.") },
	{ udE_NotInitialized,TEXT("The request can't be processed because an object hasn't been configured yet.") },
	{ udE_InvalidConfiguration,TEXT("Something in the request is not correctly configured or has conflicting settings.") },
	{ udE_InvalidParameter,TEXT("One or more parameters is not of the expected format.") },
	{ udE_OutstandingReferences,TEXT("The requested operation failed because there are still references to this object.") },
	{ udE_MemoryAllocationFailure,TEXT("udSDK wasn't able to allocate enough memory for the requested feature.") },
	{ udE_CountExceeded,TEXT("An internal count was exceeded by the request, generally going beyond the end of a buffer or internal limit.") },
	{ udE_NotFound,TEXT("The requested item wasn't found or isn't currently available.") },
	{ udE_BufferTooSmall,TEXT("Either the provided buffer or an internal one wasn't big enough to fufill the request.") },
	{ udE_FormatVariationNotSupported,TEXT("The supplied item is an unsupported variant of a supported format.") },
	{ udE_ObjectTypeMismatch,TEXT("There was a mismatch between what was expected and what was found.") },
	{ udE_CorruptData,TEXT("The data/file was corrupt.") },
	{ udE_InputExhausted,TEXT("The input buffer was exhausted so no more processing can occur.") },
	{ udE_OutputExhausted,TEXT("The output buffer was exhausted so no more processing can occur.") },
	{ udE_CompressionError,TEXT("There was an error in compression or decompression.") },
	{ udE_Unsupported,TEXT("This functionality has not yet been implemented (usually some combination of inputs isn't compatible yet).") },
	{ udE_Timeout,TEXT("The requested operation timed out. Trying again later may be successful.") },
	{ udE_AlignmentRequired,TEXT("Memory alignment was required for the operation.") },
	{ udE_DecryptionKeyRequired,TEXT("A decryption key is required and wasn't provided.") },
	{ udE_DecryptionKeyMismatch,TEXT("The provided decryption key wasn't the required one.") },
	{ udE_SignatureMismatch,TEXT("The digital signature did not match the expected signature.") },
	{ udE_ObjectExpired,TEXT("The supplied object has expired.") },
	{ udE_ParseError,TEXT("A requested resource or input was unable to be parsed.") },
	{ udE_InternalCryptoError,TEXT("There was a low level cryptography issue.") },
	{ udE_OutOfOrder,TEXT("There were inputs that were provided out of order.") },
	{ udE_OutOfRange,TEXT("The inputs were outside the expected range.") },
	{ udE_CalledMoreThanOnce,TEXT("This function was already called.") },
	{ udE_ImageLoadFailure,TEXT("An image was unable to be parsed. This is usually an indication of either a corrupt or unsupported image format.") },
	{ udE_StreamerNotInitialised,TEXT("The streamer needs to be initialised before this function can be called.") },
	{ udE_OpenFailure,TEXT("The requested resource was unable to be opened.") },
	{ udE_CloseFailure,TEXT("The resource was unable to be closed.") },
	{ udE_ReadFailure,TEXT("A requested resource was unable to be read.") },
	{ udE_WriteFailure,TEXT("A requested resource was unable to be written.") },
	{ udE_SocketError,TEXT("There was an issue with a socket problem.") },
	{ udE_DatabaseError,TEXT("A database error occurred.") },
	{ udE_ServerError,TEXT("The server reported an error trying to complete the request.") },
	{ udE_AuthError,TEXT("The provided credentials were declined (usually email or password issue).") },
	{ udE_NotAllowed,TEXT("The requested operation is not allowed (usually this is because the operation isn't allowed in the current state).") },
	{ udE_InvalidLicense,TEXT("The required license isn't available or has expired.") },
	{ udE_Pending,TEXT("A requested operation is pending.") },
	{ udE_Cancelled,TEXT("The requested operation was cancelled (usually by the user).") },
	{ udE_OutOfSync,TEXT("There is an inconsistency between the internal udSDK state and something external. This is usually because of a time difference between the local machine and a remote server.") },
	{ udE_SessionExpired,TEXT("The udServer has terminated your session.") },
	{ udE_ProxyError,TEXT("There was some issue with the provided proxy information (either a proxy is in the way or the provided proxy info wasn't correct).") },
	{ udE_ProxyAuthRequired,TEXT("A proxy has requested authentication.") },
	{ udE_ExceededAllowedLimit,TEXT("The requested operation failed because it would exceed the allowed limits (generally used for exceeding server limits like number of projects).") },
	{ udE_RateLimited,TEXT("This functionality is currently being rate limited or has exhausted a shared resource. Trying again later may be successful.") },
	{ udE_PremiumOnly,TEXT("The requested operation failed because the current session is not for a premium user.") },
	{ udE_Count,TEXT("Type the number") }
};

static auto GetError = [](udError e)->const TCHAR* {
	if (e <= udE_Count)
	{
		return *g_udSDKErrorInfo[e];
	}
	return TEXT("Unknown error.");
};

template <class Type>
class FUdSDKResourceBulkData : public FResourceBulkDataInterface
{
public:
	FUdSDKResourceBulkData()
	{
		
	}

	void ResizeArray(int32 Size)
	{
		Data.SetNum(Size, false);
		//Data.Empty(Size);
		//Data.AddUninitialized(Size);
	}

	/**
	* @return ptr to the resource memory which has been preallocated
	*/
	virtual const void* GetResourceBulkData() const override
	{
		return Data.GetData();
	}

	Type* GetData()
	{
		return Data.GetData();
	}

	uint32 GetTypeSize()
	{
		return sizeof(Type);
	}

	/**
	* @return size of resource memory
	*/
	virtual uint32 GetResourceBulkDataSize() const override
	{
		return Data.Num() * sizeof(Type);
	}

	/**
	* Free memory after it has been used to initialize RHI resource
	*/
	virtual void Discard() override
	{
		//Data.Empty();
	}

private:
	TArray<Type>	Data;
};






