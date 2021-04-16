#include "HorizontalSelectMenu.hpp"

void HorizontalSelectMenu::onSelect(cocos2d::CCObject* pSender) {
    for (auto i = 0u; i < this->m_vToggles.size(); i++)
        if (pSender != this->m_vToggles.at(i))
            this->m_vToggles.at(i)->toggle(true);
        else {
            this->m_vToggles.at(i)->toggle(true);
    
            this->m_nSelIndex = i;
        }
}

void HorizontalSelectMenu::select(unsigned int _index) {
    for (auto i = 0u; i < this->m_vToggles.size(); i++)
        this->m_vToggles.at(i)->toggle(_index != i);
    
    this->m_nSelIndex = _index;
}

unsigned int HorizontalSelectMenu::getSelected() {
    return this->m_nSelIndex;
}

bool HorizontalSelectMenu::init(std::vector<const char*> const& _opts) {
    if (!cocos2d::CCNode::init())
        return false;

    this->m_pBGLayer = cocos2d::extension::CCScale9Sprite::create(
        "square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f }
    );

    this->m_pBGLayer->setScale(.5f);
    this->m_pBGLayer->setColor({ 0, 0, 0 });
    this->m_pBGLayer->setOpacity(75);

    auto menu = cocos2d::CCMenu::create();
    float fMenuWidth = 0.0f;
    float fMenuPadding = 5.0f;

    for (auto const& opt : _opts) {
        auto pToggleOnSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
        auto pToggleOffSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");

        pToggleOnSpr->setScale(.8f);
        pToggleOffSpr->setScale(.8f);

        auto pToggle = gd::CCMenuItemToggler::create(
            pToggleOnSpr,
            pToggleOffSpr,
            this,
            (cocos2d::SEL_MenuHandler)&HorizontalSelectMenu::onSelect
        );

        this->m_vToggles.push_back(pToggle);
        menu->addChild(pToggle);

        auto pLabel = cocos2d::CCLabelBMFont::create(opt, "bigFont.fnt");
        pLabel->setScale(.6f);

        menu->addChild(pLabel);

        fMenuWidth += pToggleOffSpr->getScaledContentSize().width +
                      pLabel->getScaledContentSize().width +
                      fMenuPadding;
    }

    menu->alignItemsHorizontallyWithPadding(fMenuPadding);
    menu->setContentSize({ fMenuWidth + 35.0f, 40.0f });
    menu->setPosition(0, 0);

    this->m_pBGLayer->setContentSize(menu->getContentSize() * 2);
    this->setContentSize(menu->getContentSize());

    this->addChild(this->m_pBGLayer);
    this->addChild(menu);

    this->select(0u);

    return true;
}

HorizontalSelectMenu* HorizontalSelectMenu::create(std::vector<const char*> const& _opts) {
    auto pRet = new HorizontalSelectMenu();

    if (pRet && pRet->init(_opts)) {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}
