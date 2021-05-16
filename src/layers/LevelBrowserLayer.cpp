#include "LevelBrowserLayer.hpp"
#include "../utils/gdshare.hpp"
#include "../utils/helpers.hpp"

void doImport(std::string const& _data) {
    tinyxml2::XMLDocument doc;

    auto parseRes = doc.Parse(_data.c_str());

    if (parseRes == tinyxml2::XMLError::XML_NO_ERROR) {
        auto addedLevel = gd::GameLevelManager::createNewLevel();

        addedLevel->levelName = "Unknown";

        for (auto child = doc.FirstChildElement("d")->FirstChildElement("k"); child; child = child->NextSiblingElement("k"))
            switch (h$(child->GetText())) {
                case h$("k2"): {
                    auto data = child->NextSiblingElement()->GetText();
                    if (!data) break;

                    addedLevel->levelName = data;
                } break;

                case h$("k3"): {
                    auto descData = child->NextSiblingElement()->GetText();
                    if (!descData)
                        break;

                    auto desc = std::string(descData);

                    if (desc.length())
                        desc = gdshare::decoder::Base64(desc);
                    
                    addedLevel->levelDesc = desc;
                } break;

                case h$("k4"): {
                    const char* lvlStr = child->NextSiblingElement()->GetText();

                    if (lvlStr == nullptr) break;

                    // spooky string assignment shit because
                    // std::string is faulty for some reason
                    // huge thank you to mat

                    auto len = strlen(lvlStr);
                    auto addedLvlStr = as<uintptr_t>(addedLevel) + 0x12c;

                    *as<size_t*>(addedLvlStr + 16) = len;   // length
                    *as<size_t*>(addedLvlStr + 20) = max(len, 15); // capacity

                    if (len <= 15)
                        memcpy(reinterpret_cast<char*>(addedLvlStr), lvlStr, len + 1);
                    else {
                        void* newb = malloc(len + 1);
                        memcpy(newb, lvlStr, len + 1);
                        *reinterpret_cast<void**>(addedLvlStr) = newb;
                    }
                } break;

                case h$("k8"): {
                    addedLevel->audioTrack = std::stoi(child->NextSiblingElement()->GetText());
                } break;

                case h$("k45"): {
                    addedLevel->songID = std::stoi(child->NextSiblingElement()->GetText());
                } break;
            }

        auto scene = cocos2d::CCScene::create();

        auto blayer = LevelBrowserLayer::create(
            gd::GJSearchObject::create(gd::SearchType::MyLevels)
        );

        scene->addChild(blayer);

        cocos2d::CCDirector::sharedDirector()->replaceScene(
            cocos2d::CCTransitionFade::create(0.5f, scene)
        );
    } else
        gd::FLAlertLayer::create(
            nullptr,
            "Error importing",
            "OK", nullptr,
            "<cr>Error</c>: <ca>XML</c> parse error ("_s + std::to_string(parseRes) + ")"
        )->show();

        // gd::GameSoundManager::playSound("gdshare_very_important_export_sound_effect.mp3");
}

// check if first 512 bytes of
// two vectors are the same
bool compareVectors(std::vector<uint8_t> const& vec1, std::vector<uint8_t> const& vec2) {
    for (auto ix = 0u; ix < 512u; ix++)
        if (vec1.at(ix) != vec2.at(ix))
            return false;
    
    return true;
}

class SongFileQuestion : public gd::FLAlertLayerProtocol {
    private:
        std::string levelData;
        std::vector<uint8_t> songContent;
        std::string songFile;

    public:
        void FLAlert_Clicked(gd::FLAlertLayer*, bool btn2) override {
            if (btn2) {
                std::filesystem::path newName = this->songFile;
                while (std::filesystem::exists(newName))
                    newName.replace_filename(newName.stem().string() + "_.mp3");

                std::filesystem::rename(songFile, newName);

                saveFileBinary(songFile, songContent);
            }

            doImport(this->levelData);
        }

        SongFileQuestion(std::string const& _data, std::vector<uint8_t> const& _cont, std::string const& _file)
            : levelData(_data), songContent(_cont), songFile(_file) {}
};

void LevelBrowserLayer::importLevel(std::string const& _fname) {
    std::string songFile;
    std::vector<uint8_t> songContent;
    auto lvl = gdshare::loadLevelFromFile(_fname, &songContent, &songFile);

    if (lvl.success) {
        gdshare::removeNullbytesFromString(lvl.data);

        if (songFile.size()) {
            if (std::filesystem::exists(songFile)) {
                auto oFile = readFileBinary(songFile);

                if (oFile.size() == songContent.size())
                    if (compareVectors(oFile, songContent))
                        return doImport(lvl.data);

                gd::FLAlertLayer::create(
                    new SongFileQuestion(lvl.data, songContent, songFile),
                    "Import Song",
                    "No", "Yes",
                    350.0f,
                    "This level was exported with the <cc>song</c> file, but it seems "_s
                    "you already have a file with the <co>same ID</c>. Would you like "
                    "to <cl>switch</c> it to the level's song file? (You will not lose either file)"
                )->show();

                return;
            }

            saveFileBinary(songFile, songContent);
        }
        
        doImport(lvl.data);
    } else
        gd::FLAlertLayer::create(
            nullptr,
            "Error importing",
            "OK", nullptr,
            "<cr>Error</c>: "_s + lvl.data
        )->show();
}

void LevelBrowserLayer::onImport(cocos2d::CCObject* pSender) {
    nfdchar_t* path = nullptr;
    const nfdchar_t* filter = "gmd,gmd2,lvl";
    nfdresult_t res = NFD_OpenDialog(filter, nullptr, &path);

    if (res == NFD_OKAY) {
        this->importLevel(path);

        free(path);
    }
}

bool __fastcall LevelBrowserLayer::initHook(LevelBrowserLayer* _self, uintptr_t, gd::GJSearchObject* _sobj) {
    if (!init(_self, _sobj))
        return false;

    if (_sobj->getType() == gd::SearchType::MyLevels) {
        cocos2d::CCMenu* btnMenu = getChild<cocos2d::CCMenu*>(
            _self, 8
        );

        auto importButton = gd::CCMenuItemSpriteExtra::create(
            cocos2d::CCSprite::create("BE_Import_File.png"),
            btnMenu,
            (cocos2d::SEL_MenuHandler)&LevelBrowserLayer::onImport
        );

        cocos2d::CCNode* newBtn = getChild<cocos2d::CCNode*>(btnMenu, 0);

        btnMenu->addChild(importButton);

        importButton->setPosition(newBtn->getPositionX(), newBtn->getPositionY() + 60.0f);
    }

    return true;
}
