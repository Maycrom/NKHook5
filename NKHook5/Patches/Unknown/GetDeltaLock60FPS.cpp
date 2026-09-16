#include "GetDeltaLock60FPS.h"
#include <iostream>
#include <iomanip>
#include "Logging/Logger.h"

#include <Windows.h>
#include <Psapi.h>

#include <magic_enum/magic_enum.hpp>

#include <libhat/Scanner.hpp>

#include <cassert>
#include <vector>
namespace NKHook5
{
    namespace Patches
    {
        namespace Unknown
        {
            // Temporary test helper: searches specifically in .rdata.
            static uintptr_t FindPatternRdata(std::string_view pattern)
            {
                std::cout << "[FindPatternRdata] Parsing pattern: "
                          << pattern << std::endl;

                auto sig = hat::parse_signature(pattern);

                if (!sig.has_value())
                {
                    std::cerr << "[FindPatternRdata] ERROR: Failed to parse "
                                 "signature: "
                              << pattern << std::endl;

                    return 0;
                }

                std::cout << "[FindPatternRdata] Searching .rdata..."
                          << std::endl;

                const auto result =
                    hat::find_pattern(sig.value(), ".rdata");

                if (!result.has_result())
                {
                    std::cerr << "[FindPatternRdata] ERROR: Pattern not found "
                                 "in .rdata: "
                              << pattern << std::endl;

                    return 0;
                }

                const uintptr_t address =
                    reinterpret_cast<uintptr_t>(result.get());

                std::cout << "[FindPatternRdata] Pattern found at 0x"
                          << std::hex << address
                          << std::dec << std::endl;

                return address;
            }

            auto GetDeltaLock60FPS::Apply() -> bool
            {
                std::cout << "[GetDeltaLock60FPS] Apply() started"
                          << std::endl;

                //
                // PATCH 1: Profile.save
                //
                {
                    constexpr std::string_view pattern =
                        "50 72 6F 66 69 6C 65 2E 73 61 76 65";

                    std::cout << "[GetDeltaLock60FPS] "
                                 "Searching for \"Profile.save\"..."
                              << std::endl;

                    const uintptr_t address =
                        FindPatternRdata(pattern);

                    if (address == 0)
                    {
                        std::cerr << "[GetDeltaLock60FPS] ERROR: "
                                     "\"Profile.save\" was not found"
                                  << std::endl;

                        return false;
                    }

                    std::cout << "[GetDeltaLock60FPS] \"Profile.save\" found "
                                 "at 0x"
                              << std::hex << address
                              << std::dec << std::endl;

                    constexpr char replacement[] = "Profile.mod";

                    std::cout << "[GetDeltaLock60FPS] Replacing with \""
                              << replacement << "\""
                              << std::endl;

                    this->WriteBytes(
                        address,
                        replacement,
                        sizeof(replacement)
                    );

                    std::cout << "[GetDeltaLock60FPS] "
                                 "\"Profile.save\" patch applied"
                              << std::endl;
                }

                //
                // PATCH 2: API URL
                //
                {
                    constexpr std::string_view pattern =
                        "68 74 74 70 73 3A 2F 2F 73 74 61 74 69 63 2D "
                        "61 70 69 2E 6E 6B 73 74 61 74 69 63 2E 63 6F 6D";

                    std::cout << "[GetDeltaLock60FPS] "
                                 "Searching for \"https://static-api.nkstatic.com\"..."
                              << std::endl;

                    const uintptr_t address =
                        FindPatternRdata(pattern);

                    if (address == 0)
                    {
                        std::cerr << "[GetDeltaLock60FPS] ERROR: "
                                     "\"https://static-api.nkstatic.com\" "
                                     "was not found"
                                  << std::endl;

                        return false;
                    }

                    std::cout << "[GetDeltaLock60FPS] API URL found at 0x"
                              << std::hex << address
                              << std::dec << std::endl;

                    constexpr char replacement[] =
                        "https://mvb2.github.io";

                    std::cout << "[GetDeltaLock60FPS] Replacing URL with \""
                              << replacement << "\""
                              << std::endl;

                    this->WriteBytes(
                        address,
                        replacement,
                        sizeof(replacement)
                    );

                    std::cout << "[GetDeltaLock60FPS] "
                                 "API URL patch applied"
                              << std::endl;
                }

                std::cout << "[GetDeltaLock60FPS] All patches applied "
                             "successfully"
                          << std::endl;

                return true;
            }
        }
    }
}