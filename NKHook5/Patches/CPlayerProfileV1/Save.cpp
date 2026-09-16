#include "Save.h"

#include "../../Classes/CPlayerProfileV1.h"
#include "../../Mod/SaveData.h"
#include "../../Signatures/Signature.h"
#include "../../Util/FlagManager.h"
#include <Logging/Logger.h>
#include <fstream>
#include <string>

extern NKHook5::Util::FlagManager g_towerFlags;

namespace NKHook5
{
    namespace Patches
    {
        namespace CPlayerProfileV1
        {
            using namespace Mod;
            using namespace Signatures;
            using namespace Common;
            using namespace Common::Logging;
            using namespace Common::Logging::Logger;

            static uint64_t o_func;
            bool __fastcall cb_hook(Classes::CPlayerProfileV1* profile, int pad, class CBaseFileIO* pFileIO, bool param_2, bool param_3) {
                bool result = PLH::FnCast(o_func, &cb_hook)(profile, pad, pFileIO, param_2, param_3);
				if (result) {
					// Force the 4-byte integer at offset 0x168 to be 2
					*reinterpret_cast<int32_t*>(reinterpret_cast<uintptr_t>(profile) + 0x168) = 2;
					Print("Forced control scheme change.");
				}
                /*SaveData* customData = SaveData::GetInstance();
                for (const auto& [towerId, unlocked] : profile->towerUnlocks) {
                    std::string towerName = g_towerFlags.GetName(towerId);
                    customData->SetTowerUnlocked(towerName, unlocked);
                }
                customData->Save("./Modded.save");*/
                return result;
            }

            auto Save::Apply() -> bool
            {
                const void* address = Signatures::GetAddressOf(Sigs::CPlayerProfileV1_Save);
                if (address)
                {
                    PLH::x86Detour* detour = new PLH::x86Detour((const uint64_t)address, (const uintptr_t)&cb_hook, &o_func);
                    if (detour->hook())
                    {
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
                else
                {
                    return false;
                }
            }
        }
    }
}