#pragma once

class Settings
{
public:
	[[nodiscard]] static Settings* GetSingleton()
	{
		static Settings singleton;
		return std::addressof(singleton);
	}

	[[nodiscard]] float GetAutoSpinRateMult();

private:
	Settings();

	float autoSpinRateMult{ 1.0f };
};
