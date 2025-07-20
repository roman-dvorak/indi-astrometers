# AMTEST01 - INDI Test Driver

Simple INDI driver for testing serial communication and console output.

## Features

- **Serial Communication**: Connects to any serial device (real or simulated)
- **Console Output**: Prints all received data to console with timestamps
- **INDI Integration**: Full INDI driver with properties for control and status
- **Simulation Mode**: Built-in test data generation

## Properties

### Connection Properties
- `CONNECTION`: Standard INDI connection control
- `DEVICE_PORT`: Serial port selection (default: /dev/ttyACM0)
- `DEVICE_BAUD_RATE`: Baud rate selection (default: 9600)

### Control Properties
- `READ_DATA`: Start/Stop continuous data reading
  - `START`: Begin reading serial data
  - `STOP`: Stop reading serial data

### Status Properties
- `DEVICE_STATUS`: Current device status
  - `DEVICE`: Device name (AMTEST01)
  - `STATUS`: Connection status
  - `LAST_DATA`: Last received data line

## Usage

### Installation
```bash
# Build and install
cd /path/to/indi-astrometers
mkdir build && cd build
cmake .. && make -j4
sudo make install
```

### Running with INDI Server
```bash
# Start INDI server
indiserver indi_amtest01

# In another terminal:
# Connect in simulation mode
indi_setprop "AMTEST01.SIMULATION.ENABLE=On"
indi_setprop "AMTEST01.CONNECTION.CONNECT=On"

# Start reading data
indi_setprop "AMTEST01.READ_DATA.START=On"

# Check status
indi_getprop "AMTEST01.DEVICE_STATUS.*"

# Stop reading
indi_setprop "AMTEST01.READ_DATA.STOP=On"
```

### Real Serial Device
```bash
# Set port and connect to real device
indi_setprop "AMTEST01.DEVICE_PORT.PORT=/dev/ttyUSB0"
indi_setprop "AMTEST01.CONNECTION.CONNECT=On"
indi_setprop "AMTEST01.READ_DATA.START=On"
```

## Data Format

The driver reads line-based data from the serial port. Any text data ending with newline character is processed and:

1. **Displayed in console** with timestamp: `[AMTEST01] [HH:MM:SS] DATA: received_line`
2. **Updated in INDI property**: `DEVICE_STATUS.LAST_DATA`
3. **Logged via INDI**: Available in INDI logs

## Simulation Mode

When in simulation mode, the driver generates test data:
```
TEST_DATA_0,temperature=20.0,humidity=50.0
TEST_DATA_1,temperature=21.0,humidity=51.0
...
```

## Console Output Examples

```
[AMTEST01] Device connected successfully
[AMTEST01] Started continuous data reading
[AMTEST01] [16:15:23] DATA: sensor_reading,temp=22.5,humidity=65.2
[AMTEST01] [16:15:24] DATA: status_update,battery=85,signal=good
[AMTEST01] [16:15:25] DATA: measurement,pressure=1013.25,altitude=120
[AMTEST01] Stopped data reading
```

## Version History

- **v1.0**: Initial release with basic serial reading and console output

## Author

Roman Dvořák <info@astrometers.cz>  
Copyright (C) 2025 Astrometers
