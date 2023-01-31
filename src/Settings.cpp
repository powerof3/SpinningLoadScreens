#include "Settings.h"

Settings::Settings()
{
	constexpr auto path = L"Data/SKSE/Plugins/po3_SpinningLoadScreens.ini";

	CSimpleIniA ini;
	ini.SetUnicode();

	ini.LoadFile(path);

	ini::get_value(ini, autoSpinRateMult, "Settings", "Spin Rate Multiplier", nullptr);

	(void)ini.SaveFile(path);
}

float Settings::GetAutoSpinRateMult()
{
	return autoSpinRateMult;
}
