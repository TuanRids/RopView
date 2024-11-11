#include "OmCreateSetup.h"
#include "LoadLibraries.h"
#include "OpenView.Configuration.h"
#include "OpenView.ScanPlan.h"


bool OmCreateSetup::SaveSetup()
{
    try
    {
        HINSTANCE handle = OpenView::Libraries::LoadStorage();
        auto setup = Storage::CreateSetup();

        OpenView::ScanPlan::Create(setup);
        OpenView::Configuration::Create(setup);
                
        auto signature = setup->GetSignature();
        signature->SetCompanyName(L"Company X");
        signature->SetSoftwareName(L"App Y");
        signature->SetSoftwareVersion(L"1.2.3");
        auto outputFile = getFilePath();
        SaveSetupFile(outputFile, setup);

        OpenView::Libraries::UnLoad(handle);
    }
    catch (const exception& ex)
    {
        cout << ex.what() << endl;
        return false;
    }

    return true;

}
void OmCreateSetup::ConfigurePhasedArray(IPhasedArrayConfigurationPtr config)
{
    constexpr const size_t BeamQty = 5;
    constexpr const size_t ElementQty = 16;
    constexpr const size_t FirstElement = 1;
    constexpr const double BeamAngleStart = 45.;
    double exitPoint[BeamQty]{ 28., 27.75, 27.5, 27.25, 27. }; //mm
    double beamDelay[BeamQty]{ 19000., 19100., 19200., 19300., 19400. }; //ns
    double digitizingDelay[BeamQty]{ 5400., 5450., 5500., 5550., 5600. }; //ns
    double elementDelay[ElementQty]{ 374., 355., 336., 315., 294., 272., 249., 225., 200., 174., 148., 120., 91., 62., 31., 0. };

    /*config->SetGain(25.);
    config->SetVelocity(3235.);
    config->SetReferenceAmplitude(80.);

    auto pulsingSettings = config->GetPulsingSettings();
    pulsingSettings->SetPulseWidth(100.);*/
    

    //auto digitizingSettings = config->GetDigitizingSettings();
    //digitizingSettings->GetAmplitudeSettings()->SetAscanDataSize(IAmplitudeSettings::AscanDataSize::EightBits);
    //digitizingSettings->GetAmplitudeSettings()->SetAscanRectification(IAmplitudeSettings::RectificationType::Full);

//    for (size_t beamIdx(0); beamIdx < BeamQty; ++beamIdx)
//    {
//        auto beam = config->AddBeam();
//        /*
//        beam->SetBeamDelay(beamDelay[beamIdx]);
//        beam->SetGainOffset(2.2);
//        beam->SetDigitizingDelay(digitizingDelay[beamIdx]);
//        beam->SetDigitizingLength(2560.);
//        beam->SetExitPointPrimary(exitPoint[beamIdx]);
//        beam->SetRefractedAnglePrimary(BeamAngleStart + beamIdx);
//
//        auto beamFormation = beam->CreateBeamFormation(ElementQty, ElementQty);
//        auto pulserDelays = beamFormation->GetPulserDelayCollection();
//        auto receiverDelays = beamFormation->GetReceiverDelayCollection();
//        for (size_t elementIdx(0); elementIdx < ElementQty; ++elementIdx)
//        {
//            pulserDelays->GetElementDelay(elementIdx)->SetElementId(FirstElement + elementIdx);
//            pulserDelays->GetElementDelay(elementIdx)->SetDelay(elementDelay[elementIdx]);
//
//            receiverDelays->GetElementDelay(elementIdx)->SetElementId(FirstElement + elementIdx);
//            receiverDelays->GetElementDelay(elementIdx)->SetDelay(elementDelay[elementIdx]);
//        }*/
//#
//        auto gateConfig = beam->GetGateConfigurations();
//
//        auto gateI = gateConfig->Add(GATE_I);
//        gateI->SetDelay(5500.);
//        gateI->SetLength(1500.);
//        gateI->SetThreshold(60);
//
//        auto gateA = gateConfig->Add(GATE_A);
//        gateA->SetDelay(6000.);
//        gateA->SetLength(2000.);
//        gateA->SetThreshold(35);
//        gateA->ReserveCscanBuffer(true);
//
//        auto gateB = gateConfig->Add(GATE_B);
//        gateB->SetDelay(9000.);
//        gateB->SetLength(2500.);
//        gateB->SetThreshold(45);
//        gateB->ReserveCscanBuffer(true);
//    }
}
void OmCreateSetup::ConfigureParameters(ISetupPtr setup)
{
    auto configs = setup->GetInspectionConfigurations()->GetConfiguration(0)->GetConfigurations();
    for (size_t configIdx(0); configIdx < configs->GetCount(); ++configIdx)
    {
        auto config = configs->GetConfiguration(configIdx);
        auto configUT = dynamic_pointer_cast<IConventionalConfiguration>(config);
        if (!configUT)
        {
            auto configPA = dynamic_pointer_cast<IPhasedArrayConfiguration>(config);
            if (configPA)
                ConfigurePhasedArray(configPA);
            else
                throw bad_cast{};
        }
    }
}
