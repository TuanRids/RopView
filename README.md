# 3D PAUT Visualization and Real-time Robotics Integration

## Overview
This project provides real-time 3D visualization of **Phased Array Ultrasonic Testing (PAUT)** data using **Vulkan**, integrated with **Finite Element Method (FEM)** analysis. It also incorporates real-time control of a **robot** to scan objects using **OmniScan**, and includes future plans for **AI**-based optimization in FEM processing.

## Key Features

### 1. 3D Phased Array Ultrasound Testing
- Real-time calculation, controlling, and rendering of PAUT data.
- 3D data processing from **CT scans**.
- Uses **Vulkan** to create a real-time 3D environment.

### 2. FEM Integration
- **Finite Element Method (FEM)** applied to the processed PAUT data.
- Use CUDA for calculating.
- Integrated into the Vulkan 3D viewport for accurate simulation.

### 3. Real-time Robot Control
- Real-time robot control and visualization using **IPC** or **pure C++**.
- **Vision detection** integrated to assist in autonomous PAUT scanning.
  
### 4. OmniScan Integration
- Real-time processing of OmniScan data, including video capture and display using **Vulkan**.
- **USB/Arduino** control of OmniScan, with real-time signal transmission for interaction.
- Full handling of **A, B, and C-scan** data.

### 5. AI Application (Upcoming)
- **AI** integration planned to enhance FEM analysis and optimize performance.
  
## Technologies Used
- **Vulkan**: Real-time 3D rendering and viewport creation.
- **C++**: Core development language for real-time control and data processing.
- **IPC**: For real-time communication between systems.
- **Arduino**: Used for controlling OmniScan signals.
- **FEM**: Applied for accurate simulation based on PAUT data.
- **AI (Planned)**: Future integration to optimize FEM processing.
- **CUDA**: For applying FEM Integration.
---

This project combines cutting-edge techniques in **real-time 3D graphics**, **robotics**, and **data processing**, with plans for **AI**-based analysis enhancements. The system is designed for scalability and advanced use cases, such as real-time industrial applications.
