# INDI Astrometers Drivers

This repository contains INDI drivers for Astrometers devices.

## Author
**Roman Dvořák** - info@astrometers.cz  
Copyright (C) 2025 Astrometers

## Supported Devices

### AMFOC01 - Focuser Driver

Professional focuser with advanced features:

#### Features:
- **Position Control:** Absolute positioning with 1,000,000 step range
- **Sync Functionality:** Position synchronization support
- **Temperature Monitoring:** Real-time temperature reading
- **Temperature Compensation:** 
  - Driver mode (software compensation)
  - Focuser mode (hardware compensation)
- **Real-time Polling:** Automatic status updates
- **Protocol Support:** Complete AMFOC01 serial protocol implementation

#### Technical Specifications:
- **Communication:** Serial 9600 baud, 10ms timeout
- **Position Range:** 0 - 1,000,000 steps
- **Temperature Range:** -50°C to +70°C
- **Compensation Range:** -999.9 to +999.9 steps/°C

## Building

### Prerequisites
- CMake 3.13+
- INDI development libraries
- C++14 compatible compiler

### Build Instructions
```bash
mkdir build
cd build
cmake ..
make
sudo make install
```

### Installation Locations
- **Binary:** `/usr/local/bin/indi_amfoc01`
- **XML Definition:** `/usr/local/share/indi/indi_amfoc01.xml`

## Usage

### Starting the Driver
```bash
# Start driver directly
indi_amfoc01

# Start with verbose output
indi_amfoc01 -v
```

### INDI Client Connection
The driver appears as "AMFOC01" in INDI clients like KStars/Ekos.

### Configuration Tabs

#### Main Control
- **Absolute Position:** Slider control (0-1,000,000)
- **Relative Position:** Step-based movement
- **Sync Position:** Position synchronization
- **Temperature:** Real-time display

#### Options
- **Temperature Compensation Mode:** Off/Driver/Focuser
- **Temperature Coefficient:** Compensation rate (steps/°C)
- **Compensation Settings:** Update period and threshold

## Protocol Documentation

The driver implements the complete AMFOC01 serial protocol:

### GET Commands (Reading)
- `:GP#` - Get actual position
- `:GN#` - Get target position  
- `:GT#` - Get temperature
- `:GI#` - Check if motor is moving
- `:GD#` - Get device type
- `:GV#` - Get firmware version
- `:GH#` - Get maximum position
- `:GC#` - Get command status

### SET Commands (Configuration)
- `:SN<pos>#` - Set future position
- `:SP<pos>#` - Set current position (sync)
- `:SD<type>#` - Set device type
- `:SC<val>#` - Set calibration value
- `:PO<val>#` - Set output port

### Motion Commands
- `:FG#` - Start movement to future position
- `:FQ#` - Stop movement
- `:C##` - Trigger temperature measurement

## Development

### Project Structure
```
indi-astrometers/
├── CMakeLists.txt
├── drivers/
│   └── focuser/
│       └── AMFOC01/
│           ├── amfoc01.h
│           ├── amfoc01.cpp
│           ├── CMakeLists.txt
│           └── indi_amfoc01.xml.cmake
└── README.md
```

### Contributing
1. Fork the repository
2. Create a feature branch
3. Make changes with proper testing
4. Submit pull request

## License

This project is licensed under the terms specified in the LICENSE file.

## Support

For support and questions, contact:
- **Email:** info@astrometers.cz
- **Website:** https://www.astrometers.cz

---
**INDI Astrometers Drivers v1.0**
