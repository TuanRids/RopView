#ifndef FACTORYMGR_H
#define FACTORYMGR_H
#include "../pch.h"
#include "mainwindow.h"
// forward declaration for fixing Circular reference
class CviewFrame; // Oxy
class SviewFrame; // Oyz
class AviewFrame; // Oz
class BviewFrame; // Oxz

// Frame Factory
class nFactoryFrame {
public:
	static std::shared_ptr<CviewFrame> crCviewFrm();
    static std::shared_ptr<SviewFrame> crSViewFrm();
    static std::shared_ptr<AviewFrame> crAviewFrm();
    static std::shared_ptr<BviewFrame> crBviewFrm();
};


#endif // FACTORYMGR_H
