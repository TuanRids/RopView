#pragma once

namespace Olympus { namespace Indication {

inline namespace ReadingName {
  ///  Amplitude peak in gate A: %
  constexpr wchar_t  APercent[] = L"A%";
  ///  Difference in dB between the amplitude peak value in gate A and the gate A level: dB
  constexpr wchar_t  AdBA[] = L"AdBA";
  ///  Difference in dB between the amplitude peak value in gate A and the amplitude peak value in gate A on the reference curve: dB
  constexpr wchar_t  AdBR[] = L"AdBR";
  ///  Amplitude peak in gate B: %
  constexpr wchar_t  BPercent[] = L"B%";
  ///  Difference in dB between the amplitude peak value in gate B and the gate B level: dB
  constexpr wchar_t  BdBB[] = L"BdBB";
  ///  Difference in dB between the amplitude peak value in gate B and the amplitude peak value in gate B on the reference curve: dB
  constexpr wchar_t  BdBR[] = L"BdBR";
  ///  Position in gate A (according to the gate measurement mode): second
  constexpr wchar_t  APosition[] = L"APosition";
  ///  Position in gate B (according to the gate measurement mode): second
  constexpr wchar_t  BPosition[] = L"BPosition";
  ///  Position in gate I (according to the gate measurement mode): second
  constexpr wchar_t  IPosition[] = L"IPosition";
  ///  Position in gate I using the velocity of water (according to the gate measurement mode): second
  constexpr wchar_t  IPositionWater[] = L"IPositionWater";
  ///  Thickness (according to the current thickness mode): second
  constexpr wchar_t  Thickness[] = L"Thickness";
  ///  Percentage of material loss: %
  constexpr wchar_t  MaterialLoss[] = L"MaterialLoss";
  ///  Amplitude value of the reference position: %
  constexpr wchar_t  Ar[] = L"Ar";
  ///  Amplitude value of the measure position: %
  constexpr wchar_t  Am[] = L"Am";
  ///  Amplitude difference between reference and measure position: %
  constexpr wchar_t  Amr[] = L"Amr";
  ///  Reference position on the ultrasound axis: second
  constexpr wchar_t  Ur[] = L"Ur";
  ///  Measure position on the ultrasound axis: second
  constexpr wchar_t  Um[] = L"Um";
  ///  Distance between reference and measure position on the ultrasound axis: second
  constexpr wchar_t  Umr[] = L"Umr";
  ///  Reference position on the probe primary axis: second
  constexpr wchar_t  Pr[] = L"Pr";
  ///  Measurement position on the probe primary axis: second
  constexpr wchar_t  Pm[] = L"Pm";
  ///  Distance between reference and measure positions on the probe primary axis: second
  constexpr wchar_t  Pmr[] = L"Pmr";
  ///  Scan position of the reference: millimeter
  constexpr wchar_t  Sr[] = L"Sr";
  ///  Scan position of the measure: millimeter
  constexpr wchar_t  Sm[] = L"Sm";
  ///  Scan distance between reference and measure position: millimeter
  constexpr wchar_t  Smr[] = L"Smr";
  ///  Index position of the reference: millimeter
  constexpr wchar_t  Ir[] = L"Ir";
  ///  Index position of the measure: millimeter
  constexpr wchar_t  Im[] = L"Im";
  ///  Index distance between reference and measure position: millimeter
  constexpr wchar_t  Imr[] = L"Imr";
  ///  Distance between the beam exit point and the gate A position: millimeter
  ///  @image html Readings_RA_PA_DA.png width=30%
  constexpr wchar_t  RA[] = L"RA";
  ///  Distance between the beam exit point and the gate B position: millimeter
  constexpr wchar_t  RB[] = L"RB";
  ///  Distance between the probe front face and the gate A position: millimeter
  constexpr wchar_t  PA[] = L"PA";
  ///  Distance between the probe front face and the gate B position: millimeter
  constexpr wchar_t  PB[] = L"PB";
  ///  Depth at gate A position: millimeter
  constexpr wchar_t  DA[] = L"DA";
  ///  Depth at gate B position: millimeter
  constexpr wchar_t  DB[] = L"DB";
  ///  Total sound path at gate A position: millimeter
  /// @image html Readings_SA_LA.png width=30%
  constexpr wchar_t  SA[] = L"SA";
  ///  Total sound path at gate B position: millimeter
  constexpr wchar_t  SB[] = L"SB";
  ///  Volumetric position in gate A on the scan axis: millimeter
  /// @image html Readings_ViA_VsA.png width=30%
  constexpr wchar_t  VsA[] = L"VsA";
  ///  Volumetric position in gate B on the scan axis: millimeter
  constexpr wchar_t  VsB[] = L"VsB";
  ///  Skip qty at gate A position: cardinal
  constexpr wchar_t  LA[] = L"LA";
  ///  Skip qty at gate B position: cardinal
  constexpr wchar_t  LB[] = L"LB";
  ///  Defect Length at gate A amplitude peak value according to API code calibration: millimeter
  constexpr wchar_t  API_DL[] = L"API_DL";
  ///  Half Wave width 6 dB lower than the amplitude peak value in gate A: millimeter
  constexpr wchar_t API_HW[] = L"API_HW";
  ///  Gain needed to set the signal in gate A to 80%: dB
  constexpr wchar_t AWS_A[] = L"AWS_A";
  ///  Reference gain of the AWS code: dB
  constexpr wchar_t AWS_B[] = L"AWS_B";
  ///  Factor for the attenuation: factor
  constexpr wchar_t AWS_C[] = L"AWS_C";
  ///  Factor that allow us to class the defect according to a table: factor
  constexpr wchar_t AWS_D[] = L"AWS_D";
  ///  Defect class in gate A at the current angle, if it's a supported angle (45, 60, 70): class
  constexpr wchar_t AWS_CLASS_CL[] = L"AWS_CLASS_CL";
  ///  Depth of the reference position: millimeter
  constexpr wchar_t  TOFD_ReferencePosition[] = L"TOFD_ReferencePosition";
  ///  Depth of the measure position: millimeter
  constexpr wchar_t  TOFD_MeasurePosition[] = L"TOFD_MeasurePosition";
  ///  Length of the defect. TOFD(r) - TORD(m) (absolute): millimeter
  constexpr wchar_t  TOFD_Length[] = L"TOFD_Length";
  ///  Volumetric position in gate A on the index axis: millimeter
  constexpr wchar_t  ViA[] = L"ViA";
  ///  Volumetric position in gate B on the index axis: millimeter
  constexpr wchar_t  ViB[] = L"ViB";
  ///  Signal's amplitude at the ultrasound measure position: %
  constexpr wchar_t  Amplitude_Um[] = L"Amplitude_Um";
  ///  Signal's amplitude at the ultrasound reference position: %
  constexpr wchar_t  Amplitude_Ur[] = L"Amplitude_Ur";
  ///  Equivalent Reflector Size, for DGS: millimeter
  constexpr wchar_t  ERS[] = L"ERS";
  ///  For sizing, the difference in between the signal amplitude peak in gate A and the sizing curve level
  constexpr wchar_t  APercentCurve[] = L"A%Curve";
  ///  For sizing, the difference in between the signal amplitude peak in gate B and the sizing curve level
  constexpr wchar_t  BPercentCurve[] = L"B%Curve";
  ///  For sizing, the difference in dB between the signal amplitude peak in gate A and the sizing curve level: dB
  constexpr wchar_t  AdBCurve[] = L"AdBCurve";
  ///  For sizing, the difference in dB between the signal amplitude peak in gate B and the sizing curve level: dB
  constexpr wchar_t  BdBCurve[] = L"BdBCurve";
  ///  For sizing, the absolute Maximum difference in dB between the signal amplitude peak in gate A and the sizing curve level: dB
  constexpr wchar_t  MaxAdBCurve[] = L"MaxAdBCurve";
  ///  For sizing, the absolute Maximum difference in dB between the signal amplitude peak in gate B and the sizing curve level: dB
  constexpr wchar_t  MaxBdBCurve[] = L"MaxBdBCurve";
  ///  For sizing, the absolute Maximum difference in between the signal amplitude peak in gate A and the sizing curve level
  constexpr wchar_t  MaxAPercentCurve[] = L"MaxA%Curve";
  ///  For sizing, the absolute Maximum difference in between the signal amplitude peak in gate B and the sizing curve level
  constexpr wchar_t  MaxBPercentCurve[] = L"MaxB%Curve";
  ///  Diagonal distance between I and U reference and measure positions: millimeter
  constexpr wchar_t  IUmr[] = L"IUmr";
  ///  Measure position on the ultrasound axis with the water velocity: second
  constexpr wchar_t  Uwm[] = L"Uwm";
  ///  Reference position on the ultrasound axis with the water velocity: second
  constexpr wchar_t  Uwr[] = L"Uwr";
  ///  Distance between reference and measure position on the ultrasound axis with the water velocity: second
  constexpr wchar_t  Uwmr[] = L"Uwmr";
  ///  Minimum thickness on this beamset for the complete patch: millimeter
  constexpr wchar_t  MinimumThickness[] = L"MinimumThickness";
  ///  Minimum thickness scan position - For each group for the complete patch: millimeter
  constexpr wchar_t  MinimumThickness_ScanPosition[] = L"MinimumThickness_ScanPosition";
  ///  Minimum thickness index position - For each group for the complete patch: millimeter
  constexpr wchar_t  MinimumThickness_IndexPosition[] = L"MinimumThickness_IndexPosition";
  ///  Beam on which the minimum thickness is located
  constexpr wchar_t  MinimumThickness_ContextIndex[] = L"MinimumThickness_ContextIndex";
  ///  Minimum Thickness on this beamset inside the zone defined by positions: millimeter
  constexpr wchar_t  MinimumThicknessZ[] = L"MinimumThicknessZ";
  ///  Minimum thickness scan position - For each group for zone created by the reference and measure positions: millimeter
  constexpr wchar_t  MinimumThickness_ScanPositionZ[] = L"MinimumThickness_ScanPositionZ";
  ///  Minimum thickness index position - For each group for zone created by the reference and measure positions: millimeter
  constexpr wchar_t  MinimumThickness_IndexPositionZ[] = L"MinimumThickness_IndexPositionZ";
  ///  Beam on which the minimum thickness is located for the zone created by the reference and measure positions: millimeter
  constexpr wchar_t  MinimumThickness_ContextIndexZ[] = L"MinimumThickness_ContextIndexZ";
  ///  Amplitude peak in gate I: %
  constexpr wchar_t  IPercent[] = L"I%";
  ///  Minimum ERS in the zone created by the reference and measure positions: millimeter
  constexpr wchar_t  MinimumERSZ[] = L"MinimumERSZ";
  ///  Position on the scan axis of the minimum ERS in the zone created by the reference and measure positions: millimeter
  constexpr wchar_t  MinimumERS_ScanPositionZ[] = L"MinimumERS_ScanPositionZ";
  ///  Position on the index axis of the minimum ERS in the zone created by the reference and measure positions: millimeter
  constexpr wchar_t  MinimumERS_IndexPositionZ[] = L"MinimumERS_IndexPositionZ";
  ///  Angle of the context of the minimum ERS in the zone created by the reference and measure positions: degree
  constexpr wchar_t  MinimumERS_ContextIndexZ[] = L"MinimumERS_ContextIndexZ";
  ///  Maximum ERS in the zone created by the reference and measure positions: millimeter
  constexpr wchar_t  MaximumERSZ[] = L"MaximumERSZ";
  ///  Position on the scan axis of the maximum ERS in the zone created by the reference and measure positions: millimeter
  constexpr wchar_t  MaximumERS_ScanPositionZ[] = L"MaximumERS_ScanPositionZ";
  ///  Position on the index axis of the maximum ERS in the zone created by the reference and measure positions: millimeter
  constexpr wchar_t  MaximumERS_IndexPositionZ[] = L"MaximumERS_IndexPositionZ";
  ///  Angle of the context of the maximum ERS in the zone created by the reference and measure positions: degree
  constexpr wchar_t  MaximumERS_ContextIndexZ[] = L"MaximumERS_ContextIndexZ";
  ///  Depth at reference position: millimeter
  constexpr wchar_t  Dr[] = L"Dr";
  ///  Depth at measurement position: millimeter
  constexpr wchar_t  Dm[] = L"Dm";
  ///  Depth difference between reference and measure positions: millimeter
  constexpr wchar_t  Dmr[] = L"Dmr";
  ///  Difference in between the amplitude peak value in gate A and the amplitude peak value in gate A on the reference curve
  constexpr wchar_t  ApercentR[] = L"A%R";
  ///  Difference in between the amplitude peak value in gate B and the amplitude peak value in gate B on the reference curve
  constexpr wchar_t  BpercentR[] = L"B%R";
  ///  Aspect ratio of defect height to defect length as measured by the reference and measurement positions: ratio
  constexpr wchar_t  HL[] = L"HL";
  ///  Aspect ratio of defect height as measured by the reference and measurement positions compared to defined part thickness: ratio
  constexpr wchar_t  HT[] = L"HT";
  ///  Scan distance between the reference and measurement positions corrected for part curvature and flaw depth: millimeter
  constexpr wchar_t  Smr_Csc[] = L"Smr_Csc";
  ///  Reference position in seconds excluding wedge delay: second
  constexpr wchar_t  TOF_Ur[] = L"TOF_Ur";
  ///  Measure position in seconds excluding wedge delay: second
  constexpr wchar_t  TOF_Um[] = L"TOF_Um";
  ///  Difference between the measure and reference positions in seconds excluding wedge delay: second
  constexpr wchar_t  TOF_Umr[] = L"TOF_Umr";
  /// Volumetric position in gate A on the scan axis at the reference cursor position: millimeter
  constexpr wchar_t  VsAr[] = L"VsAr";
  /// Volumetric position in gate A on the scan axis at the measure cursor position: millimeter
  constexpr wchar_t  VsAm[] = L"VsAm";
  /// Volumetric position in gate B on the scan axis at the reference cursor position: millimeter
  constexpr wchar_t  VsBr[] = L"VsBr";
  /// Volumetric position in gate B on the scan axis at the measure cursor position: millimeter
  constexpr wchar_t  VsBm[] = L"VsBm";
  /// Volumetric position in gate A on the index axis at the reference cursor position: millimeter
  constexpr wchar_t  ViAr[] = L"ViAr";
  /// Volumetric position in gate A on the index axis at the measure cursor position: millimeter
  constexpr wchar_t  ViAm[] = L"ViAm";
  /// Volumetric position in gate B on the index axis at the reference cursor position: millimeter
  constexpr wchar_t  ViBr[] = L"ViBr";
  /// Volumetric position in gate B on the index axis at the measure cursor position: millimeter
  constexpr wchar_t  ViBm[] = L"ViBm";
}
}}