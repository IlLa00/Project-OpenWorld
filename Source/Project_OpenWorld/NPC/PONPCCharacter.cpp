#include "PONPCCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "../Claude/POClaudeAPIManager.h"
#include "../Weather/POWeatherSystemManager.h"
#include "../Weather/WeatherTypes.h"

APONPCCharacter::APONPCCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APONPCCharacter::BeginPlay()
{
	Super::BeginPlay();

	AActor* FoundClaude = UGameplayStatics::GetActorOfClass(GetWorld(), APOClaudeAPIManager::StaticClass());
	ClaudeManager = Cast<APOClaudeAPIManager>(FoundClaude);
	if (!ClaudeManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("[NPCCharacter] ClaudeAPIManager를 레벨에서 찾을 수 없습니다. 대화 기능이 비활성화됩니다."));
	}

	AActor* FoundWeather = UGameplayStatics::GetActorOfClass(GetWorld(), APOWeatherSystemManager::StaticClass());
	WeatherManager = Cast<APOWeatherSystemManager>(FoundWeather);
	if (!WeatherManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("[NPCCharacter] WeatherSystemManager를 레벨에서 찾을 수 없습니다. 날씨 반응이 비활성화됩니다."));
	}

	GetWorldTimerManager().SetTimer(
		WeatherRefreshTimerHandle,
		this,
		&APONPCCharacter::RefreshWeatherState,
		WeatherRefreshInterval,
		true  // 반복
	);

	RefreshWeatherState();
}

void APONPCCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void APONPCCharacter::RefreshWeatherState()
{
	if (!WeatherManager)
	{
		return;
	}

	static const TMap<EWeatherType, FString> WeatherNames = {
		{EWeatherType::Clear,  TEXT("맑음")},
		{EWeatherType::Cloudy, TEXT("흐림")},
		{EWeatherType::Rainy,  TEXT("비")},
		{EWeatherType::Snowy,  TEXT("눈")},
		{EWeatherType::Foggy,  TEXT("안개")},
		{EWeatherType::Stormy, TEXT("폭풍")}
	};

	const EWeatherType WT = WeatherManager->GetCurrentWeather();
	CurrentWeatherName = WeatherNames.Contains(WT) ? WeatherNames[WT] : TEXT("맑음");
}


void APONPCCharacter::StartConversation(const FString& PlayerMessage)
{
	if (TalkState == ENPCTalkState::WaitingForAPI || TalkState == ENPCTalkState::Cooldown)
	{
		UE_LOG(LogTemp, Warning, TEXT("[NPCCharacter] 대화 중 또는 쿨다운 중 - 요청 무시"));
		return;
	}

	if (!ClaudeManager)
	{
		UE_LOG(LogTemp, Error, TEXT("[NPCCharacter] ClaudeAPIManager 없음 - 대화 불가"));
		return;
	}

	// 상태 전환: WaitingForAPI
	TalkState = ENPCTalkState::WaitingForAPI;

	// UI에 "생각 중..." 표시 (bIsThinking = true)
	OnDialogueUpdated.Broadcast(TEXT(""), true);

	UE_LOG(LogTemp, Log, TEXT("[NPCCharacter] 대화 시작: \"%s\" (날씨: %s)"), *PlayerMessage, *CurrentWeatherName);

	// Claude API 호출 (날씨/시간 컨텍스트 자동 수집)
	FOnClaudeResponse Callback;
	Callback.BindUFunction(this, FName("OnClaudeResponseReceived"));

	ClaudeManager->SendMessageWithAutoContext(
		PlayerMessage,
		NPCName,
		NPCPersonality,
		Callback
	);
}

void APONPCCharacter::OnClaudeResponseReceived(bool bSuccess, const FString& ResponseText)
{
	if (bSuccess)
	{
		LastNPCResponse = ResponseText;

		// 대화 이력 추가
		FNPCDialogueEntry Entry;
		Entry.NPCResponse    = ResponseText;
		Entry.WeatherContext = CurrentWeatherName;

		DialogueHistory.Add(Entry);

		if (DialogueHistory.Num() > MaxDialogueHistory)
		{
			DialogueHistory.RemoveAt(0);
		}

		// 상태 전환: Talking
		TalkState = ENPCTalkState::Talking;

		// UI에 응답 전달 (bIsThinking = false)
		OnDialogueUpdated.Broadcast(ResponseText, false);

		UE_LOG(LogTemp, Log, TEXT("[NPCCharacter] 응답 수신: %s"), *ResponseText);
	}
	else
	{
		// 오류 시 Idle로 복귀
		TalkState = ENPCTalkState::Idle;
		OnDialogueUpdated.Broadcast(ResponseText, false);

		UE_LOG(LogTemp, Error, TEXT("[NPCCharacter] Claude 응답 실패: %s"), *ResponseText);
	}
}

void APONPCCharacter::EndConversation()
{
	if (TalkState == ENPCTalkState::Idle)
	{
		return;
	}

	// 쿨다운 상태로 전환
	TalkState = ENPCTalkState::Cooldown;

	// TalkCooldown 초 후 OnCooldownFinished() 호출
	GetWorldTimerManager().SetTimer(
		CooldownTimerHandle,
		this,
		&APONPCCharacter::OnCooldownFinished,
		TalkCooldown,
		false  // 반복 없음
	);

	UE_LOG(LogTemp, Log, TEXT("[NPCCharacter] 대화 종료 - %.1f초 쿨다운 시작"), TalkCooldown);
}

void APONPCCharacter::OnCooldownFinished()
{
	TalkState = ENPCTalkState::Idle;
	UE_LOG(LogTemp, Log, TEXT("[NPCCharacter] 쿨다운 완료 - Idle 복귀"));
}

bool APONPCCharacter::IsInConversation() const
{
	return TalkState == ENPCTalkState::WaitingForAPI
		|| TalkState == ENPCTalkState::Talking;
}
