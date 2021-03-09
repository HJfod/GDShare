#include <string>
#include <vector>
#include "gd/DS_Dictionary.hpp"

namespace gdshare {
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

    namespace filetypes {
        /**
         * Modern filetype, equivalent to a .zip.
         * Further specifications in Standards.md
        */
        static constexpr const char* GDShare2 = "gmd2";

        /**
         * Legacy GDShare filetype standard
         * Further specifications in Standards.md
        */
        static constexpr const char* GDShare = "gmd";

        /**
         * Spu7nix's LvlShare filetype standard
         * Further specifications in Standards.md
        */
        static constexpr const char* LvlShare = "lvl";

        /**
         * Default export type
        */
        static constexpr const char* Default = GDShare2;
    }

    DS_Dictionary* parseFile(const std::string &);
    std::string decodeFile(const std::string &);
}