#include "LevelInfoLayer.hpp"
#include "EditLevelLayer.hpp"

bool __fastcall LevelInfoLayer::initHook(LevelInfoLayer* _self, uintptr_t, gd::GJGameLevel* _lvl) {
    if (!init(_self, _lvl))
        return false;

    auto exportButton = gd::CCMenuItemSpriteExtra::create(
        cocos2d::CCSprite::create("BE_Export_File.png"),
        _self,
        (cocos2d::SEL_MenuHandler)&EditLevelLayer::onExport
    );
    exportButton->setUserData(_lvl);

    if (_self->m_pCloneBtn) {
        _self->m_pCloneBtn->getParent()->addChild(
            exportButton
        );

        exportButton->setPosition(
            _self->m_pCloneBtn->getPositionX(),
            _self->m_pCloneBtn->getPositionY() + 50.0f
        );
    } else if (_self->m_pLikeBtn) {
        auto menu = _self->m_pLikeBtn->getParent();
        auto l_ix = menu->getChildren()->indexOfObject(_self->m_pLikeBtn);
        auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

        auto pos_x = - _self->m_pLikeBtn->getPositionX();
        auto pos_y = 0.0f;

        for (auto ix = l_ix; ix < menu->getChildrenCount(); ix++) {
            auto obj = getChild<cocos2d::CCNode*>(menu, ix);

            if (obj->getPositionX() < 80.0f && obj->getPositionY() > pos_y)
                    pos_y = obj->getPositionY();
        }

        menu->addChild(exportButton);

        exportButton->setPosition(pos_x, pos_y + 50.0f);
    } else {
        _self->m_pPlayBtnMenu->addChild(exportButton);

        exportButton->setPosition(-80.0f, 0.0f);
    }

    return true;
}
