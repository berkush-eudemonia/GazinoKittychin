// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuWidget.h"

void UMenuWidget::Setup()
{
	this->AddToViewport();
	this->SetIsFocusable(true);

	UWorld* World = GetWorld();
	if (!ensure(World!=nullptr)) return;
	
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController!=nullptr)) return;
	
	FInputModeUIOnly InputModeData;
	InputModeData.SetWidgetToFocus(this->TakeWidget());
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	PlayerController->SetInputMode(InputModeData);
	PlayerController->bShowMouseCursor = true;
}

void UMenuWidget::Teardown()
{
	this->RemoveFromParent();

	UWorld* World = GetWorld();
	if (!ensure(World!=nullptr)) return;
	
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController!=nullptr)) return;

	FInputModeGameOnly InputModeData;
	PlayerController->SetInputMode(InputModeData);
	PlayerController->bShowMouseCursor = false;
}

void UMenuWidget::SetMenuInterface(IMenuInterface* _MenuInterface) //void UMenuWidget::SetMenuInterface(IMenuInterface* MenuInterface) 
{
	//UMenuWidget::MenuInterface = MenuInterface;
	//this->MenuInterface = MenuInterface; //idi windowsta hata veriyor
	this->MenuInterface = _MenuInterface;
}

