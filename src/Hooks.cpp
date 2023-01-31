#include "Hooks.h"
#include "Settings.h"

namespace Hooks
{
	struct detail
	{
		static void SyncRotation(const RE::NiPointer<RE::BSFadeNode>& a_root)
		{
			float x, y, z;
			a_root->local.rotate.ToEulerAnglesXYZ(x, y, z);

			constexpr float QUARTER_PI = RE::NI_PI / 4.0f;
			rotateAboutY = x < RE::NI_PI && x > QUARTER_PI || x > -RE::NI_PI && x < -QUARTER_PI;

			if (rotateAboutY) {
				angle = y;
			} else {
				angle = z;
			}

			if (x <= 0) {
				rotateWithNegOffset = true;
			}

			cachedXYZ.emplace(x, y, z);
		}
	};

	namespace AutoSpin
	{
		struct GetLoadScreenModel
		{
			static RE::TESModelTextureSwap* thunk(RE::TESLoadScreen* a_loadScreen)
			{
				cachedXYZ = std::nullopt;
				autoSpinRateMult = Settings::GetSingleton()->GetAutoSpinRateMult();
				stopSpin = false;

				return func(a_loadScreen);
			}
			static inline REL::Relocation<decltype(thunk)> func;
		};

		struct AdvanceMovie
		{
			static void thunk(RE::MistMenu* a_this, float a_interval, std::uint32_t a_currentTime)
			{
				func(a_this, a_interval, a_currentTime);

				if (auto root = a_this->showLoadScreen ? a_this->loadScreenModel : RE::NiPointer<RE::BSFadeNode>()) {
					// init values
					if (!cachedXYZ) {
						detail::SyncRotation(root);
					} else {
						if (a_this->leftButtonHeldDown || a_this->rightButtonHeldDown) {
							stopSpin = true;
						} else {
							if (stopSpin) {
								// update current rotation
								detail::SyncRotation(root);
								stopSpin = false;
							} else {
								if (rotateAboutY) {
									root->local.rotate.SetEulerAnglesXYZ(cachedXYZ->x, angle, cachedXYZ->z);
								} else {
									root->local.rotate.SetEulerAnglesXYZ(cachedXYZ->x, cachedXYZ->y, angle);
								}

								if (rotateWithNegOffset) {
									angle -= (a_interval * autoSpinRateMult);
								} else {
									angle += (a_interval * autoSpinRateMult);
								}

								// constraint angle
								angle = std::fmod(angle, RE::NI_TWO_PI);
								if (angle < 0) {
									angle += RE::NI_TWO_PI;
								}

								RE::BSVisit::TraverseScenegraphCollision(root.get(), [](RE::bhkNiCollisionObject* a_col) -> RE::BSVisit::BSVisitControl {
									a_col->flags.set(RE::bhkNiCollisionObject::Flag::kReset);
									return RE::BSVisit::BSVisitControl::kContinue;
								});

								RE::NiUpdateData data{};
								root->Update(data);
							}
						}
					}
				}
			}
			static inline REL::Relocation<decltype(thunk)> func;
			static inline constexpr std::size_t size = 0x5;
		};
	}

	void Install()
	{
		REL::Relocation<std::uintptr_t> target{ RELOCATION_ID(51048, 51929), OFFSET(0x384, 0x27B) };
		stl::write_thunk_call<AutoSpin::GetLoadScreenModel>(target.address());

		stl::write_vfunc<RE::MistMenu, AutoSpin::AdvanceMovie>();
	}
}
