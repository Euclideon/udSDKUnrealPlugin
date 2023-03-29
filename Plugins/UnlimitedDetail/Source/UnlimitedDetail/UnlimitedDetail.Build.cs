// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class UnlimitedDetail : ModuleRules
{
	private string ThirdPartyPath
	{
		get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "../UnlimitedDetailThirdParty/")); }
	}
	
	public string ProjectDirectory
	{
		get
		{
			return Path.GetFullPath(Path.Combine(ModuleDirectory, "../../../../"));
		}
	}

	private void CopyToTargetBinaries(string FilePathName, string TargetPathName, ReadOnlyTargetRules Target)
	{
		string BineriesDirectory = Path.Combine(TargetPathName, "Binaries", Target.Platform.ToString());
		string Filename = Path.GetFileName(FilePathName);

		if (!Directory.Exists(BineriesDirectory))
		{
			Directory.CreateDirectory(BineriesDirectory);
		}

		string FileFullName = Path.Combine(BineriesDirectory, Filename);
		if (!File.Exists(FileFullName))
		{
			File.Copy(FilePathName, FileFullName, true);
		}

		RuntimeDependencies.Add(FileFullName);
	}
	
	public UnlimitedDetail(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
		);
				

		PrivateIncludePaths.AddRange(
				new string[] {
					//required for FPostProcessMaterialInputs
					EngineDirectory + "/Source/Runtime/Renderer/Private",
					"UnlimitedDetail/Public",
			}
		);


		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"RenderCore",
				"RHI",
				"Renderer",
				"Projects",
				"JsonUtilities",
				"Json",
				"HTTP"
				// ... add other public dependencies that you statically link with here ...
			}
		);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"Renderer",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
		);

		if (Target.Type != TargetRules.TargetType.Server)
		{
			PrivateDependencyModuleNames.AddRange(new string[] {
				"UMG",
		   });
		}

		string IncludePath = Path.Combine(ThirdPartyPath, "include");
		PublicIncludePaths.Add(IncludePath);

		string LibPath = Path.Combine(ThirdPartyPath, "lib");

		//PublicLibraryPaths.Add(LibPath);
		PublicAdditionalLibraries.Add(Path.Combine(LibPath, "udSDK.lib"));

		// Public Delay Load attempts to load the DLL after program start (beacuse our DLL isn't a windows DLL)
		PublicDelayLoadDLLs.Add(Path.Combine(LibPath, "udSDK.dll")); //Load the UDSDK DLL
		
		CopyToTargetBinaries(Path.Combine(LibPath, "udSDK.dll"), ProjectDirectory, Target);
		CopyToTargetBinaries(Path.Combine(LibPath, "udSDK.dll"), PluginDirectory, Target);
	}
}