#include "sim900.h"
#include <SoftwareSerial.h>

//SoftwareSerial SIM(RX_PIN,TX_PIN);
//String _buffer;

Sim900::Sim900(void) : SoftwareSerial(DEFAULT_RX_PIN, DEFAULT_TX_PIN)
{
    RX_PIN 		= DEFAULT_RX_PIN;
    TX_PIN 		= DEFAULT_TX_PIN;
}

Sim900::Sim900(uint8_t rx, uint8_t tx) : SoftwareSerial(rx, tx)
{
    RX_PIN 		= rx;
    TX_PIN 		= tx;
}

void Sim900::begin()
{

    _baud = DEFAULT_BAUD_RATE;			// Default baud rate 9600
    this->SoftwareSerial::begin(_baud);

    _sleepMode = 0;
    _functionalityMode = 1;
    _buffer.reserve(BUFFER_RESERVE_MEMORY); // Reserve memory to prevent intern fragmention
}

void Sim900::begin(uint32_t baud)
{
    _baud = baud;
    this->SoftwareSerial::begin(_baud);
    _sleepMode = 0;
    _functionalityMode = 1;
    _buffer.reserve(BUFFER_RESERVE_MEMORY); // Reserve memory to prevent intern fragmention
}

/*
 * AT+CFUN=0	Minimum functionality
 * AT+CFUN=1	Full functionality (defualt)
 * AT+CFUN=4	Flight mode (disable RF function)
*/
bool Sim900::setFunctionalityMode(uint8_t fun)
{

    if (fun==0 || fun==1 || fun==4)
    {

        _functionalityMode = fun;

        switch(_functionalityMode)
        {
        case 0:
            this->SoftwareSerial::print(F("AT+CFUN=0\r\n "));
            break;
        case 1:
            this->SoftwareSerial::print(F("AT+CFUN=1\r\n "));
            break;
        case 4:
            this->SoftwareSerial::print(F("AT+CFUN=4\r\n "));
            break;
        }

        if ( (_readSerial().indexOf("ER")) == -1)
        {
            return false;
        }
        else return true;
        // Error found, return 1
        // Error NOT found, return 0
    }
    return false;
}

uint8_t Sim900::getFunctionalityMode()
{
    return _functionalityMode;
}

bool Sim900::setPIN(String pin)
{
    String command;
    command  = "AT+CPIN=";
    command += pin;
    command += "\r";

    // Can take up to 5 seconds

    this->SoftwareSerial::print(command);

    if ( (_readSerial(5000).indexOf("ER")) == -1)
    {
        return false;
    }
    else return true;
    // Error found, return 1
    // Error NOT found, return 0
}

String Sim900::getProductInfo()
{
    this->SoftwareSerial::print("ATI\r");
    return (_readSerial());
}

String Sim900::getOperatorsList()
{

    // Can take up to 45 seconds

    this->SoftwareSerial::print("AT+COPS=?\r");

    return _readSerial(45000);

}

String Sim900::getOperator()
{

    this->SoftwareSerial::print("AT+COPS ?\r");

    return _readSerial();

}

//
//PUBLIC METHODS
//

void Sim900::reset()
{
    this->SoftwareSerial::print(F("AT\r\n"));
    while (_readSerial().indexOf("OK")==-1 )
    {
        this->SoftwareSerial::print(F("AT\r\n"));
    }

    //wait for sms ready
    while (_readSerial().indexOf("SMS")==-1 );
}

void Sim900::setPhoneFunctionality()
{
    /*AT+CFUN=<fun>[,<rst>]
    Parameters
    <fun> 0 Minimum functionality
    1 Full functionality (Default)
    4 Disable phone both transmit and receive RF circuits.
    <rst> 1 Reset the MT before setting it to <fun> power level.
    */
    this->SoftwareSerial::print (F("AT+CFUN=1\r\n"));
}

String Sim900::signalQuality()
{
    /*Response
    +CSQ: <rssi>,<ber>Parameters
    <rssi>
    0 -115 dBm or less
    1 -111 dBm
    2...30 -110... -54 dBm
    31 -52 dBm or greater
    99 not known or not detectable
    <ber> (in percent):
    0...7 As RXQUAL values in the table in GSM 05.08 [20]
    subclause 7.2.4
    99 Not known or not detectable
    */
    this->SoftwareSerial::print (F("AT+CSQ\r\n"));
    return(_readSerial());
}

bool Sim900::sendSms(int pdulength, String pdu)
{

    // Can take up to 60 seconds

    this->SoftwareSerial::print (F("AT+CMGS="));  	// command to send sms
    this->SoftwareSerial::print (pdulength);
    this->SoftwareSerial::print(F("\r"));
    _buffer=_readSerial();
    this->SoftwareSerial::print (pdu);
    // _buffer=_readSerial();
    this->SoftwareSerial::print((char)26);
    _buffer=_readSerial(60000);
    //expect CMGS:xxx   , where xxx is a number,for the sending sms.
    if ( (_buffer.indexOf("ER")) == -1)
    {
        return false;
    }
    else return true;
    // Error found, return 1
    // Error NOT found, return 0
}

String Sim900::readSms()
{

    // Can take up to 5 seconds

    this->SoftwareSerial::print (F("AT+CMGF=0\r"));

    if (( _readSerial(5000).indexOf("ER")) ==-1)
    {
        this->SoftwareSerial::print (F("AT+CMGL=\r"));
        _buffer =_readSerial();
        if (_buffer.indexOf("CMGL:")!=-1)
        {
            return _buffer;
        }
        else return "NoSMS";
    }
    else
        return "NoSMS";
}

bool Sim900::delAllSms()
{
    // Can take up to 25 seconds

    this->SoftwareSerial::print(F("AT+CMGD=4\n\r"));
    _buffer=_readSerial(25000);
    if ( (_buffer.indexOf("ER")) == -1)
    {
        return false;
    }
    else return true;
    // Error found, return 1
    // Error NOT found, return 0
}

//
//PRIVATE METHODS
//
String Sim900::_readSerial()
{

    uint64_t timeOld = millis();

    while (!this->SoftwareSerial::available() && !(millis() > timeOld + TIME_OUT_READ_SERIAL))
    {
        delay(13);
    }

    String str;

    while(this->SoftwareSerial::available())
    {
        if (this->SoftwareSerial::available()>0)
        {
            str += (char) this->SoftwareSerial::read();
        }
    }

    return str;

}

String Sim900::_readSerial(uint32_t timeout)
{

    uint64_t timeOld = millis();

    while (!this->SoftwareSerial::available() && !(millis() > timeOld + timeout))
    {
        delay(13);
    }

    String str;

    while(this->SoftwareSerial::available())
    {
        if (this->SoftwareSerial::available()>0)
        {
            str += (char) this->SoftwareSerial::read();
        }
    }

    return str;

}