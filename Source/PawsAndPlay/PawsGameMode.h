#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PawsGameMode.generated.h"

// Core loop: collect kibble / watch ad -> earn points -> spend points to dispense a real portion on the ESP32 feeder.
UCLASS(Config=Game)
class APawsGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APawsGameMode();
	virtual void BeginPlay() override;

	void AddPoints(int32 N) { Points += N; }
	void SpawnKibble();
	void WatchAd();
	void RequestFeed();
	void RefreshStatus();

	bool IsAdPlaying() const;
	float AdSecondsLeft() const;

	// Empty FeederUrl = simulation mode (no hardware needed for demos).
	UPROPERTY(Config) FString FeederUrl;
	UPROPERTY(Config) FString FeederKey;
	UPROPERTY(Config) int32 PointsPerPortion = 10;
	UPROPERTY(Config) int32 AdReward = 5;
	UPROPERTY(Config) float AdSeconds = 5.f;
	UPROPERTY(Config) float AdCooldown = 30.f;
	UPROPERTY(Config) int32 KibbleCount = 15;
	UPROPERTY(Config) float SpawnRadius = 1500.f;

	int32 Points = 0;
	int32 PortionsSent = 0;
	bool bFeedInFlight = false;
	bool bFeederOnline = false;
	FString Message;     // last action feedback
	FString FeederInfo;  // device state from GET /status

private:
	void OnAdFinished();
	FVector SpawnCenter = FVector::ZeroVector;
	FTimerHandle AdTimer;
	double NextAdTime = 0.0;
};
