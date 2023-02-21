#include "UdSDKFunctionLibrary.h"
#include "UdSDKComposite.h"

bool UUdSDKFunctionLibrary::Load()
{
	int Success = -1;
	FUdAsset assert;
	//if (CUdSDKComposite::Get())
	//	Success = CUdSDKComposite::Get()->Load(assert);

	return Success == 0 ? true : false;
}

bool UUdSDKFunctionLibrary::IsLogin()
{
	bool Flag = false;
	if (CUdSDKComposite::Get())
	{
		Flag = CUdSDKComposite::Get()->IsLogin();
	}
	return Flag;
}

bool UUdSDKFunctionLibrary::Login()
{
	int Success = -1;
	if (CUdSDKComposite::Get())
	{
		Success = CUdSDKComposite::Get()->LoginFunction();
	}
	return Success == 0 ? true : false;
}

bool UUdSDKFunctionLibrary::Exit()
{
	int Success = -1;
	if (CUdSDKComposite::Get())
	{
		Success = CUdSDKComposite::Get()->Exit();
	}
	return Success == 0 ? true : false;
}

FDelegateHandle UUdSDKFunctionLibrary::AddLoginDelegateLambda(const FunUdSDKDelegate& FuncDelegate)
{
	if (CUdSDKComposite::Get())
	{
		return CUdSDKComposite::Get()->LoginDelegate.AddLambda(FuncDelegate);
	}
	return FDelegateHandle();
}

void UUdSDKFunctionLibrary::DeleteLoginDelegateHandle(const FDelegateHandle & Handle)
{
	CUdSDKComposite::Get()->LoginDelegate.Remove(Handle);
}

FDelegateHandle UUdSDKFunctionLibrary::AddExitDelegateLambda(const FunUdSDKDelegate& FuncDelegate)
{
	if (CUdSDKComposite::Get())
	{
		return CUdSDKComposite::Get()->ExitLaterDelegate.AddLambda(FuncDelegate);
	}
	return FDelegateHandle();
}

void UUdSDKFunctionLibrary::DeleteExitDelegateHandle(const FDelegateHandle& Handle)
{
	if (CUdSDKComposite::Get())
	{
		CUdSDKComposite::Get()->ExitLaterDelegate.Remove(Handle);
	}
}
