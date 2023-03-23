// Copyright (C) RenZhai.2020.All Rights Reserved.

#pragma once 

#include "HAL/PlatformFilemanager.h"
#include "Misc/FileHelper.h"

#if PLATFORM_WINDOWS
#include "Windows/WindowsPlatformMisc.h"
#else
#if PLATFORM_LINUX
#include "Lumin/LuminPlatformMisc.h"
#else
#if PLATFORM_MAC
#include "Mac/MacPlatformMisc.h"
#else
#if PLATFORM_IOS
#include "IOS/IOSPlatformMisc.h"
#else
#if PLATFORM_ANDROID
#include "Android/AndroidPlatformMisc.h"
#else
#include "GenericPlatform/GenericPlatformFile.h"
#endif
#endif
#endif
#endif
#endif


#define DECLARATION_CALLBACK_DELEGATE(Func) \
FProgressCallbackDelegate Func##_Delegate; \
void Func##_Local(int64 ConsumedBytes, int64 TotalBytes) \
{ \
	FScopeLock ScopeLock(&OSSMutex); \
	auto EventRef = FFunctionGraphTask::CreateAndDispatchWhenReady([ConsumedBytes,TotalBytes]()\
	{\
		Func##_Delegate.ExecuteIfBound(ConsumedBytes, TotalBytes);\
	}, TStatId(), nullptr, ENamedThreads::GameThread);\
/*	FTaskGraphInterface::Get().WaitUntilTaskCompletes(EventRef);*/\
}
#define DECLARATION_1_CALLBACK_DELEGATE(Func,DelegateName) \
FBufferByOSSCallbackDelegate Func##DelegateName; \
void Func##_##DelegateName(const FString &InBuffer) \
{ \
	FScopeLock ScopeLock(&OSSMutex); \
	auto EventRef = FFunctionGraphTask::CreateAndDispatchWhenReady([InBuffer]()\
	{\
		Func##DelegateName.ExecuteIfBound(InBuffer);\
	}, TStatId(), nullptr, ENamedThreads::GameThread);\
/*	FTaskGraphInterface::Get().WaitUntilTaskCompletes(EventRef);*/\
}

#define DECLARATION_END_CALLBACK_DELEGATE(Func) \
DECLARATION_1_CALLBACK_DELEGATE(Func,_End_Delegate)

#define DECLARATION_START_CALLBACK_DELEGATE(Func) \
DECLARATION_1_CALLBACK_DELEGATE(Func,_Start_Delegate)

#define DECLARATION_CALLBACK_DELEGATE_PART(Func) \
FCallbackUploadPartDelegate Func##_Delegate; \
void Func##_Local(FString UploadId, int64  PartCount, int64 Size, int64 InTotal, bool bPart, bool  bComplete) \
{ \
	FScopeLock ScopeLock(&OSSMutex); \
	auto EventRef = FFunctionGraphTask::CreateAndDispatchWhenReady([UploadId,PartCount,Size,InTotal,bPart,bComplete]()\
	{\
		Func##_Delegate.ExecuteIfBound(UploadId, PartCount,Size,InTotal,bPart,bComplete);\
	}, TStatId(), nullptr, ENamedThreads::GameThread);\
/*	FTaskGraphInterface::Get().WaitUntilTaskCompletes(EventRef);*/\
}



#define UDSDK_INFO_MSG(Format, ...) \
{ \
	SET_WARN_COLOR(COLOR_CYAN); \
	const FString Msg = FString::Printf(TEXT(Format), ##__VA_ARGS__); \
	UE_LOG(LogTemp, Log, TEXT("%s"), *Msg); \
	CLEAR_WARN_COLOR(); \
}

#define UDSDK_DEBUG_MSG(Format, ...) \
{ \
	SET_WARN_COLOR(COLOR_CYAN); \
	const FString Msg = FString::Printf(TEXT(Format), ##__VA_ARGS__); \
	UE_LOG(LogTemp, Log, TEXT("[DEBUG] %s"), *Msg); \
	CLEAR_WARN_COLOR(); \
}

#define UDSDK_SCREENDE_DEBUG_MSG(Format, ...) \
{ \
	SET_WARN_COLOR(COLOR_CYAN); \
	const FString Msg = FString::Printf(TEXT(Format), ##__VA_ARGS__); \
	UE_LOG(LogTemp, Log, TEXT("[SUCCESS] %s"), *Msg); \
	CLEAR_WARN_COLOR(); \
	GEngine->AddOnScreenDebugMessage(-1, 100.f, FColor::White, Msg); \
}

#define UDSDK_SUCCESS_MSG(Format, ...) \
{ \
	SET_WARN_COLOR(COLOR_GREEN); \
	const FString Msg = FString::Printf(TEXT(Format), ##__VA_ARGS__); \
	UE_LOG(LogTemp, Log, TEXT("[SUCCESS] %s"), *Msg); \
	CLEAR_WARN_COLOR(); \
}

#define UDSDK_SCREENDE_SUCCESS_MSG(Format, ...) \
{ \
	SET_WARN_COLOR(COLOR_GREEN); \
	const FString Msg = FString::Printf(TEXT(Format), ##__VA_ARGS__); \
	UE_LOG(LogTemp, Log, TEXT("[SUCCESS] %s"), *Msg); \
	CLEAR_WARN_COLOR(); \
	GEngine->AddOnScreenDebugMessage(-1, 100.f, FColor::Green, Msg); \
}

#define UDSDK_WARNING_MSG(Format, ...) \
{ \
	SET_WARN_COLOR(COLOR_YELLOW); \
	const FString Msg = FString::Printf(TEXT(Format), ##__VA_ARGS__); \
	UE_LOG(LogTemp, Warning, TEXT("[WARNING] %s"), *Msg); \
	CLEAR_WARN_COLOR(); \
}

#define UDSDK_SCREEN_WARNING_MSG(Format, ...) \
{ \
	SET_WARN_COLOR(COLOR_YELLOW); \
	const FString Msg = FString::Printf(TEXT(Format), ##__VA_ARGS__); \
	const FString NewMsg = FString::Printf(TEXT("[WARNING] %s"), *Msg); \
	UE_LOG(LogTemp, Warning, TEXT("%s"), *NewMsg); \
	CLEAR_WARN_COLOR(); \
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, NewMsg); \
}

#define UDSDK_ERROR_MSG(Format, ...) \
{ \
	SET_WARN_COLOR(COLOR_RED); \
	const FString Msg = FString::Printf(TEXT(Format), ##__VA_ARGS__); \
	UE_LOG(LogTemp, Error, TEXT("[ERROR] %s"), *Msg); \
	CLEAR_WARN_COLOR(); \
}

#define UDSDK_SCREEN_ERROR_MSG(Format, ...) \
{ \
	SET_WARN_COLOR(COLOR_RED); \
	const FString Msg = FString::Printf(TEXT(Format), ##__VA_ARGS__); \
	const FString NewMsg = FString::Printf(TEXT("[ERROR] %s"), *Msg); \
	UE_LOG(LogTemp, Error, TEXT("%s"), *NewMsg); \
	CLEAR_WARN_COLOR(); \
	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, NewMsg); \
}

#define UDSDK_SCREENDE_MESSAGE(Format, ...) \
{ \
	const FString Msg = FString::Printf(TEXT(Format), ##__VA_ARGS__); \
	GEngine->AddOnScreenDebugMessage(-1, 100.f, FColor::White, Msg); \
}