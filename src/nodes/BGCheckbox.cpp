#include "BGCheckbox.hpp"

bool BGCheckbox::init(const char* _text) {
    if (!cocos2d::CCNode::init())
        return false;

    this->m_pBGLayer = cocos2d::extension::CCScale9Sprite::create(
        "square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f }
    );

    this->m_pBGLayer->setScale(.5f);
    this->m_pBGLayer->setColor({ 0, 0, 0 });
    this->m_pBGLayer->setOpacity(75);

    auto menu = cocos2d::CCMenu::create();

    float fMenuPadding = 5.0f;

    auto pToggleOnSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
    auto pToggleOffSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");

    pToggleOnSpr->setScale(.8f);
    pToggleOffSpr->setScale(.8f);

    this->m_pToggler = gd::CCMenuItemToggler::create(
        pToggleOffSpr,
        pToggleOnSpr,
        this,
        nullptr
    );

    menu->addChild(this->m_pToggler);

    this->m_pLabel = cocos2d::CCLabelBMFont::create(_text, "bigFont.fnt");
    this->m_pLabel->setScale(.6f);

    menu->addChild(this->m_pLabel);

    menu->alignItemsHorizontallyWithPadding(fMenuPadding);
    menu->setContentSize({ this->m_pLabel->getScaledContentSize().width +
        this->m_pToggler->getScaledContentSize().width +
        fMenuPadding + 20.0f,
        40.0f
    });
    menu->setPosition(0, 0);

    this->m_pBGLayer->setContentSize(menu->getContentSize() * 2);
    this->setContentSize(menu->getContentSize());

    this->addChild(this->m_pBGLayer);
    this->addChild(menu);

    return true;
}

void BGCheckbox::setEnabled(bool _e) {
    this->m_pToggler->setEnabled(_e);
    this->m_pLabel->setColor(
        _e ? cocos2d::ccColor3B { 255, 255, 255 } :
        cocos2d::ccColor3B { 130, 130, 130 }
    );
}

BGCheckbox* BGCheckbox::create(const char* _text) {
    auto pRet = new BGCheckbox();

    if (pRet && pRet->init(_text)) {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}

