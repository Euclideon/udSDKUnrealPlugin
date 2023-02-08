#pragma once
#include "CoreMinimal.h"
#include "UdSDKDefine.h"
#include <functional>

#include "UdSDKFunctionLibrary.generated.h"

typedef std::function<void()>			FunUdSDKDelegate;

UCLASS()
class UDSDKUPSCALING_API UUdSDKFunctionLibrary : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "UdComposite_Login", Keywords = "UDC"), Category = "UDComposite")
	static bool Login();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "UdComposite_Exit", Keywords = "UDC"), Category = "UDComposite")
	static bool Exit();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "UdComposite_Load", Keywords = "UDC"), Category = "UDComposite")
	static bool Load();

	UFUNCTION(BlueprintPure, meta = (DisplayName = "UdComposite_IsLogin", Keywords = "UDC"), Category = "UDComposite")
	static bool IsLogin();


	static FDelegateHandle AddLoginDelegateLambda(const FunUdSDKDelegate& FuncDelegate);
	static void DeleteLoginDelegateHandle(const FDelegateHandle& Handle);

	static FDelegateHandle AddExitDelegateLambda(const FunUdSDKDelegate& FuncDelegate);
	static void DeleteExitDelegateHandle(const FDelegateHandle& Handle);
	
};




