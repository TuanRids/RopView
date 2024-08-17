////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file Instrumentation.h
///
/// Declares the Instrumentation.
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <instrumentation/InstrumentationExport.h>
#include <instrumentation/InstrumentationTypes.h>
#include <instrumentation/Exception.h>
#include <instrumentation/IGpioConnector.h>
#include <instrumentation/IGpioConnectorCollection.h>
#include <instrumentation/IDigitalInputPin.h>
#include <instrumentation/IDigitalInputPinCollection.h>
#include <instrumentation/IDigitalOutputPin.h>
#include <instrumentation/IDigitalOutputPinCollection.h>
#include <instrumentation/IDeviceConfiguration.h>
#include <instrumentation/IDevice.h>
#include <instrumentation/IDeviceInfo.h>
#include <instrumentation/ISyncedDeviceCollection.h>
#include <instrumentation/ILinkedDeviceCollection.h>
#include <instrumentation/IFirmwarePackage.h>
#include <instrumentation/IFirmwarePackageCollection.h>
#include <instrumentation/IFirmwarePackageScanner.h>
#include <instrumentation/IDeviceDiscovery.h>
#include <instrumentation/IEncoderData.h>
#include <instrumentation/IEncoderDataCollection.h>
#include <instrumentation/IEncoder.h>
#include <instrumentation/IEncoderCollection.h>
#include <instrumentation/IAscanCollection.h>
#include <instrumentation/ICscanCollection.h>
#include <instrumentation/ICycleData.h>
#include <instrumentation/IAcquisition.h>
#include <instrumentation/IDigitalInputDataCollection.h>
#include <instrumentation/IDigitalInputData.h>
#include <instrumentation/IDeviceNetworkInfo.h>
#include <instrumentation/DevicePorts.h>

#include <ultrasoundConfiguration/UltrasoundTypes.h>
#include <ultrasoundConfiguration/IAdjustedParameter.h>
#include <ultrasoundConfiguration/IAdjustedParameterCollection.h>
#include <ultrasoundConfiguration/IConnector.h>
#include <ultrasoundConfiguration/IConnectorCollection.h>
#include <ultrasoundConfiguration/IElementDelay.h>
#include <ultrasoundConfiguration/IElementDelayCollection.h>
#include <ultrasoundConfiguration/IBeamFormation.h>
#include <ultrasoundConfiguration/IBeamFormationCollection.h>
#include <ultrasoundConfiguration/IPulserVoltageCollection.h>
#include <ultrasoundConfiguration/ISmoothingFilterCollection.h>
#include <ultrasoundConfiguration/IDigitalBandPassFilter.h>
#include <ultrasoundConfiguration/IDigitalBandPassFilterCollection.h>
#include <ultrasoundConfiguration/IDigitizerTechnology.h>
#include <ultrasoundConfiguration/IGate.h>
#include <ultrasoundConfiguration/IGateCollection.h>
#include <ultrasoundConfiguration/ITcgPoint.h>
#include <ultrasoundConfiguration/ITcgPointCollection.h>
#include <ultrasoundConfiguration/ITcg.h>
#include <ultrasoundConfiguration/IAmplitudeSettings.h>
#include <ultrasoundConfiguration/IFilterSettings.h>
#include <ultrasoundConfiguration/IPulsingSettings.h>
#include <ultrasoundConfiguration/ITimeSettings.h>
#include <ultrasoundConfiguration/IDigitizingSettings.h>
#include <ultrasoundConfiguration/IBeam.h>
#include <ultrasoundConfiguration/IBeamSet.h>
#include <ultrasoundConfiguration/IFiringBeam.h>
#include <ultrasoundConfiguration/IFiringBeamSet.h>
#include <ultrasoundConfiguration/IFiringBeamSetCollection.h>
#include <ultrasoundConfiguration/IBeamSetFactory.h>
#include <ultrasoundConfiguration/IUltrasoundConfiguration.h>
#include <ultrasoundConfiguration/ITcgTypeCollection.h>

#include <DAL/IAscan.h>
#include <DAL/IDataRange.h>
#include <DAL/ISyncSource.h>
#include <DAL/ICscan.h>

namespace Instrumentation
{
  __declspec(deprecated("Deprecated. Use GetLibraryVersion")) INSTRUMENTATION_API std::string GetVersion();
  INSTRUMENTATION_API bool EnableLogger();
  INSTRUMENTATION_API unsigned short GetMaximumTransmissionUnit(std::string const& addressIPv4);
  __declspec(deprecated("Deprecated. Use GetLibraryVersionEx")) INSTRUMENTATION_API std::string GetLibraryVersion();
  INSTRUMENTATION_API std::wstring GetLibraryVersionEx();
}
