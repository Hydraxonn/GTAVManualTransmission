/*
 * Massive thanks to t-mat on GitHub for having a simple C++ DInput
 * application! https://gist.github.com/t-mat/1391291
 */

#pragma once

#include <array>
#include <vector>
#include "DiJoyStick.h"
#include <unordered_map>

// https://stackoverflow.com/questions/24113864/what-is-the-right-way-to-use-a-guid-as-the-key-in-stdhash-map
namespace std {
	template<> struct hash<GUID> : public std::_Bitwise_hash<GUID> { };
}

class WheelDirectInput {
public:
	static const int MAX_RGBBUTTONS = 128;
	static const int AVGSAMPLES = 2;

	enum DIAxis {
		lX,
		lY,
		lZ,
		lRx,
		lRy,
		lRz,
		rglSlider0,
		rglSlider1,
		UNKNOWN_AXIS,
		SIZEOF_DIAxis
	};

	enum POV {
		N = 3600,
		NE = 4500,
		E = 9000,
		SE = 13500,
		S = 18000,
		SW = 22500,
		W = 27000,
		NW = 31500,
	};

	const std::array<std::string, SIZEOF_DIAxis> DIAxisHelper {
		"lX",
		"lY",
		"lZ",
		"lRx",
		"lRy",
		"lRz",
		"rglSlider0",
		"rglSlider1",
		"UNKNOWN_AXIS"
	};

	WheelDirectInput();
	~WheelDirectInput();
	bool PreInit();

	bool InitWheel();

	bool InitFFB(GUID guid, DIAxis ffAxis);
	void UpdateCenterSteering(GUID guid, DIAxis steerAxis);
	const DiJoyStick::Entry *FindEntryFromGUID(GUID guid);

	// Should be called every update()
	void Update();

	bool IsConnected(GUID device);
	bool IsButtonPressed(int btn, GUID device);
	bool IsButtonJustPressed(int btn, GUID device);
	bool IsButtonJustReleased(int btn, GUID device);
	bool WasButtonHeldForMs(int btn, GUID device, int millis);
	void UpdateButtonChangeStates();

	void SetConstantForce(GUID device, DIAxis ffAxis, int force);

	DIAxis StringToAxis(std::string& axisString);

	int GetAxisValue(DIAxis axis, GUID device);
	float GetAxisSpeed(DIAxis axis, GUID device);

	std::vector<GUID> GetGuids();
	void PlayLedsDInput(GUID guid, const FLOAT currentRPM, const FLOAT rpmFirstLedTurnsOn, const FLOAT rpmRedLine);

private:
	void updateAxisSpeed();
	bool createConstantForceEffect(const DiJoyStick::Entry *e, DIAxis ffAxis);
	void formatError(HRESULT hr, std::string &hrStr);

	static const int POVDIRECTIONS = 8;
	const std::array<POV, POVDIRECTIONS> POVDirections{
		N, NE, E, SE, S, SW, W, NW
	};
	std::vector<GUID> foundGuids;

	DiJoyStick djs;
	LPDIRECTINPUT lpDi = nullptr;
	LPDIRECTINPUTEFFECT pCFEffect;
	LPDIRECTINPUTEFFECT pFREffect;
	std::unordered_map<GUID, std::array<__int64, MAX_RGBBUTTONS>> rgbPressTime;
	std::unordered_map<GUID, std::array<__int64, MAX_RGBBUTTONS>> rgbReleaseTime;
	std::unordered_map<GUID, std::array<bool, MAX_RGBBUTTONS>>	rgbButtonCurr;
	std::unordered_map<GUID, std::array<bool, MAX_RGBBUTTONS>>	rgbButtonPrev;
	std::unordered_map<GUID, std::array<__int64, POVDIRECTIONS>>	povPressTime;
	std::unordered_map<GUID, std::array<__int64, POVDIRECTIONS>>	povReleaseTime;
	std::unordered_map<GUID, std::array<bool, POVDIRECTIONS>>		povButtonCurr;
	std::unordered_map<GUID, std::array<bool, POVDIRECTIONS>>		povButtonPrev;

	std::unordered_map<GUID, std::array<int, SIZEOF_DIAxis>> prevPosition;
	std::unordered_map<GUID, std::array<__int64, SIZEOF_DIAxis>> prevTime;
	std::unordered_map<GUID, std::array<std::array<float, AVGSAMPLES>, SIZEOF_DIAxis>> samples;

	std::unordered_map<GUID, std::array<int, SIZEOF_DIAxis>> averageIndex;
	std::unordered_map<GUID, std::array<bool, SIZEOF_DIAxis>> hasForceFeedback;
};
