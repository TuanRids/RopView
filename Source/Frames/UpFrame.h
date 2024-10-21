#include "MainUI/FactoryMgr.h"
class nFrameUpdate : public nFrame {
public:
    void upSharedBuffer(std::vector<int> data) { sharedBuffer.push(data); }
};