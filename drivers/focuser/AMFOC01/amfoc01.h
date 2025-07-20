/*
    AMFOC01 INDI Driver Header
    
    Author: Roman Dvořák <info@astrometers.cz>
    Copyright (C) 2025 Astrometers
*/

#pragma once

#include <libindi/defaultdevice.h>
#include <libindi/connectionplugins/connectionserial.h>
#include <libindi/connectionplugins/connectiontcp.h>
#include <ctime>

class AMFOC01 : public INDI::DefaultDevice
{
public:
    AMFOC01();
    virtual ~AMFOC01();
    
    virtual bool initProperties() override;
    virtual bool updateProperties() override;
    virtual const char *getDefaultName() override;
    virtual void TimerHit() override;
    
    virtual bool ISNewNumber(const char *dev, const char *name, double values[], char *names[], int n) override;
    virtual bool ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[], int n) override;

private:
    // Connection
    Connection::Serial *serialConnection{nullptr};
    Connection::TCP *tcpConnection{nullptr};
    
    // Device Info
    ITextVectorProperty DeviceInfoTP;
    IText DeviceInfoT[3] {};
    
    // Focuser properties
    INumberVectorProperty FocusAbsPosNP;
    INumber FocusAbsPosN[1];
    
    INumberVectorProperty FocusRelPosNP;
    INumber FocusRelPosN[1];
    
    // Sync functionality
    INumberVectorProperty FocusSyncNP;
    INumber FocusSyncN[1];
    
    INumberVectorProperty FocusSpeedNP;
    INumber FocusSpeedN[1];
    
    INumberVectorProperty TemperatureNP;
    INumber TemperatureN[1];
    
    // Temperature compensation settings
    ISwitchVectorProperty TempCompModeSP;
    ISwitch TempCompModeS[3]; // OFF, DRIVER, FOCUSER
    
    INumberVectorProperty TempCoeffNP;
    INumber TempCoeffN[1];
    
    INumberVectorProperty TempCompSettingsNP;
    INumber TempCompSettingsN[2]; // Period, Threshold
    
    // Internal state variables
    uint32_t currentPosition{0};
    double currentTemperature{0.0};
    double lastTemperature{0.0};
    bool tempCompEnabled{false};
    bool tempCompInDriver{false}; // true = driver handles, false = focuser handles
    double tempCoefficient{0.0};
    double tempCompPeriod{60.0};  // seconds
    double tempCompThreshold{0.1}; // degrees C
    time_t lastTempCompTime{0};
    int timerID{-1};
    
    // Communication methods
    bool sendCommand(const char* cmd);
    bool sendCommandWithParam(const char* cmd, uint32_t param, int paramLength = 4);
    bool readResponse(char* response, int maxLen);
    bool sendAndReceive(const char* cmd, char* response, int maxLen);
    bool getActualPosition(uint32_t& position);      // :GP#
    bool getTemperature(double& temp);               // :GT#
    bool setFuturePosition(uint32_t position);       // :SN<value>#
    bool setCurrentPosition(uint32_t position);      // :SP<value>#
    bool startMovement();                            // :FG#
    uint32_t hexToUint32(const char* hex);
    void uint32ToHex(uint32_t value, char* hex, int length);
    
    // Temperature compensation methods
    bool enableTempCompensationInFocuser(bool enable);
    bool setTempCoefficientInFocuser(double coefficient);
    bool performDriverTempCompensation();
    
    // Helper functions
    bool callHandshake();
    bool getDeviceInfo();
    bool getCurrentPosition();
    bool updateStatus();
    bool syncPosition(uint32_t position);
    bool gotoAbsolutePosition(uint32_t position);
    bool gotoRelativePosition(int32_t steps);
    void setupTimer();
    void stopTimer();
};
