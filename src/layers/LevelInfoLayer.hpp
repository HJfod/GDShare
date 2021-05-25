#pragma once

#include "../offsets.hpp"

class LevelInfoLayer : public cocos2d::CCLayer,
    gd::LevelDownloadDelegate,
    gd::LevelUpdateDelegate,
    gd::RateLevelDelegate,
    gd::LikeItemDelegate,
    gd::FLAlertLayerProtocol,
    gd::LevelDeleteDelegate,
    gd::NumberInputDelegate,
    gd::SetIDPopupDelegate
{
    protected:
        PAD(0x4)
        cocos2d::CCMenu* m_pPlayBtnMenu;
        gd::GJGameLevel* m_pLevel;
        cocos2d::CCArray* m_pUnknown;
        gd::CCMenuItemSpriteExtra* m_pLikeBtn;
        gd::CCMenuItemSpriteExtra* m_pStarRateBtn;
        gd::CCMenuItemSpriteExtra* m_pDemonRateBtn;
        PAD(0x4)
        gd::CCMenuItemToggler* m_pToggler;
        cocos2d::CCLabelBMFont* m_pLabel0;
        cocos2d::CCLabelBMFont* m_pLabel1;
        cocos2d::CCLabelBMFont* m_pLabel2;
        cocos2d::CCLabelBMFont* m_pLabel3;
        cocos2d::CCLabelBMFont* m_pLabel4;
        cocos2d::CCLabelBMFont* m_pLabel5;
        gd::CCMenuItemSpriteExtra* m_pCloneBtn;
        PAD(0x4)

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
