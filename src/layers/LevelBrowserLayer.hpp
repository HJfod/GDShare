#pragma once

#include "../offsets.hpp"

class LevelBrowserLayer : public cocos2d::CCLayer {
    protected:
        PAD(0x14)
        gd::TextArea* m_pTextarea;
        gd::GJListLayer* m_pListLayer;
        gd::CCMenuItemSpriteExtra* m_pButton0;
        gd::CCMenuItemSpriteExtra* m_pButton1;
        gd::CCMenuItemSpriteExtra* m_pButton2;
        gd::CCMenuItemSpriteExtra* m_pButton3;
        PAD(0x4)
        cocos2d::CCArray* m_pArray;
        gd::GJSearchObject* m_pSearchObject;
        cocos2d::CCLabelBMFont* m_pLabel0;
        cocos2d::CCLabelBMFont* m_pLabel1;
        gd::CCMenuItemSpriteExtra* m_pButton4;
        cocos2d::CCLabelBMFont* m_pLabel2;
        gd::CCMenuItemSpriteExtra* m_pButton5;
        // probably more buttons here
        PAD(0x10)
        gd::LoadingCircle* m_pLoadingCircle;
        PAD(0x4)

        inline static bool (__thiscall* init)(LevelBrowserLayer*, gd::GJSearchObject*);
        static bool __fastcall initHook(LevelBrowserLayer*, uintptr_t, gd::GJSearchObject*);

        void onImport(cocos2d::CCObject*);
    
    public:
        static inline bool loadHook() {
            return MH_CreateHook(
                (PVOID)(gd::base + 0x15a040),
                as<LPVOID>(LevelBrowserLayer::initHook),
                as<LPVOID*>(&LevelBrowserLayer::init)
            ) == MH_OK;
        }

        static void importLevel(std::string const&);
        
        static LevelBrowserLayer* create(gd::GJSearchObject* pSearch) {
            return reinterpret_cast<LevelBrowserLayer*(__fastcall*)(
                gd::GJSearchObject*
            )>(
                gd::base + 0x159fa0
            )(
                pSearch
            );
        }
};
