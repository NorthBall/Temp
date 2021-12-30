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
    //привязываем действия к кнопкам в конструкторе, чтобы кнопки уже были инициализированы в preconstruct в blueprint
    virtual void NativeConstruct() override;
    //Текст с результатами игры
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        class UTextBlock* ResultOfGame;
    //Кнопка для привязки функции закрытия игры
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        class UButton* Button_Quit;
    //Кнопка для привязки функции перезапуска игры
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        UButton* Button_NewGame;
    //Закрывает игру
    UFUNCTION()
    void QuitGame();
    //Перезапускает уровень заново
    UFUNCTION()
        void StartNewGame();
    /*Меняет текст в blueprint (чтобы можно было использовать кириллицу) в зависимости от результата MatchResult
    *@param MatchResult - Результат матча на победном экране(-1,0,1) проигрыш, ничья и выигрыш соответственно
    */
    UFUNCTION(BlueprintImplementableEvent)
    void SetTextForResult(int MatchResult);
};
