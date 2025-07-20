/*
    AMSKY01 INDI Weather Station Driver
    Simple and clean implementation for AMSKY01 weather station
    
    Protocol: Serial CSV data with $light, $cloud, $hygro messages
    
    Author: Roman Dvořák <info@astrometers.cz>
    Copyright (C) 2025 Astrometers
*/

#pragma once

#include <libindi/defaultdevice.h>
#include <libindi/connectionplugins/connectionserial.h>

namespace Connection
{
    class Serial;
}

class AMSKY01 : public INDI::DefaultDevice
{
public:
    AMSKY01();
    virtual ~AMSKY01();
    
    virtual bool initProperties() override;
    virtual bool updateProperties() override;
    virtual const char *getDefaultName() override;
    
protected:
    virtual void TimerHit() override;
    virtual bool ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[], int n) override;

private:
    // Serial connection
    bool Handshake();
    bool sendCommand(const char *cmd);
    int PortFD{-1};
    Connection::Serial *serialConnection{nullptr};
    
    // Properties
    ITextVectorProperty StatusTP;
    IText StatusT[3];
    
    ISwitchVectorProperty ReadDataSP;
    ISwitch ReadDataS[2];
    
    // Data reading
    bool readSerialData();
    void processData(const std::string& data);
    
    // Timer for continuous reading
    bool isReading{false};
};
