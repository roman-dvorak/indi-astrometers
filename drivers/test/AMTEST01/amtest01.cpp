/*
    AMTEST01 INDI Test Driver
    Simple serial port data reader and console output
*/

#include "amtest01.h"
#include "indicom.h"
#include "libindi/connectionplugins/connectionserial.h"
#include <termios.h>

#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>

static std::unique_ptr<AMTEST01> amtest01(new AMTEST01());

AMTEST01::AMTEST01()
{
    setVersion(1, 0);
}

AMTEST01::~AMTEST01()
{
    delete serialConnection;
}

const char *AMTEST01::getDefaultName()
{
    return "AMTEST01";
}

bool AMTEST01::initProperties()
{
    INDI::DefaultDevice::initProperties();

    // Device info
    addDebugControl();
    addSimulationControl();
    addConfigurationControl();
    
    // Status display
    IUFillText(&StatusT[0], "DEVICE", "Device", "AMTEST01");
    IUFillText(&StatusT[1], "STATUS", "Status", "Disconnected");
    IUFillText(&StatusT[2], "LAST_DATA", "Last Data", "None");
    IUFillTextVector(&StatusTP, StatusT, 3, getDeviceName(), "DEVICE_STATUS", "Device Status", MAIN_CONTROL_TAB, IP_RO, 60, IPS_IDLE);

    // Read data control
    IUFillSwitch(&ReadDataS[0], "START", "Start Reading", ISS_OFF);
    IUFillSwitch(&ReadDataS[1], "STOP", "Stop Reading", ISS_OFF);
    IUFillSwitchVector(&ReadDataSP, ReadDataS, 2, getDeviceName(), "READ_DATA", "Data Reading", MAIN_CONTROL_TAB, IP_RW, ISR_1OFMANY, 60, IPS_IDLE);

    // Serial connection
    serialConnection = new Connection::Serial(this);
    serialConnection->registerHandshake([&]() { return Handshake(); });
    serialConnection->setDefaultBaudRate(Connection::Serial::B_9600);
    serialConnection->setDefaultPort("/dev/ttyACM0");
    registerConnection(serialConnection);
    
    // Add standard controls
    addAuxControls();

    return true;
}

bool AMTEST01::updateProperties()
{
    INDI::DefaultDevice::updateProperties();

    if (isConnected())
    {
        // Add properties when connected
        defineProperty(&StatusTP);
        defineProperty(&ReadDataSP);
        
        // Update status
        IUSaveText(&StatusT[1], "Connected");
        StatusTP.s = IPS_OK;
        IDSetText(&StatusTP, nullptr);
        
        printf("[AMTEST01] Device connected successfully\n");
        std::cout.flush();
    }
    else
    {
        // Remove properties when disconnected
        deleteProperty(StatusTP.name);
        deleteProperty(ReadDataSP.name);
        
        // Stop reading if active
        if (isReading)
        {
            isReading = false;
            printf("[AMTEST01] Stopped reading data\n");
            std::cout.flush();
        }
        
        printf("[AMTEST01] Device disconnected\n");
        std::cout.flush();
    }

    return true;
}

bool AMTEST01::Handshake()
{
    if (isSimulation())
    {
        LOGF_INFO("Connected successfully to simulated %s.", getDeviceName());
        printf("[AMTEST01] Connected to simulated device\n");
        std::cout.flush();
        return true;
    }

    PortFD = serialConnection->getPortFD();
    LOGF_DEBUG("Handshake: Serial port FD = %d", PortFD);
    
    if (PortFD < 0)
    {
        LOG_ERROR("Serial port not connected");
        printf("[AMTEST01] ERROR: Serial port not connected\n");
        std::cout.flush();
        return false;
    }

    printf("[AMTEST01] Connected to serial port (FD: %d)\n", PortFD);
    std::cout.flush();
    
    return true;
}

bool AMTEST01::sendCommand(const char *cmd)
{
    int nbytes_read = 0, nbytes_written = 0, tty_rc = 0;
    char res[256] = {0};
    LOGF_DEBUG("CMD <%s>", cmd);

    if (!isSimulation())
    {
        tcflush(PortFD, TCIOFLUSH);
        if ((tty_rc = tty_write_string(PortFD, cmd, &nbytes_written)) != TTY_OK)
        {
            char errorMessage[MAXRBUF];
            tty_error_msg(tty_rc, errorMessage, MAXRBUF);
            LOGF_ERROR("Serial write error: %s", errorMessage);
            printf("[AMTEST01] Serial write error: %s\n", errorMessage);
            std::cout.flush();
            return false;
        }
    }

    if (isSimulation())
    {
        strncpy(res, "OK#", 8);
        nbytes_read = 3;
    }
    else
    {
        if ((tty_rc = tty_read_section(PortFD, res, '#', 1, &nbytes_read)) != TTY_OK)
        {
            char errorMessage[MAXRBUF];
            tty_error_msg(tty_rc, errorMessage, MAXRBUF);
            LOGF_ERROR("Serial read error: %s", errorMessage);
            // Don't print error for timeout - normal for continuous reading
            return false;
        }
    }

    res[nbytes_read - 1] = '\0';
    LOGF_DEBUG("RES <%s>", res);

    return true;
}

bool AMTEST01::ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[], int n)
{
    if (dev != nullptr && strcmp(dev, getDeviceName()) == 0)
    {
        // Read data control
        if (strcmp(name, "READ_DATA") == 0)
        {
            IUUpdateSwitch(&ReadDataSP, states, names, n);
            
            if (ReadDataS[0].s == ISS_ON) // Start reading
            {
                isReading = true;
                IUSaveText(&StatusT[1], "Reading Data");
                ReadDataSP.s = IPS_BUSY;
                printf("[AMTEST01] Started continuous data reading\n");
                std::cout.flush();
                SetTimer(100); // Read every 100ms
            }
            else // Stop reading
            {
                isReading = false;
                IUSaveText(&StatusT[1], "Connected");
                ReadDataSP.s = IPS_OK;
                printf("[AMTEST01] Stopped data reading\n");
                std::cout.flush();
            }
            
            IDSetSwitch(&ReadDataSP, nullptr);
            StatusTP.s = IPS_OK;
            IDSetText(&StatusTP, nullptr);
            return true;
        }
    }

    return INDI::DefaultDevice::ISNewSwitch(dev, name, states, names, n);
}

void AMTEST01::TimerHit()
{
    if (isConnected() && isReading)
    {
        readSerialData();
        SetTimer(100); // Continue reading every 100ms
    }
}

bool AMTEST01::readSerialData()
{
    if (PortFD < 0)
    {
        return false;
    }

    char buffer[1024] = {0};
    int nbytes_read = 0;
    
    if (isSimulation())
    {
        // Generate some test data
        static int counter = 0;
        snprintf(buffer, sizeof(buffer), "TEST_DATA_%d,temperature=%.1f,humidity=%.1f", 
                counter++, 20.0 + (counter % 10), 50.0 + (counter % 20));
        nbytes_read = strlen(buffer);
        processData(std::string(buffer));
        return true;
    }

    LOGF_DEBUG("Reading serial data from FD %d", PortFD);
    
    // Try to read a line of data (ending with newline)
    int tty_rc = tty_read_section(PortFD, buffer, '\n', 3, &nbytes_read);
    
    if (tty_rc == TTY_OK && nbytes_read > 0)
    {
        buffer[nbytes_read - 1] = '\0'; // Remove newline
        std::string dataLine(buffer);
        processData(dataLine);
        return true;
    }
    else if (tty_rc == TTY_TIME_OUT)
    {
        // Timeout is normal when no data is available
        return true;
    }
    else if (tty_rc != TTY_OK)
    {
        char errorMessage[MAXRBUF];
        tty_error_msg(tty_rc, errorMessage, MAXRBUF);
        LOGF_DEBUG("Serial read error: %s", errorMessage);
        return false;
    }
    
    return true;
}

void AMTEST01::processData(const std::string& data)
{
    if (data.empty())
        return;
        
    // Print to console with timestamp
    time_t rawtime;
    struct tm * timeinfo;
    char timestamp[80];
    
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(timestamp, sizeof(timestamp), "%H:%M:%S", timeinfo);
    
    // Print to console
    printf("[AMTEST01] [%s] DATA: %s\n", timestamp, data.c_str());
    std::cout.flush();
    
    // Update INDI property
    IUSaveText(&StatusT[2], data.c_str());
    StatusTP.s = IPS_OK;
    IDSetText(&StatusTP, nullptr);
    
    LOGF_INFO("Received data: %s", data.c_str());
}
