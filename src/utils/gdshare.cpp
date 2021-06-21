#include "gdshare.hpp"
#include "helpers.hpp"
#include "../ext/Base64.hpp"
#include "../ext/ZlibHelper.hpp"

#include <zipper.h>
#include <unzipper.h>

// fix json.hpp not being fucked wucked
#undef snprintf
#include "../ext/json.hpp"

std::vector<uint8_t> gdshare::decoder::Convert(const std::string & _data) {
    return std::vector<uint8_t> (_data.begin(), _data.end());
}

std::string gdshare::decoder::Convert(const std::vector<uint8_t> & _data) {
    return std::string(_data.begin(), _data.end());
}


std::vector<uint8_t> gdshare::decoder::XORX(const std::vector<uint8_t> & _data, int _key) {
    std::vector<uint8_t> copy(_data);

    for (unsigned int ix = 0; ix < copy.size(); ix++)
        copy[ix] ^= _key;

    return copy;
}

std::vector<uint8_t> gdshare::decoder::Base64X(const std::vector<uint8_t> & _data) {
    gdcrypto::base64::Base64 b64(gdcrypto::base64::URL_SAFE_DICT);
    return b64.decode(_data);
}

std::vector<uint8_t> gdshare::decoder::GZipX(const std::vector<uint8_t> & _data) {
    return gdcrypto::zlib::inflateBuffer(_data);
}


std::string gdshare::decoder::GZip(const std::string & _data) {
    std::vector<uint8_t> data = gdshare::decoder::Convert(_data);

    return gdshare::decoder::Convert(gdshare::decoder::GZipX(data));
}

std::string gdshare::decoder::Base64(const std::string & _data) {
    gdcrypto::base64::Base64 b64(gdcrypto::base64::URL_SAFE_DICT);

    std::vector<uint8_t> data = b64.decode(_data);

    return gdshare::decoder::Convert(data);
}

std::string gdshare::decoder::XOR(const std::string & _data, int _key) {
    std::vector<uint8_t> data = gdshare::decoder::Convert(_data);

    for (uint8_t b : data)
        b ^= _key;

    return gdshare::decoder::Convert(data);
}


std::vector<uint8_t> gdshare::encoder::GZip(const std::vector<uint8_t> & _data) {
    return gdcrypto::zlib::deflateBuffer(_data);
}

std::vector<uint8_t> gdshare::encoder::Base64(const std::vector<uint8_t> & _data) {
    gdcrypto::base64::Base64 b64(gdcrypto::base64::URL_SAFE_DICT);

    std::string data = b64.encode(_data);

    return gdshare::decoder::Convert(data);
}

std::vector<uint8_t> gdshare::encoder::XOR(const std::vector<uint8_t> & _data, int _key) {
    // xor is two-way so i can literally
    // just call the decoder XOR function

    return gdshare::decoder::XORX(_data, _key);
}

void gdshare::removeNullbytesFromString(std::string & str) {
    // fix for XML Parse Error 10
    for (auto i = 0u; i < str.size(); i++)
        if (!str.at(i))
            str.at(i) = 32;
}

bool gdshare::saveFileFormat(
    std::string const& _path,
    std::string const& _data,
    ExportFormat const& _type,
    std::string const& _songpath,
    bool _customsong
) {
    switch (_type) {
        case ExportFormat::gmd2: {
            nlohmann::json metajson = nlohmann::json::object({
                { "compression", "none" }
            });

            if (_songpath.size()) {
                metajson["song-file"] = std::filesystem::path(_songpath).filename().string();
                metajson["song-is-custom"] = _customsong;
            }

            std::string metadata = metajson.dump();

            if (std::filesystem::exists(_path))
                if (!std::filesystem::remove(_path))
                    return false;

            zipper::Zipper zip (_path);

            std::istringstream dataStream (_data);
            std::istringstream metaStream (metadata);

            zip.add(dataStream, "level.data");
            zip.add(metaStream, "level.meta");

            if (_songpath.size())
                zip.add(_songpath);

            zip.close();

            return true;
        } break;
        
        case ExportFormat::gmd: {
            return saveFileText(_path, _data);
        } break;

        case ExportFormat::lvl: {
            std::string data = _data.substr(3, _data.length() - 7);

            return saveFileBinary(
                _path,
                gdcrypto::zlib::deflateBuffer(
                    gdshare::decoder::Convert(data)
                )
            );
        } break;
    }

    return false;
}

std::string gdshare::exportLevel(
    gd::GJGameLevel* const& _lvl,
    std::string const& _path,
    ExportFormat _type,
    int _flags
) {
    if (!_lvl->levelString.size())
        return "Level string is empty!";

    std::string song;

    if (_lvl->songID)
        song = "<k>k45</k><i>" + std::to_string(_lvl->songID) + "</i>";
    else
        if (_lvl->audioTrack)
            song = "<k>k8</k><i>" + std::to_string(_lvl->audioTrack) + "</i>";


    // jesus fucking christ

    std::stringstream data;

    {
        data
        << "<d>"
            << "<k>kCEK</k>"
            << "<i>4</i>"
            << "<k>k2</k>"
            << "<s>" << _lvl->levelName << "</s>"
            << "<k>k3</k>"
            << "<s>" <<
                gdshare::decoder::Convert(
                    gdshare::encoder::Base64(
                        gdshare::decoder::Convert(
                            _lvl->levelDesc
                        )
                    )
                )
            << "</s>"
            << "<k>k4</k>"
            << "<s>" <<
                // levelString is already stored encoded
                _lvl->levelString
            << "</s>"
            << song
            << "<k>k13</k>"
            << "<t/>"
            << "<k>k21</k>"
            << "<i>2</i>"
            << "<k>k50</k>"
            << "<i>35</i>"
        << "</d>";
    }
    
    std::string songFilePath = "";
    if (_flags & ExportFlags::EF_IncludeSong)
        songFilePath = _lvl->getAudioFileName();
    
    auto dataStr = data.str();

    removeNullbytesFromString(dataStr);

    if (!gdshare::saveFileFormat(_path, dataStr, _type, songFilePath, _lvl->songID))
        return "Unable to save file!\n";
    
    return "";
}

gdshare::Result<std::string> gdshare::loadLevelFromFile(
    std::string const& _path,
    std::vector<uint8_t> * _song,
    std::string * _songpath
) {
    if (!std::filesystem::exists(_path))
        return { false, "File does not exist! (Likely reason is that the path or "
                        "filename contains <co>unrecognized</c> characters; <cy>Move</c> "
                        "the file to a different location and try again)" };

    std::string type = std::filesystem::path(_path).extension().string();

    if (!type.length())
        return { false, "Unable to descern file type!" };
    // remove .
    type = type.substr(1);

    switch (h$(type.c_str())) {
        case h$(gdshare::getExportTypeString(ExportFormat::gmd2)): {
            zipper::Unzipper zip (_path);

            std::vector<uint8_t> metaBuffer;
            std::vector<uint8_t> dataBuffer;

            zip.extractEntryToMemory("level.meta", metaBuffer);
            zip.extractEntryToMemory("level.data", dataBuffer);

            if (!metaBuffer.size())
                return { false, "Unable to read level metadata!" };

            if (!dataBuffer.size())
                return { false, "Unable to read level data!" };
            
            std::string metadata = gdshare::decoder::Convert(metaBuffer);
            std::string data = gdshare::decoder::Convert(dataBuffer);

            nlohmann::json metaj;
            try {
                metaj = nlohmann::json::parse(metadata);
            } catch (...) {
                zip.close();

                return { false, "Unable to parse metadata!" };
            }

            try {
                std::string songfile = metaj["song-file"];

                if (songfile.size()) {
                    if (_song) {
                        if (!zip.extractEntryToMemory(songfile, *_song))
                            goto skip_song_file;

                        std::string targetPath;
                        if (metaj["song-is-custom"]) {
                            try {
                                targetPath = gd::MusicDownloadManager::pathForSong(
                                    std::stoi(songfile.substr(0, songfile.find_first_of(".")))
                                );
                            } catch (...) {
                                goto skip_song_file;
                            }
                        } else
                            // official songs are always in resources
                            targetPath = "Resources/" + songfile;

                        if (_songpath)
                            *_songpath = targetPath;
                    }
                }
            } catch (...) {}
        skip_song_file:

            zip.close();
            
            std::string compr = metaj["compression"];

            compr = sanitizeString(compr);

            data = decodeCompression(data, compr);

            if (!data.size())
                return { false, "Unable to decode compression!" };

            return { true, data };
        } break;

        case h$(gdshare::getExportTypeString(ExportFormat::gmd)): {
            std::string data = readFileString(_path);

            if (!data.size())
                return { false, "File is empty" };

            return { true, data };
        } break;

        case h$(gdshare::getExportTypeString(ExportFormat::lvl)): {
            std::string datac = readFileString(_path);

            if (!datac.length())
                return { false, "File is empty" };
            
            std::string data = gdshare::decoder::GZip(datac);

            data = "<d>" + data + "</d>";

            return { true, data };
        } break;

        default:
            return { false, "Unknown file type" };
    }

    return { false, "How did you even manage to get this error" };
}
