#include "Phobos.h"

#include <Drawing.h>
#include <SessionClass.h>
#include <Unsorted.h>

#include <Utilities/Debug.h>
#include <Utilities/Patch.h>
#include <Utilities/Macro.h>
#include "Utilities/Parser.h"

HANDLE Phobos::hInstance = 0;

char Phobos::readBuffer[Phobos::readLength];
wchar_t Phobos::wideBuffer[Phobos::readLength];

bool Phobos::DisplayDamageNumbers = false;
bool Phobos::IsLoadingSaveGame = false;

bool Phobos::Config::SaveGameOnScenarioStart = true;

#ifdef IS_RELEASE_VER
const wchar_t* Phobos::VersionDescription = L"Chang_zhi Custom Phobos Extension build #" _STR(BUILD_NUMBER) L". Please test the build before shipping.";
#endif

void Phobos::ExeRun()
{
	Patch::ApplyStatic();
}

void Phobos::ExeTerminate()
{
	Console::Release();
}

// =============================
// hooks

bool __stdcall DllMain(HANDLE hInstance, DWORD dwReason, LPVOID v)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		Phobos::hInstance = hInstance;
	}
	return true;
}

DEFINE_HOOK(0x7CD810, ExeRun, 0x9)
{
	Phobos::ExeRun();

	return 0;
}

DEFINE_HOOK(0x52F639, YR_CmdLineParse, 0x5)
{
	GET(char**, ppArgs, ESI);
	GET(int, nNumArgs, EDI);

	Debug::LogDeferredFinalize();
	return 0;
}

DEFINE_HOOK(0x67E44D, LoadGame_SetFlag, 0x5)
{
	Phobos::IsLoadingSaveGame = true;
	return 0;
}

DEFINE_HOOK(0x67E68A, LoadGame_UnsetFlag, 0x5)
{
	Phobos::IsLoadingSaveGame = false;
	return 0;
}
