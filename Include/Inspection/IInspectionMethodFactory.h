////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file IInspectionMethodFactory.h
///
/// Declares the inspection method factory class.
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <memory>
#include <string>
#include <Inspection/Formation.h>
#include <Inspection/InspectionMethod.h>
#include <PartDefinition/IPatch.h>
#include <storage/EquipmentTypes.h>

namespace Olympus
{
  namespace Inspection
  {
    class IInspectionMethodPhasedArray;
    using IInspectionMethodPhasedArrayPtr = std::shared_ptr<IInspectionMethodPhasedArray>;
    using IInspectionMethodPhasedArrayConstPtr = std::shared_ptr<const IInspectionMethodPhasedArray>;

    class IInspectionMethodPulseEcho;
    using IInspectionMethodPulseEchoPtr = std::shared_ptr<IInspectionMethodPulseEcho>;
    using IInspectionMethodPulseEchoConstPtr = std::shared_ptr<const IInspectionMethodPulseEcho>;

    class IInspectionMethodTofd;
    using IInspectionMethodTofdPtr = std::shared_ptr<IInspectionMethodTofd>;
    using IInspectionMethodTofdConstPtr = std::shared_ptr<const IInspectionMethodTofd>;

    class IInspectionMethodPitchCatch;
    using IInspectionMethodPitchCatchPtr = std::shared_ptr<IInspectionMethodPitchCatch>;
    using IInspectionMethodPitchCatchConstPtr = std::shared_ptr<const IInspectionMethodPitchCatch>;

    class IInspectionMethodTotalFocusing;
    using IInspectionMethodTotalFocusingPtr = std::shared_ptr<IInspectionMethodTotalFocusing>;
    using IInspectionMethodTotalFocusingConstPtr = std::shared_ptr<const IInspectionMethodTotalFocusing>;

    class IInspectionMethodFactory;
    using IInspectionMethodFactoryPtr = std::shared_ptr<IInspectionMethodFactory>;
    using IInspectionMethodFactoryConstPtr = std::shared_ptr<const IInspectionMethodFactory>;

    class IInspectionMethodFactory
    {
    public:
      virtual ~IInspectionMethodFactory() = default;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Add a conventional pulse echo inspection method.
      ///
      /// @param name The name.
      /// @param patch The patch.
      /// @param probe The probe.
      ///
      /// @return
      /// The new conventional pulse echo inspection method..
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual IInspectionMethodPulseEchoPtr AddConventionalPulseEcho(const std::wstring& name,
        PartDefinition::IPatchPtr patch, Equipment::IProbeConventionalPtr probe) const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Add a Conventional TOFD inspection method.
      ///
      /// @param name The name.
      /// @param patch The patch.
      /// @param probe The pulser probe.
      /// @param probe The receiver probe.
      ///
      /// @return
      /// The new tofd inspection method..
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual IInspectionMethodTofdPtr AddConventionalTOFD(const std::wstring& name, PartDefinition::IPatchPtr patch,
        Equipment::IProbeConventionalPtr pulserProbe, Equipment::IProbeConventionalPtr receiverProbe) const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Creates a linear formation.
      ///
      /// @param angle The angle.
      /// @param nbElementPrimary The nb element primary.
      /// @param firstElement The first element.
      /// @param lastElement The last element.
      /// @param stepElement The step element.
      ///
      /// @return
      /// The new linear formation.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual IFormationLinearPtr CreateLinearFormation(double angle, int nbElementPrimary, int firstElement, int lastElement, int stepElement) const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Creates a sectorial formation.
      ///
      /// @param start The start primary refracted angle.
      /// @param stop The stop primary refracted angle.
      /// @param step The step primary refracted angle.
      /// @param firstElement The first element.
      /// @param nbElementPrimary The nb element primary.
      ///
      /// @return
      /// The new sectorial formation.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual IFormationSectorialPtr CreateSectorialFormation(double start, double stop, double step, int firstElement, int nbElementPrimary) const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Creates a compound formation.
      ///
      /// @param start The start primary refracted angle.
      /// @param stop The stop primary refracted angle.
      /// @param step The step primary refracted angle.
      /// @param firstElement The first element.
      /// @param lastElement The last element.
      /// @param stepElement The step element.
      ///
      /// @return
      /// The new compound formation.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual IFormationCompoundPtr CreateCompoundFormation(double start, double stop, double step, int firstElement, int lastElement, int nbElementPrimary) const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Add a phased array linear inspection method.
      ///
      /// @param name The name.
      /// @param patch The patch.
      /// @param probe The probe.
      /// @param formation The formation.
      ///
      /// @return
      /// The new phased array linear inspection method.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual IInspectionMethodPhasedArrayPtr AddPhasedArrayLinear(const std::wstring& name, PartDefinition::IPatchPtr patch,
        Equipment::IProbeRectangularArrayPtr probe, IFormationLinearPtr formation) const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Add a phased array linear inspection method.
      ///
      /// @param name The name.
      /// @param patch The patch.
      /// @param probe The probe.
      /// @param formation The formation.
      ///
      /// @return
      /// The new phased array linear inspection method.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual IInspectionMethodPhasedArrayPtr AddPhasedArrayLinear(const std::wstring& name, PartDefinition::IPatchPtr patch,
        Equipment::IProbeDualRectangularArrayPtr probe, IFormationLinearPtr formation) const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Add a phased array sectorial inspection method.
      ///
      /// @param name The name.
      /// @param patch The patch.
      /// @param probe The probe.
      /// @param formation The formation.
      ///
      /// @return
      /// The new phased array sectorial inspection method.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual IInspectionMethodPhasedArrayPtr AddPhasedArraySectorial(const std::wstring& name, PartDefinition::IPatchPtr patch,
        Equipment::IProbeRectangularArrayPtr probe, IFormationSectorialPtr formation) const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Add a phased array sectorial inspection method.
      ///
      /// @param name The name.
      /// @param patch The patch.
      /// @param probe The probe.
      /// @param formation The formation.
      ///
      /// @return
      /// The new phased array sectorial inspection method.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual IInspectionMethodPhasedArrayPtr AddPhasedArraySectorial(const std::wstring& name, PartDefinition::IPatchPtr patch,
        Equipment::IProbeDualRectangularArrayPtr probe, IFormationSectorialPtr formation) const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Add a phased array compound inspection method.
      ///
      /// @param name The name.
      /// @param patch The patch.
      /// @param probe The probe.
      /// @param formation The formation.
      ///
      /// @return
      /// The new phased array compound inspection method.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual IInspectionMethodPhasedArrayPtr AddPhasedArrayCompound(const std::wstring& name, PartDefinition::IPatchPtr patch,
        Equipment::IProbeRectangularArrayPtr probe, IFormationCompoundPtr formation) const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Add a phased array compound inspection method.
      ///
      /// @param name The name.
      /// @param patch The patch.
      /// @param probe The probe.
      /// @param formation The formation.
      ///
      /// @return
      /// The new phased array compound inspection method.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual IInspectionMethodPhasedArrayPtr AddPhasedArrayCompound(const std::wstring& name, PartDefinition::IPatchPtr patch,
        Equipment::IProbeDualRectangularArrayPtr probe, IFormationCompoundPtr formation) const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Add a pitch and catch inspection method.
      ///
      /// @param name The name.
      /// @param patch The patch.
      /// @param probe The pulser probe.
      /// @param probe The receiver probe.
      ///
      /// @return
      /// The new pitch and catch inspection method..
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual IInspectionMethodPitchCatchPtr AddConventionalPitchCatch(const std::wstring& name, PartDefinition::IPatchPtr patch,
              Equipment::IProbeConventionalPtr pulserProbe, Equipment::IProbeConventionalPtr receiverProbe) const = 0;

      virtual IInspectionMethodTotalFocusingPtr AddTotalFocusingMethod(const std::wstring& name,
        PartDefinition::IPatchPtr patch, Equipment::IProbeRectangularArrayPtr probe) const = 0;


      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Creates a linear formation.
      ///
      /// @param angle The angle.
      /// @param nbElementPrimary The nb element primary.
      /// @param firstElement The first element.
      /// @param lastElement The last element.
      /// @param stepElement The step element.
      ///
      /// @return
      /// The new linear formation.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual IFormationLinearPtr CreateLinearFormationEx(double angle, int nbElementPrimary, int firstElement, int lastElement, double stepElement) const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Creates a custom formation.
      ///
      /// @param ...
      ///
      /// @return
      /// The new custom formation.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual IFormationCustomPtr CreateCustomFormation() const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Add a phased array custom inspection method.
      ///
      /// @param name The name.
      /// @param patch The patch.
      /// @param probe The probe.
      /// @param formation The formation.
      ///
      /// @return
      /// The new phased array custom inspection method.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual IInspectionMethodPhasedArrayPtr AddPhasedArrayCustom(const std::wstring& name, PartDefinition::IPatchPtr patch,
        Equipment::IProbeRectangularArrayPtr probe, IFormationCustomPtr formation) const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Add a phased array custom inspection method.
      ///
      /// @param name The name.
      /// @param patch The patch.
      /// @param probe The probe.
      /// @param formation The formation.
      ///
      /// @return
      /// The new phased array custom inspection method.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual IInspectionMethodPhasedArrayPtr AddPhasedArrayCustom(const std::wstring& name, PartDefinition::IPatchPtr patch,
        Equipment::IProbeDualRectangularArrayPtr probe, IFormationCustomPtr formation) const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Creates a sectorial formation.
      ///
      /// @param start The start primary refracted angle.
      /// @param stop The stop primary refracted angle.
      /// @param step The step primary refracted angle.
      /// @param skewStart The start skew angle.
      /// @param skewStop The stop skew angle.
      /// @param skewStep The step skew angle.

      /// @param firstElement The first element.
      /// @param nbElementPrimary The nb element primary.
      ///
      /// @return
      /// The new sectorial formation.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual IFormationSectorialPtr CreateSectorialFormation(double start, double stop, double step, double skewStart, double skewStop, double skewStep, int firstElement, int nbElementPrimary) const = 0;



      virtual IInspectionMethodTotalFocusingPtr AddTotalFocusingMethod(const std::wstring& name,
        PartDefinition::IPatchPtr patch, Equipment::IProbeDualRectangularArrayPtr probe) const = 0;
    };
  }
}

