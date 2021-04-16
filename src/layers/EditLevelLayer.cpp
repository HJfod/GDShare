#include "EditLevelLayer.hpp"
#include "ExportSettingsLayer.hpp"

void EditLevelLayer::onExport(cocos2d::CCObject* pSender) {
    ExportSettingsLayer::create(
        as<gd::GJGameLevel*>(
            as<cocos2d::CCNode*>(pSender)->getUserData()
        )
    )->show();
}

bool __fastcall EditLevelLayer::initHook(EditLevelLayer* _self, uintptr_t, gd::GJGameLevel* _lvl) {
    if (!init(_self, _lvl))
        return false;
    
    cocos2d::CCArray* children = _self->getChildren();

    cocos2d::CCMenu* m = ((cocos2d::CCMenu*)children->objectAtIndex(14));

    auto exportButton = gd::CCMenuItemSpriteExtra::create(
        cocos2d::CCSprite::create("BE_Export_File.png"),
        _self,
        (cocos2d::SEL_MenuHandler)&EditLevelLayer::onExport
    );
    exportButton->setUserData(_lvl);

    auto folderButton = getChild<gd::CCMenuItemSpriteExtra*>(
        m, m->getChildrenCount() - 1
    );
    auto moveUpButton = getChild<cocos2d::CCNode*>(
        m, m->getChildrenCount() - 2
    );

    auto xOffset = getChild<cocos2d::CCNode*>(
        m, 0
    )->getPositionX();
    auto yOffset =  moveUpButton->getPositionY()
                    - getChild<cocos2d::CCNode*>(
                        m, m->getChildrenCount() - 3
                    )->getPositionY();
    
    exportButton->setPosition(
        xOffset,
        moveUpButton->getPositionY() + yOffset
    );

    m->removeChild(folderButton, false);
    m->addChild(exportButton);
    m->addChild(folderButton);

    _self->removeChild((cocos2d::CCNode*)children->objectAtIndex(2), true);

    return true;
}
