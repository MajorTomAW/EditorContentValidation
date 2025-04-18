#pragma once

#include "ISourceControlModule.h"
#include "EditorContentValidation/Validation/EditorValidator.h"

#include "Interfaces/IPluginManager.h"

#include "Styling/SlateStyleRegistry.h"
#include "Widgets/Input/SSpinBox.h"

#define LOCTEXT_NAMESPACE "EditorContentValidation"

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
	static void GenerateValidatorOptionsMenu(UToolMenu* InMenu);

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

inline void FValidatorToolbarButton::GenerateValidatorOptionsMenu(UToolMenu* InMenu)
{
	FToolMenuSection& Section = InMenu->AddSection("ValidatorOptions", LOCTEXT("ValidatorOptionsHeading", "Validator Options"));

	// Max Packages to load
	{
		const TSharedRef<SWidget> MaxPackages = SNew(SBox)
			.HAlign(HAlign_Right)
			[
				SNew(SSpinBox<int32>)
				.ContentPadding(FMargin(8.f, 2.f))
				.MinDesiredWidth(64.f)
				.MinValue(1000)
				.MinSliderValue(1000)
				.MaxSliderValue(12500)
				.Delta(100)
				.ToolTipText(LOCTEXT("MaxPackagesToolTip", "The maximum number of packages to load when validating content."))
				.Value_Static(&UEditorValidator::GetMaxPackagesToLoad)
				.OnValueCommitted_Lambda([] (int32 Value, ETextCommit::Type)
				{
					IConsoleManager::Get().FindConsoleVariable(TEXT("EditorValidator.MaxPackagesToLoad"))
					->Set(Value, ECVF_SetByConsole);
				})
				.OnValueChanged_Lambda([] (int32 Value)
				{
					IConsoleManager::Get().FindConsoleVariable(TEXT("EditorValidator.MaxPackagesToLoad"))
					->Set(Value, ECVF_SetByConsole);
				})	
			];

		Section.AddEntry(FToolMenuEntry::InitWidget(
			"MaxPackagesToLoad",
			MaxPackages,
			LOCTEXT("MaxPackagesToLoadLabel", "Max Packages to load"),
			true, true, false,
			LOCTEXT("MaxPackagesToLoadTooltip", "The maximum number of packages to load when validating content.")));
	}

	// Max Assets changed by a header
	{
		const TSharedRef<SWidget> MaxAssets = SNew(SBox)
			.HAlign(HAlign_Right)
			[
				SNew(SSpinBox<int32>)
				.ContentPadding(FMargin(8.f, 2.f))
				.MinDesiredWidth(64.f)
				.MinValue(100)
				.MinSliderValue(100)
				.MaxSliderValue(1000)
				.Delta(10)
				.ToolTipText(LOCTEXT("MaxAssetsToolTip", "The maximum number of assets to validate when a single header file changes."))
				.Value_Static(&UEditorValidator::GetMaxAssetsChangedByAHeader)
				.OnValueCommitted_Lambda([] (int32 Value, ETextCommit::Type)
				{
					IConsoleManager::Get().FindConsoleVariable(TEXT("EditorValidator.MaxAssetsChangedByAHeader"))
					->Set(Value, ECVF_SetByConsole);
				})
				.OnValueChanged_Lambda([](int32 Value)
				{
					IConsoleManager::Get().FindConsoleVariable(TEXT("EditorValidator.MaxAssetsChangedByAHeader"))
					->Set(Value, ECVF_SetByConsole);
				})
			];

		Section.AddEntry(FToolMenuEntry::InitWidget(
			"MaxAssetsChangedByAHeader",
			MaxAssets,
			LOCTEXT("MaxAssetsChangedByAHeaderLabel", "Max header chain"),
			true, true, false,
			LOCTEXT("MaxAssetsChangedByAHeaderTooltip", "The maximum number of assets to validate when a single header file changes.")));
	}

	// Source Control disabled warning
	{
		Section.AddEntry(FToolMenuEntry::InitWidget(
			"RevisionControlDisabled",
			SNew(SBox)
			.Visibility_Lambda([]()
			{
				return ISourceControlModule::Get().IsEnabled()
				 ? EVisibility::Collapsed
				 : EVisibility::Visible;
			})
			.Padding(FMargin(16.f, 3.f))
			[
				SNew(STextBlock)
				.ColorAndOpacity(FStyleColors::Warning)
				.Text(LOCTEXT("RevisionControlDisabled", "Revision control is disabled. \nThe content validation will not work properly."))
			],
			FText::GetEmpty()
		));
	}
}

inline bool FValidatorToolbarButton::HasNoPlayWorld()
{
	return GEditor->PlayWorld == nullptr;
}

#undef LOCTEXT_NAMESPACE