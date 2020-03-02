#ifndef sim900x
#define sim900x
#include <SoftwareSerial.h>
#include <Arduino.h>

#define DEFAULT_RX_PIN 		15
#define DEFAULT_TX_PIN 		2

#define BUFFER_RESERVE_MEMORY	255
#define DEFAULT_BAUD_RATE		9600
#define TIME_OUT_READ_SERIAL	5000


class Sim900 : public SoftwareSerial
{
private:

    uint32_t _baud;
    int _timeout;
    String _buffer;
    bool _sleepMode;
    uint8_t _functionalityMode;
    String _locationCode;
    String _longitude;
    String _latitude;

    String _readSerial();
    String _readSerial(uint32_t timeout);


public:

    uint8_t	RX_PIN;
    uint8_t TX_PIN;

    Sim900(void);
    Sim900(uint8_t rx, uint8_t tx);

    void begin();					//Default baud 9600
    void begin(uint32_t baud);
    void reset();

    bool setFunctionalityMode(uint8_t fun);
    uint8_t getFunctionalityMode();

    bool setPIN(String pin);
    String getProductInfo();

    String getOperatorsList();
    String getOperator();

    bool sendSms(int pdulength,char* pdu);
    String readSms(uint8_t index);
    String getNumberSms(uint8_t index);
    bool delAllSms();

    String signalQuality();
    void setPhoneFunctionality();
};

#endif