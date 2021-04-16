#include "LevelInfoLayer.hpp"
#include "EditLevelLayer.hpp"

bool __fastcall LevelInfoLayer::initHook(LevelInfoLayer* _self, uintptr_t, gd::GJGameLevel* _lvl) {
    if (!init(_self, _lvl))
        return false;

    auto menu = getChild<cocos2d::CCMenu*>(_self, 2);
    auto copyBtn = getChild<gd::CCMenuItemSpriteExtra*>(menu, menu->getChildrenCount() - 1);

    auto exportButton = gd::CCMenuItemSpriteExtra::create(
        cocos2d::CCSprite::create("BE_Export_File.png"),
        _self,
        (cocos2d::SEL_MenuHandler)&EditLevelLayer::onExport
    );
    exportButton->setUserData(_lvl);

    exportButton->setPosition(
        copyBtn->getPositionX(),
        copyBtn->getPositionY() - 55.0f
    );

    menu->addChild(exportButton);

    return true;
}
