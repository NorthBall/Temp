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
    //����������� ������������ ��� �������� widget �� blueprint
    APlayerController_Main();
    //�������������� ��� ����������
    virtual void BeginPlay() override;
    //��������� ����������� ����� ������
    virtual void SetupInputComponent() override;
    //�������� � ������ ����� ����� ������ �� �������� ����
    void RegisterClick();
    //����������� ���� ������������ Pawn ���������� ������ ADataPawn
    class ADataPawn* PlayerDataPawn;
    //��� ���� ���������� ��� ��������������� ������
    class ACameraActor* DefaultCamera;
    //������ ��������� ������
    class UWidgetData_EndGameScreen* EndGameScreen;
    //����� ������� ��������� ������
    UClass* WidgetClass;
    //���������� ���������� ����� ������
    FHitResult LastHitUnderCursor;    
    
};
