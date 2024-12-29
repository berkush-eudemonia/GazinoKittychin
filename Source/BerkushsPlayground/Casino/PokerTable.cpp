// 2024'te R. Berk Sozgen tarafindan gelistirilmeye baslanmistir.


#include "PokerTable.h"
#include "Card.h"
#include "CasinoPlayer.h"
#include "Net/UnrealNetwork.h"

// Sets default values
APokerTable::APokerTable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	DeckSpawnPosition = CreateDefaultSubobject<USceneComponent>(TEXT("DeckSpawnPosition"));
	DeckSpawnPosition->SetupAttachment(RootComponent);
	DeskCard1 = CreateDefaultSubobject<USceneComponent>(TEXT("DeskCard1"));
	DeskCard1->SetupAttachment(RootComponent);
	DeskCard2 = CreateDefaultSubobject<USceneComponent>(TEXT("DeskCard2"));
	DeskCard2->SetupAttachment(RootComponent);
	DeskCard3 = CreateDefaultSubobject<USceneComponent>(TEXT("DeskCard3"));
	DeskCard3->SetupAttachment(RootComponent);
	DeskCard4 = CreateDefaultSubobject<USceneComponent>(TEXT("DeskCard4"));
	DeskCard4->SetupAttachment(RootComponent);
	DeskCard5 = CreateDefaultSubobject<USceneComponent>(TEXT("DeskCard5"));
	DeskCard5->SetupAttachment(RootComponent);
	

	ConstructorHelpers::FClassFinder<AActor> CardBPClass(TEXT("/Game/Casino/BP_Card"));
	if (!ensure(CardBPClass.Class != nullptr)) return;

	CardClass = CardBPClass.Class;

}

// Called when the game starts or when spawned
void APokerTable::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		if(UWorld* World = GetWorld())
		{
			float Delay = 5.f;
			FTimerHandle TimerHandle;
			World->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateWeakLambda(this, [this]
			{
				UE_LOG(LogTemp, Display, TEXT("Timer Fired"));
				FTransform SpawnTransform = DeckSpawnPosition->GetComponentTransform();
				Server_PokerInitilizaer(SpawnTransform);
			}),Delay,false/*in loop*/);
		}
	}
}

// Called every frame
void APokerTable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APokerTable::Server_PokerInitilizaer_Implementation(FTransform Transform)
{
	MultiCast_PokerInitilizaer(Transform);
}

void APokerTable::MultiCast_PokerInitilizaer_Implementation(FTransform Transform)
{
	UE_LOG(LogTemp, Warning, TEXT("PokerTable::PokerInitilizaer"));
	UWorld* World = GetWorld();

	if (World==nullptr) return;
	
	for (int i = 0; i < 4; i++)
	{
		for (int j = 2; j < 15; j++)
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.Owner = this;
			SpawnInfo.Instigator = GetInstigator();
			SpawnInfo.Name = *FString::Printf(TEXT("PokerCard_%i_%i"), i, j);
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			ACard* CurrentCard = Cast<ACard>(World->SpawnActor(CardClass, &Transform, SpawnInfo));
			//CurrentCard = Cast<ACard>(World->SpawnActorDeferred<AActor>(CardClass, DeckSpawnPosition));
			CurrentCard->CardSymbol = i;
			CurrentCard->CardPower = j;
			//CurrentCard->MyInitialize();
			PokerCards.Add(CurrentCard);
		}
	}
}

void APokerTable::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(APokerTable, var name);
}

AActor* APokerTable::Poker_WhoWinner(TArray<class ACasinoPlayer*> Players, TArray<class ACard*> DeskCards)
{
	TArray<FWinnerCardInfo> WinnersCardInfos;

	//Royal Flush Winner Check
	//Royal Flush Implementation End

	//Straight Flush Winner Check
	//Straight Flush Implementation End

	//Quad Winner Check
	//Quad Implementation End

	//Full House Winner Check
	//Full House Implementation End

	//Flush Winner Check
	//Flush Implementation End

	//Straight Winner Check
	//Straight Implementation End

	//Three of a Kind Winner Check
	//Three of a Kind Implementation End
	
	//Two Pair Winner Check
	//Two Pair Implementation End
	
	//Pair Winner Check
	//Pair Implementation End
	
	//High Card Winner Check
	for (int i = 0; i < Players.Num(); i++)
	{
		FWinnerCardInfo CurrentCardInfo = Poker_HighCard(Players[i]->PlayersHand);
		CurrentCardInfo.PlayerIndex = i;
		WinnersCardInfos.Add(CurrentCardInfo);
	}
	FWinnerCardInfo Winner = WinnersCardInfos[0];
	for (int i = 1; i < WinnersCardInfos.Num(); i++)
	{
		if (WinnersCardInfos[i].CardPower > Winner.CardPower)
		{
			Winner.CardPower=WinnersCardInfos[i].CardPower;
			Winner.CardSymbol=WinnersCardInfos[i].CardSymbol;
			Winner.PlayerIndex=WinnersCardInfos[i].PlayerIndex;
		}
		else if (WinnersCardInfos[i].CardPower == Winner.CardPower && WinnersCardInfos[i].CardSymbol > Winner.CardSymbol)
		{
			Winner.CardPower=WinnersCardInfos[i].CardPower;
			Winner.CardSymbol=WinnersCardInfos[i].CardSymbol;
			Winner.PlayerIndex=WinnersCardInfos[i].PlayerIndex;
		}
	}
	//High Card Implementation End
	return Players[Winner.PlayerIndex];
}

FWinnerCardInfo APokerTable::Poker_RoyalFlush(TArray <class ACard*> PlayerCards, TArray<class ACard*> DeskCards)
{
	FWinnerCardInfo CardInfo;
	CardInfo.WinFunctionName = TEXT("Royal Flush");
	CardInfo.bIsWin = false;
    FWinnerCardInfo StraightFlush = Poker_StraightFlush(PlayerCards, DeskCards);
	if (StraightFlush.CardPower == 14)
	{
		CardInfo.CardPower = 14;
		CardInfo.CardSymbol = StraightFlush.CardSymbol;
		CardInfo.bIsWin = true;
	}
	return CardInfo;
}
FWinnerCardInfo APokerTable::Poker_StraightFlush(TArray <class ACard*> PlayerCards, TArray<class ACard*> DeskCards)
{
	FWinnerCardInfo CardInfo;
	CardInfo.WinFunctionName = TEXT("Straight Flush");
	CardInfo.bIsWin = false;

	TArray<ACard*> StraightCards = Poker_Straight_Array(PlayerCards, DeskCards);
	if (StraightCards.Num() < 5) return CardInfo; //devamini kontrol etmeye gerek yok
	uint8 SinekCount = 0;
	uint8 MacaCount = 0;
	uint8 KaroCount = 0;
	uint8 KupaCount = 0;
	for(auto Card : StraightCards)
	{
		if (Card->CardSymbol == 0) SinekCount++;
		else if (Card->CardSymbol == 1) MacaCount++;
		else if (Card->CardSymbol == 2) KaroCount++;
		else if (Card->CardSymbol == 3) KupaCount++;
	}
	if(SinekCount == 5 || MacaCount == 5 || KaroCount == 5 || KupaCount == 5)
	{ //Hepsi icin ayri logic kurmam gerekebilir buna bak! ama calisiyor yani btw
		FWinnerCardInfo PokerStraightCardInfo = Poker_Straight(PlayerCards, DeskCards);
		CardInfo.CardPower = PokerStraightCardInfo.CardPower;
		CardInfo.CardSymbol = PokerStraightCardInfo.CardSymbol;
		CardInfo.bIsWin = true;
	}
	return CardInfo;
}
FWinnerCardInfo APokerTable::Poker_Quad(TArray <class ACard*> PlayerCards, TArray<class ACard*> DeskCards)
{
	FWinnerCardInfo CardInfo;
	CardInfo.WinFunctionName = TEXT("Quad");
	CardInfo.bIsWin = false;
	
	uint8 FirstCardQuadCount = 0;
	uint8 SecondCardQuadCount = 0;
	//Elindeki iki kart es ise 2, degil ise 3 tane daha kart bulmamiz lazim
	if(PlayerCards[0]->CardPower == PlayerCards[1]->CardPower)
	{
		FirstCardQuadCount++;
		SecondCardQuadCount++;
	}
	//Ortadaki es kartlari bu
	for(int i = 0; i < 5; i++)
	{
		if(PlayerCards[0]->CardPower == DeskCards[i]->CardPower) FirstCardQuadCount++;
		if(PlayerCards[1]->CardPower == DeskCards[i]->CardPower) SecondCardQuadCount++;
	}
	//Quad sayisi 3 olmussa kazaniyor
	if(FirstCardQuadCount == 3 && SecondCardQuadCount < 3)
	{
		CardInfo.CardPower = PlayerCards[0]->CardPower;
		CardInfo.CardSymbol = 3; //En iyisi olmasi lazim
		CardInfo.bIsWin = true;
		return CardInfo;
	}
	if (FirstCardQuadCount < 3 && SecondCardQuadCount == 3)
	{
		CardInfo.CardPower = PlayerCards[1]->CardPower;
		CardInfo.CardSymbol = 3; //En iyisi olmasi lazim
		CardInfo.bIsWin = true;
		return CardInfo;
	}
	if (FirstCardQuadCount == 3 && SecondCardQuadCount == 3)
	{
		if(PlayerCards[0]->CardPower == PlayerCards[1]->CardPower)
		{//Kartlarin ayni olma durumu
			CardInfo.CardPower = PlayerCards[1]->CardPower;
			CardInfo.CardSymbol = 3; //En iyisi olmasi lazim
			CardInfo.bIsWin = true;
			return CardInfo;
		}
		/*else*/
		{
			CardInfo.CardPower = (PlayerCards[0]->CardPower > PlayerCards[1]->CardPower) ? PlayerCards[0]->CardPower : PlayerCards[1]->CardPower;
		}
	}
	
	return CardInfo;
}
FWinnerCardInfo APokerTable::Poker_FullHouse(TArray <class ACard*> PlayerCards, TArray<class ACard*> DeskCards)
{
	FWinnerCardInfo CardInfo;
	CardInfo.WinFunctionName = TEXT("Full House");
	CardInfo.bIsWin = false;
	
	//Oyuncunun elindeki kartlarin ayni olup olmadigini kontrol et
	if(PlayerCards[0]->CardPower == PlayerCards[1]->CardPower) return CardInfo; //Oyuncunun ellerindeki iki kartta aynu full house olamaz
	
	bool bHasFirstCardThreeOfAKind = Poker_ThreeOfAKind_Individual(PlayerCards[0],DeskCards).bIsWin;
	bool bHasFirstCardPair = Poker_Pair_Individual(PlayerCards[0],DeskCards).bIsWin;
	
	bool bHasSecondCardThreeOfAKind = Poker_ThreeOfAKind_Individual(PlayerCards[1],DeskCards).bIsWin;
	bool bHasSecondCardPair = Poker_Pair_Individual(PlayerCards[0],DeskCards).bIsWin;

	//Kart1 Three of a kind ve Kart2'nin Pair olma konumunu yokla
	bool bIsFirstThreeOfAKindAndSecondHasPair = (bHasFirstCardThreeOfAKind && bHasSecondCardPair);
	//Kart2 Three of a kind ve Kart1'nin Pair olma konumunu yokla
	bool bIsSecondThreeOfAKindAndFirstHasPair = (bHasSecondCardThreeOfAKind && bHasFirstCardPair);

	if(bIsFirstThreeOfAKindAndSecondHasPair && !bIsSecondThreeOfAKindAndFirstHasPair)
	{
		//Kart1 Three of a kind ve Kart2'nin Pair olma durumu
		FWinnerCardInfo IGuessThreeOfAKindIsMoreValuable = Poker_ThreeOfAKind_Individual(PlayerCards[0],DeskCards);
		CardInfo.CardPower = IGuessThreeOfAKindIsMoreValuable.CardPower;
		CardInfo.CardSymbol = IGuessThreeOfAKindIsMoreValuable.CardSymbol;
		CardInfo.bIsWin = true;
		return CardInfo;
	}
	if(!bIsFirstThreeOfAKindAndSecondHasPair && bIsSecondThreeOfAKindAndFirstHasPair)
	{
		//Kart2 Three of a kind ve Kart1'nin Pair olma durumu
		FWinnerCardInfo IGuessThreeOfAKindIsMoreValuable = Poker_ThreeOfAKind_Individual(PlayerCards[1],DeskCards);
		CardInfo.CardPower = IGuessThreeOfAKindIsMoreValuable.CardPower;
		CardInfo.CardSymbol = IGuessThreeOfAKindIsMoreValuable.CardSymbol;
		CardInfo.bIsWin = true;
		return CardInfo;
	}
	if(bIsFirstThreeOfAKindAndSecondHasPair && bIsSecondThreeOfAKindAndFirstHasPair)
	{
		//Iki kartin da 2 kosulu saglama durumu
		FWinnerCardInfo FirstCardInfo = Poker_ThreeOfAKind_Individual(PlayerCards[0],DeskCards);
		FWinnerCardInfo SecondCardInfo = Poker_ThreeOfAKind_Individual(PlayerCards[1],DeskCards);
		//Iki kartin da degeri ayni olmayacaina gore
		CardInfo.CardPower = FirstCardInfo.CardPower;
		CardInfo.CardSymbol = FirstCardInfo.CardSymbol;
		if (SecondCardInfo.CardPower > CardInfo.CardPower)
		{
			CardInfo.CardPower = SecondCardInfo.CardPower;
			CardInfo.CardSymbol = SecondCardInfo.CardSymbol;
		}
		CardInfo.bIsWin = true;
		return CardInfo;
	}
	//Hic bir kosul saglanmiyor, f for player :/
	return CardInfo;
}
FWinnerCardInfo APokerTable::Poker_Flush(TArray <class ACard*> PlayerCards, TArray<class ACard*> DeskCards)
{
	FWinnerCardInfo CardInfo;
	CardInfo.WinFunctionName = TEXT("Flush");
	CardInfo.bIsWin = false;
	if (PlayerCards[0]->CardSymbol == PlayerCards[1]->CardSymbol)
	{
		uint8 HowManyCardsWeNeed = 3;
		for (int i = 0; i < 5; i++)
		{
			if (PlayerCards[0]->CardSymbol == DeskCards[i]->CardSymbol)
			{
				--HowManyCardsWeNeed;
				if (HowManyCardsWeNeed <= 0)
				{
					if (PlayerCards[0]->CardPower > PlayerCards[1]->CardPower)
					{
						CardInfo.CardPower = PlayerCards[0]->CardPower;
						CardInfo.CardSymbol = PlayerCards[0]->CardSymbol;
						CardInfo.bIsWin = true;
						return CardInfo;
					}
					/*else*/
					{
						CardInfo.CardPower = PlayerCards[1]->CardPower;
						CardInfo.CardSymbol = PlayerCards[1]->CardSymbol;
						CardInfo.bIsWin = true;
						return CardInfo;
					}
				}
			}
		}
	}
	/*else*/
	{
		uint8 HowManyCardsWeNeedForFirstCard = 4;
		uint8 HowManyCardsWeNeedForSecondCard = 4;
		for (int i = 0; i < 5; i++)
		{
			if (PlayerCards[0]->CardSymbol == DeskCards[i]->CardSymbol)
			{
				HowManyCardsWeNeedForFirstCard--;
			}
			if (PlayerCards[1]->CardSymbol == DeskCards[i]->CardSymbol)
			{
				HowManyCardsWeNeedForSecondCard--;
			}
		}
		
		if (HowManyCardsWeNeedForFirstCard <= 0 && HowManyCardsWeNeedForSecondCard > 0)
		{
			CardInfo.CardPower = PlayerCards[0]->CardPower;
			CardInfo.CardSymbol = PlayerCards[0]->CardSymbol;
			CardInfo.bIsWin = true;
			return CardInfo;
		}
		/*else*/ if (HowManyCardsWeNeedForFirstCard > 0 && HowManyCardsWeNeedForSecondCard <= 0)
		{
			CardInfo.CardPower = PlayerCards[1]->CardPower;
			CardInfo.CardSymbol = PlayerCards[1]->CardSymbol;
			CardInfo.bIsWin = true;
			return CardInfo;
		}
		/*else*/ if (HowManyCardsWeNeedForFirstCard <= 0 && HowManyCardsWeNeedForSecondCard <= 0)
		{
			if(PlayerCards[0]->CardPower > PlayerCards[1]->CardPower)
			{
				CardInfo.CardPower = PlayerCards[0]->CardPower;
				CardInfo.CardSymbol = PlayerCards[0]->CardSymbol;
				CardInfo.bIsWin = true;
				return CardInfo;
			}
			/*else*/
			{
				CardInfo.CardPower = PlayerCards[1]->CardPower;
				CardInfo.CardSymbol = PlayerCards[1]->CardSymbol;
				CardInfo.bIsWin = true;
				return CardInfo;
			}
		}
		/*else*/
		{
			CardInfo.bIsWin = false;
			return CardInfo;
		}
	}
}
FWinnerCardInfo APokerTable::Poker_Straight_Individual_Checker(TArray <class ACard*> PlayerCards, TArray<class ACard*> DeskCards)
{
	FWinnerCardInfo CardInfo;
	CardInfo.WinFunctionName = TEXT("Poker_Straight_Individual_Checker, It Musn't Going Out To Scope");
	CardInfo.bIsWin = false;
	//Kart1 Den Basla
	FWinnerCardInfo PlayerCard1Info = Poker_Straight_Individual(PlayerCards[0], DeskCards);
	//Kart2 yi Hesapla
	FWinnerCardInfo PlayerCard2Info = Poker_Straight_Individual(PlayerCards[1], DeskCards);
	//Kazanan karti geri dondur, hicbiri kazanmamisa CardInfo false yolla
	if(PlayerCard1Info.bIsWin && !PlayerCard2Info.bIsWin) return PlayerCard1Info;
	if(!PlayerCard1Info.bIsWin && PlayerCard2Info.bIsWin) return PlayerCard2Info;
	if(PlayerCard1Info.bIsWin && PlayerCard2Info.bIsWin) //Iki kartta buyuk, buyuk karti bul
	{
		if(PlayerCard1Info.CardPower > PlayerCard2Info.CardPower) return PlayerCard1Info;
		if(PlayerCard1Info.CardPower < PlayerCard2Info.CardPower) return PlayerCard2Info;
		if(PlayerCard1Info.CardSymbol > PlayerCard2Info.CardSymbol) return PlayerCard1Info;
		else return PlayerCard2Info;
	}
	return CardInfo; //Playerin hic bir individual karti kazanmadi bos yolla
}
FWinnerCardInfo APokerTable::Poker_Straight_Individual(class ACard* PlayerCard, TArray<class ACard*> DeskCards)
{
	FWinnerCardInfo CardInfo;
	CardInfo.WinFunctionName = TEXT("Poker_Straight_Individual, It Musn't Going Out To Scope");
	CardInfo.bIsWin = false;
	//Oyuncunun elindeki tak kartin Straight olusturabilirmesi icin
	//4 Kucuk -> Return player card
	//3 Kucuk 1 Buyuk -> Return big card
	//2 Kucuk 2 Buyuk -> Return big card
	//1 Kucuk 3 Buyuk -> Return big card
	//4 Buyuk -> Return big card
	bool bFourValueSmaller = false;
	bool bThreeValueSmaller = false;
	bool bTwoValueSmaller = false;
	bool bOneValueSmaller = false;
	bool bOneValueGreater = false;
	bool bTwoValueGreater = false;
	bool bThreeValueGreater = false;
	bool bFourValueGreater = false;

	uint8 PlayerCardPower = PlayerCard->CardPower;
	for(int i = 0; i < 5; i++)
	{
		if (PlayerCardPower - 4 == DeskCards[i]->CardPower) bFourValueSmaller = true;
		if (PlayerCardPower - 3 == DeskCards[i]->CardPower) bThreeValueSmaller = true;
		if (PlayerCardPower - 2 == DeskCards[i]->CardPower) bTwoValueSmaller = true;
		if (PlayerCardPower - 1 == DeskCards[i]->CardPower) bOneValueSmaller = true;
		if (PlayerCardPower + 1 == DeskCards[i]->CardPower) bOneValueGreater = true;
		if (PlayerCardPower + 2 == DeskCards[i]->CardPower) bTwoValueGreater = true;
		if (PlayerCardPower + 3 == DeskCards[i]->CardPower) bThreeValueGreater = true;
		if (PlayerCardPower + 4 == DeskCards[i]->CardPower) bFourValueGreater = true;
	}

	//4 Kucuk Var Mi Control Et
	if (bFourValueSmaller && bThreeValueSmaller && bTwoValueSmaller && bOneValueSmaller)
	{
		CardInfo.CardPower = PlayerCardPower;
		CardInfo.CardSymbol = PlayerCard->CardSymbol;
		CardInfo.bIsWin = true;
		return CardInfo;
	}
	//3 Kucuk 1 Buyuk Var Mi Control Et
	if (bThreeValueSmaller && bTwoValueSmaller && bOneValueSmaller && bOneValueGreater)
	{
		//1 Buyuk Kartin Degerlerini Al
		for (int i = 0; i < 5; i++)
		{
			if (PlayerCardPower + 1 == DeskCards[i]->CardPower)
			{
				CardInfo.CardPower = PlayerCardPower + 1;
				CardInfo.CardSymbol = DeskCards[i]->CardSymbol;
				break;
			}
		}
		CardInfo.bIsWin = true;
		return CardInfo;
	}
	//2 Kucuk 2 Buyuk Var Mi Control Et
	if (bTwoValueSmaller && bOneValueSmaller && bOneValueGreater && bTwoValueGreater)
	{
		//2 Buyuk Kartin Degerlerini Al
		for (int i = 0; i < 5; i++)
		{
			if (PlayerCardPower + 2 == DeskCards[i]->CardPower)
			{
				CardInfo.CardPower = PlayerCardPower + 2;
				CardInfo.CardSymbol = DeskCards[i]->CardSymbol;
				break;
			}
		}
		CardInfo.bIsWin = true;
		return CardInfo;
	}
	//1 Kucuk 3 Buyuk Var Mi Control Et
	if (bOneValueSmaller && bOneValueGreater && bTwoValueGreater && bThreeValueGreater)
	{
		//3 Buyuk Kartin Degerlerini Al
		for (int i = 0; i < 5; i++)
		{
			if (PlayerCardPower + 3 == DeskCards[i]->CardPower)
			{
				CardInfo.CardPower = PlayerCardPower + 2;
				CardInfo.CardSymbol = DeskCards[i]->CardSymbol;
				break;
			}
		}
		CardInfo.bIsWin = true;
		return CardInfo;
	}
	//4 Buyuk Var Mi Control Et
	if (bOneValueGreater && bTwoValueGreater && bThreeValueGreater && bFourValueGreater)
	{
		//4 Buyuk Kartin Degerlerini Al
		for (int i = 0; i < 5; i++)
		{
			if (PlayerCardPower + 4 == DeskCards[i]->CardPower)
			{
				CardInfo.CardPower = PlayerCardPower + 2;
				CardInfo.CardSymbol = DeskCards[i]->CardSymbol;
				break;
			}
		}
		CardInfo.bIsWin = true;
		return CardInfo;
	}
	//Array Olmuyorsa GG;
	return CardInfo;
}
FWinnerCardInfo APokerTable::Poker_Straight(TArray <class ACard*> PlayerCards, TArray<class ACard*> DeskCards)
{
	FWinnerCardInfo CardInfo;
	CardInfo.WinFunctionName = TEXT("Straight");
	CardInfo.bIsWin = false;
	//Check Player Hand Can be Valid for Straight and they aren't same power
	uint8 PlayerCardsDifferance = FMath::Abs(PlayerCards[0]->CardPower - PlayerCards[1]->CardPower);
	if (PlayerCardsDifferance <= 4 && (PlayerCards[0]->CardPower - PlayerCards[1]->CardPower) != 0)
	{
		if(PlayerCardsDifferance == 1)
		{
			//Bu kartlar ardisik
			bool bThreeValueSmallerThanSmaller = false;
			bool bTwoValueSmallerThanSmaller = false;
			bool bOneValueSmallerThanSmaller = false;
			bool bOneValueGreaterThanGreater = false;
			bool bTwoValueGreaterThanGreater = false;
			bool bThreeValueGreaterThanGreater = false;

			uint8 PlayerBetterCardPower = Poker_ReturnHigherCard(PlayerCards[0], PlayerCards[1])->CardPower;
			uint8 ThreeValueSmallerCardPower = PlayerBetterCardPower - 4;
			uint8 TwoValueSmallerCardPower = PlayerBetterCardPower - 3;
			uint8 OneValueSmallerCardPower = PlayerBetterCardPower - 2;
			uint8 OneValueGreaterCardPower = PlayerBetterCardPower + 1;
			uint8 TwoValueGreaterCardPower = PlayerBetterCardPower + 2;
			uint8 ThreeValueGreaterCardPower = PlayerBetterCardPower + 3;

			for(int i = 0; i < 5; i++)
			{
				if(ThreeValueSmallerCardPower == DeskCards[i]->CardPower)
				{
					bThreeValueSmallerThanSmaller = true;
					continue;
				}
				if(TwoValueSmallerCardPower == DeskCards[i]->CardPower)
				{
					bTwoValueSmallerThanSmaller = true;
					continue;
				}
				if(OneValueSmallerCardPower == DeskCards[i]->CardPower)
				{
					bOneValueSmallerThanSmaller = true;
					continue;
				}
				if(OneValueGreaterCardPower == DeskCards[i]->CardPower)
				{
					bOneValueGreaterThanGreater = true;
					continue;
				}
				if(TwoValueGreaterCardPower == DeskCards[i]->CardPower)
				{
					bTwoValueGreaterThanGreater = true;
					continue;
				}
				if(ThreeValueGreaterCardPower == DeskCards[i]->CardPower)
				{
					bThreeValueGreaterThanGreater = true;
					continue;
				}
			}

			//Oyuncunun eline ek kazanma sartlari = 3 kucuk, 2 kucuk 1 buyuk, 1 kucuk 2 buyuk, 3 buyuk
			if(bThreeValueSmallerThanSmaller && bTwoValueSmallerThanSmaller && bOneValueSmallerThanSmaller)
			{
				return CardInfo;
			}
			if(bTwoValueSmallerThanSmaller && bOneValueSmallerThanSmaller && bOneValueGreaterThanGreater)
			{
				return CardInfo;
			}
			if(bOneValueSmallerThanSmaller && bOneValueGreaterThanGreater && bTwoValueGreaterThanGreater)
			{
				return CardInfo;
			}
			if(bOneValueGreaterThanGreater && bTwoValueGreaterThanGreater && bThreeValueGreaterThanGreater)
			{
				return CardInfo;
			}
			//Demekki kent yok puhaha
			return CardInfo;
		}
		/*else*/ if (PlayerCardsDifferance == 2)
		{
			//Aralarinda bir kart var, onun degerini bul
			uint8 BetweenCardPower = (PlayerCards[0]->CardPower + PlayerCards[1]->CardPower) / 2;
			bool bIsThereBetweenCard = false;
			for(int i = 0; i < 5; i++)
			{
				if(BetweenCardPower == DeskCards[i]->CardPower)
				{
					bIsThereBetweenCard = true;
					break;
				}
			}
			if (!bIsThereBetweenCard)
			{
				return CardInfo;
			}
			//Aralarinda bir kart var, kalan senaryolar = kucukten 2 kucuk, buyukten 2 buyuk, kucukten 1 kucuk ve buyukten 1 buyuk
			bool bTwoValueSmallerThanSmaller = false;
			bool bOneValueSmallerThanSmaller = false;
			bool bOneValueGreaterThanGreater = false;
			bool bTwoValueGreaterThanGreater = false;

			uint8 TwoValueSmallerCardPower = BetweenCardPower - 3;
			uint8 OneValueSmallerCardPower = BetweenCardPower - 2;
			uint8 OneValueGreaterCardPower = BetweenCardPower + 2;
			uint8 TwoValueGreaterCardPower = BetweenCardPower + 3;
			
			for(int i = 0; i < 5; i++)
			{
				if(TwoValueSmallerCardPower == DeskCards[i]->CardPower)
				{
					bTwoValueSmallerThanSmaller = true;
					continue;
				}
				if(OneValueSmallerCardPower == DeskCards[i]->CardPower)
				{
					bOneValueSmallerThanSmaller = true;
					continue;
				}
				if(OneValueGreaterCardPower == DeskCards[i]->CardPower)
				{
					bOneValueGreaterThanGreater = true;
					continue;
				}
				if(TwoValueGreaterCardPower == DeskCards[i]->CardPower)
				{
					bTwoValueGreaterThanGreater = true;
					continue;
				}
			}
			
			if(bTwoValueSmallerThanSmaller && bOneValueSmallerThanSmaller)
			{
				//Oyuncunun elindeki buyuk card buyuk
				CardInfo.bIsWin = true;
				CardInfo.CardPower = Poker_ReturnHigherCard(PlayerCards[0], PlayerCards[1])->CardPower;
				CardInfo.CardSymbol = Poker_ReturnHigherCard(PlayerCards[0], PlayerCards[1])->CardPower;
				return CardInfo;
			}
			if (bOneValueSmallerThanSmaller && bOneValueGreaterThanGreater)
			{
				//Masada Kent'i tamamlayan, oyununun elindeki buyuk karttan 1 degerli olan card buyuk
				CardInfo.bIsWin = true;
				for(int i = 0; i < 5; i++)
				{
					if (OneValueGreaterCardPower == DeskCards[i]->CardPower)
					{
						CardInfo.CardPower = DeskCards[i]->CardPower;
						CardInfo.CardSymbol = DeskCards[i]->CardSymbol;
						break;
					}
				}
				return CardInfo;
			}
			if (bOneValueGreaterThanGreater && bTwoValueGreaterThanGreater)
			{
				//Masada Kent'i tamalayan, oyuncunun elindeki karttan 2 degerli olan kart buyuk
				CardInfo.bIsWin = true;
				for(int i = 0; i < 5; i++)
				{
					if (TwoValueGreaterCardPower == DeskCards[i]->CardPower)
					{
						CardInfo.CardPower = DeskCards[i]->CardPower;
						CardInfo.CardSymbol = DeskCards[i]->CardSymbol;
						break;
					}
				}
				return CardInfo;
			}
			//Oyuncu Kent yapamadi
			return CardInfo;
		}
		/*else*/ if(PlayerCardsDifferance == 3)
		{
			//Oyuncunun kartlari arasinda 2 tane kart var
			//Elindeki buyuk karttan 1 deger kucuk kart var mi kontrol et
			bool bOneValueSmallerThanBigger = false;
			ACard* PlayersBigCard = Poker_ReturnHigherCard(PlayerCards[0], PlayerCards[1]);
			uint8 PlayersBigCardsPower = PlayersBigCard->CardPower;
			for(int i = 0; i < 5; i++)
			{
				if (PlayersBigCardsPower - 1 == DeskCards[i]->CardPower)
				{
					bOneValueSmallerThanBigger = true;
					break;
				}
			}
			//Elindeki buyuk karttan 2 deger kucuk kart var mi kontrol et
			bool bTwoValueSmallerThanBigger = false;
			for(int i = 0; i < 5; i++)
			{
				if (PlayersBigCardsPower - 2 == DeskCards[i]->CardPower)
				{
					bTwoValueSmallerThanBigger = true;
					break;
				}
			}
			if (!(bOneValueSmallerThanBigger && bTwoValueSmallerThanBigger))
				//Oyuncu elindeki 2 kart ile Kent yapamiyor, kartlari bireysel kontrol et!
			{
				return Poker_Straight_Individual_Checker(PlayerCards, DeskCards);
			}
			//Kod buraya kadar calismis ise oyuncunun 2 kartinin ortasinda 2 kart var
			//Gerekli kosullar kucuk karttan 1 kucuk veya buyuk karttan 1 buyuk kartin ortada olmasi gerekiyor
			//Buyuk karttan 1 buyuk kart var mi kontrol et
			for(int i = 0; i < 5; i++)
			{
				if (PlayersBigCardsPower + 1 == DeskCards[i]->CardPower)
				{
					CardInfo.CardPower = DeskCards[i]->CardPower;
					CardInfo.CardSymbol = DeskCards[i]->CardSymbol;
					CardInfo.bIsWin = true;
					return CardInfo;
				}
			}
			//Kucuk karttan 1 kucuk kart var mi kontrol et
			for(int i = 0; i < 5; i++)
			{
				if (PlayersBigCardsPower - 4 == DeskCards[i]->CardPower)
				{
					CardInfo.CardPower = PlayersBigCardsPower;
					CardInfo.CardSymbol = PlayersBigCard->CardSymbol;
					CardInfo.bIsWin = true;
					return CardInfo;
				}
			}
			//Kent yapamadi
			return CardInfo;
		}
		/*else*/ if(PlayerCardsDifferance == 4)
		{
			//Oyuncunun kartlari arasinda 3 kart var
			uint8 BetweenCardPower = (PlayerCards[0]->CardPower + PlayerCards[1]->CardPower) / 2;
			bool bIsThereTrueBetweenCard = false; //Ortadaki
			bool bIsThereSmallBetweenCard = false; //Kucukten 1 buyuk
			bool bIsThereBigBetweenCard = false; //Buyukten 1 kucuk
			
			for(int i = 0; i < 5; i++)
			{
				if(BetweenCardPower == DeskCards[i]->CardPower) bIsThereTrueBetweenCard = true;
				if(BetweenCardPower - 1 == DeskCards[i]->CardPower) bIsThereSmallBetweenCard = true;
				if(BetweenCardPower + 1 == DeskCards[i]->CardPower) bIsThereBigBetweenCard = true;
			}
			if(!(bIsThereTrueBetweenCard && bIsThereSmallBetweenCard && bIsThereBigBetweenCard))
			{
				//Oyuncunun kartlari arasinda kart yok, individual olarak kontrol et
				return Poker_Straight_Individual_Checker(PlayerCards, DeskCards);
			}
			//Aralarinda uc kart var, buyuk kartin verisini yolla
			ACard* PlayersBigCard = Poker_ReturnHigherCard(PlayerCards[0], PlayerCards[1]);
			CardInfo.CardPower = BetweenCardPower + 2;
			CardInfo.CardSymbol = PlayersBigCard->CardSymbol;
			CardInfo.bIsWin = true;
			return CardInfo;
		}
		else // default //Kodun buraya girmesi imkansiz
		{
			return CardInfo;
		}
	}
	/*else*/ // Player Hand Wasn't valid for straight (or he has same powered cards), check 2 cards separately
	{
		if ((PlayerCards[0]->CardPower - PlayerCards[1]->CardPower) == 0) //Player has same powered card
		{
			ACard* BetterCard = Poker_ReturnHigherCard(PlayerCards[0], PlayerCards[1]);
			return Poker_Straight_Individual(BetterCard, DeskCards);
		}
		/*else*/ //Check 2 Cards Separately
		{
			return Poker_Straight_Individual_Checker(PlayerCards, DeskCards);
		}
	}
}
TArray<class ACard*> APokerTable::Poker_Straight_Array(TArray <class ACard*> PlayerCards, TArray<class ACard*> DeskCards)
{
	TArray<ACard*> StraightCards;
	TArray<ACard*> PlayerAndDeskCards;
	//Bu info mantiken en buyuk karti dondurecek, simdi ondan kucuk kartlari listeye ekle
	FWinnerCardInfo StraightCardInfo = Poker_Straight(PlayerCards,DeskCards);
	
	if(StraightCardInfo.bIsWin)
	{
		PlayerAndDeskCards.Add(PlayerCards[0]);
		PlayerAndDeskCards.Add(PlayerCards[1]);
		for(int i = 0; i < 5; i++) PlayerAndDeskCards.Add(DeskCards[i]);

		for(auto Card : PlayerAndDeskCards)
		{
			if (Card->CardPower == StraightCardInfo.CardPower) StraightCards.Add(Card);
			else if (Card->CardPower -1 == StraightCardInfo.CardPower) StraightCards.Add(Card);
			else if (Card->CardPower -2 == StraightCardInfo.CardPower) StraightCards.Add(Card);
			else if (Card->CardPower -3 == StraightCardInfo.CardPower) StraightCards.Add(Card);
			else if (Card->CardPower -4 == StraightCardInfo.CardPower) StraightCards.Add(Card);
		}
	}
	return StraightCards; //5 ten fazla olabilir bu bu yuzden listeyi donecegiz straight flush icin
}
FWinnerCardInfo APokerTable::Poker_ThreeOfAKind_Individual(class ACard* PlayerCard, TArray<class ACard*> DeskCards)
{
	FWinnerCardInfo CardInfo;
	CardInfo.WinFunctionName = TEXT("Three of a Kind_Individual It Musn't Be Out Of Scope");
	CardInfo.bIsWin = false;

	uint8 CardPairCount = 0;
	for (int i = 0; i < 5; i++)
	{
		if (PlayerCard->CardPower == DeskCards[i]->CardPower)
		{
			CardPairCount++;
		}
	}
	if (CardPairCount >= 2)
	{
		CardInfo.bIsWin = true;
	}
	return CardInfo;
}
FWinnerCardInfo APokerTable::Poker_ThreeOfAKind(TArray <class ACard*> PlayerCards, TArray<class ACard*> DeskCards)
{
	FWinnerCardInfo CardInfo;
	CardInfo.WinFunctionName = TEXT("Three of a Kind");
	CardInfo.bIsWin = false;
	if (PlayerCards[0]->CardPower == PlayerCards[1]->CardPower) //Kartlar ayni, Bir tane daha bulsan yeter
	{
		for (int i = 0; i < 5; i++)
		{
			if (PlayerCards[0]->CardPower == DeskCards[i]->CardPower)
			{
				CardInfo.CardPower = PlayerCards[0]->CardPower;
				CardInfo.CardSymbol = PlayerCards[0]->CardSymbol;
				return CardInfo;
			}
		}
	}
	/*else*/ //Individual Pair Check
	{
		bool bHasFirstCard2MorePair = Poker_ThreeOfAKind_Individual(PlayerCards[0],DeskCards).bIsWin;
		bool bHasSecondCard2MorePair = Poker_ThreeOfAKind_Individual(PlayerCards[1],DeskCards).bIsWin;

		if (bHasFirstCard2MorePair && !bHasSecondCard2MorePair)
		{
			CardInfo.CardPower = PlayerCards[0]->CardPower;
			CardInfo.CardSymbol = PlayerCards[0]->CardSymbol;
			CardInfo.bIsWin = true;
			return CardInfo;
		}
		/*else*/ if (!bHasFirstCard2MorePair && bHasSecondCard2MorePair)
		{
			CardInfo.CardPower = PlayerCards[1]->CardPower;
			CardInfo.CardSymbol = PlayerCards[1]->CardSymbol;
			CardInfo.bIsWin = true;
			return CardInfo;
		}
		/*else*/ if (bHasFirstCard2MorePair && bHasSecondCard2MorePair)
		{
			ACard* PlayersPowerfullCard = Poker_ReturnHigherCard(PlayerCards[0], PlayerCards[1]);
			CardInfo.CardPower = PlayersPowerfullCard->CardPower;
			CardInfo.CardSymbol = PlayersPowerfullCard->CardSymbol;
			CardInfo.bIsWin = true;
			return CardInfo;
		}
		/*else*/
		{
			return CardInfo;
		}
	}
}
FWinnerCardInfo APokerTable::Poker_TwoPair(TArray <class ACard*> PlayerCards, TArray<class ACard*> DeskCards)
{
	FWinnerCardInfo CardInfo;
	CardInfo.WinFunctionName = TEXT("Two Pair");
	CardInfo.bIsWin = false;
	bool bHasFirstCardHasPair = false;
	for (int i = 0; i < 5; i++)
	{
		if (PlayerCards[0]->CardPower == DeskCards[i]->CardPower)
		{
			bHasFirstCardHasPair = true;
			break;
		}
	}
	if (!bHasFirstCardHasPair) return CardInfo;
	for (int i = 0; i < 5; i++)
	{
		if (PlayerCards[1]->CardPower == DeskCards[i]->CardPower)
		{
			CardInfo.bIsWin = true;
			return CardInfo;
		}
	}
	return CardInfo;
}
FWinnerCardInfo APokerTable::Poker_Pair_Individual(ACard* PlayerCard, TArray<class ACard*> DeskCards)
{
	FWinnerCardInfo CardInfo;
	CardInfo.WinFunctionName = TEXT("Pair_Individual It Shouldnt be outside in a scope");
	CardInfo.bIsWin = false;

	CardInfo.CardSymbol = PlayerCard->CardSymbol;
	CardInfo.CardPower = PlayerCard->CardPower;
	for (int i = 0; i < 5; i++)
	{
		if (PlayerCard->CardPower == DeskCards[i]->CardPower)
		{
			CardInfo.bIsWin = true;
			if (DeskCards[i]->CardSymbol > CardInfo.CardSymbol) CardInfo.CardSymbol = DeskCards[i]->CardSymbol;
		}
	}
	return CardInfo;
}
FWinnerCardInfo APokerTable::Poker_Pair(TArray <class ACard*> PlayerCards, TArray<class ACard*> DeskCards)
{
	FWinnerCardInfo CardInfo;
	CardInfo.WinFunctionName = TEXT("Pair");
	CardInfo.bIsWin = false;
	//Player Has Pair On His Hand
	if (PlayerCards[0]->CardPower == PlayerCards[1]->CardPower)
	{
		CardInfo.CardPower = PlayerCards[0]->CardPower;
		CardInfo.CardSymbol = (PlayerCards[0]->CardSymbol > PlayerCards[1]->CardSymbol) ? PlayerCards[0]->CardSymbol : PlayerCards[1]->CardSymbol;
		CardInfo.bIsWin = true;
		return CardInfo;
	}
	//Check Player Has Pair With Desk
	FWinnerCardInfo PlayerCard1Info = Poker_Pair_Individual(PlayerCards[0], DeskCards);
	FWinnerCardInfo PlayerCard2Info = Poker_Pair_Individual(PlayerCards[1], DeskCards);

	if(PlayerCard1Info.bIsWin && !PlayerCard2Info.bIsWin)
	{
		CardInfo.CardPower = PlayerCard1Info.CardPower;
		CardInfo.CardSymbol = PlayerCard1Info.CardSymbol;
		CardInfo.bIsWin = true;
		return CardInfo;
	}
	if(!PlayerCard1Info.bIsWin && PlayerCard2Info.bIsWin)
	{
		CardInfo.CardPower = PlayerCard2Info.CardPower;
		CardInfo.CardSymbol = PlayerCard2Info.CardSymbol;
		CardInfo.bIsWin = true;
		return CardInfo;
	}
	if(PlayerCard1Info.bIsWin && PlayerCard2Info.bIsWin)
	{
		CardInfo.CardPower = PlayerCard1Info.CardPower;
		CardInfo.CardSymbol = PlayerCard1Info.CardSymbol;
		if(PlayerCard1Info.CardPower > PlayerCard2Info.CardPower)
		{
			CardInfo.CardPower = PlayerCard2Info.CardPower;
			CardInfo.CardSymbol = PlayerCard2Info.CardSymbol;
		}
		CardInfo.bIsWin = true;
		return CardInfo;
	}
	return CardInfo;
}
FWinnerCardInfo APokerTable::Poker_HighCard(TArray <class ACard*> PlayerCards)
{
	FWinnerCardInfo CardInfo;
	CardInfo.WinFunctionName = TEXT("High Card");
	CardInfo.CardPower=PlayerCards[0]->CardPower;
	CardInfo.CardSymbol=PlayerCards[0]->CardSymbol;
	if (PlayerCards[1]->CardPower > CardInfo.CardPower)
	{
		CardInfo.CardPower=PlayerCards[1]->CardPower;
		CardInfo.CardSymbol=PlayerCards[1]->CardSymbol;
	}
	else if (PlayerCards[1]->CardPower == CardInfo.CardPower && PlayerCards[1]->CardSymbol > CardInfo.CardSymbol)
	{
		CardInfo.CardPower=PlayerCards[1]->CardPower;
		CardInfo.CardSymbol=PlayerCards[1]->CardSymbol;
	}
	return CardInfo;
}

ACard* APokerTable::Poker_ReturnHigherCard(class ACard* Card1, class ACard* Card2)
{
	if (Card1->CardPower > Card2->CardPower) return Card1;
	if (Card2->CardPower > Card1->CardPower) return Card2;
	if (Card1->CardSymbol > Card2->CardSymbol) return Card1;
	return Card2;
}