// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "DataPawn.generated.h"

UCLASS()
class SECONDTASK_API ADataPawn : public APawn
{
	GENERATED_BODY()

public:
	// В конструкторе загружаем материал
	ADataPawn();
	//Материал для крестиков и перечеркивающих линий
	class UMaterial* BlackMaterial;
	//Ставит крестик, если сейчас ход игрока, на выбранное место
	void ClickOnField(FVector ClickLocation);
	//Проверка, чтобы игрок не мог сходить 2 раза подряд
	bool bPlayerTurn = true;
	//матрица клеток 4на4. 1-крестик, -1-нолик, 0-пустой
	TArray<TArray<int>> Matrix = { {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0} };
	//Координаты последнего клика игрока
	int PlayerX, PlayerY;

	/*Ставит выбранную фигуру WhichFigure(1-крестик,-1 -нолик) на заданные координаты CordinateX,CordinateY.
	*При этом и спавнит фигуру в мире и изменяет все логические переменные, связанные с AI
	*@return возвращает результат хода- (-1,0,1) Поражение, неопределенный результат, победа
	*/
	int PlaceMark(int CordinateX, int CordinateY, int WhichFigure);
	/*Ставит выбранную фигуру WhichFigure(1-крестик,-1 -нолик) на заданные координаты CordinateX,CordinateY.
	*Только изменяет логические переменные
	*@Out ChangedLines - Линии логики AI, которые были изменены
	*@Out PlayerToRevert - Места в массиве MarksToPlayerVictory, которое занимали ChangedLines
	*@Out AIToRevert - Места в массиве MarksToAIVictory, которое занимали ChangedLines
	*@return возвращает результат хода- (-1,0,1) Поражение, неопределенный результат, победа
	*/
	int MarkAndCalculateResult(int CordinateX, int CordinateY, int WhichFigure, TArray<TArray<int>*>& ChangedLines, TArray<int>& PlayerToRevert, TArray<int>& AIToRevert);
	//Останавливает игру, выводит победный экран с результатом WinnerOfGame (-1,0,1)
	void EndGame(int WinnerOfGame);
	//Ход ИИ
	void AITakeTurn();

	//Для хранения линий логики AI. Каждая линия хранит все клетки (их номера 0-15), которые нужно заполнить одновременно для победы. То есть всего среди таких линий: 4 горизонтали, 4 вертикали и 2 диагонали.
	TArray<TArray<int>> HorizontalLines;
	TArray<TArray<int>> VerticalLines;
	TArray<TArray<int>> Diagonals;
	//Массивы, показывающие насколько каждая линия близка к заполнению Игроком MarksToPlayerVictory или ИИ MarksToAIVictory
	//Если линия находится в i-ом элементе массива, значит нужно заполнить еще i клеток для победы
	TArray<TArray<TArray<int>*>> MarksToPlayerVictory;
	TArray<TArray<TArray<int>*>> MarksToAIVictory;

	//Ищет все линии, проходящие через клетку с координатами CellX,CellY.
	//Результат возвращает через параметр OutFoundLines
	void FindLinesWithCell(int CellX, int CellY, TArray<TArray<int>*>& OutFoundLines);
	/*Используется вместе с FindLinesWithCell. Пересчитывает насколько близка линия LineToChange к заполнению
	*@param bPlayerMark - Кто сделал пометку на линии (Игрок или ИИ)
	*@Out PlayerLevelOfline - Значение заполненности линии Игроком ДО изменений
	*@Out AILevelOfLine - Значение заполненности линии ИИ ДО изменений
	*/
	void RecalcLineAfterMark(TArray<int>* LineToChange, bool bPlayerMark,int&PlayerLevelOfline,int&AILevelOfLine);
	/*Отменяет изменения, сделанные RecalcLineAfterMark.
	*@param LineToChange -линия, измененная RecalcLineAfterMark
	*@param PlayerLevelOfline - Значение заполненности линии Игроком ДО изменений RecalcLineAfterMark
	*@Param AILevelOfLine - Значение заполненности линии ИИ ДО изменений RecalcLineAfterMark
	*/
	void RecalcLineAfterErase(TArray<int>* LineToChange, int& PlayerLevelOfLine, int& AILevelOfLine);

	/*МинМакс динамика, для расчета наилучшего хода
	*@param TurnsToCalc На сколько ходов вперед осталось просчитывать игру
	*@param bAITurn чей ход просчитывается в этой итерации (ИИ или игрока)
	*@return Насколько успешен сделанный ход (-1000 победа ИИ, 1000 - победа игрока, Metric() иначе)
	*/
	int TurnDynamic(int TurnsToCalc,bool bAITurn);
	//Метрика, для оценки положения на поле
	int Metric();
	//Константа, на сколько ходов вперед ИИ будет просчитывать игру
	const int DynamicLength = 4;
protected:
	// Инициализация линий логики AI
	virtual void BeginPlay() override;
};
