// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class EditorContentValidation : ModuleRules
{
	public EditorContentValidation(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicDefinitions.Add("SHIPPING_DRAW_DEBUG_ERROR=1");
		
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"EditorFramework",
			"UnrealEd"
		});
			
		
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"InputCore",
			"Slate",
			"SlateCore",
			"ToolMenus", 
			"EditorStyle",
			"DataValidation",
			"MessageLog",
			"Projects",
			"DeveloperToolSettings",
			"CollectionManager",
			"SourceControl",
		});
	}
}
