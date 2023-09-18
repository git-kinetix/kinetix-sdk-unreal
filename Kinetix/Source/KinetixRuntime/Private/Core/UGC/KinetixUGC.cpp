// Copyright Kinetix. All Rights Reserved.

#include "Core/UGC/KinetixUGC.h"

#include "ImageUtils.h"
#include "Core/Account/KinetixAccount.h"
#include "Managers/UGCManager.h"
#include "Utils/QrCodeGenerator.h"

DEFINE_LOG_CATEGORY(LogKinetixUGC);

UKinetixUGC::UKinetixUGC()
{}

UKinetixUGC::UKinetixUGC(FVTableHelper& Helper)
{}

UKinetixUGC::~UKinetixUGC()
{}

void UKinetixUGC::Initialize_Implementation(const FKinetixCoreConfiguration& CoreConfiguration, bool& bResult)
{
	IKinetixSubcoreInterface::Initialize_Implementation(CoreConfiguration, bResult);

	bResult = true;
}

bool UKinetixUGC::IsUGCAvailable() const
{
	return FUGCManager::Get().IsUGCAvailable();
}

void UKinetixUGC::StartPollingForUGC() const
{
	FUGCManager::Get().StartPollingUGC();
}

void UKinetixUGC::StartPollingForNewUGCToken() const
{
	FUGCManager::Get().StartPollingForNewUGCToken();
}

void UKinetixUGC::GetUGCUrl(const FOnUGCUrlFetched& InUrlFetchedCallback)
{
	UrlFetchedCallback = InUrlFetchedCallback;
	FUGCManager::Get().GetUgcUrl(
		TDelegate<void(FString)>::CreateUObject(this, &UKinetixUGC::OnUrlFetched));
}

void UKinetixUGC::OnUrlFetched(FString String)
{
	UrlFetchedCallback.ExecuteIfBound(String);
}


UTexture2D* UKinetixUGC::GetQRCode(UObject* WorldContextObject, const FString& QrCodeContent)
{
	Kinetix::qrcodegen::QrCode::Ecc ErrorCorrectionLevel = Kinetix::qrcodegen::QrCode::Ecc::LOW;

	Kinetix::qrcodegen::QrCode Qr =
		Kinetix::qrcodegen::QrCode::encodeText(
			TCHAR_TO_UTF8(*QrCodeContent), ErrorCorrectionLevel);

	uint8 Size = Qr.getSize();
	TArray<FColor> Pixels;
	Pixels.SetNumZeroed(Size*Size);

	FColor Color;
	for (int x = 0; x < Size; ++x)
	{
		for (int y = 0; y < Size; ++y)
		{
			Color = Qr.getModule(x, y) ? FColor::White : FColor::Black;
			Pixels[x + y * Size] = Color;
		}
	}

	UTexture2D* QrTexture =
		UTexture2D::CreateTransient(
			Size, Size, PF_B8G8R8A8, TEXT("QrCode"));
	void* Data = QrTexture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(Data, Pixels.GetData(), Size * Size * 4);
	QrTexture->GetPlatformData()->Mips[0].BulkData.Unlock();
	QrTexture->UpdateResource();
	QrTexture->Filter = TF_Nearest;

	return QrTexture;
}
