// 2024'te R. Berk Sozgen tarafindan gelistirilmeye baslanmistir.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PokerTable.generated.h"

USTRUCT(BlueprintType)
struct FWinnerCardInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	bool bIsWin = false;
	UPROPERTY()
	uint8 CardPower = 0;
	UPROPERTY()
	uint8 CardSymbol = 0;
	UPROPERTY()
	uint8 PlayerIndex = 0;
	UPROPERTY()
	FString WinFunctionName = TEXT("Error!");
};

UCLASS()
class BERKUSHSPLAYGROUND_API APokerTable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APokerTable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	public:
	TSubclassOf<class AActor> CardClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* DeckSpawnPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* DeskCard1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* DeskCard2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* DeskCard3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* DeskCard4;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* DeskCard5;
	
	
	void PokerGame(TArray<class ACasinoPlayer*> Players, TArray<class AActor*> Cards) {}; //Bu burada mi olmali cozemedim
	
	UFUNCTION(Server, Reliable)
	void Server_PokerInitilizaer(FTransform Transform);

	UFUNCTION(NetMulticast, Reliable)
	void MultiCast_PokerInitilizaer(FTransform Transform);

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	TArray<class ACard*> PokerCards;


private:

	AActor* Poker_WhoWinner(TArray<class ACasinoPlayer*> Players, TArray<class ACard*> DeskCards);

	static FWinnerCardInfo Poker_RoyalFlush(TArray <class ACard*> PlayerCards, TArray<class ACard*> DeskCards);
	static FWinnerCardInfo Poker_StraightFlush(TArray <class ACard*> PlayerCards, TArray<class ACard*> DeskCards);
	static FWinnerCardInfo Poker_Quad(TArray <class ACard*> PlayerCards, TArray<class ACard*> DeskCards);
	static FWinnerCardInfo Poker_FullHouse(TArray <class ACard*> PlayerCards, TArray<class ACard*> DeskCards);
	static FWinnerCardInfo Poker_Flush(TArray <class ACard*> PlayerCards, TArray<class ACard*> DeskCards);
	static FWinnerCardInfo Poker_Straight_Individual_Checker(TArray <class ACard*> PlayerCards, TArray<class ACard*> DeskCards);
	static FWinnerCardInfo Poker_Straight_Individual(class ACard* PlayerCard, TArray<class ACard*> DeskCards);
	static FWinnerCardInfo Poker_Straight(TArray <class ACard*> PlayerCards, TArray<class ACard*> DeskCards);
	static TArray<class ACard*> Poker_Straight_Array(TArray <class ACard*> PlayerCards, TArray<class ACard*> DeskCards);
	static FWinnerCardInfo Poker_ThreeOfAKind_Individual(class ACard* PlayerCard, TArray<class ACard*> DeskCards);
	static FWinnerCardInfo Poker_ThreeOfAKind(TArray <class ACard*> PlayerCards, TArray<class ACard*> DeskCards);
	static FWinnerCardInfo Poker_TwoPair(TArray <class ACard*> PlayerCards, TArray<class ACard*> DeskCards);
	static FWinnerCardInfo Poker_Pair_Individual(ACard* PlayerCard, TArray<class ACard*> DeskCards);
	static FWinnerCardInfo Poker_Pair(TArray <class ACard*> PlayerCards, TArray<class ACard*> DeskCards);
	static FWinnerCardInfo Poker_HighCard(TArray <class ACard*> PlayerCards);
	static class ACard* Poker_ReturnHigherCard(class ACard* Card1, class ACard* Card2);

};
