// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerController_Main.generated.h"

/**
 * 
 */
UCLASS()
class SECONDTASK_API APlayerController_Main : public APlayerController
{
	GENERATED_BODY()
public:
    //Конструктор используется для загрузки widget из blueprint
    APlayerController_Main();
    //Инициализирует все переменные
    virtual void BeginPlay() override;
    //добавляет возможность клика мышкой
    virtual void SetupInputComponent() override;
    //Передает в модель место клика мышкой по игровому полю
    void RegisterClick();
    //Управляемая этим контроллером Pawn выбранного класса ADataPawn
    class ADataPawn* PlayerDataPawn;
    //Для игры используем эту ортографическую камеру
    class ACameraActor* DefaultCamera;
    //виджет победного экрана
    class UWidgetData_EndGameScreen* EndGameScreen;
    //класс виджета победного экрана
    UClass* WidgetClass;
    //Координаты последнего клика игрока
    FHitResult LastHitUnderCursor;    
    
};
