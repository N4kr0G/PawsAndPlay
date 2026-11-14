#include "PawsGameMode.h"
#include "Kibble.h"
#include "PawsPlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "UObject/ConstructorHelpers.h"

APawsGameMode::APawsGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PawnBP(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PawnBP.Succeeded())
	{
		DefaultPawnClass = PawnBP.Class;
	}
	PlayerControllerClass = APawsPlayerController::StaticClass();
}

void APawsGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* Start = UGameplayStatics::GetActorOfClass(this, APlayerStart::StaticClass()))
	{
		SpawnCenter = Start->GetActorLocation();
	}
	for (int32 i = 0; i < KibbleCount; ++i)
	{
		SpawnKibble();
	}

	Message = TEXT("Mama tanelerini topla, puan kazan!");
	RefreshStatus();
	FTimerHandle StatusTimer;
	GetWorldTimerManager().SetTimer(StatusTimer, this, &APawsGameMode::RefreshStatus, 10.f, true);
}

void APawsGameMode::SpawnKibble()
{
	// Drop a ray from above a random point; spawn where it hits the ground.
	for (int32 Try = 0; Try < 10; ++Try)
	{
		const FVector2D Offset = FMath::RandPointInCircle(SpawnRadius);
		const FVector Top = SpawnCenter + FVector(Offset.X, Offset.Y, 1000.f);
		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(UGameplayStatics::GetPlayerPawn(this, 0));
		if (GetWorld()->LineTraceSingleByChannel(Hit, Top, Top - FVector(0, 0, 3000.f), ECC_Visibility, Params))
		{
			GetWorld()->SpawnActor<AKibble>(Hit.ImpactPoint + FVector(0, 0, 50.f), FRotator::ZeroRotator);
			return;
		}
	}
}

bool APawsGameMode::IsAdPlaying() const
{
	return GetWorldTimerManager().IsTimerActive(AdTimer);
}

float APawsGameMode::AdSecondsLeft() const
{
	return GetWorldTimerManager().GetTimerRemaining(AdTimer);
}

void APawsGameMode::WatchAd()
{
	if (IsAdPlaying())
	{
		return;
	}
	const double Now = GetWorld()->GetTimeSeconds();
	if (Now < NextAdTime)
	{
		Message = FString::Printf(TEXT("Yeni reklam %d sn sonra hazır."), FMath::CeilToInt(NextAdTime - Now));
		return;
	}
	// shortcut: simulated ad (timer + overlay), swap for a real ad SDK (AdMob/Unity Ads) on mobile builds.
	GetWorldTimerManager().SetTimer(AdTimer, this, &APawsGameMode::OnAdFinished, AdSeconds, false);
}

void APawsGameMode::OnAdFinished()
{
	Points += AdReward;
	NextAdTime = GetWorld()->GetTimeSeconds() + AdCooldown;
	Message = FString::Printf(TEXT("Reklam için teşekkürler! +%d puan"), AdReward);
}

void APawsGameMode::RequestFeed()
{
	if (bFeedInFlight)
	{
		return;
	}
	if (Points < PointsPerPortion)
	{
		Message = FString::Printf(TEXT("Bir porsiyon için %d puan daha gerekli."), PointsPerPortion - Points);
		return;
	}

	if (FeederUrl.IsEmpty())
	{
		Points -= PointsPerPortion;
		++PortionsSent;
		Message = TEXT("[SİMÜLASYON] Mama döküldü! Afiyet olsun.");
		return;
	}

	bFeedInFlight = true;
	Message = TEXT("Mama kabına bağlanılıyor...");

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Req = FHttpModule::Get().CreateRequest();
	Req->SetURL(FeederUrl + TEXT("/feed"));
	Req->SetVerb(TEXT("POST"));
	Req->SetHeader(TEXT("X-Feeder-Key"), FeederKey);
	Req->SetTimeout(8.f);
	Req->OnProcessRequestComplete().BindWeakLambda(this, [this](FHttpRequestPtr, FHttpResponsePtr Resp, bool bOk)
	{
		bFeedInFlight = false;
		const int32 Code = (bOk && Resp.IsValid()) ? Resp->GetResponseCode() : 0;
		bFeederOnline = Code != 0;
		// Points are only spent once the device confirms; a failed request costs the player nothing.
		if (Code == 200)
		{
			Points -= PointsPerPortion;
			++PortionsSent;
			Message = TEXT("Mama döküldü! Afiyet olsun.");
		}
		else if (Code == 429)
		{
			Message = TEXT("Mama kabı dinleniyor (bekleme süresi / günlük limit). Puanın korundu.");
		}
		else if (Code == 401)
		{
			Message = TEXT("Cihaz anahtarı hatalı. Puanın korundu.");
		}
		else
		{
			Message = TEXT("Mama kabına ulaşılamadı. Puanın korundu.");
		}
		RefreshStatus();
	});
	Req->ProcessRequest();
}

void APawsGameMode::RefreshStatus()
{
	if (FeederUrl.IsEmpty())
	{
		bFeederOnline = false;
		FeederInfo = TEXT("Cihaz: SİMÜLASYON modu");
		return;
	}

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Req = FHttpModule::Get().CreateRequest();
	Req->SetURL(FeederUrl + TEXT("/status"));
	Req->SetVerb(TEXT("GET"));
	Req->SetTimeout(5.f);
	Req->OnProcessRequestComplete().BindWeakLambda(this, [this](FHttpRequestPtr, FHttpResponsePtr Resp, bool bOk)
	{
		TSharedPtr<FJsonObject> Json;
		if (!bOk || !Resp.IsValid() || Resp->GetResponseCode() != 200
			|| !FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Resp->GetContentAsString()), Json) || !Json.IsValid())
		{
			bFeederOnline = false;
			FeederInfo = TEXT("Cihaz: ÇEVRİMDIŞI");
			return;
		}
		bFeederOnline = true;
		FeederInfo = FString::Printf(TEXT("Cihaz: ÇEVRİMİÇİ\nBugün %d / %d porsiyon\nBekleme: %d sn"),
			Json->GetIntegerField(TEXT("portionsToday")),
			Json->GetIntegerField(TEXT("maxPerDay")),
			Json->GetIntegerField(TEXT("cooldownLeft")));
	});
	Req->ProcessRequest();
}
