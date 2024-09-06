#ifndef ASCANPROCESSOR_HPP
#define ASCANPROCESSOR_HPP

#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <optional>
#include <string>
#include <hdf5.h>
#include <rapidjson/schema.h>
#include <rapidjson/document.h>
#include "../Source/PAUTFileReader/AscanDattype.hpp"
#include "../Source/OmConnect/readconfig.h"
#include "..\Source\MainUI\statuslogs.h"

namespace rj = rapidjson;
namespace fs = std::filesystem;

class nFrame;  // Forward declaration cho nFrame

class AscanProcessor {
public:
    AscanProcessor();
    ~AscanProcessor() {};

    bool analyze( nFrame* frame);
    AscanData ReadAscanData(hid_t fileId_, const rj::Value& ascanDataset_);

private:
    std::string ReadJsonSetup(hid_t fileId);
    rj::ParseErrorCode DecodeJs(const std::string& jsonSetup);
    // void ParseAscanStatuses(const rj::Value& ascanDataset, const AscanData& ascanData);

    hid_t fileId;
    rj::Document jsonDoc;
    fs::path NDEfilePath;
    fs::path ExportedFile;
    fs::path schemaPathFilename;
    nmainUI::statuslogs* sttlogs;

};

#endif // ASCANPROCESSOR_HPP

