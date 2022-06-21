#include <Windows.h>
#include <cocos2d.h>
#include <MinHook.h>
#include <gd.h>
#include <fstream>

using std::uintptr_t;
using namespace cocos2d;

void (__thiscall* GJRewardUnlockLayer_playRewardEffect)(CCLayer* self);
void __fastcall GJRewardUnlockLayer_playRewardEffect_H(CCLayer* self, void*) {
    GJRewardUnlockLayer_playRewardEffect(self);

    auto* container_layer = dynamic_cast<CCLayer*>(self->getChildren()->objectAtIndex(0));

    auto* chestSprite = dynamic_cast<CCSprite*>(container_layer->getChildren()->objectAtIndex(3));

    if(chestSprite != nullptr) {
        auto colorPath = CCFileUtils::sharedFileUtils()->fullPathForFilename("GJRewardUnlockLayer_bg.txt", false);
        if(std::filesystem::exists(colorPath)) {
            std::string color_string;
            std::ifstream file;
            file.open(colorPath, std::ios::in);
            std::ostringstream sstr;
            sstr << file.rdbuf();
            color_string = sstr.str();
            file.close();

            int color_r;
            int color_g;
            int color_b;

            std::stringstream ss_r;
            ss_r << std::hex << color_string.substr(0, 2);
            ss_r >> color_r;

            std::stringstream ss_g;
            ss_g << std::hex << color_string.substr(2, 2);
            ss_g >> color_g;

            std::stringstream ss_b;
            ss_b << std::hex << color_string.substr(4, 2);
            ss_b >> color_b;

            dynamic_cast<CCSprite*>(chestSprite->getChildren()->objectAtIndex(6))->setColor(ccc3(color_r - 7, color_g - 7, color_b - 7));
            dynamic_cast<CCSprite*>(chestSprite->getChildren()->objectAtIndex(7))->setColor(ccc3(color_r - 7, color_g - 7, color_b - 7));
            auto* lastObject = dynamic_cast<CCSprite*>(chestSprite->getChildren()->objectAtIndex(8));
            lastObject->setColor(ccc3(color_r - 7, color_g - 7, color_b - 7));
            lastObject->setPositionY(lastObject->getPositionY() + 1.65f);
        }
    }
}

DWORD WINAPI thread_func(void* hModule) {
//#define _CONSOLE

#ifdef _CONSOLE
    AllocConsole();
    std::ofstream conout("CONOUT$", std::ios::out);
    std::ifstream conin("CONIN$", std::ios::in);
    std::cout.rdbuf(conout.rdbuf());
    std::cin.rdbuf(conin.rdbuf());
#endif

    MH_Initialize();

    auto base = reinterpret_cast<uintptr_t>(GetModuleHandle(0));

    MH_CreateHook(reinterpret_cast<void*>(base + 0x219980),
                  reinterpret_cast<void*>(&GJRewardUnlockLayer_playRewardEffect_H),
                  reinterpret_cast<void**>(&GJRewardUnlockLayer_playRewardEffect));
    MH_EnableHook(MH_ALL_HOOKS);

    return 0;
}

BOOL APIENTRY DllMain(HMODULE handle, DWORD reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        auto h = CreateThread(0, 0, thread_func, handle, 0, 0);
        if (h)
            CloseHandle(h);
        else
            return FALSE;
    }
    return TRUE;
}