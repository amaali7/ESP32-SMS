#ifndef sim900x
#define sim900x
#include <SoftwareSerial.h>
#include <Arduino.h>
#include <Regexp.h>

#define DEFAULT_RX_PIN 		15
#define DEFAULT_TX_PIN 		2

#define BUFFER_RESERVE_MEMORY	255
#define DEFAULT_BAUD_RATE		9600
#define TIME_OUT_READ_SERIAL	5000

#define sapG "⁁⁂⁁"
#define sapL "⁂⁁⁂"


void SplitString(String *data, String *token, String separator);

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
    void beginServe();
    bool setFunctionalityMode(uint8_t fun);
    uint8_t getFunctionalityMode();

    bool setPIN(String pin);
    String getProductInfo();

    String getOperatorsList();
    String getOperator();

    bool sendSms(int pdulength, String pdu);
    String readSms();
    bool delAllSms();
    bool delReadedSms();
    bool delSendedSms();
    

    String signalQuality();
    void setPhoneFunctionality();
};

#endif