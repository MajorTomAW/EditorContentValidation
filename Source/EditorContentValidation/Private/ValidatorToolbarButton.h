#pragma once

#define LOCTEXT_NAMESPACE "EditorContentValidation"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"

//////////////////////////////////////////////////////////////////////////
/// FValidatorEditorStyle

#define IMAGE_PLUGIN_BRUSH(RelativePath, ...) FSlateImageBrush(FValidatorEditorStyle::InContent(RelativePath, + "png"), __VA_ARGS__)
#define IMAGE_PLUGIN_BRUSH_SVG(RelativePath, ...) FSlateVectorImageBrush(FValidatorEditorStyle::InContent(RelativePath, + ".svg"),  __VA_ARGS__)

class FValidatorEditorStyle : public FSlateStyleSet
{
	friend class FValidatorToolbarButton;
public:
	FValidatorEditorStyle()
		: FSlateStyleSet(TEXT("ValidatorEditorStyle"))
	{
	}

	virtual ~FValidatorEditorStyle() override
	{
		Shutdown();
	}

	static FValidatorEditorStyle& Get()
	{
		if (!StyleSet.IsValid())
		{
			StyleSet = MakeUnique<FValidatorEditorStyle>();
			StyleSet->Initialize();
		}

		return *StyleSet.Get();
	}

	static const FName& GetValidatorStyleSetName()
	{
		return Get().GetStyleSetName();
	}

protected:
	void Initialize()
	{
		FSlateStyleSet::SetContentRoot(FPaths::EngineContentDir() / TEXT("Editor/Slate"));
		FSlateStyleSet::SetCoreContentRoot(FPaths::EngineContentDir() / TEXT("Slate"));

		Set("GameEditor.CheckContent", new IMAGE_PLUGIN_BRUSH_SVG("Slate/Icons/CheckContent", CoreStyleConstants::Icon20x20));

		FSlateStyleRegistry::RegisterSlateStyle(*this);
	}

	static void Shutdown()
	{
		if (StyleSet.IsValid())
		{
			FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet.Get());
			ensure(StyleSet);
			StyleSet.Reset();
		}
	}

	static FString InContent(const FString& RelativePath, const ANSICHAR* Extension)
	{
		static FString ContentDir = IPluginManager::Get().FindPlugin(TEXT("EditorContentValidation"))->GetBaseDir()
		/ TEXT("Resources");
	
		return (ContentDir / RelativePath) + Extension;
	}

private:
	static TUniquePtr<FValidatorEditorStyle> StyleSet;
};

TUniquePtr<FValidatorEditorStyle> FValidatorEditorStyle::StyleSet = nullptr;
#undef IMAGE_PLUGIN_BRUSH
#undef IMAGE_PLUGIN_BRUSH_SVG

//////////////////////////////////////////////////////////////////////////
/// FValidatorToolbarButton

class FValidatorToolbarButton
{
public:
	static FToolMenuEntry GetValidatorToolbarButton(const FExecuteAction& InExecuteAction);

private:
	static bool HasNoPlayWorld();
};

inline FToolMenuEntry FValidatorToolbarButton::GetValidatorToolbarButton(const FExecuteAction& InExecuteAction)
{
	FToolMenuEntry ToolMenuEntry = FToolMenuEntry::InitToolBarButton
	(
		"CheckContent",
		FUIAction
			(
				InExecuteAction,
				FCanExecuteAction::CreateStatic(&HasNoPlayWorld),
				FIsActionChecked(),
				FIsActionButtonVisible::CreateStatic(&HasNoPlayWorld)
			),
		LOCTEXT("CheckContentButtonLabel", "Check Content"),
		LOCTEXT("CheckContentButtonTooltip", "Runs the Content Validation job on all checked out assets to look for warnings and errors."),
		FSlateIcon(FValidatorEditorStyle::GetValidatorStyleSetName(), "GameEditor.CheckContent")
	);

	return ToolMenuEntry;
}

inline bool FValidatorToolbarButton::HasNoPlayWorld()
{
	return GEditor->PlayWorld == nullptr;
}

#undef LOCTEXT_NAMESPACE