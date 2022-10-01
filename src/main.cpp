#include "../include/GDShare.hpp"
#include <Geode/Loader.hpp>
#include <Geode/Modify.hpp>

USE_GEODE_NAMESPACE();
using namespace gdshare;

static auto IMPORT_PICK_OPTIONS = file::FilePickOptions {
    "",
    {
        {
            "Level Files",
            { "*.gmd2", "*.gmd", "*.lvl" }
        },
        {
            "GMD Files",
            { "*.gmd2", "*.gmd" }
        }
    }
};

class $modify(LevelBrowserLayer) {
    static void importFile(ghc::filesystem::path const& path) {
        auto import = ImportGmdFile::from(path);
        if (!import.tryInferType()) {
            // todo: show popup to pick type
            return FLAlertLayer::create(
                "Error Importing",
                "Unable to figure out <cy>file type</c>!",
                "OK"
            )->show();
        }
        auto res = import.intoLevel();
        if (!res) {
            return FLAlertLayer::create(
                "Error Importing",
                res.error(),
                "OK"
            )->show();
        }
        LocalLevelManager::get()->m_localLevels->insertObject(res.value(), 0);

        auto scene = CCScene::create();

        auto layer = LevelBrowserLayer::create(
            GJSearchObject::create(SearchType::MyLevels)
        );
        scene->addChild(layer);

        CCDirector::sharedDirector()->replaceScene(
            CCTransitionFade::create(.5f, scene)
        );
    }

    bool init(GJSearchObject* search) {
        if (!LevelBrowserLayer::init(search))
            return false;
        
        if (search->m_searchType == SearchType::MyLevels) {
            // 3rd button menu is the one with the 'New' button
            // todo: add some sort of common API to Geode for this
            auto btnMenu = getChildOfType<CCMenu>(this, 2);

            auto importBtn = CCMenuItemSpriteExtra::create(
                SafeCreate<CCSprite>()
                    .makeWithFrame("import.png"_spr)
                    .orMakeWithFrame<CCSprite>("GJ_plusBtn_001.png"),
                this,
                makeMenuSelector([this](CCObject*) {
                    if (auto file = file::pickFile(
                        file::PickMode::OpenFile,
                        IMPORT_PICK_OPTIONS
                    )) {
                        this->importFile(file.value());
                    }
                })
            );
            btnMenu->addChild(importBtn);

            auto newBtn = getChild(btnMenu, 0);
            importBtn->setPosition(
                newBtn->getPositionX(),
                newBtn->getPositionY() + 60.f
            );
        }

        return true;
    }
};
