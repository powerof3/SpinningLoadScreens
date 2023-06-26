#pragma once

namespace Hooks
{
	inline float angle{ 0.0f };
	inline float autoSpinRateMult{ 1.0f };

	inline std::optional<RE::NiPoint3> cachedXYZ{ std::nullopt };

	inline bool rotateAboutY{ false };
	inline bool rotateWithNegOffset{ false };

	inline bool stopSpin{ false };

	void Install();
}
