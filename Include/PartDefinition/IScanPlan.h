////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file IScanPlan.h
///
/// NDE file domain path: dataEncodings[index] / discreteGrid
/// NDE file domain path: specimens
/// 
/// Declares the ScanPlan structure
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <memory>
#include <storage/Storage.h>

namespace Olympus { namespace PartDefinition
{
  enum class ScanPath;

  class ISpecimen;
  using ISpecimenPtr = std::shared_ptr<ISpecimen>;
  using ISpecimenConstPtr = std::shared_ptr<const ISpecimen>;

  class IWeldedSpecimen;
  using IWeldedSpecimenPtr = std::shared_ptr<IWeldedSpecimen>;
  using IWeldedSpecimenConstPtr = std::shared_ptr<const IWeldedSpecimen>;

class IScanPlan
{
public:
  virtual ~IScanPlan() = default;

  virtual IPatchCollectionConstPtr GetPatches() const = 0;
  virtual IPatchCollectionPtr GetPatches() = 0;
  virtual ISpecimenConstPtr GetSpecimen() const = 0;
  virtual ISpecimenPtr GetSpecimen() = 0;

  virtual Equipment::IAcquisitionUnitCollectionPtr GetAcquisitionUnits() = 0;
  virtual Equipment::IAcquisitionUnitCollectionConstPtr GetAcquisitionUnits() const = 0;

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  /// Creates a plate specimen
  ///
  /// @param length
  ///  The length is parallel to the scan axis and measured in millimeters
  /// @param width
  ///  The width is parallel to the index axis and measured in millimeters
  /// @param thick
  ///  The thickness is parallel to the ultrasound axis and measured in millimeters
  ///
  /// @return
  /// The newly created specimen
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  virtual ISpecimenPtr CreatePlateSpecimen(
    double length, double width, double thick) = 0;

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  /// Creates a pipe specimen
  ///
  /// @param length
  ///  The length is measured along the cylinder�s axis and measured in millimeters
  /// @param diam
  ///  The diameter of the pipe measured in millimeters
  /// @param thick
  ///  The thickness of the pipe wall measured in millimeters
  /// @param scanPath
  ///  Indicates if the scan axis will be around the pipe or along the pipe
  /// @param angleStart
  ///  When the inspection is not 360 degree, angle start represents the beginning of the inspection in degree
  /// @param angleRange
  ///  When the inspection is not 360 degree, angle range is the number of degree covered by the inspection
  ///
  /// @return
  /// The newly created specimen
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  virtual ISpecimenPtr CreatePipeSpecimen(
    double length, double diam, double thick,
    ScanPath scanPath, double angleStart = 0., double angleRange = 360.) = 0;

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  /// Creates a bar specimen
  ///
  /// @param length
  ///  The length is measured along the cylinder�s axis and measured in millimeters
  /// @param diam
  ///  The diameter of the bar measured in millimeters
  /// @param scanPath
  ///  Indicates if the scan axis will be around the pipe or along the pipe
  ///
  /// @return
  /// The newly created specimen
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  virtual ISpecimenPtr CreateBarSpecimen(double length, double diam, ScanPath scanPath) = 0;

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  /// Creates a plate specimen including a weld overlay
  ///
  /// @param length
  ///  The length is parallel to the scan axis and measured in millimeters
  /// @param width
  ///  The width is parallel to the index axis and measured in millimeters
  ///
  /// @return
  /// The newly created specimen
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  virtual IWeldedSpecimenPtr CreateWeldedPlateSpecimen(
    double length, double width) = 0;

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  /// Creates a pipe specimen including a weld overlay
  ///
  /// @param length
  ///  The length is measured along the cylinder�s axis and measured in millimeters
  /// @param diam
  ///  The diameter of the pipe measured in millimeters
  /// @param scanPath
  ///  Indicates if the scan axis will be around the pipe or along the pipe
  /// @param angleStart
  ///  When the inspection is not 360 degree, angle start represents the beginning of the inspection in degree
  /// @param angleRange
  ///  When the inspection is not 360 degree, angle range is the number of degree covered by the inspection
  ///
  /// @return
  /// The newly created specimen
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  virtual IWeldedSpecimenPtr CreateWeldedPipeSpecimen(
    double length, double diam,
    ScanPath scanPath, double angleStart = 0., double angleRange = 360.) = 0;
  virtual Equipment::IEquipmentFactoryPtr GetEquipmentFactory() const = 0;
  virtual Inspection::IInspectionMethodFactoryPtr GetInspectionMethodFactory() const = 0;
  virtual Inspection::IInspectionMethodCollectionPtr GetInspectionMethodCollection() const = 0;
  virtual Inspection::ITransformation2DPtr GetSpecimenPosition() const = 0;
};

using IScanPlanPtr = std::shared_ptr<IScanPlan>;
using IScanPlanConstPtr = std::shared_ptr<const IScanPlan>;
}}
