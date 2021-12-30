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
	//Проверяем не пытается ли игрок сходить в ход ИИ
	if (!bPlayerTurn) return;
	//Вычисляем выбранную игроком клетку
	PlayerX = ClickLocation.X / 100;
	PlayerY = ClickLocation.Y / 100;
	if (PlayerX < 4 && PlayerX >= 0 && PlayerY < 4 && PlayerY >= 0)
	{
		//проверяем, можно ли ставить фигуру на выбранную клетку
		if (Matrix[PlayerX][PlayerY] == 0)
		{
			//ставим крестик на выбранную клетку и передаем ход ИИ
			if (PlaceMark(PlayerX, PlayerY, 1)==0)
			{
				AITakeTurn();
			}
		}
	}
}

void ADataPawn::FindLinesWithCell(int CellX, int CellY, TArray<TArray<int>*>& OutFoundLines)
{
	//вспомогательная функция, вычисления элементарны
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
		//ищем насколько заполнена выбранная линия
		if (MarksToPlayerVictory[i].Contains(LineToChange))
		{
			if (bPlayerMark) 
			{
				//если заполнял игрок, то для игрока линия становится более заполненной (на 1 элемент меньше до победы).
				//Если заполнял ИИ, то игрок уже никогда не сможет заполнить линию и ее можно не рассматривать
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
		//Дублирование логики для победы не игрока, а ИИ
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
	//Вызываем виджет с победным окном
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
	//смотрим кто выиграл, чтобы узнать в каком массиве искать полностью заполненные линии
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
		//Ищем линию в массивах горизонтальных, вертикальных и диагональных линий, чтобы точно понять ее расположение в пространстве
		FoundIndex = HorizontalLines.Find(*WinLine);
		if ( FoundIndex!= INDEX_NONE)
		{
			LocalMeshActor = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), FVector((FoundIndex + 0.5f) * 100.f, 200.f, 5.f), FRotator(0.f, 0.f, 0.f));
			
			if (LocalMeshActor != NULL && FigureToSpawn != NULL)
			{
				//Задаем Static Mesh и его параметры для линии
				LocalMeshActor->SetMobility(EComponentMobility::Movable);
				LocalMeshActor->SetActorScale3D(FVector(0.1f, 4.f, 0.1f));
				LocalMeshActor->GetStaticMeshComponent()->SetStaticMesh(FigureToSpawn);
				LocalMeshActor->GetStaticMeshComponent()->SetMaterial(0, BlackMaterial);
			}
		}
		//повторение логики для вертикальных линий
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
		//повторение логики для диагоналей
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
	//простейшая метрика, оценивающая ситуацию по заполненности линий
	return 5*MarksToPlayerVictory[2].Num()+ 2* MarksToPlayerVictory[3].Num()+ MarksToPlayerVictory[4].Num()-(5 * MarksToAIVictory[2].Num() + 2 * MarksToAIVictory[3].Num() + MarksToAIVictory[4].Num());
}


void ADataPawn::BeginPlay()
{
	Super::BeginPlay();
	TArray<int> Temp;
	int i, j;
	//Задаем горизонтальные и вертикальные линии ИИ.
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
	//Задаем диагонали ИИ
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
	//Добавляем все линии ИИ в 4 элемент обобщающих массивов, так как на старте для победы нужно заполнить все 4 клетки любой из линий
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
	//Спавним Static Mesh Actor для поставленной на координаты CordinateX,CordinateY фигуры 
	AStaticMeshActor* LocalMeshActor;
	LocalMeshActor = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), FVector((CordinateX + 0.5f) * 100.f, (CordinateY + 0.5f) * 100.f, 5.f), FRotator(0.f, 0.f, 0.f));
	UStaticMesh* FigureToSpawn;
	//Выбираем крестик или нолик в зависимости от значения WhichFigure поставленной фигуры
	if (WhichFigure == 1)
	{
		FigureToSpawn = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, TEXT("/Game/Meshes/SM_Cross.SM_Cross")));

	}
	else
	{
		FigureToSpawn = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, TEXT("/Game/Meshes/SM_Circle.SM_Circle")));
	}
	//Задаем Static Mesh для Static Mesh Actor
	if (LocalMeshActor != NULL && FigureToSpawn != NULL)
	{
		LocalMeshActor->SetMobility(EComponentMobility::Movable);
		LocalMeshActor->GetStaticMeshComponent()->SetStaticMesh(FigureToSpawn);
	}
	//Вызываем MarkAndCalculateResult для изменения логических переменных AI
	TArray<TArray<int>*> ChangedLines;
	TArray<int> OldLinePlaceForPlayer, OldLinePlaceForAI;
	int ResultOfTurn=MarkAndCalculateResult(CordinateX, CordinateY, WhichFigure,ChangedLines,OldLinePlaceForPlayer,OldLinePlaceForAI);
	//Проверяем не закончилась ли игра победой ИИ или грока
	if (ResultOfTurn != 0)
	{
		EndGame(ResultOfTurn);
	}
	return ResultOfTurn;
}

int ADataPawn::MarkAndCalculateResult(int CordinateX, int CordinateY, int WhichFigure, TArray<TArray<int>*>& ChangedLines,TArray<int>&PlayerToRevert,TArray<int>&AIToRevert)
{
	//Заполняем матрицу клеток, чтобы никто не мог походить на ту же клетку
	Matrix[CordinateX][CordinateY] = WhichFigure;
	//Инициализируем массивы
	ChangedLines.Empty();
	PlayerToRevert.Empty();
	AIToRevert.Empty();
	//Ищем все линии ИИ, которые надо будет изменить
	FindLinesWithCell(CordinateX, CordinateY, ChangedLines);
	int PlayerMarksInLine, AIMarksInLine;
	for (auto& LineToChange : ChangedLines)
	{
		//меняем выбранные линии по одной
		RecalcLineAfterMark(LineToChange, WhichFigure==1,PlayerMarksInLine, AIMarksInLine);
		PlayerToRevert.Add(PlayerMarksInLine);
		AIToRevert.Add(AIMarksInLine);
	}
	//Проверяем не закончилась ли игра победой игрока или ИИ после завершения хода
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
	//запрещаем игроку ходить на ходу ИИ
	bPlayerTurn = false;
	int i, j, k;
	//Оценка результата каждого хода
	int ResultOfDynamic;
	//Лучший из доступных результатов
	int BestMetric = 1000;
	//Сколько доступных ходов осталось на поле
	int PossibleTurns = 0;
	//Все ходы с лучшим результатом BestMetric. (номера клеток 0-15)
	TArray<int> BestTurns;
	//Все измененные в результате проверки хода линии ИИ. После оценки хода, возвращаются в прежнее состояние InPlayerPlace, InAIPlace
	TArray<TArray<int>*> SaveLinesToRevert;
	//Места хранения линий SaveLinesToRevert до изменений.
	TArray<int> InPlayerPlace, InAIPlace;
	for (i = 0; i < Matrix.Num(); i++)
	{
		for (j = 0; j < Matrix[i].Num(); j++)
		{
			//Отбрасываем непустые клетки
			if (Matrix[i][j] != 0) continue;
			PossibleTurns++;
			//Пересчитываем логические переменные, как они изменятся, если выбрать клетку с координатами i,j
			MarkAndCalculateResult(i, j, -1, SaveLinesToRevert, InPlayerPlace, InAIPlace);
			//Вычисляем результат динамики
			ResultOfDynamic = TurnDynamic(DynamicLength, false);
			//Так как ходит ИИ, для него лучшим ходом является наименьшее число (в идеале -1000). Записываем все ходы с наименьшей метрикой
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
			//откатываем изменения, так как ход только просчитывался, а не выбирался ИИ.
			Matrix[i][j] = 0;
			for (k = 0; k < SaveLinesToRevert.Num(); k++)
			{
				RecalcLineAfterErase(SaveLinesToRevert[k], InPlayerPlace[k], InAIPlace[k]);
			}
			
		}
	}
	//Если нет свободных клеток (в том числе и для ИИ) - значит ничья
	if (BestTurns.Num() == 0)
	{
		EndGame(0);
		return;
	}
	//Добавляем элемент случайности. Все равно все ходы из BestTurns одинаково эффективны между собой.
	int ChoosedTurn = FMath::RandRange(0, BestTurns.Num() - 1);
	bool bGameWonByLastTurn = PlaceMark(BestTurns[ChoosedTurn] / 4, BestTurns[ChoosedTurn] % 4, -1) != 0;
	//Если ИИ заполнил последнюю свободную клетку и при этом не выиграл - значит ничья.
	if (PossibleTurns <= 1&&!bGameWonByLastTurn)
	{
		EndGame(0);
		return;
	}
	//возвращаем игроку ход
	bPlayerTurn = true;
}

int ADataPawn::TurnDynamic(int TurnsToCalc, bool bAITurn)
{
	//ИИ просчитывает как свои ходы, так и ходы игрока.
	//Поэтому переменные свой-чужой EnemyMarksToVictory, OurMarksToVictory, лучший результат Result, и тип фигуры для заполнения  Figure(крестик/нолик)
	//должны переворачиваться в зависимости от типа хода bAITurn
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
	//Если перед вызовом динамики оппонент выиграл своим последним ходом, передаем лучший для него результат
	if ((*EnemyMarksToVictory)[0].Num()!=0)
	{
		return Result;
	}
	//Если у нас есть возможность выиграть за 1 ход, возвращаем оппоненту. что он проиграл
	if ((*OurMarksToVictory)[1].Num()!= 0)
	{
		return -Result;
	}
	int i, j, k;
	 TArray<TArray<int>*> SaveLinesToRevert;
	 TArray<int> InPlayerPlace, InAIPlace;
	 int X, Y;
	 //Если мы не можем выиграть за 1 ход, а оппонент имеет возможность следующим ходом выиграть-надо ему мешать
	 if ((*EnemyMarksToVictory)[1].Num() != 0)
	 {
		 //берем первую попавшуюся линию, которую противник может заполнить за 1 ход
		 for (i=0;i<(*(*EnemyMarksToVictory)[1][0]).Num();i++)
		 {
			 //ищем координаты единственной незаполненной клетки в этой линии
			 X = (*(*EnemyMarksToVictory)[1][0])[i] / 4;
			 Y = (*(*EnemyMarksToVictory)[1][0])[i] % 4;
			 if (Matrix[X][Y] == 0)
			 {
				 //вычисляем, что произойдет, если заполнить клетку
				 MarkAndCalculateResult(X, Y, Figure, SaveLinesToRevert, InPlayerPlace, InAIPlace);
				 //запоминаем результат этого безальтернативного хода (динамику не углубляем, так как ход мы не выбирали и это не замедлит исполнение программы)
				 Result=TurnDynamic(TurnsToCalc, !bAITurn);
				 //откатываем логические переменные в состояние до проверки хода
				 Matrix[X][Y]=0;
				 for ( k = 0; k < SaveLinesToRevert.Num(); k++)
				 {
					 RecalcLineAfterErase(SaveLinesToRevert[k], InPlayerPlace[k], InAIPlace[k]);
				 }
				 //возвращаем безальтернативный результат
				 return Result;
			 }
		 }
	 }
	 //Завершение динамики, если мы дошли до нулевого уровня
	 if (TurnsToCalc < 1)
	 {
		 return Metric();
	 }
	 //Ищем лучший из доступных ходов TurnDynamic(TurnsToCalc-1, !bAITurn)
	 //Считаем количество пустых клеток
	 int PossibleTurns = 0;
	 int ResultOfDynamic;
	 for (i = 0; i < Matrix.Num(); i++)
	 {
		 for (j = 0; j < Matrix[i].Num(); j++)
		 {
			 //проверяем пустая ли клетка
			 if (Matrix[i][j] != 0) continue;
			 PossibleTurns++;
			 //Пересчитываем логические переменные, как они изменятся, если выбрать клетку с координатами i,j
			 MarkAndCalculateResult(i, j, Figure, SaveLinesToRevert, InPlayerPlace, InAIPlace);
			 //результат динамики на 1 уровень ниже текущего TurnsToCalc, передаем ход оппоненту
			 ResultOfDynamic = TurnDynamic(TurnsToCalc-1, !bAITurn);
			 //проверяем не является ли новый результат ResultOfDynamic лучше текущего Result, в зависимости от хода bAITurn(ИИ или игрока)
			 if ((ResultOfDynamic<Result&&bAITurn)||(ResultOfDynamic > Result && !bAITurn))
			 {
				 Result = ResultOfDynamic;
			 }
			 //откатываем изменения
			 Matrix[i][j] = 0;
			 for (k = 0; k < SaveLinesToRevert.Num(); k++)
			 {
				 RecalcLineAfterErase(SaveLinesToRevert[k], InPlayerPlace[k], InAIPlace[k]);
			 }

		 }
	 }
	 //если не было ни одного доступного хода-ничья
	 if (PossibleTurns < 1) return 0;
	 //возвращаем лучший результат
	return Result;
}