#pragma once

#include <Geode/DefaultInclude.hpp>
#include <Geode/utils/general.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <optional>

#ifdef GEODE_IS_WINDOWS
    #ifdef HJFOD_GDSHARE_EXPORTING
        #define GDSHARE_DLL __declspec(dllexport)
    #else
        #define GDSHARE_DLL __declspec(dllimport)
    #endif
#else
    #define GDSHARE_DLL
#endif

namespace gdshare {
    class ImportGmdFile;
    class ExportGmdFile;

    enum class GmdType {
        Lvl,
        Gmd,
        Gmd2,
    };

    constexpr auto DEFAULT_GMD_TYPE = GmdType::Gmd;
    constexpr auto GMD2_VERSION = 1;

    constexpr const char* gmdTypeToString(GmdType type) {
        switch (type) {
            case GmdType::Lvl:  return "lvl";
            case GmdType::Gmd:  return "gmd";
            case GmdType::Gmd2: return "gmd2";
            default:            return nullptr;
        }
    }

    constexpr std::optional<GmdType> gmdTypeFromString(const char* type) {
        using geode::utils::hash;
        switch (hash(type)) {
            case hash("lvl"):  return GmdType::Lvl;
            case hash("gmd"):  return GmdType::Gmd;
            case hash("gmd2"): return GmdType::Gmd2;
            default:           return std::nullopt;
        }
    }

    template<class T>
    class IGmdFile {
    protected:
        std::optional<GmdType> m_type;
    
    public:
        T& setType(GmdType type) {
            m_type = type;
            return *static_cast<T*>(this);
        }
    };

    class GDSHARE_DLL ImportGmdFile : public IGmdFile<ImportGmdFile> {
    protected:
        ghc::filesystem::path m_path;
        bool m_importSong = false;

        ImportGmdFile(ghc::filesystem::path const& path);

        geode::Result<std::string> getLevelData() const;

    public:
        static ImportGmdFile from(ghc::filesystem::path const& path);
        bool tryInferType();
        ImportGmdFile& inferType();
        ImportGmdFile& setImportSong(bool song);
        geode::Result<GJGameLevel*> intoLevel() const;
    };

    class GDSHARE_DLL ExportGmdFile : public IGmdFile<ExportGmdFile> {
    protected:
        GJGameLevel* m_level;

        ExportGmdFile(GJGameLevel* level);

    public:
        static ExportGmdFile from(GJGameLevel* level);
        geode::Result<geode::byte_array> intoBytes() const;
        geode::Result<> intoFile(ghc::filesystem::path const& path) const;
    };

    /**
     * Export a level as a GMD file. For more control over the exporting 
     * options, use the ExportGmdFile class.
     */
    GDSHARE_DLL geode::Result<> exportLevelAsGmd(
        GJGameLevel* level,
        ghc::filesystem::path const& to,
        GmdType type = DEFAULT_GMD_TYPE
    );

    /**
     * Import a level from a GMD file. For more control over the importing 
     * options, use the ImportGmdFile class.
     */
    GDSHARE_DLL geode::Result<GJGameLevel*> importGmdAsLevel(
        ghc::filesystem::path const& from
    );
}
