#include "../pch.h"
#include <unordered_map>
#include <vector>
#include <mutex>
#include <utility>
#include <iostream>
#include "MainUI/statuslogs.h"
#include "Instrumentation/IAScanCollection.h"

struct BeamData {
    std::unordered_map<int, std::vector<int>> beamDataMap; 
    
    void addBeamData(int beamID, const std::vector<int>& data) {
        beamDataMap[beamID] = data;
    }

    std::vector<int> getAscanData(int setIndex) const {
        return beamDataMap.at(setIndex);
    }
};

class BeamSet {
private:
    std::unordered_map<int, BeamData> beamDataMap;  
    std::queue<int> beamQueue; 
    std::mutex mtx;

public:
    void addBeamData(int setIndex, int beamID, const std::vector<int>& data) {
        std::lock_guard<std::mutex> lock(mtx);
        if (beamDataMap.find(setIndex) == beamDataMap.end()) {
            beamQueue.push(setIndex);
        }
        beamDataMap[setIndex].addBeamData(beamID, data);
    }
    void popBeamData() {
        std::lock_guard<std::mutex> lock(mtx);

        if (beamQueue.empty()) {
            return; 
        }

        int setIndex = beamQueue.front();
        beamQueue.pop(); 

        auto setIt = beamDataMap.find(setIndex);
        if (setIt != beamDataMap.end()) {
            beamDataMap.erase(setIt);  
        }
    }
    BeamData getBeamData() {
        std::lock_guard<std::mutex> lock(mtx);

        if (beamQueue.empty()) {
            throw std::runtime_error("No beam data available.");
        }

        int setIndex = beamQueue.front();  
        auto setIt = beamDataMap.find(setIndex);
        if (setIt != beamDataMap.end()) {
            return setIt->second;
        }

        throw std::runtime_error("Beam data not found.");
    }

    size_t getBeamSetSize() {
        std::lock_guard<std::mutex> lock(mtx);
        return beamDataMap.size();
    }
};

