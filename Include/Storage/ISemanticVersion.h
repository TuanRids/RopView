////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file ISemanticVersion.h
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <string>
#include <memory>

namespace Olympus
{
  namespace FileManagement
  {
    class ISemanticVersion
    {
    public:
      virtual ~ISemanticVersion() = default;


      virtual size_t GetMajor() const = 0;
      virtual size_t GetMinor() const = 0;
      virtual size_t GetRevision() const = 0;
      virtual size_t GetBuild() const = 0;

      virtual const std::string ToString() const = 0;
      virtual const std::wstring ToWString() const = 0;
    };

    using ISemanticVersionPtr = std::shared_ptr<ISemanticVersion>;
  }
}
