#include "InternalLoad.h"

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

            static void DumpProfileToDisk(Classes::CPlayerProfileV1* profile, const char* path = "CPlayerProfileV1_dump.bin")
            {
                if (!profile) {
                    Print("Cannot dump profile: null pointer");
                    return;
                }

                std::ofstream out(path, std::ios::binary | std::ios::trunc);
                if (!out) {
                    Print("Failed to open profile dump file: %s", path);
                    return;
                }

                out.write(reinterpret_cast<const char*>(profile), sizeof(Classes::CPlayerProfileV1));

                if (!out) {
                    Print("Failed while writing profile dump file: %s", path);
                    return;
                }

                Print("Dumped CPlayerProfileV1 (%llu bytes) to %s",
                    static_cast<unsigned long long>(sizeof(Classes::CPlayerProfileV1)),
                    path);
            }

            bool __fastcall cb_hook(Classes::CPlayerProfileV1* profile, int pad, class CBaseFileIO* pFileIO, nfw::string fileName, bool param_3) {
                bool result = PLH::FnCast(o_func, &cb_hook)(profile, pad, pFileIO, fileName, param_3);

                // Dump the loaded profile before we modify it.
                if (result) {
					// Dump the loaded profile before we modify it.
					//DumpProfileToDisk(profile);

					// Force the 4-byte integer at offset 0x168 to be 2
					*reinterpret_cast<int32_t*>(reinterpret_cast<uintptr_t>(profile) + 0x168) = 2;
					Print("Forced profile offset 0x168 to 2");
				}

                /*SaveData* customData = SaveData::GetInstance();
                customData->Load("./Modded.save");*/

                //Add all towers to the profile
                Print("Adding all towers to save...");
                const auto& allTowerFlags = g_towerFlags.GetAll();
                for (const auto& [flag, str] : allTowerFlags) {
                    if (!g_towerFlags.IsVanilla(flag))
                    {
                        profile->towerUnlocks[flag] = true;// customData->IsTowerUnlocked(pair.second);
                        Print("Added tower with ID '%llx' to save", flag);
                    }
                }
                Print("Done!");
                return result;
            }

            auto InternalLoad::Apply() -> bool
            {
                const void* address = Signatures::GetAddressOf(Sigs::CPlayerProfileV1_InternalLoad);
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