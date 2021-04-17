#include "LevelInfoLayer.hpp"
#include "EditLevelLayer.hpp"

bool __fastcall LevelInfoLayer::initHook(LevelInfoLayer* _self, uintptr_t, gd::GJGameLevel* _lvl) {
    if (!init(_self, _lvl))
        return false;

    auto menu = getChild<cocos2d::CCMenu*>(_self, 2);

    gd::CCMenuItemSpriteExtra* targBtn = nullptr;
    auto chix = menu->getChildrenCount();
    while (getChild<gd::CCMenuItemSpriteExtra*>(menu, --chix)->getPositionX() < 0) {
        targBtn = getChild<gd::CCMenuItemSpriteExtra*>(menu, chix );

        if (chix == 0) break;
    }

    auto exportButton = gd::CCMenuItemSpriteExtra::create(
        cocos2d::CCSprite::create("BE_Export_File.png"),
        _self,
        (cocos2d::SEL_MenuHandler)&EditLevelLayer::onExport
    );
    exportButton->setUserData(_lvl);

    exportButton->setPosition(
        targBtn->getPositionX(),
        targBtn->getPositionY() + 55.0f
    );

    menu->addChild(exportButton);

    return true;
}
