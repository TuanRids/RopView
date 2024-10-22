#ifndef DATA_PROCESS_H
#define DATA_PROCESS_H
#include "..\PAUTFileReader\AscanDattype.hpp"
#include "..\Model3D\CMesh.h"
#include "MainUI/statuslogs.h"
class IData3DProcecss {
public:
	virtual void getAscanData(const AscanData& ascanData) = 0;
	virtual Mesh processData() = 0;
protected:
	static AscanData m_dataset;

};

class Data3DProcess: public IData3DProcecss
{
private:
	void processData_volume();
public:
	Data3DProcess() {
		if (!sttlogs) { sttlogs = &nmainUI::statuslogs::getinstance(); } 
	};
	void getAscanData(const AscanData& ascanData) {m_dataset = ascanData;}
	Mesh processData() override;
	nmainUI::statuslogs* sttlogs;
};





#endif	// DATA_PROCESS_H

/*

Filtering points with z above the limits: You filter voxels whose values exceed a set threshold, helping to identify significant areas or defects.

Creating surfaces and objects: From the filtered points, you can generate 3D surfaces. These surfaces are then used to create separate objects in 3D space
	(such as the main volume and any defects).

Applying AI like YOLO3D: Once the surface is created, YOLO3D can be applied to detect objects on this surface. YOLO3D is particularly useful when you
	have clear objects and the space has been reduced to surfaces.

Applying FEM: After identifying the objects with YOLO3D, you can use the Finite Element Method (FEM) to simulate the physical properties of these objects.

*/






/*

X and Y Coordinates: These are already available from the scanning data, for example, from B-scan or C-scan:

X: Derived from the horizontal position during the scan.
Y: Derived from the vertical axis of the scanning area, based on the B-scan.
Z Coordinate: This is calculated from the A-scan data:

Z corresponds to the depth (vertical dimension) of the defect within the object, based on the reflected ultrasound signal.
When the A-scan signal is 0: This indicates areas without defects, as there is no reflection from those points.
When the A-scan signal is non-zero: This signals the presence of a defect, and the defect's depth can be determined by the point at which the A-scan signal starts to rise and returns to zero.
Example:
Start of the defect: At a specific X and Y position, if the A-scan signal begins rising at depth 30, this indicates the defect begins at Z = 30.
End of the defect: If the A-scan signal falls back to zero at depth 44, this indicates the defect ends at Z = 44.

*/
