/*
    AMFOC01 INDI Driver with Temperature Compensation
    
    Author: Roman Dvořák <info@astrometers.cz>
    Copyright (C) 2025 Astrometers
*/

#include "amfoc01.h"

#include <memory>
#include <cstring>
#include <cstdlib>
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>

// Indicate auto detection
std::unique_ptr<AMFOC01> amfoc01(new AMFOC01());

void ISGetProperties(const char *dev)
{
    amfoc01->ISGetProperties(dev);
}

void ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[], int n)
{
    amfoc01->ISNewSwitch(dev, name, states, names, n);
}

void ISNewText(const char *dev, const char *name, char *texts[], char *names[], int n)
{
    amfoc01->ISNewText(dev, name, texts, names, n);
}

void ISNewNumber(const char *dev, const char *name, double values[], char *names[], int n)
{
    amfoc01->ISNewNumber(dev, name, values, names, n);
}

void ISNewBLOB(const char *dev, const char *name, int sizes[], int blobsizes[], char *blobs[], char *formats[], char *names[], int n)
{
    INDI_UNUSED(dev);
    INDI_UNUSED(name);
    INDI_UNUSED(sizes);
    INDI_UNUSED(blobsizes);
    INDI_UNUSED(blobs);
    INDI_UNUSED(formats);
    INDI_UNUSED(names);
    INDI_UNUSED(n);
}

void ISSnoopDevice(XMLEle *root)
{
    amfoc01->ISSnoopDevice(root);
}

AMFOC01::AMFOC01()
{
    setDeviceName("AMFOC01");
    setVersion(1, 0);
    
    // We can connect via serial
    serialConnection = new Connection::Serial(this);
    serialConnection->registerHandshake([&]() { return callHandshake(); });
    registerConnection(serialConnection);
    
    // Set serial parameters according to protocol: 9600 baud, 10ms timeout
    serialConnection->setDefaultBaudRate(Connection::Serial::B_9600);
    serialConnection->setDefaultPort("/dev/ttyUSB0");
}

AMFOC01::~AMFOC01()
{
    delete serialConnection;
}

bool AMFOC01::initProperties()
{
    INDI::DefaultDevice::initProperties();
    
    // Device information
    IUFillText(&DeviceInfoT[0], "DEVICE_MODEL", "Model", "AMFOC01");
    IUFillText(&DeviceInfoT[1], "DEVICE_FIRMWARE", "Firmware", "1.0");
    IUFillText(&DeviceInfoT[2], "DEVICE_SERIAL", "Serial Number", "AM001");
    IUFillTextVector(&DeviceInfoTP, DeviceInfoT, 3, getDeviceName(), "DEVICE_INFO", 
                     "Device Info", INFO_TAB, IP_RO, 60, IPS_IDLE);
    
    // Focuser absolute position - SLIDER
    IUFillNumber(&FocusAbsPosN[0], "FOCUS_ABSOLUTE_POSITION", "Position", "%.f", 0, 1000000, 1, 0);
    IUFillNumberVector(&FocusAbsPosNP, FocusAbsPosN, 1, getDeviceName(), "ABS_FOCUS_POSITION",
                       "Absolute Position", MAIN_CONTROL_TAB, IP_RW, 0, IPS_IDLE);
    
    IUFillNumber(&FocusRelPosN[0], "FOCUS_RELATIVE_POSITION", "Steps", "%.f", -32768, 32767, 100, 0);
    IUFillNumberVector(&FocusRelPosNP, FocusRelPosN, 1, getDeviceName(), "REL_FOCUS_POSITION",
                       "Relative Position", MAIN_CONTROL_TAB, IP_RW, 60, IPS_IDLE);
    
    // Sync position
    IUFillNumber(&FocusSyncN[0], "FOCUS_SYNC_POSITION", "Sync to Position", "%.f", 0, 1000000, 1, 0);
    IUFillNumberVector(&FocusSyncNP, FocusSyncN, 1, getDeviceName(), "FOCUS_SYNC",
                       "Sync Position", MAIN_CONTROL_TAB, IP_RW, 60, IPS_IDLE);
    
    // Focus speed
    IUFillNumber(&FocusSpeedN[0], "FOCUS_SPEED", "Speed", "%.f", 1, 5, 1, 3);
    IUFillNumberVector(&FocusSpeedNP, FocusSpeedN, 1, getDeviceName(), "FOCUS_SPEED",
                       "Speed", OPTIONS_TAB, IP_RW, 60, IPS_IDLE);
    
    // Temperature (read-only)
    IUFillNumber(&TemperatureN[0], "TEMPERATURE", "Celsius", "%.1f", -50, 70, 0, 0);
    IUFillNumberVector(&TemperatureNP, TemperatureN, 1, getDeviceName(), "FOCUS_TEMPERATURE",
                       "Temperature", MAIN_CONTROL_TAB, IP_RO, 60, IPS_IDLE);
    
    // Temperature compensation mode
    IUFillSwitch(&TempCompModeS[0], "OFF", "Off", ISS_ON);
    IUFillSwitch(&TempCompModeS[1], "DRIVER", "Driver", ISS_OFF);
    IUFillSwitch(&TempCompModeS[2], "FOCUSER", "Focuser", ISS_OFF);
    IUFillSwitchVector(&TempCompModeSP, TempCompModeS, 3, getDeviceName(), "TEMP_COMP_MODE",
                       "Temperature Compensation Mode", OPTIONS_TAB, IP_RW, ISR_1OFMANY, 60, IPS_IDLE);
    
    // Temperature coefficient
    IUFillNumber(&TempCoeffN[0], "TEMP_COEFF", "Coefficient (steps/°C)", "%.1f", -999.9, 999.9, 0.1, 0.0);
    IUFillNumberVector(&TempCoeffNP, TempCoeffN, 1, getDeviceName(), "TEMP_COEFF",
                       "Temperature Coefficient", OPTIONS_TAB, IP_RW, 60, IPS_IDLE);
    
    // Temperature compensation settings
    IUFillNumber(&TempCompSettingsN[0], "TEMP_PERIOD", "Update Period (s)", "%.f", 1, 3600, 1, 60);
    IUFillNumber(&TempCompSettingsN[1], "TEMP_THRESHOLD", "Threshold (°C)", "%.2f", 0.01, 10.0, 0.01, 0.1);
    IUFillNumberVector(&TempCompSettingsNP, TempCompSettingsN, 2, getDeviceName(), "TEMP_SETTINGS",
                       "Compensation Settings", OPTIONS_TAB, IP_RW, 60, IPS_IDLE);
    
    addDebugControl();
    addConfigurationControl();
    
    return true;
}

bool AMFOC01::updateProperties()
{
    INDI::DefaultDevice::updateProperties();
    
    if (isConnected())
    {
        defineProperty(&DeviceInfoTP);
        defineProperty(&FocusAbsPosNP);
        defineProperty(&FocusRelPosNP);
        defineProperty(&FocusSyncNP);
        defineProperty(&FocusSpeedNP);
        defineProperty(&TemperatureNP);
        defineProperty(&TempCompModeSP);
        defineProperty(&TempCoeffNP);
        defineProperty(&TempCompSettingsNP);
        
        // Start periodic polling
        setupTimer();
    }
    else
    {
        deleteProperty(DeviceInfoTP.name);
        deleteProperty(FocusAbsPosNP.name);
        deleteProperty(FocusRelPosNP.name);
        deleteProperty(FocusSyncNP.name);
        deleteProperty(FocusSpeedNP.name);
        deleteProperty(TemperatureNP.name);
        deleteProperty(TempCompModeSP.name);
        deleteProperty(TempCoeffNP.name);
        deleteProperty(TempCompSettingsNP.name);
        
        // Stop timer
        stopTimer();
    }
    
    return true;
}

const char *AMFOC01::getDefaultName()
{
    return "AMFOC01";
}

void AMFOC01::TimerHit()
{
    if (!isConnected())
        return;
        
    // Poll current position from device
    updateStatus();
    
    // Perform internal temperature compensation if enabled
    if (tempCompEnabled && tempCompInDriver)
    {
        performDriverTempCompensation();
    }
    
    // Schedule next polling
    SetTimer(getCurrentPollingPeriod());
}

bool AMFOC01::ISNewNumber(const char *dev, const char *name, double values[], char *names[], int n)
{
    if (dev != nullptr && strcmp(dev, getDeviceName()) == 0)
    {
        // Absolute position (slider)
        if (!strcmp(name, FocusAbsPosNP.name))
        {
            if (!isConnected())
            {
                FocusAbsPosNP.s = IPS_ALERT;
                IDSetNumber(&FocusAbsPosNP, "Device not connected");
                return true;
            }
            
            IUUpdateNumber(&FocusAbsPosNP, values, names, n);
            
            if (gotoAbsolutePosition(static_cast<uint32_t>(FocusAbsPosN[0].value)))
            {
                FocusAbsPosNP.s = IPS_BUSY;
                LOGF_INFO("Moving to position %d...", static_cast<int>(FocusAbsPosN[0].value));
            }
            else
            {
                FocusAbsPosNP.s = IPS_ALERT;
                LOG_ERROR("Failed to move to position.");
            }
            
            IDSetNumber(&FocusAbsPosNP, nullptr);
            return true;
        }
        
        // Relative position
        if (!strcmp(name, FocusRelPosNP.name))
        {
            if (!isConnected())
            {
                FocusRelPosNP.s = IPS_ALERT;
                IDSetNumber(&FocusRelPosNP, "Device not connected");
                return true;
            }
            
            IUUpdateNumber(&FocusRelPosNP, values, names, n);
            
            if (gotoRelativePosition(static_cast<int32_t>(FocusRelPosN[0].value)))
            {
                FocusRelPosNP.s = IPS_BUSY;
                LOGF_INFO("Moving %d steps...", static_cast<int>(FocusRelPosN[0].value));
            }
            else
            {
                FocusRelPosNP.s = IPS_ALERT;
                LOG_ERROR("Failed to move relative position.");
            }
            
            IDSetNumber(&FocusRelPosNP, nullptr);
            return true;
        }
        
        // Sync position
        if (!strcmp(name, FocusSyncNP.name))
        {
            if (!isConnected())
            {
                FocusSyncNP.s = IPS_ALERT;
                IDSetNumber(&FocusSyncNP, "Device not connected");
                return true;
            }
            
            IUUpdateNumber(&FocusSyncNP, values, names, n);
            
            if (syncPosition(static_cast<uint32_t>(FocusSyncN[0].value)))
            {
                FocusSyncNP.s = IPS_OK;
                LOGF_INFO("Position synced to %d", static_cast<int>(FocusSyncN[0].value));
            }
            else
            {
                FocusSyncNP.s = IPS_ALERT;
                LOG_ERROR("Failed to sync position.");
            }
            
            IDSetNumber(&FocusSyncNP, nullptr);
            return true;
        }
        
        // Temperature coefficient
        if (!strcmp(name, TempCoeffNP.name))
        {
            IUUpdateNumber(&TempCoeffNP, values, names, n);
            tempCoefficient = TempCoeffN[0].value;
            TempCoeffNP.s = IPS_OK;
            IDSetNumber(&TempCoeffNP, nullptr);
            LOGF_INFO("Temperature coefficient set to %.1f steps/°C", tempCoefficient);
            
            // Apply to focuser if in focuser mode
            if (tempCompEnabled && !tempCompInDriver)
            {
                setTempCoefficientInFocuser(tempCoefficient);
            }
            
            return true;
        }
        
        // Temperature compensation settings
        if (!strcmp(name, TempCompSettingsNP.name))
        {
            IUUpdateNumber(&TempCompSettingsNP, values, names, n);
            tempCompPeriod = TempCompSettingsN[0].value;
            tempCompThreshold = TempCompSettingsN[1].value;
            TempCompSettingsNP.s = IPS_OK;
            IDSetNumber(&TempCompSettingsNP, nullptr);
            LOGF_INFO("Temperature compensation settings updated: period=%.0fs, threshold=%.2f°C", 
                       tempCompPeriod, tempCompThreshold);
            return true;
        }
        
        // Focus speed
        if (!strcmp(name, FocusSpeedNP.name))
        {
            IUUpdateNumber(&FocusSpeedNP, values, names, n);
            FocusSpeedNP.s = IPS_OK;
            IDSetNumber(&FocusSpeedNP, nullptr);
            return true;
        }
    }
    
    return INDI::DefaultDevice::ISNewNumber(dev, name, values, names, n);
}

bool AMFOC01::ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[], int n)
{
    if (dev != nullptr && strcmp(dev, getDeviceName()) == 0)
    {
        // Temperature compensation mode
        if (!strcmp(name, TempCompModeSP.name))
        {
            IUUpdateSwitch(&TempCompModeSP, states, names, n);
            TempCompModeSP.s = IPS_OK;
            
            // Determine mode
            if (TempCompModeS[0].s == ISS_ON) // Off
            {
                tempCompEnabled = false;
                enableTempCompensationInFocuser(false);
                LOG_INFO("Temperature compensation turned off");
            }
            else if (TempCompModeS[1].s == ISS_ON) // Driver
            {
                tempCompEnabled = true;
                tempCompInDriver = true;
                enableTempCompensationInFocuser(false);
                lastTemperature = currentTemperature;
                lastTempCompTime = time(nullptr);
                LOG_INFO("Temperature compensation set to driver mode");
            }
            else if (TempCompModeS[2].s == ISS_ON) // Focuser
            {
                tempCompEnabled = true;
                tempCompInDriver = false;
                enableTempCompensationInFocuser(true);
                setTempCoefficientInFocuser(tempCoefficient);
                LOG_INFO("Temperature compensation set to focuser mode");
            }
            
            IDSetSwitch(&TempCompModeSP, nullptr);
            return true;
        }
    }
    
    return INDI::DefaultDevice::ISNewSwitch(dev, name, states, names, n);
}

bool AMFOC01::callHandshake()
{
    return getDeviceInfo();
}

bool AMFOC01::getDeviceInfo()
{
    // For now, simulate getting device info
    IUSaveText(&DeviceInfoT[0], "AMFOC01");
    IUSaveText(&DeviceInfoT[1], "1.0");
    IUSaveText(&DeviceInfoT[2], "AM001");
    
    DeviceInfoTP.s = IPS_OK;
    IDSetText(&DeviceInfoTP, nullptr);
    
    return true;
}

bool AMFOC01::getCurrentPosition()
{
    return getActualPosition(currentPosition);
}

bool AMFOC01::updateStatus()
{
    // Poll current position from device using :GP# command
    uint32_t pos;
    if (getActualPosition(pos))
    {
        if (pos != currentPosition)
        {
            currentPosition = pos;
            FocusAbsPosN[0].value = pos;
            FocusAbsPosNP.s = IPS_OK;
            IDSetNumber(&FocusAbsPosNP, nullptr);
            LOGF_DEBUG("Position updated from device: %d", pos);
        }
    }
    else
    {
        LOG_DEBUG("Failed to read position from device");
    }
    
    // Also poll temperature if needed
    double temp;
    if (getTemperature(temp))
    {
        if (temp != currentTemperature)
        {
            currentTemperature = temp;
            TemperatureN[0].value = temp;
            TemperatureNP.s = IPS_OK;
            IDSetNumber(&TemperatureNP, nullptr);
        }
    }
    
    return true;
}

bool AMFOC01::syncPosition(uint32_t position)
{
    LOGF_DEBUG("Syncing position to %d", position);
    
    // Use :SP# command to set current position (synchronization)
    if (setCurrentPosition(position))
    {
        currentPosition = position;
        
        // Update the absolute position display
        FocusAbsPosN[0].value = position;
        FocusAbsPosNP.s = IPS_OK;
        IDSetNumber(&FocusAbsPosNP, nullptr);
        
        return true;
    }
    
    return false;
}

bool AMFOC01::getActualPosition(uint32_t& position)
{
    char response[32];
    if (sendAndReceive(":GP#", response, sizeof(response)))
    {
        position = hexToUint32(response);
        return true;
    }
    return false;
}

bool AMFOC01::getTemperature(double& temp)
{
    char response[32];
    if (sendAndReceive(":GT#", response, sizeof(response)))
    {
        uint32_t tempRaw = hexToUint32(response);
        // Convert according to device specification
        temp = (double)tempRaw / 100.0; // Assuming 0.01°C resolution
        return true;
    }
    return false;
}

bool AMFOC01::setFuturePosition(uint32_t position)
{
    return sendCommandWithParam("SN", position, 5);
}

bool AMFOC01::setCurrentPosition(uint32_t position)
{
    return sendCommandWithParam("SP", position, 5);
}

bool AMFOC01::startMovement()
{
    return sendCommand(":FG#");
}

bool AMFOC01::sendCommand(const char* cmd)
{
    int fd = serialConnection->getPortFD();
    if (fd < 0)
        return false;
        
    int nbytes = write(fd, cmd, strlen(cmd));
    if (nbytes != (int)strlen(cmd))
    {
        LOGF_ERROR("Failed to send command: %s", cmd);
        return false;
    }
    
    // Flush output buffer
    tcdrain(fd);
    return true;
}

bool AMFOC01::sendCommandWithParam(const char* cmd, uint32_t param, int paramLength)
{
    char fullCmd[32];
    char hexParam[16];
    
    uint32ToHex(param, hexParam, paramLength);
    snprintf(fullCmd, sizeof(fullCmd), ":%s%s#", cmd, hexParam);
    
    LOGF_DEBUG("Sending command: %s", fullCmd);
    return sendCommand(fullCmd);
}

bool AMFOC01::readResponse(char* response, int maxLen)
{
    int fd = serialConnection->getPortFD();
    if (fd < 0)
        return false;
        
    memset(response, 0, maxLen);
    
    int bytesRead = 0;
    char c;
    
    // Read until we find '#' or timeout
    while (bytesRead < maxLen - 1)
    {
        fd_set readfds;
        struct timeval timeout;
        
        FD_ZERO(&readfds);
        FD_SET(fd, &readfds);
        
        // 10ms timeout as per protocol specification
        timeout.tv_sec = 0;
        timeout.tv_usec = 10000;
        
        int result = select(fd + 1, &readfds, nullptr, nullptr, &timeout);
        
        if (result <= 0)
        {
            if (bytesRead > 0)
                break; // We got some data, might be incomplete response
            return false;
        }
        
        if (read(fd, &c, 1) == 1)
        {
            if (c == '#')
            {
                response[bytesRead] = '\0';
                return true;
            }
            else
            {
                response[bytesRead++] = c;
            }
        }
        else
        {
            break;
        }
    }
    
    response[bytesRead] = '\0';
    return bytesRead > 0;
}

bool AMFOC01::sendAndReceive(const char* cmd, char* response, int maxLen)
{
    if (!sendCommand(cmd))
        return false;
        
    return readResponse(response, maxLen);
}

uint32_t AMFOC01::hexToUint32(const char* hex)
{
    return strtoul(hex, nullptr, 16);
}

void AMFOC01::uint32ToHex(uint32_t value, char* hex, int length)
{
    snprintf(hex, length + 1, "%0*X", length, value);
}

void AMFOC01::setupTimer()
{
    if (timerID > 0)
        RemoveTimer(timerID);
        
    timerID = SetTimer(getCurrentPollingPeriod());
}

void AMFOC01::stopTimer()
{
    if (timerID > 0)
    {
        RemoveTimer(timerID);
        timerID = -1;
    }
}

bool AMFOC01::gotoAbsolutePosition(uint32_t position)
{
    LOGF_DEBUG("Moving to absolute position: %d", position);
    
    // Send :SN# command to set future position
    if (!setFuturePosition(position))
    {
        LOG_ERROR("Failed to set future position");
        return false;
    }
    
    // Send :FG# command to start movement
    if (!startMovement())
    {
        LOG_ERROR("Failed to start movement");
        return false;
    }
    
    return true;
}

bool AMFOC01::gotoRelativePosition(int32_t steps)
{
    uint32_t newPosition = currentPosition + steps;
    
    // Clamp to valid range
    if (steps < 0 && (uint32_t)(-steps) > currentPosition)
        newPosition = 0;
    else if (newPosition > 1000000)
        newPosition = 1000000;
    
    LOGF_DEBUG("Moving relative: %d steps to position %d", steps, newPosition);
    
    return gotoAbsolutePosition(newPosition);
}

bool AMFOC01::performDriverTempCompensation()
{
    time_t now = time(nullptr);
    
    // Check if enough time has passed since last compensation
    if (now - lastTempCompTime < tempCompPeriod)
        return true;
    
    // Check if temperature change is significant
    double tempDiff = currentTemperature - lastTemperature;
    if (fabs(tempDiff) < tempCompThreshold)
        return true;
    
    // Calculate required movement
    int32_t compensationSteps = static_cast<int32_t>(tempDiff * tempCoefficient);
    
    if (compensationSteps != 0)
    {
        LOGF_INFO("Driver temperature compensation: %.2f°C change, moving %d steps", 
                   tempDiff, compensationSteps);
        
        // Perform relative movement
        uint32_t newPosition = currentPosition + compensationSteps;
        
        // Clamp to valid range
        if (compensationSteps < 0 && (uint32_t)(-compensationSteps) > currentPosition)
            newPosition = 0;
        else if (newPosition > 1000000)
            newPosition = 1000000;
        
        // Execute the compensation movement
        gotoAbsolutePosition(newPosition);
    }
    
    lastTemperature = currentTemperature;
    lastTempCompTime = now;
    
    return true;
}

bool AMFOC01::enableTempCompensationInFocuser(bool enable)
{
    // Here would be the command to enable/disable compensation in the focuser
    // For now, just log the action
    LOGF_INFO("%s temperature compensation in focuser", enable ? "Enabling" : "Disabling");
    
    // TODO: Implement actual protocol command to enable/disable temp compensation
    // Example: sendCommand(enable ? ":TC1#" : ":TC0#");
    
    return true;
}

bool AMFOC01::setTempCoefficientInFocuser(double coefficient)
{
    // Here would be the command to set temperature coefficient in the focuser
    LOGF_INFO("Setting temperature coefficient in focuser to %.1f steps/°C", coefficient);
    
    // TODO: Implement actual protocol command to set coefficient
    // Example: sendCommandWithParam("TF", static_cast<uint32_t>(coefficient * 10), 4);
    
    return true;
}
