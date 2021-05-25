#pragma once

#include "../offsets.hpp"

enum ExportFormat {
    gmd = 0,
    gmd2 = 1,
    lvl = 2
};

namespace gdshare {
    template<typename T>
    struct Result {
        bool success;
        T data;
    };

    namespace encoder {
        /**
         * Encode given vector with XOR
         * @param data The data to encode
         * @param key They key to use
        */
        std::vector<uint8_t> XOR(const std::vector<uint8_t> & data, int key);
        /**
         * Encode given vector with Base64
         * @param data The data to encode
        */
        std::vector<uint8_t> Base64(const std::vector<uint8_t> & data);
        /**
         * Encode given vector with GZip
         * @param data The data to encode
        */
        std::vector<uint8_t> GZip(const std::vector<uint8_t> & data);
    }

    namespace decoder {
        /**
         * Decode a string with XOR.
         * @param data The data to decode
         * @param key The key to use
        */
        std::string XOR(const std::string & data, int key);
        /**
         * Decode a string with Base 64.
         * @param data The data to decode
        */
        std::string Base64(const std::string & data);
        /**
         * Decode a string with GZip.
         * @param data The data to decode
        */
        std::string GZip(const std::string & data);

        // advanced versions for faster CCFile decoding

        /**
         * Decode a vector with XOR. To turn a string into a vector, use decoder::Convert.
         * @param data The data to decode
         * @param key The key to use
        */
        std::vector<uint8_t> XORX(const std::vector<uint8_t> & data, int key);
        /**
         * Decode a string with Base64. To turn a string into a vector, use decoder::Convert.
         * @param data The data to decode
        */
        std::vector<uint8_t> Base64X(const std::vector<uint8_t> & data);
        /**
         * Decode a string with GZip. To turn a string into a vector, use decoder::Convert.
         * @param data The data to decode
        */
        std::vector<uint8_t> GZipX(const std::vector<uint8_t> & data);

        // convert from string to vector

        /**
         * Turn a string into a vector of uint8_ts
         * @param string The string to convert
        */
        std::vector<uint8_t> Convert(const std::string & string);
        /**
         * Turn a vector of uint8_ts into a string
         * @param vector The vector to convert
        */
        std::string Convert(const std::vector<uint8_t> & vector);
    };

    const enum ExportFlags {
        EF_None             = 0b0000000,
        EF_IncludeSong      = 0b0000001,
    };

    bool saveFileFormat(std::string const&, std::string const&, ExportFormat const&, std::string const& = "", bool = false);
    std::string exportLevel(gd::GJGameLevel* const&, std::string const&, ExportFormat, int = ExportFlags::EF_None);
    Result<std::string> loadLevelFromFile(std::string const&, std::vector<uint8_t>* = nullptr, std::string * = nullptr);
    void removeNullbytesFromString(std::string &);

    inline constexpr const char* getExportTypeString(ExportFormat const& _type) {
        switch (_type) {
            case ExportFormat::gmd:     return "gmd";
            case ExportFormat::gmd2:    return "gmd2";
            case ExportFormat::lvl:     return "lvl";
        }

        return "gmd";
    }
};
