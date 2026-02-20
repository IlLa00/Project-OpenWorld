#include "POClaudeAPIManager.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "../Weather/POWeatherSystemManager.h"
#include "../Weather/WeatherTypes.h"
#include "../TimeOfDay/POTimeOfDayManager.h"
#include "../RVT/PORVTManager.h"

APOClaudeAPIManager::APOClaudeAPIManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APOClaudeAPIManager::BeginPlay()
{
	Super::BeginPlay();
	LoadAPIKeyFromConfig();
}

void APOClaudeAPIManager::LoadAPIKeyFromConfig()
{
	if (GConfig)
	{
		GConfig->GetString(TEXT("Claude"), TEXT("APIKey"), APIKey, GGameIni);

		if (APIKey.IsEmpty())
		{
			UE_LOG(LogTemp, Warning,
				TEXT("[ClaudeAPIManager] API 키가 DefaultGame.ini [Claude] APIKey에 설정되지 않았습니다."));
		}
		else
		{
			UE_LOG(LogTemp, Log,
				TEXT("[ClaudeAPIManager] API 키 로딩 완료 (길이: %d)"), APIKey.Len());
		}
	}
}

void APOClaudeAPIManager::SendMessageToClaude(const FClaudeRequestContext& Context,const FOnClaudeResponse& ResponseCallback)
{
	if (bRequestInProgress)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ClaudeAPIManager] 이미 요청 처리 중입니다."));
		ResponseCallback.ExecuteIfBound(false, TEXT("잠시 후 다시 말씀해 주세요."));
		return;
	}

	if (APIKey.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("[ClaudeAPIManager] API 키가 설정되지 않았습니다."));
		ResponseCallback.ExecuteIfBound(false, TEXT("(API 키 오류)"));
		return;
	}

	bRequestInProgress = true;

	FHttpModule& HttpModule = FHttpModule::Get();
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = HttpModule.CreateRequest();

	Request->SetURL(TEXT("https://api.anthropic.com/v1/messages"));
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"),       TEXT("application/json"));
	Request->SetHeader(TEXT("x-api-key"),          APIKey);
	Request->SetHeader(TEXT("anthropic-version"),  TEXT("2023-06-01"));
	Request->SetContentAsString(BuildRequestJson(Context));

	Request->OnProcessRequestComplete().BindLambda(
		[this, ResponseCallback](
			FHttpRequestPtr Req,
			FHttpResponsePtr Res,
			bool bConnectedSuccessfully)
		{
			bRequestInProgress = false;

			if (!bConnectedSuccessfully || !Res.IsValid())
			{
				UE_LOG(LogTemp, Error, TEXT("[ClaudeAPIManager] HTTP 연결 실패"));
				ResponseCallback.ExecuteIfBound(false, TEXT("(네트워크 오류)"));
				return;
			}

			const int32 StatusCode = Res->GetResponseCode();
			if (StatusCode != 200)
			{
				UE_LOG(LogTemp, Error,
					TEXT("[ClaudeAPIManager] API 오류 %d: %s"),
					StatusCode, *Res->GetContentAsString());
				ResponseCallback.ExecuteIfBound(false,
					FString::Printf(TEXT("(API 오류: %d)"), StatusCode));
				return;
			}

			const FString ParsedText = ParseClaudeResponse(Res->GetContentAsString());
			UE_LOG(LogTemp, Log, TEXT("[ClaudeAPIManager] 응답 수신: %s"), *ParsedText);
			ResponseCallback.ExecuteIfBound(true, ParsedText);
		});

	Request->ProcessRequest();
	UE_LOG(LogTemp, Log, TEXT("[ClaudeAPIManager] 요청 전송 중... (날씨: %s, 시간: %.1fh)"),
		*Context.WeatherType, Context.TimeOfDay);
}

void APOClaudeAPIManager::SendMessageWithAutoContext(
	const FString& PlayerMessage,
	const FString& NPCName,
	const FString& NPCPersonality,
	const FOnClaudeResponse& ResponseCallback)
{
	FClaudeRequestContext Ctx;
	Ctx.PlayerMessage   = PlayerMessage;
	Ctx.NPCName         = NPCName;
	Ctx.NPCPersonality  = NPCPersonality;

	// WeatherManager에서 현재 날씨 수집
	APOWeatherSystemManager* WM = FindWeatherManager();
	if (WM)
	{
		static const TMap<EWeatherType, FString> WeatherNames = {
			{EWeatherType::Clear,  TEXT("맑음")},
			{EWeatherType::Cloudy, TEXT("흐림")},
			{EWeatherType::Rainy,  TEXT("비")},
			{EWeatherType::Snowy,  TEXT("눈")},
			{EWeatherType::Foggy,  TEXT("안개")},
			{EWeatherType::Stormy, TEXT("폭풍")}
		};
		const EWeatherType WT = WM->GetCurrentWeather();
		Ctx.WeatherType = WeatherNames.Contains(WT) ? WeatherNames[WT] : TEXT("맑음");
	}

	// RVTManager에서 날씨 수치 파라미터 수집
	APORVTManager* RVT = FindRVTManager();
	if (RVT)
	{
		Ctx.RainIntensity = RVT->RainIntensity;
		Ctx.SnowCoverage  = RVT->SnowCoverage;
		Ctx.WindStrength  = RVT->WindStrength;
	}

	// TimeOfDayManager에서 현재 시간 수집
	APOTimeOfDayManager* TM = FindTimeOfDayManager();
	if (TM)
	{
		Ctx.TimeOfDay = TM->GetCurrentTime();
	}

	SendMessageToClaude(Ctx, ResponseCallback);
}

FString APOClaudeAPIManager::BuildSystemPrompt(const FClaudeRequestContext& Context) const
{
	// 시간대 설명
	const FString TimeDesc = TimeOfDayToKorean(Context.TimeOfDay);

	// 날씨 수치 → 상세 묘사 문자열
	FString WeatherDetail;
	if (Context.RainIntensity > 0.5f)
		WeatherDetail += TEXT("비가 세차게 내리고 있습니다. ");
	else if (Context.RainIntensity > 0.1f)
		WeatherDetail += TEXT("가랑비가 내리고 있습니다. ");

	if (Context.SnowCoverage > 0.5f)
		WeatherDetail += TEXT("눈이 많이 쌓여있습니다. ");
	else if (Context.SnowCoverage > 0.1f)
		WeatherDetail += TEXT("눈이 살짝 쌓여있습니다. ");

	if (Context.WindStrength > 0.7f)
		WeatherDetail += TEXT("바람이 매우 강하게 붑니다. ");
	else if (Context.WindStrength > 0.3f)
		WeatherDetail += TEXT("바람이 제법 붑니다. ");

	if (WeatherDetail.IsEmpty())
		WeatherDetail = TEXT("특별한 이상 날씨는 없습니다.");

	return FString::Printf(
		TEXT(
		"당신은 '%s'라는 이름의 %s입니다.\n"
		"\n"
		"## 현재 환경 정보\n"
		"- 날씨: %s\n"
		"- 시간: %.1f시 (%s)\n"
		"- 날씨 상세: %s\n"
		"\n"
		"## 대화 규칙\n"
		"1. 반드시 한국어로만 답하세요.\n"
		"2. 현재 날씨와 시간을 반드시 대화에 자연스럽게 녹여내세요.\n"
		"   - 비/폭풍 날씨: 불편함, 처마 밑으로 피하고 싶다는 표현 포함\n"
		"   - 눈 날씨: 추위, 손 시림, 발이 미끄럽다는 표현 포함\n"
		"   - 안개 날씨: 앞이 안 보인다, 방향 잃을 것 같다는 표현 포함\n"
		"   - 맑은 날씨: 기분 좋음, 날씨 칭찬 포함\n"
		"   - 야간(18시~06시): 어둠, 피곤함, 집에 들어가고 싶다는 표현 포함\n"
		"3. 답변은 2~4문장으로 간결하게 하세요.\n"
		"4. 마을 주민답게 소박하고 친근하게 말하세요.\n"
		"5. 게임 캐릭터라는 사실을 절대 언급하지 마세요.\n"
		),
		*Context.NPCName,
		*Context.NPCPersonality,
		*Context.WeatherType,
		Context.TimeOfDay,
		*TimeDesc,
		*WeatherDetail
	);
}

FString APOClaudeAPIManager::BuildRequestJson(const FClaudeRequestContext& Context) const
{
	TSharedRef<FJsonObject> Root = MakeShared<FJsonObject>();
	Root->SetStringField(TEXT("model"),      ModelID);
	Root->SetNumberField(TEXT("max_tokens"), MaxTokens);
	Root->SetStringField(TEXT("system"),     BuildSystemPrompt(Context));

	TArray<TSharedPtr<FJsonValue>> Messages;
	TSharedRef<FJsonObject> UserMsg = MakeShared<FJsonObject>();
	UserMsg->SetStringField(TEXT("role"),    TEXT("user"));
	UserMsg->SetStringField(TEXT("content"), Context.PlayerMessage);
	Messages.Add(MakeShared<FJsonValueObject>(UserMsg));
	Root->SetArrayField(TEXT("messages"), Messages);

	FString Output;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Output);
	FJsonSerializer::Serialize(Root, Writer);
	return Output;
}

FString APOClaudeAPIManager::ParseClaudeResponse(const FString& ResponseBody) const
{
	TSharedPtr<FJsonObject> JsonObj;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseBody);

	if (!FJsonSerializer::Deserialize(Reader, JsonObj) || !JsonObj.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[ClaudeAPIManager] JSON 파싱 실패"));
		return TEXT("(응답을 읽을 수 없습니다.)");
	}

	const TArray<TSharedPtr<FJsonValue>>* ContentArray = nullptr;
	if (!JsonObj->TryGetArrayField(TEXT("content"), ContentArray) || !ContentArray)
	{
		UE_LOG(LogTemp, Error, TEXT("[ClaudeAPIManager] content 필드 없음"));
		return TEXT("(응답 형식 오류)");
	}

	for (const TSharedPtr<FJsonValue>& Item : *ContentArray)
	{
		TSharedPtr<FJsonObject> ItemObj = Item->AsObject();
		if (!ItemObj.IsValid()) continue;

		FString Type;
		if (ItemObj->TryGetStringField(TEXT("type"), Type) && Type == TEXT("text"))
		{
			FString Text;
			ItemObj->TryGetStringField(TEXT("text"), Text);
			return Text;
		}
	}

	return TEXT("(빈 응답)");
}

FString APOClaudeAPIManager::TimeOfDayToKorean(float TimeOfDay) const
{
	if (TimeOfDay >= 5.0f && TimeOfDay < 9.0f)   return TEXT("이른 아침");
	if (TimeOfDay >= 9.0f && TimeOfDay < 12.0f)  return TEXT("오전");
	if (TimeOfDay >= 12.0f && TimeOfDay < 14.0f) return TEXT("정오");
	if (TimeOfDay >= 14.0f && TimeOfDay < 18.0f) return TEXT("오후");
	if (TimeOfDay >= 18.0f && TimeOfDay < 21.0f) return TEXT("저녁");
	if (TimeOfDay >= 21.0f && TimeOfDay < 24.0f) return TEXT("밤");
	return TEXT("새벽");
}

APOWeatherSystemManager* APOClaudeAPIManager::FindWeatherManager() const
{
	AActor* Found = UGameplayStatics::GetActorOfClass(GetWorld(), APOWeatherSystemManager::StaticClass());
	return Cast<APOWeatherSystemManager>(Found);
}

APOTimeOfDayManager* APOClaudeAPIManager::FindTimeOfDayManager() const
{
	AActor* Found = UGameplayStatics::GetActorOfClass(GetWorld(), APOTimeOfDayManager::StaticClass());
	return Cast<APOTimeOfDayManager>(Found);
}

APORVTManager* APOClaudeAPIManager::FindRVTManager() const
{
	AActor* Found = UGameplayStatics::GetActorOfClass(GetWorld(), APORVTManager::StaticClass());
	return Cast<APORVTManager>(Found);
}
