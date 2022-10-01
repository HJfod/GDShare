#include "../include/GDShare.hpp"
#include <Geode/utils/file.hpp>
#include <Geode/Bindings.hpp>

USE_GEODE_NAMESPACE();
using namespace gdshare;

// todo: get rid of this
#ifdef GEODE_IS_WINDOWS
std::string* std_string_operator_assign(std::string* str1, char* str2) {
    return reinterpret_cast<std::string*(__thiscall*)(std::string*, char*)>(
        geode::base::get() + 0xf680
    )(str1, str2);
}
#else
std::string* std_string_operator_assign(std::string* str1, char* str2) {
    str1->assign(str2);
    return str1;
}
#endif

static std::string extensionWithoutDot(ghc::filesystem::path const& path) {
    auto ext = path.extension().string();
    if (ext.size()) {
        return ext.substr(1);
    }
    return "";
}

ImportGmdFile::ImportGmdFile(
    ghc::filesystem::path const& path
) : m_path(path) {}

bool ImportGmdFile::tryInferType() {
    if (auto ext = gmdTypeFromString(extensionWithoutDot(m_path).c_str())) {
        m_type = ext.value();
        return true;
    }
    return false;
}

ImportGmdFile& ImportGmdFile::inferType() {
    if (auto ext = gmdTypeFromString(extensionWithoutDot(m_path).c_str())) {
        m_type = ext.value();
    } else {
        m_type = DEFAULT_GMD_TYPE;
    }
    return *this;
}

ImportGmdFile ImportGmdFile::from(ghc::filesystem::path const& path) {
    return ImportGmdFile(path);
}

Result<std::string> ImportGmdFile::getLevelData() const {
    if (!m_type) {
        return Err(
            "No file type set; either it couldn't have been inferred from the "
            "file or the developer of the mod forgot to call inferType"
        );
    }
    switch (m_type.value()) {
        case GmdType::Gmd: {
            return file::readString(m_path);
        } break;
    
        case GmdType::Lvl: {
            auto data = file::readBinary(m_path);
            if (!data) {
                return Err(data.error());
            }
            unsigned char* unzippedData;
            auto count = ZipUtils::ccInflateMemory(
                data.value().data(),
                data.value().size(),
                &unzippedData
            );
            auto str = std::string(
                reinterpret_cast<const char*>(unzippedData),
                count
            );
            free(unzippedData);
            return Ok(str);
        } break;

        case GmdType::Gmd2: {
        } break;

        default: {
            return Err("Unknown file type");
        } break;
    }
}

Result<GJGameLevel*> ImportGmdFile::intoLevel() const {
    auto data = getLevelData();
    if (!data) {
        return Err(data.error());
    }

    auto value =
        "<?xml version=\"1.0\"?><plist version=\"1.0\" gjver=\"2.0\"><dict><k>root</k>" +
        data.value() +
        "</dict></plist>";
    
    auto dict = std::make_unique<DS_Dictionary>();
    if (!dict->loadRootSubDictFromString(value)) {
        return Err("Unable to parse level data");
    }
    dict->stepIntoSubDictWithKey("root");

    auto level = GJGameLevel::create();
    level->dataLoaded(dict.get());

    level->m_isEditable = true;
    level->m_levelType = GJLevelType::Editor;

    return Ok(level);
}

ExportGmdFile::ExportGmdFile(GJGameLevel* level) : m_level(level) {}

ExportGmdFile ExportGmdFile::from(GJGameLevel* level) {
    return ExportGmdFile(level);
}

Result<byte_array> ExportGmdFile::intoBytes() const {
    // todo
    return Ok(byte_array());
}

Result<> ExportGmdFile::intoFile(ghc::filesystem::path const& path) const {
    // todo
    return Ok();
}

Result<> gdshare::exportLevelAsGmd(
    GJGameLevel* level,
    ghc::filesystem::path const& to,
    GmdType type
) {
    return ExportGmdFile::from(level).setType(type).intoFile(to);
}

Result<GJGameLevel*> gdshare::importGmdAsLevel(ghc::filesystem::path const& from) {
    return ImportGmdFile::from(from).inferType().intoLevel();
}
