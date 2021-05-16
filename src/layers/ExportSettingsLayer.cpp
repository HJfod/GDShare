#include "ExportSettingsLayer.hpp"
#include <direct.h>
#include <shlobj_core.h>
#include "../utils/gdshare.hpp"

static std::string workdir() {
    char buff[FILENAME_MAX];
    _getcwd(buff, FILENAME_MAX);
    std::string current_working_dir(buff);
    return current_working_dir;
}

// globals to save export dir and type when layer is closed

unsigned int g_selectedExportType = 0u;
std::string g_exportPath = workdir();

void ExportSettingsLayer::onClose(cocos2d::CCObject* pSender) {
    g_selectedExportType = this->m_pHorizontalMenu->getSelected();
    g_exportPath = this->m_pPathInput->getString();

    BrownAlertDelegate::onClose(pSender);
}

void ExportSettingsLayer::onSelectPath(cocos2d::CCObject* pSender) {
    nfdchar_t* path = nullptr;
    nfdresult_t res = NFD_PickFolder(nullptr, &path);

    if (res == NFD_OKAY) {
        this->m_pPathInput->setString(path);

        free(path);
    }
}

void ExportSettingsLayer::onInfo(cocos2d::CCObject*) {
    gd::FLAlertLayer::create(
        nullptr,
        "Info", "OK", nullptr,
        360.0f,
        "You can export levels for multiple different level sharing programs.\n\n" \
        "<cy>.gmd2</c> is <cl>recommended</c>.\n" \
        "Integrated GDShare can read all formats.\n\n" \
        "<cy>.gmd2</c> is for <cg>GDShare 2</c>\n" \
        "<cy>.gmd</c> is for <co>GDShare (original)</c>\n"    \
        "<cy>.lvl</c> is for <cr>LvlShare</c>\n"   \
    )->show();
}

void ExportSettingsLayer::onExport(cocos2d::CCObject*) {
    std::string outputPath = this->m_pPathInput->getString();
    auto format = static_cast<ExportFormat>(this->m_pHorizontalMenu->getSelected());

    if (std::filesystem::exists(outputPath))
        if (std::filesystem::is_directory(outputPath))
            outputPath += "\\" + this->m_pLevel->levelName + "." + gdshare::getExportTypeString(format);

    int flag = gdshare::EF_None;

    if (this->m_pIncludeSong->getToggle()->isOn())
        flag |= gdshare::EF_IncludeSong;

    auto res = gdshare::exportLevel( this->m_pLevel, outputPath, format, flag );
    if (!res.size())
        gd::FLAlertLayer::create(
            new ExportResultHandler(outputPath),
            "Success",
            "OK", "Show File",
            "Succesfully exported to <co>"_s + outputPath + "</c>!"_s
        )->show();
    else
        gd::FLAlertLayer::create(
            nullptr,
            "Error",
            "OK", nullptr,
            "<cr>Error</c>: " + res
        )->show();
    
    this->onClose(nullptr);
}

ExportResultHandler::ExportResultHandler(std::string const& _str) {
    this->m_sPath = _str;
}

void ExportResultHandler::FLAlert_Clicked(gd::FLAlertLayer*, bool _btn2) {
    if (_btn2) {
        ITEMIDLIST *pidl = ILCreateFromPathA(this->m_sPath.c_str());

        if (pidl) {
            SHOpenFolderAndSelectItems(pidl, 0, 0, 0);
            ILFree(pidl);
        }
    }
}

void ExportSettingsLayer::setup() {
    auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();    
    this->m_pHorizontalMenu = HorizontalSelectMenu::create({ ".gmd", ".gmd2", ".lvl" });

    this->m_pHorizontalMenu->setPosition(winSize.width / 2, winSize.height / 2 + 45.0f);
    this->m_pHorizontalMenu->select(g_selectedExportType);

    auto pTypeLabel = cocos2d::CCLabelBMFont::create("Format:", "bigFont.fnt");
    pTypeLabel->setPosition(winSize.width / 2, winSize.height / 2 + 78.0f);
    pTypeLabel->setScale(.5f);
    this->m_pLayer->addChild(pTypeLabel);

    auto pTypeInfoSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    pTypeInfoSpr->setScale(.7f);

    auto pTypeInfo = gd::CCMenuItemSpriteExtra::create(
        pTypeInfoSpr,
        this,
        (cocos2d::SEL_MenuHandler)&ExportSettingsLayer::onInfo
    );
    pTypeInfo->setPosition(
        pTypeLabel->getScaledContentSize().width / 2 + 3.0f + pTypeInfoSpr->getScaledContentSize().width / 2,
        pTypeLabel->getPositionY() - winSize.height / 2 - 1.5f
    );
    this->m_pButtonMenu->addChild(pTypeInfo);
    
    // tysm camden <3
    this->registerWithTouchDispatcher();
    cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->incrementForcePrio(2);

    this->setTouchEnabled(true);
    this->setKeypadEnabled(true);

    this->m_pLayer->addChild(this->m_pHorizontalMenu, 150);

    this->m_pPathInput = InputNode::create(
        this->m_pLrSize.width - 40.0f,
        "Export Directory",
        "chatFont.fnt",
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789/\\.:-_,;'=%&"_s,
        MAX_PATH
    );

    this->m_pPathInput->setString(g_exportPath.c_str());
    this->m_pPathInput->getInputNode()->setLabelPlaceholderColor({ 50, 150, 255 });
    this->m_pPathInput->setPosition(winSize.width / 2, winSize.height / 2 - 20.0f);

    this->m_pLayer->addChild(this->m_pPathInput, 150);

    auto pPathLabel = cocos2d::CCLabelBMFont::create("Path:", "bigFont.fnt");
    pPathLabel->setPosition(winSize.width / 2, winSize.height / 2 + 7.0f);
    pPathLabel->setScale(.5f);
    this->m_pLayer->addChild(pPathLabel);

    auto pPathSetSpr = cocos2d::CCSprite::createWithSpriteFrameName("gj_folderBtn_001.png");
    pPathSetSpr->setScale(.7f);

    auto pPathSet = gd::CCMenuItemSpriteExtra::create(
        pPathSetSpr,
        this,
        (cocos2d::SEL_MenuHandler)&ExportSettingsLayer::onSelectPath
    );
    pPathSet->setPosition(
        pPathLabel->getScaledContentSize().width / 2 + 3.0f + pPathSetSpr->getScaledContentSize().width / 2,
        pPathLabel->getPositionY() - winSize.height / 2 - 1.5f
    );
    this->m_pButtonMenu->addChild(pPathSet);

    auto pExportButton = gd::CCMenuItemSpriteExtra::create(
        gd::ButtonSprite::create(
            "Export", 0, 0, "goldFont.fnt", "GJ_button_01.png", 0, .8f
        ),
        this,
        (cocos2d::SEL_MenuHandler)&ExportSettingsLayer::onExport
    );
    pExportButton->setPosition(
        0.0f,
        - this->m_pLrSize.height / 2 + pExportButton->getScaledContentSize().height / 2 + 12.5f
    );

    this->m_pButtonMenu->addChild(pExportButton);

    auto pToggleOnSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
    auto pToggleOffSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");

    pToggleOnSpr->setScale(.8f);
    pToggleOffSpr->setScale(.8f);

    this->m_pIncludeSong = BGCheckbox::create("Include song");
    this->m_pIncludeSong->setScale(.75f);
    this->m_pIncludeSong->setPosition(
        0.0f, - 60.0f
    );
    if (g_selectedExportType != 1)
        this->m_pIncludeSong->setEnabled(false);

    this->m_pButtonMenu->addChild(this->m_pIncludeSong);

    this->m_pHorizontalMenu->setCallback([this](unsigned int ix, bool enabled) -> void {
        if (ix == 1)
            return this->m_pIncludeSong->setEnabled(true);

        this->m_pIncludeSong->setEnabled(false);
        this->m_pIncludeSong->getToggle()->toggle(false);
    });
}

ExportSettingsLayer* ExportSettingsLayer::create(gd::GJGameLevel* _lvl) {
    auto pRet = new ExportSettingsLayer();

    // this is what beautiful code looks like
    if (pRet) {
        pRet->m_pLevel = _lvl;

        if (pRet->init(
            320.0f, 260.0f,
            "GJ_square01.png",
            ("Export "_s + _lvl->levelName).c_str()
        )) {
            pRet->autorelease();
            return pRet;
        }
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}
