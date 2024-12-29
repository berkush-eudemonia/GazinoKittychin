// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameMenu.h"

#include "Components/Button.h"

bool UInGameMenu::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success) return false;

	if (!ensure(ReturnButton!=nullptr)) return false;
	ReturnButton->OnClicked.AddDynamic(this, &UInGameMenu::ReturnPressed);

	if (!ensure(QuitButton!=nullptr)) return false;
	QuitButton->OnClicked.AddDynamic(this, &UInGameMenu::QuitPressed);
	
	return true;
}

void UInGameMenu::ReturnPressed()
{
	Teardown();
}

void UInGameMenu::QuitPressed()
{
	if (MenuInterface!=nullptr)
	{
		Teardown();
		MenuInterface->LoadMainMenu();
	}
}

