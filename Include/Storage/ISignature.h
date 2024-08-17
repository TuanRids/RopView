////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file ISignature.h
///
/// NDE file domain path / The signature is not part of the Domain Json, 
/// they must be read from the root of the file in the HDF 5 tree
/// 
/// The signature holds some metadata information such as the company name, software version and
/// software name.
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <string>
#include <memory>
#include "IGeoCoordinate.h"

namespace Olympus
{
  namespace FileManagement
  {
    class ISignature
    {
    public:
      virtual ~ISignature() = default;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets company name
      ///
      /// NDE file HDF5 file root Attirbute "Company name"
      /// 
      /// @returns The company name.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual const std::wstring& GetCompanyName() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets software name
      ///
      /// NDE file HDF5 file root Attirbute "Application name"
      /// 
      /// @returns The software name.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual const std::wstring& GetSoftwareName() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets software version
      ///
      /// NDE file HDF5 file root Attirbute "Application version"
      /// 
      /// @returns The software version.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual const std::wstring& GetSoftwareVersion() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets company name
      ///
      /// @param companyName Name of the company.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetCompanyName(const std::wstring& companyName) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets software name
      ///
      /// @param softwareName Name of the software.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetSoftwareName(const std::wstring& softwareName) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets software version
      ///
      /// @param softwareVersion The software version.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetSoftwareVersion(const std::wstring& softwareVersion) = 0;


      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets inspection date with ISO-8601's long date/time format (yyyy-mm-ddThh:mm:ss.nnnnnn+|-hh:mm)
      /// 
      /// Examples: 
      /// "2007-04-05T14:30Z"
      /// "2007-04-05T12:30-02:00"
      /// "2007-04-05T12:30.256+03:30"
      ///
      /// @returns The inspection date
      /////////////////////////////////////////////////////////////////////////////////////////////
      __declspec(deprecated("Deprecated. Use GetInspectionDateEx.")) virtual const std::string& GetInspectionDate() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets inspection date with ISO-8601's long date/time format (yyyy-mm-ddThh:mm:ss.nnnnnn+|-hh:mm)
      /// 
      /// Examples: 
      /// "2007-04-05T14:30Z"
      /// "2007-04-05T12:30-02:00"
      /// "2007-04-05T12:30.256+03:30"
      ///
      /// @throw invalid_argument If the inspection date is malformed
      /// @param inspectionDate The inspection date.
      /////////////////////////////////////////////////////////////////////////////////////////////.
      __declspec(deprecated("Deprecated. The inspection date is automatically set to the current date/time when a data file is Saved")) 
              virtual void SetInspectionDate(const std::string& inspectionDate) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets inspection date with ISO-8601's long date/time format (yyyy-mm-ddThh:mm:ss.nnnnnn+|-hh:mm)
      /// It is always in UTC time zone
      ///
      /// Examples: 
      /// "2007-04-05T14:30Z"
      ///
      /// ref: https://www.iso.org/iso-8601-date-and-time-format.html
      ///
      /// NDE file HDF5 file root Attirbute "Date created"
      /// 
      /// @returns The inspection date
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual const std::wstring& GetInspectionDateEx() const = 0;

      virtual IGeoCoordinatePtr GetGeoCoordinate() = 0;
    };

    using ISignaturePtr = std::shared_ptr<ISignature>;
    using ISignatureConstPtr = std::shared_ptr<const ISignature>;
  }
}
