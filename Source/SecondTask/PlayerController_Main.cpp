// Fill out your copyright notice in the Description page of Project Settings.

#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "DataPawn.h"
#include "Blueprint/UserWidget.h"
#include "WidgetData_EndGameScreen.h"
#include "PlayerController_Main.h"

void APlayerController_Main::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction("LeftClick", IE_Released, this, &APlayerController_Main::RegisterClick);
}

void APlayerController_Main::RegisterClick()
{
	GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, LastHitUnderCursor);
	if (PlayerDataPawn != NULL) PlayerDataPawn->ClickOnField(LastHitUnderCursor.Location);
}



void APlayerController_Main::BeginPlay()
{
	Super::BeginPlay();
	SetShowMouseCursor(true);
	//инициализируем модель
	PlayerDataPawn = Cast<ADataPawn>(GetPawn());
	//инициализируем виджет
	EndGameScreen = CreateWidget<UWidgetData_EndGameScreen>(this, WidgetClass);
	if (EndGameScreen->IsValidLowLevel())
	{
		EndGameScreen->AddToViewport();
		EndGameScreen->SetVisibility(ESlateVisibility::Hidden);
	}
	//инициализируем камеру
	if (GetWorld() != NULL)
	{
		DefaultCamera = GetWorld()->SpawnActor<ACameraActor>(ACameraActor::StaticClass(), FVector(200.f, 200.f, 205.f), FRotator(-90.f, 0.f, 90.f));
		DefaultCamera->GetCameraComponent()->SetProjectionMode(ECameraProjectionMode::Orthographic);
		DefaultCamera->GetCameraComponent()->SetOrthoWidth(400.f);
		DefaultCamera->GetCameraComponent()->SetAspectRatio(1);
		SetViewTarget(DefaultCamera); 
	}
}
APlayerController_Main::APlayerController_Main()
{
	//Подгружаем виджет победного экрана
	static ConstructorHelpers::FClassFinder<UUserWidget> EndGameScreenRef(TEXT("/Game/Widgets/Widget_EndGameScreen"));
	WidgetClass = EndGameScreenRef.Class;
}