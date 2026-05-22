#pragma once

#include <Windows.h>
#include <string>

#include <Phobos.version.h>
#include <Syringe.h>

class CCINIClass;
class AbstractClass;

constexpr auto NONE_STR = "<none>";
constexpr auto NONE_STR2 = "none";
constexpr auto SIDEBAR_SECTION = "Sidebar";
constexpr auto UISETTINGS_SECTION = "UISettings";

class Phobos
{
public:
	static void ExeRun();
	static void ExeTerminate();

	//variables
	static HANDLE hInstance;

	static const size_t readLength = 2048;
	static char readBuffer[readLength];
	static wchar_t wideBuffer[readLength];
	static constexpr auto readDelims = ",";

	static const wchar_t* VersionDescription;
	static bool DisplayDamageNumbers;
	static bool IsLoadingSaveGame;
	static bool ShouldSave;
	static std::wstring CustomGameSaveDescription;
	static void ScheduleGameSave(const std::wstring& description);
	static void PassiveSaveGame();

	class Config
	{
	public:
		static bool SaveGameOnScenarioStart;
	};

};
