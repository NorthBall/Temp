// Fill out your copyright notice in the Description page of Project Settings.

#include "Engine/StaticMeshActor.h"
#include "PlayerController_Main.h"
#include "WidgetData_EndGameScreen.h"
#include "Materials/Material.h"
#include "DataPawn.h"


ADataPawn::ADataPawn()
{
	PrimaryActorTick.bCanEverTick = false;
	static ConstructorHelpers::FObjectFinder<UMaterial> FoundMaterial(TEXT("/Game/Materials/M_SimpleBlack.M_SimpleBlack"));
	BlackMaterial = FoundMaterial.Object;
}

void ADataPawn::ClickOnField(FVector ClickLocation)
{
	//��������� �� �������� �� ����� ������� � ��� ��
	if (!bPlayerTurn) return;
	//��������� ��������� ������� ������
	PlayerX = ClickLocation.X / 100;
	PlayerY = ClickLocation.Y / 100;
	if (PlayerX < 4 && PlayerX >= 0 && PlayerY < 4 && PlayerY >= 0)
	{
		//���������, ����� �� ������� ������ �� ��������� ������
		if (Matrix[PlayerX][PlayerY] == 0)
		{
			//������ ������� �� ��������� ������ � �������� ��� ��
			if (PlaceMark(PlayerX, PlayerY, 1)==0)
			{
				AITakeTurn();
			}
		}
	}
}

void ADataPawn::FindLinesWithCell(int CellX, int CellY, TArray<TArray<int>*>& OutFoundLines)
{
	//��������������� �������, ���������� �����������
	TArray<int> Temp;
	OutFoundLines.Empty();
	OutFoundLines.Add(&HorizontalLines[CellX]);
	OutFoundLines.Add(&VerticalLines[CellY]);
	if (CellX == CellY)
	{
		OutFoundLines.Add(&Diagonals[0]);
	}
	else if (CellX + CellY == 3)
	{
		OutFoundLines.Add(&Diagonals[1]);
	}
	return;
}

void ADataPawn::RecalcLineAfterMark(TArray<int>* LineToChange, bool bPlayerMark,int& PlayerLevelOfLine,int&AILevelOfLine)
{
	int i;
	PlayerLevelOfLine = 5;
	for (i = 1; i <= 4; i++)
	{
		//���� ��������� ��������� ��������� �����
		if (MarksToPlayerVictory[i].Contains(LineToChange))
		{
			if (bPlayerMark) 
			{
				//���� �������� �����, �� ��� ������ ����� ���������� ����� ����������� (�� 1 ������� ������ �� ������).
				//���� �������� ��, �� ����� ��� ������� �� ������ ��������� ����� � �� ����� �� �������������
				MarksToPlayerVictory[i-1].Add(LineToChange);
			}
			MarksToPlayerVictory[i].Remove(LineToChange);
			PlayerLevelOfLine = i;
			break;
		}
	}
	AILevelOfLine = 5;
	for (i = 1; i <= 4; i++)
	{
		//������������ ������ ��� ������ �� ������, � ��
		if (MarksToAIVictory[i].Contains(LineToChange))
		{
			if (!bPlayerMark)
			{
				MarksToAIVictory[i - 1].Add(LineToChange);
			}
			MarksToAIVictory[i].Remove(LineToChange);
			AILevelOfLine = i;
			break;
		}
	}
}

void ADataPawn::RecalcLineAfterErase(TArray<int>* LineToChange, int& PlayerLevelOfLine, int& AILevelOfLine)
{
	if (PlayerLevelOfLine < 5)
	{
		MarksToPlayerVictory[PlayerLevelOfLine].Add(LineToChange);
		MarksToPlayerVictory[PlayerLevelOfLine-1].Remove(LineToChange);
	}
	if (AILevelOfLine < 5)
	{
		MarksToAIVictory[AILevelOfLine].Add(LineToChange);
		MarksToAIVictory[AILevelOfLine-1].Remove(LineToChange);
	}
}


void ADataPawn::EndGame(int WinnerOfGame)
{
	//�������� ������ � �������� �����
	APlayerController_Main* PossessingController = GetController<APlayerController_Main>();
	if (PossessingController != NULL)
	{
		PossessingController->SetPause(true);
		if (PossessingController->EndGameScreen->IsValidLowLevel())
		{
			PossessingController->EndGameScreen->SetVisibility(ESlateVisibility::Visible);
			PossessingController->EndGameScreen->SetTextForResult(WinnerOfGame);
		}
		
	}
	AStaticMeshActor* LocalMeshActor;
	UStaticMesh* FigureToSpawn= Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, TEXT("/Engine/BasicShapes/Cube.Cube")));;
	int FoundIndex;
	//������� ��� �������, ����� ������ � ����� ������� ������ ��������� ����������� �����
	TArray<TArray<int>*>* AllWinningLines;
	if (WinnerOfGame == 1)
	{
		AllWinningLines = &MarksToPlayerVictory[0];
	}
	else
	{
		AllWinningLines = &MarksToAIVictory[0];
	}
	for (auto& WinLine : *AllWinningLines)
	{
		//���� ����� � �������� ��������������, ������������ � ������������ �����, ����� ����� ������ �� ������������ � ������������
		FoundIndex = HorizontalLines.Find(*WinLine);
		if ( FoundIndex!= INDEX_NONE)
		{
			LocalMeshActor = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), FVector((FoundIndex + 0.5f) * 100.f, 200.f, 5.f), FRotator(0.f, 0.f, 0.f));
			
			if (LocalMeshActor != NULL && FigureToSpawn != NULL)
			{
				//������ Static Mesh � ��� ��������� ��� �����
				LocalMeshActor->SetMobility(EComponentMobility::Movable);
				LocalMeshActor->SetActorScale3D(FVector(0.1f, 4.f, 0.1f));
				LocalMeshActor->GetStaticMeshComponent()->SetStaticMesh(FigureToSpawn);
				LocalMeshActor->GetStaticMeshComponent()->SetMaterial(0, BlackMaterial);
			}
		}
		//���������� ������ ��� ������������ �����
		FoundIndex = VerticalLines.Find(*WinLine);
		if (FoundIndex != INDEX_NONE)
		{
			LocalMeshActor = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), FVector(200.f, (FoundIndex + 0.5f) * 100.f, 5.f), FRotator(0.f, 0.f, 0.f));
			if (LocalMeshActor != NULL && FigureToSpawn != NULL)
			{
				LocalMeshActor->SetMobility(EComponentMobility::Movable);
				LocalMeshActor->SetActorScale3D(FVector(4.f, 0.1f, 0.1f));
				LocalMeshActor->GetStaticMeshComponent()->SetStaticMesh(FigureToSpawn);
				LocalMeshActor->GetStaticMeshComponent()->SetMaterial(0, BlackMaterial);
			}
		}
		//���������� ������ ��� ����������
		FoundIndex = Diagonals.Find(*WinLine);
		if (FoundIndex != INDEX_NONE)
		{
			if (FoundIndex == 0)
			{
				LocalMeshActor = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), FVector(200.f, 200.f, 5.f), FRotator(0.f, 45.f, 0.f));
			}
			else
			{
				LocalMeshActor = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), FVector(200.f, 200.f, 5.f), FRotator(0.f, -45.f, 0.f));
			}
			if (LocalMeshActor != NULL && FigureToSpawn != NULL)
			{
				LocalMeshActor->SetMobility(EComponentMobility::Movable);
				LocalMeshActor->SetActorScale3D(FVector(5.f, 0.1f, 0.1f));
				LocalMeshActor->GetStaticMeshComponent()->SetStaticMesh(FigureToSpawn);
				LocalMeshActor->GetStaticMeshComponent()->SetMaterial(0, BlackMaterial);
			}
		}
	}
}

int ADataPawn::Metric()
{
	//���������� �������, ����������� �������� �� ������������� �����
	return 5*MarksToPlayerVictory[2].Num()+ 2* MarksToPlayerVictory[3].Num()+ MarksToPlayerVictory[4].Num()-(5 * MarksToAIVictory[2].Num() + 2 * MarksToAIVictory[3].Num() + MarksToAIVictory[4].Num());
}


void ADataPawn::BeginPlay()
{
	Super::BeginPlay();
	TArray<int> Temp;
	int i, j;
	//������ �������������� � ������������ ����� ��.
	for (i = 0; i < 4; i++)
	{
		Temp.Empty();
		for ( j = 0; j < 4; j++)
		{
			Temp.Add(4*i+j);
		}
		HorizontalLines.Add(Temp);
		Temp.Empty();
		for (j = 0; j < 4; j++)
		{
			Temp.Add(4*j+i);
		}
		VerticalLines.Add(Temp);
	}
	//������ ��������� ��
	Temp.Empty();
	for (j = 0; j < 4; j++)
	{
		Temp.Add(4*j+j);
	}
	Diagonals.Add(Temp);
	Temp.Empty();
	for (j = 0; j < 4; j++)
	{
		Temp.Add(4*j+4-(j+1));
	}
	Diagonals.Add(Temp);
	//��������� ��� ����� �� � 4 ������� ���������� ��������, ��� ��� �� ������ ��� ������ ����� ��������� ��� 4 ������ ����� �� �����
	TArray<TArray<int>*> ArrayOfLines;
	for (i = 0; i < 4; i++)
	{
		MarksToPlayerVictory.Add(ArrayOfLines);
		MarksToAIVictory.Add(ArrayOfLines);
	}
	for (i = 0; i < HorizontalLines.Num(); i++)
	{
		ArrayOfLines.Add(&HorizontalLines[i]);
	}
	for (i = 0; i < VerticalLines.Num(); i++)
	{
		ArrayOfLines.Add(&VerticalLines[i]);
	}
	for (i = 0; i < Diagonals.Num(); i++)
	{
		ArrayOfLines.Add(&Diagonals[i]);
	}
	MarksToPlayerVictory.Add(ArrayOfLines);
	MarksToAIVictory.Add(ArrayOfLines);

}



int ADataPawn::PlaceMark(int CordinateX, int CordinateY, int WhichFigure)
{
	if (GetWorld() == NULL) return 0;
	//������� Static Mesh Actor ��� ������������ �� ���������� CordinateX,CordinateY ������ 
	AStaticMeshActor* LocalMeshActor;
	LocalMeshActor = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), FVector((CordinateX + 0.5f) * 100.f, (CordinateY + 0.5f) * 100.f, 5.f), FRotator(0.f, 0.f, 0.f));
	UStaticMesh* FigureToSpawn;
	//�������� ������� ��� ����� � ����������� �� �������� WhichFigure ������������ ������
	if (WhichFigure == 1)
	{
		FigureToSpawn = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, TEXT("/Game/Meshes/SM_Cross.SM_Cross")));

	}
	else
	{
		FigureToSpawn = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, TEXT("/Game/Meshes/SM_Circle.SM_Circle")));
	}
	//������ Static Mesh ��� Static Mesh Actor
	if (LocalMeshActor != NULL && FigureToSpawn != NULL)
	{
		LocalMeshActor->SetMobility(EComponentMobility::Movable);
		LocalMeshActor->GetStaticMeshComponent()->SetStaticMesh(FigureToSpawn);
	}
	//�������� MarkAndCalculateResult ��� ��������� ���������� ���������� AI
	TArray<TArray<int>*> ChangedLines;
	TArray<int> OldLinePlaceForPlayer, OldLinePlaceForAI;
	int ResultOfTurn=MarkAndCalculateResult(CordinateX, CordinateY, WhichFigure,ChangedLines,OldLinePlaceForPlayer,OldLinePlaceForAI);
	//��������� �� ����������� �� ���� ������� �� ��� �����
	if (ResultOfTurn != 0)
	{
		EndGame(ResultOfTurn);
	}
	return ResultOfTurn;
}

int ADataPawn::MarkAndCalculateResult(int CordinateX, int CordinateY, int WhichFigure, TArray<TArray<int>*>& ChangedLines,TArray<int>&PlayerToRevert,TArray<int>&AIToRevert)
{
	//��������� ������� ������, ����� ����� �� ��� �������� �� �� �� ������
	Matrix[CordinateX][CordinateY] = WhichFigure;
	//�������������� �������
	ChangedLines.Empty();
	PlayerToRevert.Empty();
	AIToRevert.Empty();
	//���� ��� ����� ��, ������� ���� ����� ��������
	FindLinesWithCell(CordinateX, CordinateY, ChangedLines);
	int PlayerMarksInLine, AIMarksInLine;
	for (auto& LineToChange : ChangedLines)
	{
		//������ ��������� ����� �� �����
		RecalcLineAfterMark(LineToChange, WhichFigure==1,PlayerMarksInLine, AIMarksInLine);
		PlayerToRevert.Add(PlayerMarksInLine);
		AIToRevert.Add(AIMarksInLine);
	}
	//��������� �� ����������� �� ���� ������� ������ ��� �� ����� ���������� ����
	if (MarksToPlayerVictory[0].Num() != 0)
	{
		return 1;
	}
	else if (MarksToAIVictory[0].Num() != 0)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

void ADataPawn::AITakeTurn()
{
	//��������� ������ ������ �� ���� ��
	bPlayerTurn = false;
	int i, j, k;
	//������ ���������� ������� ����
	int ResultOfDynamic;
	//������ �� ��������� �����������
	int BestMetric = 1000;
	//������� ��������� ����� �������� �� ����
	int PossibleTurns = 0;
	//��� ���� � ������ ����������� BestMetric. (������ ������ 0-15)
	TArray<int> BestTurns;
	//��� ���������� � ���������� �������� ���� ����� ��. ����� ������ ����, ������������ � ������� ��������� InPlayerPlace, InAIPlace
	TArray<TArray<int>*> SaveLinesToRevert;
	//����� �������� ����� SaveLinesToRevert �� ���������.
	TArray<int> InPlayerPlace, InAIPlace;
	for (i = 0; i < Matrix.Num(); i++)
	{
		for (j = 0; j < Matrix[i].Num(); j++)
		{
			//����������� �������� ������
			if (Matrix[i][j] != 0) continue;
			PossibleTurns++;
			//������������� ���������� ����������, ��� ��� ���������, ���� ������� ������ � ������������ i,j
			MarkAndCalculateResult(i, j, -1, SaveLinesToRevert, InPlayerPlace, InAIPlace);
			//��������� ��������� ��������
			ResultOfDynamic = TurnDynamic(DynamicLength, false);
			//��� ��� ����� ��, ��� ���� ������ ����� �������� ���������� ����� (� ������ -1000). ���������� ��� ���� � ���������� ��������
			if (ResultOfDynamic == BestMetric)
			{
				BestTurns.Add(4 * i + j);
			}
			else if (ResultOfDynamic < BestMetric)
			{
				BestMetric = ResultOfDynamic;
				BestTurns.Empty();
				BestTurns.Add(4 * i + j);
			}
			//���������� ���������, ��� ��� ��� ������ �������������, � �� ��������� ��.
			Matrix[i][j] = 0;
			for (k = 0; k < SaveLinesToRevert.Num(); k++)
			{
				RecalcLineAfterErase(SaveLinesToRevert[k], InPlayerPlace[k], InAIPlace[k]);
			}
			
		}
	}
	//���� ��� ��������� ������ (� ��� ����� � ��� ��) - ������ �����
	if (BestTurns.Num() == 0)
	{
		EndGame(0);
		return;
	}
	//��������� ������� �����������. ��� ����� ��� ���� �� BestTurns ��������� ���������� ����� �����.
	int ChoosedTurn = FMath::RandRange(0, BestTurns.Num() - 1);
	bool bGameWonByLastTurn = PlaceMark(BestTurns[ChoosedTurn] / 4, BestTurns[ChoosedTurn] % 4, -1) != 0;
	//���� �� �������� ��������� ��������� ������ � ��� ���� �� ������� - ������ �����.
	if (PossibleTurns <= 1&&!bGameWonByLastTurn)
	{
		EndGame(0);
		return;
	}
	//���������� ������ ���
	bPlayerTurn = true;
}

int ADataPawn::TurnDynamic(int TurnsToCalc, bool bAITurn)
{
	//�� ������������ ��� ���� ����, ��� � ���� ������.
	//������� ���������� ����-����� EnemyMarksToVictory, OurMarksToVictory, ������ ��������� Result, � ��� ������ ��� ����������  Figure(�������/�����)
	//������ ���������������� � ����������� �� ���� ���� bAITurn
	TArray<TArray<TArray<int>*>>* EnemyMarksToVictory, *OurMarksToVictory;
	int Result = 1000, Figure=-1;
	if (bAITurn)
	{
		OurMarksToVictory = &MarksToAIVictory;
		EnemyMarksToVictory = &MarksToPlayerVictory;
	}
	else
	{
		EnemyMarksToVictory = &MarksToAIVictory;
		OurMarksToVictory = &MarksToPlayerVictory;
		Result = -1000;
		Figure = 1;
	}
	//���� ����� ������� �������� �������� ������� ����� ��������� �����, �������� ������ ��� ���� ���������
	if ((*EnemyMarksToVictory)[0].Num()!=0)
	{
		return Result;
	}
	//���� � ��� ���� ����������� �������� �� 1 ���, ���������� ���������. ��� �� ��������
	if ((*OurMarksToVictory)[1].Num()!= 0)
	{
		return -Result;
	}
	int i, j, k;
	 TArray<TArray<int>*> SaveLinesToRevert;
	 TArray<int> InPlayerPlace, InAIPlace;
	 int X, Y;
	 //���� �� �� ����� �������� �� 1 ���, � �������� ����� ����������� ��������� ����� ��������-���� ��� ������
	 if ((*EnemyMarksToVictory)[1].Num() != 0)
	 {
		 //����� ������ ���������� �����, ������� ��������� ����� ��������� �� 1 ���
		 for (i=0;i<(*(*EnemyMarksToVictory)[1][0]).Num();i++)
		 {
			 //���� ���������� ������������ ������������� ������ � ���� �����
			 X = (*(*EnemyMarksToVictory)[1][0])[i] / 4;
			 Y = (*(*EnemyMarksToVictory)[1][0])[i] % 4;
			 if (Matrix[X][Y] == 0)
			 {
				 //���������, ��� ����������, ���� ��������� ������
				 MarkAndCalculateResult(X, Y, Figure, SaveLinesToRevert, InPlayerPlace, InAIPlace);
				 //���������� ��������� ����� ������������������ ���� (�������� �� ���������, ��� ��� ��� �� �� �������� � ��� �� �������� ���������� ���������)
				 Result=TurnDynamic(TurnsToCalc, !bAITurn);
				 //���������� ���������� ���������� � ��������� �� �������� ����
				 Matrix[X][Y]=0;
				 for ( k = 0; k < SaveLinesToRevert.Num(); k++)
				 {
					 RecalcLineAfterErase(SaveLinesToRevert[k], InPlayerPlace[k], InAIPlace[k]);
				 }
				 //���������� ����������������� ���������
				 return Result;
			 }
		 }
	 }
	 //���������� ��������, ���� �� ����� �� �������� ������
	 if (TurnsToCalc < 1)
	 {
		 return Metric();
	 }
	 //���� ������ �� ��������� ����� TurnDynamic(TurnsToCalc-1, !bAITurn)
	 //������� ���������� ������ ������
	 int PossibleTurns = 0;
	 int ResultOfDynamic;
	 for (i = 0; i < Matrix.Num(); i++)
	 {
		 for (j = 0; j < Matrix[i].Num(); j++)
		 {
			 //��������� ������ �� ������
			 if (Matrix[i][j] != 0) continue;
			 PossibleTurns++;
			 //������������� ���������� ����������, ��� ��� ���������, ���� ������� ������ � ������������ i,j
			 MarkAndCalculateResult(i, j, Figure, SaveLinesToRevert, InPlayerPlace, InAIPlace);
			 //��������� �������� �� 1 ������� ���� �������� TurnsToCalc, �������� ��� ���������
			 ResultOfDynamic = TurnDynamic(TurnsToCalc-1, !bAITurn);
			 //��������� �� �������� �� ����� ��������� ResultOfDynamic ����� �������� Result, � ����������� �� ���� bAITurn(�� ��� ������)
			 if ((ResultOfDynamic<Result&&bAITurn)||(ResultOfDynamic > Result && !bAITurn))
			 {
				 Result = ResultOfDynamic;
			 }
			 //���������� ���������
			 Matrix[i][j] = 0;
			 for (k = 0; k < SaveLinesToRevert.Num(); k++)
			 {
				 RecalcLineAfterErase(SaveLinesToRevert[k], InPlayerPlace[k], InAIPlace[k]);
			 }

		 }
	 }
	 //���� �� ���� �� ������ ���������� ����-�����
	 if (PossibleTurns < 1) return 0;
	 //���������� ������ ���������
	return Result;
}