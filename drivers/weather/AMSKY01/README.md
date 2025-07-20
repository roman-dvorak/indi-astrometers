# AMSKY01 INDI Weather Station Driver

Professional weather station driver for cloud sensing, sky brightness monitoring, and environmental measurements.

## Author
**Roman Dvořák** - info@astrometers.cz  
Copyright (C) 2025 Astrometers

## Features

### Environmental Monitoring
- **Temperature:** Real-time temperature measurement
- **Humidity:** Relative humidity monitoring
- **Dew Point:** Calculated dew point temperature
- **Sky Brightness:** Light intensity in lux
- **Cloud Coverage:** Multi-sensor cloud detection

### Data Sources
- **Hygro Sensor:** Temperature and humidity
- **Light Sensor:** 4-channel light measurement + calculated lux
- **Cloud Sensors:** 5-point cloud detection array

### FITS Header Integration
Weather data is automatically added to FITS headers:
- `WEATHER_TEMPERATURE` - Temperature in °C
- `WEATHER_HUMIDITY` - Humidity in %
- `WEATHER_DEWPOINT` - Dew point in °C  
- `WEATHER_SKY_BRIGHTNESS` - Sky brightness in lux
- `WEATHER_CLOUD_COVERAGE` - Cloud coverage in %

## Data Format

The AMSKY01 sends data via serial connection in CSV format:

### Hygro Data
```
$hygro,20.66,60.90
```
Format: `$hygro,temperature,humidity`
- Temperature in °C
- Humidity in %

### Light Data
```
$light,0.000060,15051,4334,9876,400
```
Format: `$light,lux,ch0,ch1,ch2,ch3`
- Lux: Calculated illuminance
- ch0-ch3: Raw sensor channel values

### Cloud Data
```
$cloud,65042.19,64983.19,64937.69,64981.94,64787.19
```
Format: `$cloud,sensor1,sensor2,sensor3,sensor4,sensor5`
- 5 cloud sensor readings for coverage analysis

## Configuration

### Connection Settings
- **Port:** /dev/ttyUSB1 (configurable)
- **Baud Rate:** 9600
- **Polling Period:** 10 seconds (configurable)

### Weather Parameters
All parameters are automatically monitored and reported:
- Temperature range: -50°C to +70°C
- Humidity range: 0% to 100%
- Dew point: Calculated using Magnus formula
- Sky brightness: 0 to 100,000 lux
- Cloud coverage: 0% to 100%

## User Interface

### Weather Tab
- **Current Conditions:** Real-time weather display
- **Update Status:** Last update time and status
- **Weather Parameters:** All monitored values

### Cloud Tab
- **Cloud Sensor 1-5:** Individual sensor readings
- **Cloud Coverage:** Calculated overall coverage

### Light Tab  
- **Illuminance:** Sky brightness in lux
- **Channel 0-3:** Raw light sensor values
- **Light Conditions:** Brightness analysis

## Technical Implementation

### Dew Point Calculation
```cpp
float calculateDewPoint(float temp, float humidity) {
    const float a = 17.27;
    const float b = 237.7;
    
    float alpha = (a * temp) / (b + temp) + log(humidity / 100.0);
    float dewPoint = (b * alpha) / (a - alpha);
    
    return dewPoint;
}
```

### Cloud Coverage Algorithm
Simple average-based calculation (customizable):
```cpp
float avgCloud = (cloud1 + cloud2 + cloud3 + cloud4 + cloud5) / 5.0;
float cloudCoverage = (avgCloud / 65000.0) * 100.0;
```

### CSV Parsing
Robust parser handles real-time CSV stream:
- Line-buffered input processing
- Multi-format data recognition
- Error handling and validation

## Build Instructions

From project root:
```bash
mkdir build
cd build
cmake ..
make
sudo make install
```

Installs to:
- **Binary:** `/usr/local/bin/indi_amsky01`
- **XML:** `/usr/local/share/indi/indi_amsky01.xml`

## Usage with Astronomy Software

### KStars/Ekos Integration
1. Start INDI server with AMSKY01
2. Connect in KStars Equipment panel
3. Weather data appears in Observatory tab
4. FITS headers automatically include weather data

### Manual Operation
```bash
# Start driver
indi_amsky01

# Start with verbose output
indi_amsky01 -v
```

## FITS Header Example

When capturing images, headers will include:
```
WEATHER_TEMPERATURE = 20.66 / Temperature in Celsius
WEATHER_HUMIDITY = 60.90 / Relative humidity in percent  
WEATHER_DEWPOINT = 12.45 / Dew point in Celsius
WEATHER_SKY_BRIGHTNESS = 0.000060 / Sky brightness in lux
WEATHER_CLOUD_COVERAGE = 15.2 / Cloud coverage percentage
```

## Calibration

### Cloud Sensors
Cloud coverage calculation may need adjustment for your location:
```cpp
// Adjust this factor based on your sensor calibration
float cloudCoverage = (avgCloud / CALIBRATION_FACTOR) * 100.0;
```

### Light Sensors
Lux values are provided by the sensor, but you may want to:
- Calibrate against known light sources
- Adjust for local conditions
- Set thresholds for observing conditions

## Troubleshooting

### Common Issues
1. **No Data:** Check serial port and permissions
2. **Incomplete Readings:** Verify baud rate and connections
3. **Wrong Values:** Check sensor calibration
4. **FITS Headers Missing:** Ensure weather station is connected in client

### Debug Mode
Enable debug output to see raw CSV data:
```bash
indi_amsky01 -v
```

## Version History

### v1.0 (2025)
- Initial release
- Complete CSV parsing
- FITS header integration
- Multi-sensor support
- Real-time monitoring

## Support

For technical support:
- **Email:** info@astrometers.cz
- **Documentation:** This README and code comments
- **Protocol:** CSV format as documented above

---
**AMSKY01 INDI Weather Station Driver v1.0**
