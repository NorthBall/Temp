// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WidgetData_EndGameScreen.generated.h"

/**
 * 
 */
UCLASS()
class SECONDTASK_API UWidgetData_EndGameScreen : public UUserWidget
{
	GENERATED_BODY()
public:
    //����������� �������� � ������� � ������������, ����� ������ ��� ���� ���������������� � preconstruct � blueprint
    virtual void NativeConstruct() override;
    //����� � ������������ ����
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        class UTextBlock* ResultOfGame;
    //������ ��� �������� ������� �������� ����
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        class UButton* Button_Quit;
    //������ ��� �������� ������� ����������� ����
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        UButton* Button_NewGame;
    //��������� ����
    UFUNCTION()
    void QuitGame();
    //������������� ������� ������
    UFUNCTION()
        void StartNewGame();
    /*������ ����� � blueprint (����� ����� ���� ������������ ���������) � ����������� �� ���������� MatchResult
    *@param MatchResult - ��������� ����� �� �������� ������(-1,0,1) ��������, ����� � ������� ��������������
    */
    UFUNCTION(BlueprintImplementableEvent)
    void SetTextForResult(int MatchResult);
};
