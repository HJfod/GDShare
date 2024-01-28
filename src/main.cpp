
#include <Geode/Loader.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/EditLevelLayer.hpp>
#include <Geode/modify/IDManager.hpp>
#include <Geode/ui/Popup.hpp>
#include <hjfod.gmd-api/include/GMD.hpp>

using namespace geode::prelude;
using namespace gmd;

static auto IMPORT_PICK_OPTIONS = file::FilePickOptions {
    std::nullopt,
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

class ExportLevelLayer : public Popup<GJGameLevel*> {
protected:
    GJGameLevel* m_level;

    bool setup(GJGameLevel* level) {
        m_noElasticity = true;
        return true;
    }

public:
    static ExportLevelLayer* create(GJGameLevel* level) {
        auto ret = new ExportLevelLayer();
        if (ret && ret->init(348.f, 230.f, level)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};

void promptExportLevel(GJGameLevel* level) {
    auto opts = IMPORT_PICK_OPTIONS;
    opts.defaultPath = std::string(level->m_levelName) + ".gmd";
    file::pickFile(file::PickMode::SaveFile, opts, [=](auto path){
        auto res = exportLevelAsGmd(level, path);
        if (res) {
            createQuickPopup(
                "Exported",
                "Succesfully exported level",
                "OK", "Open File",
                [path](auto, bool btn2) {
                    if (btn2) file::openFolder(path);
                }
            );
        }
        else {
            FLAlertLayer::create(
                "Error",
                "Unable to export: " + res.unwrapErr(),
                "OK"
            )->show();
        }
    });
}

struct $modify(ExportMyLevelLayer, EditLevelLayer) {
    bool init(GJGameLevel* level) {
        if (!EditLevelLayer::init(level))
            return false;
        
        auto menu = this->getChildByID("level-actions-menu");
        if (menu) {
            auto btn = CCMenuItemSpriteExtra::create(
                CircleButtonSprite::createWithSpriteFrameName(
                    "file.png"_spr, .525f,
                    CircleBaseColor::Green,
                    CircleBaseSize::MediumAlt
                ),
                this, menu_selector(ExportMyLevelLayer::onExport)
            );
            btn->setID("export-button"_spr);
            menu->addChild(btn);
            menu->updateLayout();
        }

        return true;
    }

    void onExport(CCObject*) {
        promptExportLevel(m_level);
    }
};

struct $modify(ExportOnlineLevelLayer, LevelInfoLayer) {
    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge))
            return false;
        
        auto menu = this->getChildByID("left-side-menu");
        if (menu) {
            auto btn = CCMenuItemSpriteExtra::create(
                CircleButtonSprite::createWithSpriteFrameName(
                    "file.png"_spr, .475f,
                    CircleBaseColor::Green,
                    CircleBaseSize::Medium
                ),
                this, menu_selector(ExportOnlineLevelLayer::onExport)
            );
            btn->setID("export-button"_spr);
            menu->addChild(btn);
            menu->updateLayout();
        }

        return true;
    }

    void onExport(CCObject*) {
        promptExportLevel(m_level);
    }
};

struct $modify(ImportLayer, LevelBrowserLayer) {
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

    void onImport(CCObject*) {
        file::pickFile(
            file::PickMode::OpenFile,
            IMPORT_PICK_OPTIONS,
            [=](auto file) {
                this->importFile(file);
            }
        );
    }

    bool init(GJSearchObject* search) {
        if (!LevelBrowserLayer::init(search))
            return false;

        if (search->m_searchType == SearchType::MyLevels) {
            auto btnMenu = this->getChildByID("new-level-menu");

            auto importBtn = CCMenuItemSpriteExtra::create(
                CircleButtonSprite::createWithSpriteFrameName(
                    "file.png"_spr, .575f,
                    CircleBaseColor::Pink,
                    CircleBaseSize::Big
                ),
                this,
                menu_selector(ImportLayer::onImport)
            );
            importBtn->setID("import-level-button"_spr);
            btnMenu->addChild(importBtn);
            btnMenu->updateLayout();
        }

        return true;
    }
};
