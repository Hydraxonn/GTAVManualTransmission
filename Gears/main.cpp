#include "../../ScriptHookV_SDK/inc/main.h"

#include "script.h"
#include "Input/keyboard.h"
#include "Util/Paths.h"
#include "Util/Logger.hpp"
#include "Memory/MemoryPatcher.hpp"
#include "Memory/Versions.h"
#include "Constants.h"


BOOL APIENTRY DllMain(HMODULE hInstance, DWORD reason, LPVOID lpReserved) {
    std::string logFile = Paths::GetModuleFolder(hInstance) + mtDir +
        "\\" + Paths::GetModuleNameWithoutExtension(hInstance) + ".log";
    logger.SetFile(logFile);
    Paths::SetOurModuleHandle(hInstance);

    // ReSharper disable once CppDefaultCaseNotHandledInSwitchStatement
    switch (reason) {
        case DLL_PROCESS_ATTACH: {
            MemoryPatcher::SetPatterns(getGameVersion());
            scriptRegister(hInstance, ScriptMain);
            scriptRegisterAdditionalThread(hInstance, NPCMain);
            logger.Clear();
            logger.Write(INFO, "GTAVManualTransmission %s (build %s)", DISPLAY_VERSION, __DATE__);
            logger.Write(INFO, "Game version " + eGameVersionToString(getGameVersion()));
            if (getGameVersion() < G_VER_1_0_877_1_STEAM) {
                logger.Write(WARN, "Unsupported game version! Update your game.");
            }
            logger.Write(INFO, "Script registered");
            break;
        }
        case DLL_PROCESS_DETACH: {
            logger.Write(INFO, "Init shutdown");
            const uint8_t expected = 5;
            uint8_t actual = 0;

            if (MemoryPatcher::RevertGearboxPatches()) 
                actual++;
            if (MemoryPatcher::RestoreSteeringCorrection())
                actual++;
            if (MemoryPatcher::RestoreSteeringControl()) 
                actual++;
            if (MemoryPatcher::RestoreBrakeDecrement())
                actual++;
            if (MemoryPatcher::RestoreThrottleDecrement())
                actual++;

            resetSteeringMultiplier();
            stopForceFeedback();
            scriptUnregister(hInstance);

            if (actual == expected) {
                logger.Write(INFO, "PATCH: Script shut down cleanly.");
            }
            else {
                logger.Write(ERROR, "PATCH: Script shut down with unrestored patches!");
            }
            break;
        }
    }
    return TRUE;
}
