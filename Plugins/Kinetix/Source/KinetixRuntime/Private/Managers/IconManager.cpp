// Copyright Kinetix. All Rights Reserved.


#include "Managers/IconManager.h"

#include "EmoteManager.h"
#include "HttpModule.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "KinetixRuntimeModule.h"
#include "Blueprint/AsyncTaskDownloadImage.h"
#include "Engine/Texture2DDynamic.h"
#include "Interfaces/IHttpResponse.h"
#include "Loaders/KinetixIconDownloader.h"

// Otherwise there is no exports of static symbols
TUniquePtr<FIconManager> FIconManager::Instance(nullptr);

FIconManager::FIconManager()
{
}

FIconManager::~FIconManager()
{
	Instance = nullptr;
}

FIconManager& FIconManager::Get()
{
	if (!Instance.IsValid())
	{
		Instance = MakeUnique<FIconManager>();
	}
	return *Instance;
}

struct FDownloadIconFunctor
{
	UTexture2DDynamic* Result;
	FKinetixEmote* Emote;

	bool bTaskFinished = false;

	FDownloadIconFunctor(FKinetixEmote* InEmote, UTexture2DDynamic** OutResult)
		: Result(*OutResult), Emote(InEmote)
	{
	}

	void operator ()()
	{
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

		HttpRequest->OnProcessRequestComplete().BindLambda(
			[this](FHttpRequestPtr OriginHttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
			{
				if (bSucceeded && HttpResponse.IsValid() && HttpResponse->GetContentLength() > 0)
				{
					IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<
						IImageWrapperModule>(FName("ImageWrapper"));
					TSharedPtr<IImageWrapper> ImageWrappers[3] =
					{
						ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG),
						ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG),
						ImageWrapperModule.CreateImageWrapper(EImageFormat::BMP),
					};

					for (auto ImageWrapper : ImageWrappers)
					{
						if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(
							HttpResponse->GetContent().GetData(), HttpResponse->GetContentLength()))
						{
							TArray64<uint8> RawData;
							const ERGBFormat InFormat = ERGBFormat::BGRA;
							if (ImageWrapper->GetRaw(InFormat, 8, RawData))
							{
								if ((Result = UTexture2DDynamic::Create(
									ImageWrapper->GetWidth(), ImageWrapper->GetHeight())))
								{
									Result->SRGB = true;
									Result->UpdateResource();
									FTexture2DDynamicResource* TextureResource = static_cast<
										FTexture2DDynamicResource*>(Result->GetResource());
									if (TextureResource)
									{
										ENQUEUE_RENDER_COMMAND(FWriteRawDataToTexture)(
											[TextureResource, RawData = MoveTemp(RawData)](
											FRHICommandListImmediate& RHICmdList)
											{
												TextureResource->WriteRawToTexture_RenderThread(RawData);
											});
									}
									bTaskFinished = true;
									return;
								}
							}
						}
					}
				}
			});

		HttpRequest->SetURL(Emote->GetAnimationMetadata().IconURL.Map);
		HttpRequest->SetVerb(TEXT("GET"));
		HttpRequest->ProcessRequest();
	}
};

UTexture2DDynamic* FIconManager::GetIcon(const FAnimationID& ID, const FOnIconAvailable& OnIconAvailableDelegate)
{
	// return UE::Tasks::Launch(UE_SOURCE_LOCATION, [this, ID]()
	// {

	if (IconTexturesByID.Contains(ID))
	{
		UTexture2DDynamic* Icon = IconTexturesByID[ID];
		if (!IsValid(Icon))
			return nullptr;

		OnIconAvailableDelegate.ExecuteIfBound(Icon);
		return Icon;
	}

	UKinetixIconDownloader* IconDownloader = NewObject<UKinetixIconDownloader>();
	if (!IsValid(IconDownloader))
		return nullptr; // Should never happen though

	IconDownloader->InjectOnIconAvailableDelegate(OnIconAvailableDelegate);
		IconDownloader->DownloadIcon(FEmoteManager::Get().GetEmote(ID));
	return nullptr;

	// UTexture2DDynamic* Result = nullptr;
	// UE::Tasks::FTask DownloadTask = UE::Tasks::Launch(UE_SOURCE_LOCATION, FDownloadIconFunctor{Emote, &Result});

	// UE::Tasks::TTask DownloadTask = UE::Tasks::Launch(UE_SOURCE_LOCATION, [&Emote, &DownloadCompleteEvent]
	// {
	//
	// });

	// UE::Tasks::AddNested(DownloadTask);
	// DownloadTask.BusyWait();
	// return Result;
	// });
}

void FIconManager::AddIcon(const FAnimationID& InID, UTexture2DDynamic* Texture)
{
	if (IconTexturesByID.Contains(InID) || !IsValid(Texture))
		return;
	Texture->AddToRoot();
	IconTexturesByID.Add(InID, Texture);
}
