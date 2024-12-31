// Copyright Epic Games, Inc. All Rights Reserved.

#include "EditorContentValidationModule.h"

#include "ValidatorToolbarButton.h"
#include "EditorContentValidation/Validation/EditorValidator.h"
#include "Misc/DataValidation.h"

#define LOCTEXT_NAMESPACE "FEditorContentValidationModule"

DEFINE_LOG_CATEGORY(LogEditorContentValidation);

class FEditorContentValidationModule : public IModuleInterface
{
public:
	//~ Begin IModuleInterface interface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	//~ End IModuleInterface interface

protected:
	static void RegisterEditorToolbarMenus();
	static void Execute_CheckGameContent();

private:
	FDelegateHandle ToolMenusHandle;
};
IMPLEMENT_MODULE(FEditorContentValidationModule, EditorContentValidation)


//////////////////////////////////////////////////////////////////////////
/// Module Implementation


void FEditorContentValidationModule::StartupModule()
{
	if (IsRunningGame())
	{
		return;
	}

	FValidatorEditorStyle::Get();

	if (FSlateApplication::IsInitialized())
	{
		ToolMenusHandle = UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateStatic(&RegisterEditorToolbarMenus));
	}
}

void FEditorContentValidationModule::ShutdownModule()
{
	if (UObjectInitialized() && ToolMenusHandle.IsValid())
	{
		UToolMenus::UnRegisterStartupCallback(ToolMenusHandle);
	}
}

void FEditorContentValidationModule::RegisterEditorToolbarMenus()
{
	UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
	FToolMenuSection& Section = ToolbarMenu->AddSection("PlayGameExtensions", TAttribute<FText>(), FToolMenuInsert("Play", EToolMenuInsertType::After));

	FToolMenuEntry Entry = FValidatorToolbarButton::GetValidatorToolbarButton(FExecuteAction::CreateStatic(&Execute_CheckGameContent));
	Entry.StyleNameOverride = TEXT("CalloutToolbar");
	Section.AddEntry(Entry);
}

void FEditorContentValidationModule::Execute_CheckGameContent()
{
	UEditorValidator::ValidateCheckedOutContent(true, EDataValidationUsecase::Manual);
}

#undef LOCTEXT_NAMESPACE
