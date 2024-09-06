#ifndef ASCANDATATYPES_HPP
#define ASCANDATATYPES_HPP

#include <string>
#include <vector>
#include <optional>
#include <cstdint>

///
/// OMNISCAN X3 SERIES DATA TYPE FORMATING
///

/*
                     Root
                      |
         +------------+------------+
    $schema       version      scenario
                      |
                +-----+-----+
              id           name
                      |
                     paut
          +-----------+---------------------+
          |                                 |
          +-- dataEncodingId                +-- beams (Beam settings)
          +-- pulseEcho                     |    +-- id
          |    +-- linearFormation          |    +-- skewAngle
          |    |   +-- probeFirstEl.        |    +-- refractedAngle
          |    |   +-- probeLastEl.         |    +-- beamDelay
          |    |   +-- elementStep          |    +-- ascanStart
          |    |   +-- elementAperture      |    +-- ascanLength
          |    |   +-- beamRefracted        |    +-- gainOffset
          |    +-- probeId                  |    +-- sumGainMode
          +-- waveMode                      |    +-- sumGain
          +-- velocity                      +-- pulsers (Pulse settings)
          +-- focusing (Focus details)      |    +-- id
               +-- mode                     |    +-- elementId
               +-- distance                 |    +-- delay
                                            +-- receivers (Receiver settings)
                                                    +-- id
                                                    +-- elementId
                                                    +-- delay
*/


/// ascan[0][1][2] => ascan [z][y][x]
/// index = z * (xsize * ysize) + y * xsize + x;
/// x full => y full => z full
/// pos x,y = y*xsize + x
/// pos x,y,z = z*(xsize*ysize) + y*xsize + x

struct Color {
    uint8_t R, G, B;
};

struct BeamDescription
{
    double Velocity{};
    double SkewAngle{};
    double RefractedAngle{};
    std::optional<double> UCoordinateOffset{};
    std::optional<double> VCoordinateOffset{};
    std::optional<double> UltrasoundOffset{};
};

struct DimensionAxis
{
    std::string Name;
    uint64_t Quantity{};
    std::optional<double> Resolution{};
    std::optional<double> Offset{};
};

struct AmplitudeRange
{
    double PercentMin{};
    double PercentMax{};
    int64_t SamplingMin{};
    int64_t SamplingMax{};
};

struct AscanData
{
    AmplitudeRange AmplitudeRng;

    std::optional<double> Velocity{};
    std::optional<double> SkewAngle{};
    std::optional<double> RefractedAngle{};

    std::vector<DimensionAxis> AmplitudeAxes;
    std::vector<int16_t> Amplitudes;

    std::vector<DimensionAxis> StatusAxes;
    std::vector<uint8_t> Statuses;

    std::optional<std::vector<BeamDescription>> BeamDescriptions;
};

#endif // ASCANDATATYPES_HPP
