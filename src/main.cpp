#include <Geode/Loader.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/EditLevelLayer.hpp>
#include <Geode/modify/IDManager.hpp>
#include <Geode/modify/LevelListLayer.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/utils/async.hpp>
#include <hjfod.gmd-api/include/GMD.hpp>

using namespace geode::prelude;
using namespace gmd;

static auto IMPORT_PICK_OPTIONS = file::FilePickOptions {
    std::nullopt,
    {
        {
            "GD Level Files",
            { "*.gmd", "*.gmdl", "*.gmd2", "*.lvl" } // importing gmd2 and lvl files work (also pls thank me (fijiaura) in the changelog)
        }
    }
};

static auto EXPORT_FOLDER_OPTIONS = file::FilePickOptions {
    std::nullopt,
    {
        {
            "Folders",
            { "" }
        }
    }
};

template <class L>
static arc::Future<file::PickResult> promptExportLevel(L* level) {
    auto opts = IMPORT_PICK_OPTIONS;
    if constexpr (std::is_same_v<L, GJLevelList>) {
        opts.defaultPath = std::string(level->m_listName) + ".gmdl";
    }
    else {
        opts.defaultPath = std::string(level->m_levelName) + ".gmd";
    }
    return file::pick(file::PickMode::SaveFile, opts);
}
template <class L>
static void onExportFilePick(L* level, file::PickResult result) {
    if (result.isOk()) {
        auto path = std::move(result).unwrap();
        if (!path) return;
        std::optional<std::string> err;
        if constexpr (std::is_same_v<L, GJLevelList>) {
            err = exportListAsGmd(level, *path).err();
        }
        else {
            err = exportLevelAsGmd(level, *path).err();
        }
        if (!err) {
            createQuickPopup(
                "Exported",
                (std::is_same_v<L, GJLevelList> ?
                    "Succesfully exported list" :
                    "Succesfully exported level"
                ),
                "OK", "Open File",
                [path](auto, bool btn2) {
                    if (btn2) file::openFolder(*path);
                }
            );
        }
        else {
            FLAlertLayer::create(
                "Error",
                "Unable to export: " + err.value(),
                "OK"
            )->show();
        }
    }
    else {
        FLAlertLayer::create("Error Exporting", result.unwrapErr(), "OK")->show();
    }
}

template <class L>
static void exportMany(std::vector<L*> levels, file::PickResult result) {
    if (result.isOk()) {
        auto optPath = std::move(result).unwrap();
        if (!optPath) return;
        auto path = std::move(*optPath);
        
        std::vector<std::optional<std::string>> errs;
        for (auto level : levels) {
            std::optional<std::string> err;
            if constexpr (std::is_same_v<L, GJLevelList>) {
                err = exportListAsGmd(level, path / (std::string(level->m_listName) + ".gmdl")).err();
            }
            else {
                err = exportLevelAsGmd(level, path / (std::string(level->m_levelName) + ".gmd")).err();
            }
            if (err) errs.push_back(err);
        }
        if (errs.empty()) {
            createQuickPopup(
                "Exported",
                (std::is_same_v<L, GJLevelList> ?
                    fmt::format("Succesfully exported {} list{}", levels.size(), levels.size() == 1 ? "s" : "") :
                    fmt::format("Succesfully exported {} levels{}", levels.size(), levels.size() == 1 ? "s" : "") 
                ),
                "OK", "Open Folder",
                [path](auto, bool btn2) {
                    if (btn2) file::openFolder(path);
                }
            );
        }
        else {
            auto successPortion = (std::is_same_v<L, GJLevelList> ?
                fmt::format("Succesfully exported {} list{}", levels.size() - errs.size(), levels.size() - errs.size() == 1 ? "s" : "") :
                fmt::format("Succesfully exported {} levels{}", levels.size() - errs.size(), levels.size() - errs.size() == 1 ? "s" : "") 
            );
            auto formattedErrsAndSuccess = fmt::format("Several errors occurred:\n- {}\n\n{}", fmt::join(errs, "\n- "), successPortion);
            createQuickPopup(
                "Exported with Errors",
                formattedErrsAndSuccess,
                "OK", "Open Folder",
                [path](auto, bool btn2) {
                    if (btn2) file::openFolder(path);
                }
            );
        }
    }
    else {
        FLAlertLayer::create("Error Exporting", result.unwrapErr(), "OK")->show();
    }
}

struct $modify(ExportMyLevelLayer, EditLevelLayer) {
    struct Fields {
        async::TaskHolder<file::PickResult> pickListener;
    };

    $override
    bool init(GJGameLevel* level) {
        if (!EditLevelLayer::init(level))
            return false;
        
        auto menu = this->getChildByID("level-actions-menu");
        if (menu) {
            auto btn = CCMenuItemSpriteExtra::create(
                CircleButtonSprite::createWithSpriteFrameName(
                    "file.png"_spr, .8f,
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
        m_fields->pickListener.spawn(
            promptExportLevel(m_level),
            [level = m_level](file::PickResult ev) {
                onExportFilePick(level, std::move(ev));
            }
        );
    }
};

struct $modify(ExportOnlineLevelLayer, LevelInfoLayer) {
    struct Fields {
        async::TaskHolder<file::PickResult> pickListener;
    };

    $override
    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge))
            return false;
        
        auto menu = this->getChildByID("left-side-menu");
        if (menu) {
            auto btn = CCMenuItemSpriteExtra::create(
                CircleButtonSprite::createWithSpriteFrameName(
                    "file.png"_spr, .8f,
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
        m_fields->pickListener.spawn(
            promptExportLevel(m_level),
            [level = m_level](file::PickResult ev) {
                onExportFilePick(level, std::move(ev));
            }
        );
    }
};

struct $modify(ImportLayer, LevelBrowserLayer) {
    struct Fields {
        async::TaskHolder<file::PickManyResult> importListener;
        async::TaskHolder<file::PickResult> exportListener;
    };

    static void importFiles(std::vector<std::filesystem::path> const& paths) {
        for (auto const& path : paths) {
            switch (getGmdFileKind(path)) {
                case GmdFileKind::List: {
                    auto res = gmd::importGmdAsList(path);
                    if (res) {
                        LocalLevelManager::get()->m_localLists->insertObject(*res, 0);
                    }
                    else {
                        return FLAlertLayer::create("Error Importing", res.unwrapErr(), "OK")->show();
                    }
                } break;

                case GmdFileKind::Level: {
                    auto res = gmd::importGmdAsLevel(path);
                    if (res) {
                        LocalLevelManager::get()->m_localLevels->insertObject(*res, 0);
                    }
                    else {
                        return FLAlertLayer::create("Error Importing", res.unwrapErr(), "OK")->show();
                    }
                } break;

                case GmdFileKind::None: {
                    // todo: show popup to pick type
                    return FLAlertLayer::create(
                        "Error Importing",
                        fmt::format("Selected file '<cp>{}</c>' is not a GMD file!", path),
                        "OK"
                    )->show();
                } break;
            }
        }

        auto scene = CCScene::create();
        auto layer = LevelBrowserLayer::create(
            GJSearchObject::create(SearchType::MyLevels)
        );
        scene->addChild(layer);
        CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(.5f, scene));
    }

    void onImport(CCObject*) {
        m_fields->importListener.spawn(
            file::pickMany(IMPORT_PICK_OPTIONS),
            [](file::PickManyResult result) {
                if (result.isOk()) {
                    importFiles(std::move(result).unwrap());
                }
                else {
                    FLAlertLayer::create("Error Importing", result.unwrapErr(), "OK")->show();
                }
            }
        );
    }

    void onExport(CCObject*) {
        if (m_searchObject->m_searchType != SearchType::MyLevels) return;
        size_t count = 0;
        std::vector<GJGameLevel*> levelsForExporting = {};
        for (auto level : CCArrayExt<GJGameLevel*>(m_levels)) {
            if (!level->m_selected) continue;
            count += 1;
            levelsForExporting.push_back(level);
        }
        if (count < 1 || levelsForExporting.empty()) {
            FLAlertLayer::create("Nothing here...", "No levels selected for export.", "OK")->show();
        }
        else {
            m_fields->exportListener.spawn(
                file::pick(file::PickMode::OpenFolder, EXPORT_FOLDER_OPTIONS),
                [levelsForExporting](file::PickResult r) {
                    exportMany(levelsForExporting, std::move(r));
                }
            );
        }
    }

    $override
    bool init(GJSearchObject* search) {
        if (!LevelBrowserLayer::init(search))
            return false;

        if (search->m_searchType == SearchType::MyLevels || search->m_searchType == SearchType::MyLists) {
            auto btnMenu = this->getChildByID("new-level-menu");

            auto importBtn = CCMenuItemSpriteExtra::create(
                CircleButtonSprite::createWithSpriteFrameName(
                    "file.png"_spr, .85f,
                    CircleBaseColor::Pink,
                    CircleBaseSize::Big
                ),
                this,
                menu_selector(ImportLayer::onImport)
            );
            importBtn->setID("import-level-button"_spr);

            // This one has an ID but no layout which is CRINGE
            if (search->m_searchType == SearchType::MyLists && search->m_searchIsOverlay) {
                btnMenu->addChildAtPosition(importBtn, Anchor::BottomLeft, ccp(0, 60), false);
            }
            else {
                btnMenu->addChild(importBtn);
                btnMenu->updateLayout();
            }

            auto otherBtnMenu = this->getChildByID("my-levels-menu");

            if (otherBtnMenu && search->m_searchType == SearchType::MyLevels && !search->m_searchIsOverlay) {
                auto exportBtn = CCMenuItemSpriteExtra::create(
                    CircleButtonSprite::createWithSpriteFrameName(
                        "file.png"_spr, .85f,
                        CircleBaseColor::Cyan,
                        CircleBaseSize::Big
                    ),
                    this,
                    menu_selector(ImportLayer::onExport)
                );
                exportBtn->setID("export-level-button"_spr);
                otherBtnMenu->addChild(exportBtn);
                otherBtnMenu->updateLayout();
            }
        }

        return true;
    }
};

struct $modify(ExportListLayer, LevelListLayer) {
    struct Fields {
        async::TaskHolder<file::PickResult> pickListener;
    };

    $override
    bool init(GJLevelList* level) {
        if (!LevelListLayer::init(level))
            return false;
        
        if (auto menu = this->getChildByID("left-side-menu")) {
            auto btn = CCMenuItemSpriteExtra::create(
                CircleButtonSprite::createWithSpriteFrameName(
                    "file.png"_spr, .8f,
                    CircleBaseColor::Green,
                    CircleBaseSize::Medium
                ),
                this, menu_selector(ExportListLayer::onExport)
            );
            btn->setID("export-button"_spr);
            menu->addChild(btn);
            menu->updateLayout();
        }

        return true;
    }

    void onExport(CCObject*) {
        m_fields->pickListener.spawn(
            promptExportLevel(m_levelList),
            [list = m_levelList](file::PickResult ev) {
                onExportFilePick(list, std::move(ev));
            }
        );
    }
};

