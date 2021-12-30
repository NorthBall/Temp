// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "WidgetData_EndGameScreen.h"

//ExampleButton->OnClicked.AddDynamic(this, &MyUserWidget::OnButtonClicked);
void UWidgetData_EndGameScreen::NativeConstruct()
{
	Super::NativeConstruct();
	Button_Quit->OnClicked.AddDynamic(this, &UWidgetData_EndGameScreen::QuitGame);
	Button_NewGame->OnClicked.AddDynamic(this, &UWidgetData_EndGameScreen::StartNewGame);
}
void UWidgetData_EndGameScreen::QuitGame()
{
	UWorld* InterW = GetWorld();
	if (InterW != NULL)
	{
		InterW->GetFirstPlayerController()->ConsoleCommand(FString("quit"));
	}
}

void UWidgetData_EndGameScreen::StartNewGame()
{
	UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("Main")));
}

