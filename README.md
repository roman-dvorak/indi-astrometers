# INDI Drivers for AstroMeters.eu

![AstroMeters Logo](https://astrometers.eu/images/logo.png)

This repository contains official INDI drivers for devices from **AstroMeters.eu**, a brand specializing in high-quality components for astronomical observations. Our goal is to enable you to fully harness the potential of your telescope and entire observation assembly.

---

## 🔭 Supported Devices

### 1. **AMFOC01 - Focuser**
- **Description**: A professional, precise focuser with advanced features.
- **Key Features**:
  - Absolute positioning with a 1,000,000 step range
  - Temperature compensation (software & hardware modes)
  - Real-time position and temperature monitoring
  - Position synchronization

### 2. **AMSKY01 - Weather Station**
- **Description**: A multi-sensor device for monitoring observing conditions.
- **Key Features**:
  - Measures temperature, humidity, dew point, sky brightness (lux), and cloud coverage.
  - Automatically adds weather data to FITS headers.
  - Continuous data streaming via serial port.

### 3. **AMTEST01 - Test Driver**
- **Description**: A simple test driver for debugging serial communication.
- **Key Features**:
  - Connects to any serial device.
  - Prints all received data to the console with timestamps.
  - Includes a simulation mode for development.

---

## 📦 Installation

### Prerequisites
- A Linux-based operating system (e.g., Stellarmate, Astroberry)
- INDI Library (`libindi-dev`)
- CMake 3.13+

### Build Instructions

To build and install the drivers, run the following commands in your terminal:

```bash
# Clone the repository
git clone https://github.com/your-repo/indi-astrometers.git
cd indi-astrometers

# Create a build directory
mkdir build
cd build

# Configure and compile
cmake ..
make -j4

# Install the drivers
sudo make install
```

### Installation Paths
- **Binaries**: `/usr/bin/`
- **XML Definitions**: `/usr/share/indi/`

---

## 🚀 Usage

After installation, the drivers will be available in any INDI-compatible client (e.g., KStars/Ekos, N.I.N.A., CCDCiel).

1.  **Start your INDI server.**
2.  **Connect your client** (e.g., Ekos).
3.  **Find the AstroMeters devices** in the appropriate categories (Focuser, Weather).
4.  **Connect** and enjoy precise control over your astronomical setup!

---

## 🤝 Support & Contribution

As a brand that brings you components we use ourselves, we welcome your feedback and contributions.

- **Expert Support**: For questions and support, please visit our website.
- **Contribute**: Feel free to fork the repository, create a feature branch, and submit a pull request.

**AstroMeters.eu** - Your partner on an incredible journey of exploring the infinite.

- **Website**: [https://astrometers.eu](https://astrometers.eu)
- **Contact**: info@astrometers.eu

