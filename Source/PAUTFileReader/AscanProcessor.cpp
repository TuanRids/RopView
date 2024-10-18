
#include "AscanProcessor.h"
#include <MainUI/FactoryMgr.h>
#include <fstream>
#include <hdf5.h>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <Windows.h>


AscanProcessor::AscanProcessor() {
    if (!sttlogs) { sttlogs = &nmainUI::statuslogs::getinstance(); }
    NDEfilePath = fs::current_path() / ("sample\\DataFile_2024_08_23_11h53m57s.nde");
    //NDEfilePath = fs::current_path() / ("sample\\H402F_N3A_2024_08_16_11h46m32s.nde");
    //NDEfilePath = fs::current_path() / ("sample\\DLC.nde");
    ExportedFile = fs::current_path() / ("result");
}


bool AscanProcessor::analyze(nFrame* frame) {
    try {
        fileId = H5Fopen(NDEfilePath.string().c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
        std::string jsonSetup = ReadJsonSetup(fileId);
        auto decodingresult_ = DecodeJs(jsonSetup);
        if (decodingresult_ == rj::kParseErrorNone) {
            jsonDoc.Parse(jsonSetup.c_str());
            // PrintJSON(jsonDoc);
            for (const auto& group : jsonDoc["groups"].GetArray()) {
                if (group["dataset"].HasMember("ascan")) {
                    auto& ascanDataset = group["dataset"]["ascan"];
                    auto result = ReadAscanData(fileId, ascanDataset);
                    frame->setScandat(result);
                    std::unique_ptr<IDataProcecss> object = std::make_unique<DataProcess>();
                    object->getAscanData(result);
                }
            }
        }
    }
    catch (const std::exception& e) {
        H5Fclose(fileId);
        sttlogs->logWarning("Error: " + std::string(e.what()));
        return false;
    }
    catch (...) {
        H5Fclose(fileId);
        sttlogs->logWarning("Unknown error occurred.");
        return false;
    }
    H5Fclose(fileId);
    return true;
}

std::string AscanProcessor::ReadJsonSetup(hid_t fileId) {
    hid_t dsetId = H5Dopen(fileId, "/Domain/Setup", H5P_DEFAULT);
    hid_t dataType = H5Dget_type(dsetId);
    std::vector<char> jsonData(H5Tget_size(dataType));
    H5Dread(dsetId, dataType, H5S_ALL, H5S_ALL, H5P_DEFAULT, jsonData.data());
    H5Dclose(dsetId);
    return std::string(jsonData.begin(), jsonData.end());
}

rj::ParseErrorCode AscanProcessor::DecodeJs(const std::string& jsonSetup) {
    rj::Document doc;
    doc.Parse(jsonSetup.c_str());
    if (doc.HasParseError()) {        return doc.GetParseError();    }
    jsonDoc = std::move(doc);
    return rj::kParseErrorNone;
}

AscanData AscanProcessor::ReadAscanData(hid_t fileId_, const rj::Value& ascanDataset_) {
    AscanData ascanData;
    auto& amplitude = ascanDataset_["amplitude"];
    ascanData.AmplitudeRng.PercentMin = amplitude["dataValue"]["min"].GetDouble();
    ascanData.AmplitudeRng.PercentMax = amplitude["dataValue"]["max"].GetDouble();
    ascanData.AmplitudeRng.SamplingMin = amplitude["dataSampling"]["min"].GetInt64();
    ascanData.AmplitudeRng.SamplingMax = amplitude["dataSampling"]["max"].GetInt64();

    for (const auto& dimension : amplitude["dimensions"].GetArray())
    {
        DimensionAxis dimAxis;
        dimAxis.Name = dimension["axis"].GetString();

        if (dimAxis.Name == "UCoordinate")
        {
            if (dimension.HasMember("offset")) {
                dimAxis.Offset = dimension["offset"].GetDouble();
            }

            dimAxis.Quantity = dimension["quantity"].GetUint64();
            dimAxis.Resolution = dimension["resolution"].GetDouble();
            ascanData.AmplitudeAxes.push_back(dimAxis);
            ascanData.StatusAxes.push_back(dimAxis);
        }
        else if (dimAxis.Name == "VCoordinate")
        {
            dimAxis.Quantity = dimension["quantity"].GetUint64();
            dimAxis.Offset = dimension["offset"].GetDouble();
            dimAxis.Resolution = dimension["resolution"].GetDouble();
            ascanData.AmplitudeAxes.push_back(dimAxis);
            ascanData.StatusAxes.push_back(dimAxis);

            ascanData.Velocity = ascanDataset_["velocity"].GetDouble();
            ascanData.SkewAngle = ascanDataset_["skewAngle"].GetDouble();
            ascanData.RefractedAngle = ascanDataset_["refractedAngle"].GetDouble();
        }
        else if (dimAxis.Name == "Ultrasound")
        {
            if (dimension.HasMember("offset")) {
                dimAxis.Offset = dimension["offset"].GetDouble();
            }

            dimAxis.Quantity = dimension["quantity"].GetUint64();
            dimAxis.Resolution = dimension["resolution"].GetDouble();
            ascanData.AmplitudeAxes.push_back(dimAxis);
        }
        else if (dimAxis.Name == "Beam")
        {
            std::vector<BeamDescription> beamDescriptions;
            for (const auto& beam : dimension["beams"].GetArray())
            {
                BeamDescription beamDescription;
                beamDescription.Velocity = beam["velocity"].GetDouble();
                beamDescription.SkewAngle = beam["skewAngle"].GetDouble();
                beamDescription.RefractedAngle = beam["refractedAngle"].GetDouble();
                beamDescription.UCoordinateOffset = beam["uCoordinateOffset"].GetDouble();
                beamDescription.VCoordinateOffset = beam["vCoordinateOffset"].GetDouble();
                beamDescription.UltrasoundOffset = beam["ultrasoundOffset"].GetDouble();
                beamDescriptions.push_back(beamDescription);
            }

            dimAxis.Quantity = dimension["beams"].GetArray().Size();
            ascanData.BeamDescriptions.emplace(beamDescriptions);
            ascanData.AmplitudeAxes.push_back(dimAxis);
            ascanData.StatusAxes.push_back(dimAxis);
        }
    }

    // Open amplitude dataset
    std::string dsetJsonPath = amplitude["path"].GetString();
    hid_t propId = H5Pcreate(H5P_DATASET_ACCESS);
    hid_t dsetId = H5Dopen(fileId_, dsetJsonPath.c_str(), propId);

    hid_t dspaceId = H5Dget_space(dsetId);
    int rank = H5Sget_simple_extent_ndims(dspaceId);
    std::vector<uint64_t> dimensions(rank);
    H5Sget_simple_extent_dims(dspaceId, dimensions.data(), nullptr);

    // Read amplitude dataset
    ascanData.Amplitudes.resize(H5Sget_simple_extent_npoints(dspaceId));
    herr_t errorCode = H5Dread(dsetId, H5Dget_type(dsetId), dspaceId, dspaceId, H5P_DEFAULT, ascanData.Amplitudes.data());
    errorCode = H5Sclose(dspaceId);
    errorCode = H5Dclose(dsetId);

    // Open status dataset
    dsetJsonPath = ascanDataset_["status"]["path"].GetString();
    propId = H5Pcreate(H5P_DATASET_ACCESS);
    dsetId = H5Dopen(fileId_, dsetJsonPath.c_str(), propId);

    dspaceId = H5Dget_space(dsetId);
    rank = H5Sget_simple_extent_ndims(dspaceId);
    dimensions = std::vector<uint64_t>(rank);
    H5Sget_simple_extent_dims(dspaceId, dimensions.data(), nullptr);

    // Read status dataset
    ascanData.Statuses.resize(H5Sget_simple_extent_npoints(dspaceId));
    errorCode = H5Dread(dsetId, H5Dget_type(dsetId), dspaceId, dspaceId, H5P_DEFAULT, ascanData.Statuses.data());
    errorCode = H5Sclose(dspaceId);
    errorCode = H5Dclose(dsetId);

    

    return ascanData;
}


/*
#include "AscanProcessor.h"
#include <opencv2/opencv.hpp>
#include <Windows.h>

std::shared_ptr<QImage> AscanProcessor::qImage = std::make_shared<QImage>();
bool AscanProcessor::exportImage = false;
hid_t AscanProcessor::fileId = -1;
rj::Document AscanProcessor::jsonDoc = rj::Document();

AscanProcessor::AscanProcessor(): sttlogs(nullptr)
{
    if (!sttlogs) { sttlogs = &nmainUI::statuslogs::getinstance(); }
    NDEfilePath = fs::current_path() /( "sample\\" + readcf::readconfig("NDEfilePath1"));
    ExportedFile = fs::current_path() / ("result");
    schemaPathFilename = fs::current_path() / ("sample\\" + readcf::readconfig("validationFile")) ;
}
void AscanProcessor::printJsonLabels(const rj::Value& jsonValue, const std::string& indent = "") {
    if (jsonValue.IsObject()) {
        for (auto it = jsonValue.MemberBegin(); it != jsonValue.MemberEnd(); ++it) {
            std::string label = it->name.GetString();
            std::cerr << indent << "- Label: " << label << std::endl;

            printJsonLabels(it->value, indent + "  ");
        }
    }
    else if (jsonValue.IsArray()) {
        for (rj::SizeType i = 0; i < jsonValue.Size(); ++i) {
            printJsonLabels(jsonValue[i], indent);
        }
    }
}
bool AscanProcessor::showingData(unsigned int x_level_ = 0, unsigned int y_level_ = 0, unsigned int z_level_ = 0)
{
    //if (jsonDoc.IsObject()) { printJsonLabels(jsonDoc); }
    //else { std::cerr << "Invalid JSON document." << std::endl; }
    
    for (const auto& group : jsonDoc["groups"].GetArray()) {
        if (group["dataset"].HasMember("ascan")) {
            auto& ascanDataset = group["dataset"]["ascan"];
            const AscanData& ascanData = ReadAscanData(fileId, ascanDataset);
            ParseAscanStatuses(ascanDataset, ascanData);

            if (exportImage) {
                if (ExportXYView(group["name"].GetString(), ascanData, z_level_)) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool AscanProcessor::analyze(unsigned int x_level_ = 0, unsigned int y_level_ = 0, unsigned int z_level_ = 0) {
    if (exportImage)
    {
        if (showingData(x_level_ , y_level_ , z_level_ )) { return true; }
        else { return false; }
    }
    try {
        
        fileId = H5Fopen(NDEfilePath.string().c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
        std::string jsonSetup = ReadJsonSetup(fileId);
        auto file = std::ifstream(schemaPathFilename);
        auto decodingresult_ = DecodeJs(jsonSetup);
        if (decodingresult_ == rj::kParseErrorNone) {
            jsonDoc.Parse(jsonSetup.c_str());
            // PrintJSON(jsonDoc);
            exportImage = true;      
            if (showingData(z_level_)) { return true; }
            else { return false; }
        }
    }
    catch (const std::exception& e) {
        sttlogs->logWarning ("Error: " + std::string(e.what()));
        return false;
    }
    catch (...) {
		sttlogs->logWarning ("Unknown error occurred.");
        return false;
    }
}

std::string LoadJsonFromResource(int resourceID) {
    HMODULE hModule = GetModuleHandle(nullptr);
    HRSRC hResource = FindResourceW(hModule, MAKEINTRESOURCEW(resourceID), L"JSON");
    if (hResource == nullptr) {        
        throw std::runtime_error("Failed to find JSON resource.");
    }

    HGLOBAL hResourceData = LoadResource(hModule, hResource);
    if (hResourceData == nullptr) {
        throw std::runtime_error("Failed to load JSON resource.");
    }

    DWORD resourceSize = SizeofResource(hModule, hResource);
    const char* resourceData = static_cast<const char*>(LockResource(hResourceData));

    return std::string(resourceData, resourceSize);
}

// Private
std::string AscanProcessor::ReadJsonSetup(hid_t fileId) {
    hid_t propId = H5Pcreate(H5P_DATASET_ACCESS);
    hid_t dsetId = H5Dopen(fileId, "/Domain/Setup", propId);
    hid_t dataType = H5Dget_type(dsetId);
    std::vector<char> json(H5Tget_size(dataType));
    herr_t errorCode = H5Dread(dsetId, dataType, H5S_ALL, H5S_ALL, H5P_DEFAULT, json.data());
    errorCode = H5Dclose(dsetId);

    return std::string(json.begin(), json.end());
}

rj::ParseErrorCode AscanProcessor::DecodeJs(const std::string& jsonSetup) {
    for (int resourceID = 101; resourceID <= 106; resourceID++) {
        rj::Document setupJsonDoc;
        std::string schemaJson = LoadJsonFromResource(resourceID);
        rj::Document schemaJsonDoc;

        if (setupJsonDoc.Parse(jsonSetup.c_str()).HasParseError()) {
            sttlogs->logCritical("FAILED [ID] " + std::to_string(resourceID) + " - Error in jsonSetup");
            continue;
        }

        if (schemaJsonDoc.Parse(schemaJson.c_str()).HasParseError()) {
            sttlogs->logCritical("FAILED [ID] " + std::to_string(resourceID) + " - Error in schemaJson");
            continue;
        }

        rj::SchemaDocument schemaDoc(schemaJsonDoc);
        rj::SchemaValidator schemaValidator(schemaDoc);

        if (!setupJsonDoc.Accept(schemaValidator)) {
            sttlogs->logCritical("FAILED [ID] " + std::to_string(resourceID));
            continue;
        }

        return rj::kParseErrorNone;
    }

    return rj::kParseErrorValueInvalid;
}

AscanData AscanProcessor::ReadAscanData(const hid_t fileId_, const rj::Value& ascanDataset_)
{
    AscanData ascanData;

    auto& amplitude = ascanDataset_["amplitude"];
    ascanData.AmplitudeRng.PercentMin = amplitude["dataValue"]["min"].GetDouble();
    ascanData.AmplitudeRng.PercentMax = amplitude["dataValue"]["max"].GetDouble();
    ascanData.AmplitudeRng.SamplingMin = amplitude["dataSampling"]["min"].GetInt64();
    ascanData.AmplitudeRng.SamplingMax = amplitude["dataSampling"]["max"].GetInt64();

    for (const auto& dimension : amplitude["dimensions"].GetArray())
    {
        DimensionAxis dimAxis;
        dimAxis.Name = dimension["axis"].GetString();

        if (dimAxis.Name == "UCoordinate")
        {
            if (dimension.HasMember("offset")) {
                dimAxis.Offset = dimension["offset"].GetDouble();
            }

            dimAxis.Quantity = dimension["quantity"].GetUint64();
            dimAxis.Resolution = dimension["resolution"].GetDouble();
            ascanData.AmplitudeAxes.push_back(dimAxis);
            ascanData.StatusAxes.push_back(dimAxis);
        }
        else if (dimAxis.Name == "VCoordinate")
        {
            dimAxis.Quantity = dimension["quantity"].GetUint64();
            dimAxis.Offset = dimension["offset"].GetDouble();
            dimAxis.Resolution = dimension["resolution"].GetDouble();
            ascanData.AmplitudeAxes.push_back(dimAxis);
            ascanData.StatusAxes.push_back(dimAxis);

            ascanData.Velocity = ascanDataset_["velocity"].GetDouble();
            ascanData.SkewAngle = ascanDataset_["skewAngle"].GetDouble();
            ascanData.RefractedAngle = ascanDataset_["refractedAngle"].GetDouble();
        }
        else if (dimAxis.Name == "Ultrasound")
        {
            if (dimension.HasMember("offset")) {
                dimAxis.Offset = dimension["offset"].GetDouble();
            }

            dimAxis.Quantity = dimension["quantity"].GetUint64();
            dimAxis.Resolution = dimension["resolution"].GetDouble();
            ascanData.AmplitudeAxes.push_back(dimAxis);
        }
        else if (dimAxis.Name == "Beam")
        {
            std::vector<BeamDescription> beamDescriptions;
            for (const auto& beam : dimension["beams"].GetArray())
            {
                BeamDescription beamDescription;
                beamDescription.Velocity = beam["velocity"].GetDouble();
                beamDescription.SkewAngle = beam["skewAngle"].GetDouble();
                beamDescription.RefractedAngle = beam["refractedAngle"].GetDouble();
                beamDescription.UCoordinateOffset = beam["uCoordinateOffset"].GetDouble();
                beamDescription.VCoordinateOffset = beam["vCoordinateOffset"].GetDouble();
                beamDescription.UltrasoundOffset = beam["ultrasoundOffset"].GetDouble();
                beamDescriptions.push_back(beamDescription);
            }

            dimAxis.Quantity = dimension["beams"].GetArray().Size();
            ascanData.BeamDescriptions.emplace(beamDescriptions);
            ascanData.AmplitudeAxes.push_back(dimAxis);
            ascanData.StatusAxes.push_back(dimAxis);
        }
    }

    // Open amplitude dataset
    std::string dsetJsonPath = amplitude["path"].GetString();
    hid_t propId = H5Pcreate(H5P_DATASET_ACCESS);
    hid_t dsetId = H5Dopen(fileId_, dsetJsonPath.c_str(), propId);

    hid_t dspaceId = H5Dget_space(dsetId);
    int rank = H5Sget_simple_extent_ndims(dspaceId);
    std::vector<uint64_t> dimensions(rank);
    H5Sget_simple_extent_dims(dspaceId, dimensions.data(), nullptr);

    // Read amplitude dataset
    ascanData.Amplitudes.resize(H5Sget_simple_extent_npoints(dspaceId));
    herr_t errorCode = H5Dread(dsetId, H5Dget_type(dsetId), dspaceId, dspaceId, H5P_DEFAULT, ascanData.Amplitudes.data());
    errorCode = H5Sclose(dspaceId);
    errorCode = H5Dclose(dsetId);
    
    // Open status dataset
    dsetJsonPath = ascanDataset_["status"]["path"].GetString();
    propId = H5Pcreate(H5P_DATASET_ACCESS);
    dsetId = H5Dopen(fileId_, dsetJsonPath.c_str(), propId);

    dspaceId = H5Dget_space(dsetId);
    rank = H5Sget_simple_extent_ndims(dspaceId);
    dimensions = std::vector<uint64_t>(rank);
    H5Sget_simple_extent_dims(dspaceId, dimensions.data(), nullptr);

    // Read status dataset
    ascanData.Statuses.resize(H5Sget_simple_extent_npoints(dspaceId));
    errorCode = H5Dread(dsetId, H5Dget_type(dsetId), dspaceId, dspaceId, H5P_DEFAULT, ascanData.Statuses.data());
    errorCode = H5Sclose(dspaceId);
    errorCode = H5Dclose(dsetId);

    return ascanData;
}

void AscanProcessor::ParseAscanStatuses(const rj::Value& ascanDataset_, const AscanData& ascanData_) {
    uint8_t hasDataMask{};
    auto& dataValue = ascanDataset_["status"]["dataValue"];

    if (dataValue.HasMember("hasData")) {
        hasDataMask = 1 << dataValue["hasData"].GetUint();
    }

    uint8_t synchedMask{};
    if (dataValue.HasMember("synched")) {
        synchedMask = 1 << dataValue["synched"].GetUint();
    }

    for (uint64_t dim0{}; dim0 < ascanData_.StatusAxes[0].Quantity; dim0++) {
        for (uint64_t dim1{}; dim1 < ascanData_.StatusAxes[1].Quantity; dim1++)
        {
            uint64_t cell = dim1 + dim0 * ascanData_.StatusAxes[1].Quantity;
            bool hasDataStatus = ascanData_.Statuses[cell] & hasDataMask;
            bool synchedStatus = ascanData_.Statuses[cell] & synchedMask;
            hasDataStatus = hasDataStatus = false;
        }
    }

}

bool AscanProcessor::ExportXYView(const std::string& groupName_, const AscanData& ascanData_,unsigned int z_level_ = 0) {
    uint64_t ysize =  ascanData_.AmplitudeAxes[1].Quantity;
    uint64_t xsize = ascanData_.AmplitudeAxes[2].Quantity;

    cv::Mat image(xsize, ysize, CV_8UC3);
    std::vector<Color> everyColors = CreateColorPalette();

    uint64_t z_offset = z_level_ * (xsize * ysize);


    for (uint64_t x = 0; x < xsize; ++x) {
        for (uint64_t y = 0; y < ysize; ++y) {
            // check if data is out of range: return
            if (z_offset + y * xsize + x >= ascanData_.Amplitudes.size()) {
                sttlogs->logWarning("Out of range data: " + std::to_string(z_offset + y * xsize + x) + " " + std::to_string(ascanData_.Amplitudes.size()));
                return false;
            }
            int16_t samplingAmplitude = std::abs(ascanData_.Amplitudes[z_offset+ y * xsize + x]);
            double percentAmplitude = samplingAmplitude / (32768 / 100.0);
            Color color = everyColors[static_cast<int16_t>(percentAmplitude)];

            image.at<cv::Vec3b>(x, y) = cv::Vec3b(color.B, color.G, color.R);
        }
    }
    cv::Mat resizedImage;    double scaleFactor = 10.0;  
    cv::resize(image, resizedImage, cv::Size(), scaleFactor, scaleFactor, cv::INTER_LINEAR);
    cv::GaussianBlur(resizedImage, resizedImage, cv::Size(5, 5), 0);
    qImage = std::make_shared<QImage>(resizedImage.data, resizedImage.cols, resizedImage.rows, resizedImage.step, QImage::Format_RGB888);
    *qImage = qImage->rgbSwapped();
    return true;
}

bool AscanProcessor::ExportXZView(const std::string& groupName_, const AscanData& ascanData_, unsigned int y_fixed, unsigned int z_level_ = 0) {
    uint64_t zsize = ascanData_.AmplitudeAxes[0].Quantity;
    uint64_t xsize = ascanData_.AmplitudeAxes[2].Quantity;

    cv::Mat image(xsize, zsize, CV_8UC3);  
    std::vector<Color> everyColors = CreateColorPalette();

    for (uint64_t z = 0; z < zsize; ++z) {
        uint64_t z_offset = z * (xsize * ascanData_.AmplitudeAxes[1].Quantity);  

        for (uint64_t x = 0; x < xsize; ++x) {

            uint64_t index = z_offset + y_fixed * xsize + x;

            if (index >= ascanData_.Amplitudes.size()) {
                sttlogs->logWarning("Out of range data: " + std::to_string(index) + " " + std::to_string(ascanData_.Amplitudes.size()));
                return false;
            }

            int16_t samplingAmplitude = std::abs(ascanData_.Amplitudes[index]);
            double percentAmplitude = samplingAmplitude / (32768 / 100.0);
            Color color = everyColors[static_cast<int16_t>(percentAmplitude)];

            image.at<cv::Vec3b>(x, z) = cv::Vec3b(color.B, color.G, color.R);
        }
    }

    cv::Mat resizedImage;
    double scaleFactor = 10.0;
    cv::resize(image, resizedImage, cv::Size(), scaleFactor, scaleFactor, cv::INTER_LINEAR);

    cv::GaussianBlur(resizedImage, resizedImage, cv::Size(5, 5), 0);

    qImage = std::make_shared<QImage>(resizedImage.data, resizedImage.cols, resizedImage.rows, resizedImage.step, QImage::Format_RGB888);
    *qImage = qImage->rgbSwapped();
    return true;
}

bool AscanProcessor::ExportYZView(const std::string& groupName_, const AscanData& ascanData_, unsigned int x_fixed, unsigned int z_level_ = 0) {
    uint64_t zsize = ascanData_.AmplitudeAxes[0].Quantity;
    uint64_t ysize = ascanData_.AmplitudeAxes[1].Quantity;

    cv::Mat image(ysize, zsize, CV_8UC3);  
    std::vector<Color> everyColors = CreateColorPalette();

    for (uint64_t z = 0; z < zsize; ++z) {
        uint64_t z_offset = z * (ascanData_.AmplitudeAxes[2].Quantity * ysize);  

        for (uint64_t y = 0; y < ysize; ++y) {
            uint64_t index = z_offset + y * ascanData_.AmplitudeAxes[2].Quantity + x_fixed;

            if (index >= ascanData_.Amplitudes.size()) {
                sttlogs->logWarning("Out of range data: " + std::to_string(index) + " " + std::to_string(ascanData_.Amplitudes.size()));
                return false;
            }

            int16_t samplingAmplitude = std::abs(ascanData_.Amplitudes[index]);
            double percentAmplitude = samplingAmplitude / (32768 / 100.0);
            Color color = everyColors[static_cast<int16_t>(percentAmplitude)];

            image.at<cv::Vec3b>(y, z) = cv::Vec3b(color.B, color.G, color.R);
        }
    }

    cv::Mat resizedImage;
    double scaleFactor = 10.0;
    cv::resize(image, resizedImage, cv::Size(), scaleFactor, scaleFactor, cv::INTER_LINEAR);

    cv::GaussianBlur(resizedImage, resizedImage, cv::Size(5, 5), 0);

    qImage = std::make_shared<QImage>(resizedImage.data, resizedImage.cols, resizedImage.rows, resizedImage.step, QImage::Format_RGB888);
    *qImage = qImage->rgbSwapped();
    return true;
}


std::vector<Color> AscanProcessor::CreateColorPalette() {
    // Define a set of key colors that will be used as reference points for the gradient.
    std::vector<Color> colors;
    std::vector<Color> everyColors;
    // Key colors in the gradient, ranging from white to deep red.
    colors.emplace_back(Color{ 255, 255, 255 }); // white
    colors.emplace_back(Color{ 184, 212, 244 }); // light blue
    colors.emplace_back(Color{ 113, 170, 233 }); // blue
    colors.emplace_back(Color{ 62, 105, 190 });  // dark blue
    colors.emplace_back(Color{ 14, 37, 143 });   // dark purple
    colors.emplace_back(Color{ 27, 72, 129 });   // purple
    colors.emplace_back(Color{ 59, 140, 127 });  // green
    colors.emplace_back(Color{ 126, 187, 94 });  // light green
    colors.emplace_back(Color{ 211, 223, 45 });  // yellow
    colors.emplace_back(Color{ 241, 211, 43 });  // light yellow
    colors.emplace_back(Color{ 222, 156, 80 });  // orange
    colors.emplace_back(Color{ 209, 121, 87 });  // light orange
    colors.emplace_back(Color{ 205, 116, 49 });  // dark orange
    colors.emplace_back(Color{ 194, 98, 23 });   // brown
    colors.emplace_back(Color{ 167, 50, 26 });   // dark brown
    colors.emplace_back(Color{ 145, 12, 29 });   // red

    // Number of interpolation points between each pair of key colors.
    size_t interpolationPoints = 7;
    double f = 1.0 / interpolationPoints;   // Fractional step size for interpolation.

    // Interpolate between each pair of consecutive key colors.
    for (size_t colorIdx(0); colorIdx < colors.size() - 1; colorIdx++)
    {
        Color x = colors[colorIdx];             // Start color
        Color y = colors[colorIdx + 1];         // End color

        // Generate intermediate colors between x and y.
        for (size_t j(0); j < interpolationPoints; j++)
        {            
            // Interpolate each RGB component separately.
            uint8_t r = static_cast<uint8_t>(x.R + (y.R - x.R) * (j * f));
            uint8_t g = static_cast<uint8_t>(x.G + (y.G - x.G) * (j * f));
            uint8_t b = static_cast<uint8_t>(x.B + (y.B - x.B) * (j * f));

            // Add the interpolated color to the resulting palette.
            everyColors.emplace_back(Color{ r, g, b });
        }
    }
    return everyColors;
}
*/