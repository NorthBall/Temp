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
	// � ������������ ��������� ��������
	ADataPawn();
	//�������� ��� ��������� � ��������������� �����
	class UMaterial* BlackMaterial;
	//������ �������, ���� ������ ��� ������, �� ��������� �����
	void ClickOnField(FVector ClickLocation);
	//��������, ����� ����� �� ��� ������� 2 ���� ������
	bool bPlayerTurn = true;
	//������� ������ 4��4. 1-�������, -1-�����, 0-������
	TArray<TArray<int>> Matrix = { {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0} };
	//���������� ���������� ����� ������
	int PlayerX, PlayerY;

	/*������ ��������� ������ WhichFigure(1-�������,-1 -�����) �� �������� ���������� CordinateX,CordinateY.
	*��� ���� � ������� ������ � ���� � �������� ��� ���������� ����������, ��������� � AI
	*@return ���������� ��������� ����- (-1,0,1) ���������, �������������� ���������, ������
	*/
	int PlaceMark(int CordinateX, int CordinateY, int WhichFigure);
	/*������ ��������� ������ WhichFigure(1-�������,-1 -�����) �� �������� ���������� CordinateX,CordinateY.
	*������ �������� ���������� ����������
	*@Out ChangedLines - ����� ������ AI, ������� ���� ��������
	*@Out PlayerToRevert - ����� � ������� MarksToPlayerVictory, ������� �������� ChangedLines
	*@Out AIToRevert - ����� � ������� MarksToAIVictory, ������� �������� ChangedLines
	*@return ���������� ��������� ����- (-1,0,1) ���������, �������������� ���������, ������
	*/
	int MarkAndCalculateResult(int CordinateX, int CordinateY, int WhichFigure, TArray<TArray<int>*>& ChangedLines, TArray<int>& PlayerToRevert, TArray<int>& AIToRevert);
	//������������� ����, ������� �������� ����� � ����������� WinnerOfGame (-1,0,1)
	void EndGame(int WinnerOfGame);
	//��� ��
	void AITakeTurn();

	//��� �������� ����� ������ AI. ������ ����� ������ ��� ������ (�� ������ 0-15), ������� ����� ��������� ������������ ��� ������. �� ���� ����� ����� ����� �����: 4 �����������, 4 ��������� � 2 ���������.
	TArray<TArray<int>> HorizontalLines;
	TArray<TArray<int>> VerticalLines;
	TArray<TArray<int>> Diagonals;
	//�������, ������������ ��������� ������ ����� ������ � ���������� ������� MarksToPlayerVictory ��� �� MarksToAIVictory
	//���� ����� ��������� � i-�� �������� �������, ������ ����� ��������� ��� i ������ ��� ������
	TArray<TArray<TArray<int>*>> MarksToPlayerVictory;
	TArray<TArray<TArray<int>*>> MarksToAIVictory;

	//���� ��� �����, ���������� ����� ������ � ������������ CellX,CellY.
	//��������� ���������� ����� �������� OutFoundLines
	void FindLinesWithCell(int CellX, int CellY, TArray<TArray<int>*>& OutFoundLines);
	/*������������ ������ � FindLinesWithCell. ������������� ��������� ������ ����� LineToChange � ����������
	*@param bPlayerMark - ��� ������ ������� �� ����� (����� ��� ��)
	*@Out PlayerLevelOfline - �������� ������������� ����� ������� �� ���������
	*@Out AILevelOfLine - �������� ������������� ����� �� �� ���������
	*/
	void RecalcLineAfterMark(TArray<int>* LineToChange, bool bPlayerMark,int&PlayerLevelOfline,int&AILevelOfLine);
	/*�������� ���������, ��������� RecalcLineAfterMark.
	*@param LineToChange -�����, ���������� RecalcLineAfterMark
	*@param PlayerLevelOfline - �������� ������������� ����� ������� �� ��������� RecalcLineAfterMark
	*@Param AILevelOfLine - �������� ������������� ����� �� �� ��������� RecalcLineAfterMark
	*/
	void RecalcLineAfterErase(TArray<int>* LineToChange, int& PlayerLevelOfLine, int& AILevelOfLine);

	/*������� ��������, ��� ������� ���������� ����
	*@param TurnsToCalc �� ������� ����� ������ �������� ������������ ����
	*@param bAITurn ��� ��� �������������� � ���� �������� (�� ��� ������)
	*@return ��������� ������� ��������� ��� (-1000 ������ ��, 1000 - ������ ������, Metric() �����)
	*/
	int TurnDynamic(int TurnsToCalc,bool bAITurn);
	//�������, ��� ������ ��������� �� ����
	int Metric();
	//���������, �� ������� ����� ������ �� ����� ������������ ����
	const int DynamicLength = 4;
protected:
	// ������������� ����� ������ AI
	virtual void BeginPlay() override;
};
