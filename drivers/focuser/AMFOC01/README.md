# AMFOC01 INDI Driver

Advanced INDI driver for AMFOC01 focuser with comprehensive feature set.

## Author
**Roman Dvořák** - info@astrometers.cz  
Copyright (C) 2025 Astrometers

## Features

### Core Functionality
- **Absolute Positioning:** Slider control with 1,000,000 step range
- **Relative Positioning:** Step-based incremental movement
- **Position Sync:** Synchronize current position with known value
- **Real-time Polling:** Automatic position and temperature updates

### Temperature Compensation
- **Three Modes:**
  - **Off:** No compensation
  - **Driver:** Software-based compensation
  - **Focuser:** Hardware-based compensation
- **Configurable Parameters:**
  - Coefficient: -999.9 to +999.9 steps/°C
  - Update period: 1-3600 seconds
  - Temperature threshold: 0.01-10.0°C

### Communication
- **Protocol:** AMFOC01 serial protocol
- **Baud Rate:** 9600
- **Timeout:** 10ms
- **Connection:** Serial port (/dev/ttyUSB0 default)

## Protocol Implementation

### Implemented Commands

#### GET Commands
| Command | Description | Response |
|---------|-------------|----------|
| `:GP#` | Get actual position | 4-digit hex |
| `:GN#` | Get target position | 4-digit hex |
| `:GT#` | Get temperature | 4-digit hex |
| `:GI#` | Motor moving status | 00# or 01# |
| `:GD#` | Device type | Decimal |
| `:GV#` | Firmware version | Decimal |
| `:GH#` | Maximum position | 4-digit hex |
| `:GC#` | Command status | Hex value |

#### SET Commands
| Command | Description | Parameter |
|---------|-------------|-----------|
| `:SN<pos>#` | Set future position | 5-digit hex |
| `:SP<pos>#` | Set current position | 5-digit hex |
| `:SD<type>#` | Set device type | 5-digit hex |
| `:SC<val>#` | Set calibration | 5-digit hex |
| `:PO<val>#` | Set output port | 3-digit hex |

#### Motion Commands
| Command | Description |
|---------|-------------|
| `:FG#` | Start movement |
| `:FQ#` | Stop movement |
| `:C##` | Measure temperature |

## Usage Examples

### Basic Movement
```cpp
// Move to absolute position 50000
gotoAbsolutePosition(50000);

// Move 1000 steps outward
gotoRelativePosition(1000);

// Sync current position to 30000
syncPosition(30000);
```

### Temperature Compensation
```cpp
// Enable driver-based compensation
tempCompEnabled = true;
tempCompInDriver = true;
tempCoefficient = -2.5; // steps per degree C

// The driver will automatically compensate for temperature changes
```

## File Structure

```
AMFOC01/
├── amfoc01.h                     # Header file
├── amfoc01.cpp                   # Main implementation
├── CMakeLists.txt                # Build configuration
├── indi_amfoc01.xml.cmake        # INDI device definition
├── README.md                     # This file
├── ENHANCED_FEATURES.md          # Feature documentation
├── POLLING_FEATURES.md           # Polling implementation
├── PROTOCOL_IMPLEMENTATION.md    # Protocol details
└── TEMPERATURE_COMPENSATION.md   # Temperature compensation
```

## Build Instructions

From the project root:
```bash
mkdir build
cd build
cmake ..
make
sudo make install
```

## Configuration

### Connection Settings
- **Port:** /dev/ttyUSB0 (configurable)
- **Baud Rate:** 9600 (fixed)
- **Timeout:** 10ms (protocol requirement)

### Temperature Compensation Settings
1. Set coefficient (e.g., -2.5 steps/°C)
2. Choose mode (Driver/Focuser/Off)
3. Configure update period and threshold
4. Enable the selected mode

## Troubleshooting

### Common Issues
1. **Connection Failed:** Check USB port and permissions
2. **No Position Updates:** Verify baud rate and cable
3. **Temperature Not Reading:** Check sensor connection
4. **Compensation Not Working:** Verify coefficient and mode

### Debug Mode
Run with verbose output:
```bash
indi_amfoc01 -v
```

## Version History

### v1.0 (2025)
- Initial release
- Complete protocol implementation
- Temperature compensation
- Real-time polling
- Position sync functionality

## Support

For technical support:
- **Email:** info@astrometers.cz
- **Documentation:** See accompanying .md files
- **Protocol:** Refer to PROTOCOL_IMPLEMENTATION.md

---
**AMFOC01 INDI Driver v1.0**
