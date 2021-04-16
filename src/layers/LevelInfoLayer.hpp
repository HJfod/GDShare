#pragma once

#include "../offsets.hpp"

class LevelInfoLayer : public cocos2d::CCLayer {
    protected:
        inline static bool (__thiscall* init)(LevelInfoLayer*, gd::GJGameLevel*);
        static bool __fastcall initHook(LevelInfoLayer*, uintptr_t, gd::GJGameLevel*);

    public:
        static bool loadHook() {
            return MH_CreateHook(
                (PVOID)(gd::base + 0x175df0),
                as<LPVOID>(LevelInfoLayer::initHook),
                as<LPVOID*>(&LevelInfoLayer::init)
            ) == MH_OK;
        }
};
