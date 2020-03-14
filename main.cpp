#include "Sim900/sim900.h"


Sim900 Sim900A(15,2);


// void SplitString(String *data, String *token, String separator)
// {
//   *token = data->substring(0, data->indexOf(separator));
//   data->remove(0, data->indexOf(separator) + separator.length());
// }

//SoftwareSerial GPRS(15, 2); // RX, TX

TaskHandle_t MainSerialH;
TaskHandle_t MainSerialR;
TaskHandle_t MainSerialS;
TaskHandle_t ResaveSMSH;
TaskHandle_t SmsSendH;

String MyQueue;
String TString;

QueueHandle_t SmsToBeSend0 = xQueueCreate(10, sizeof(MyQueue));
QueueHandle_t SerialBuffer  = xQueueCreate(100, sizeof(MyQueue));
QueueHandle_t SerialBufferfromPC  = xQueueCreate(100, sizeof(MyQueue));

SemaphoreHandle_t Sim900ASemaphore;
String _tempBuffer;

void BordRestart()
{
  Serial.println("Bord Well Be Restart in 10s .....");
  delay(1000);
  ESP.restart();
}


void MainSerialResaver(void* pt)
{ 
  String _data;
  String _data0;
  String _key;
  String _erorr;
  String _GsmId;
  while(1)
  {
    if(uxQueueMessagesWaiting(SerialBufferfromPC) > 0)
    {
      xQueueReceive(SerialBufferfromPC, &_data,(TickType_t) 5);
      if(!_data.isEmpty())
      {
        SplitString(&_data, &_key,sapG);
        _erorr = "Erorr";
        if(_key == "SendSms"){
          SplitString(&_data0, &_GsmId, sapG);
          if(_GsmId == "0")
          {
            xQueueSend(SmsToBeSend0, &_data,(TickType_t) 0);
          }
        else if (_key == "Restart")
        {
          BordRestart();
        }
             
        }
      }
    }
    vTaskDelay(10);
  }  
}

void MainSerialSender(void* pt)
{ 
  String _data0;
  
  while(1)
  {
    if(uxQueueMessagesWaiting(SerialBuffer) > 0)
    {
      xQueueReceive(SerialBuffer, &_data0,(TickType_t) 5);
      if(!_data0.isEmpty())
      {
        _data0.trim();
        Serial.println(_data0);
      }
    }
    vTaskDelay(10);
  }  
}

void MainSerialHandler(void* pt)
{ 
  String _data0;
  while(1)
  {
    if(Serial.available() > 0)
    {
      _data0 = Serial.readString();
      if(!_data0.isEmpty())
      {
        xQueueSend(SerialBufferfromPC, &_data0,(TickType_t) 0);
      }
    }
    vTaskDelay(10);
  }  
}

void SmsSend(void* pt)
{ 
  String _data0;
  String _smsId;
  int _pdulength;
  String _smsType;
  String _numberOfParts;
  String _sms;
  String _partIndex;
  String _pdu;
  String _msgs;
  String _msgf;
  int counter = 0;
  bool Status;
  while(1)
  {
    if( xSemaphoreTake( Sim900ASemaphore, ( TickType_t ) portMAX_DELAY ) == pdTRUE )
    {
      if(uxQueueMessagesWaiting(SmsToBeSend0) > 0)
      {
        xQueueReceive(SmsToBeSend0, &_data0,(TickType_t) 5);
        SplitString(&_data0, &_smsId, sapG);
        SplitString(&_data0, &_smsType, sapG);
        if (_smsType == "s")
        {
          SplitString(&_data0, &TString, sapL);
          _pdulength = TString.toInt();
          Status = Sim900A.sendSms(_pdulength, _data0);
          counter = 0;
          while(counter<1){
            if(Status){
              _msgs = "SmsSendReport" + String(sapG) + _smsId + String(sapL) +"Success";
              xQueueSend(SerialBuffer, &_msgs,(TickType_t) 0);
              break;
            }
            else
            {
              Status = Sim900A.sendSms(_pdulength, _pdu);
            }
            ++counter;
          }
          if(!Status)
          {
            _msgf = "SmsSendReport" + String(sapG) + _smsId + String(sapL) + "Fail";
            xQueueSend(SerialBuffer, &_msgf,(TickType_t) 0); 
          }
        }
        else if (_smsType == "m")
        {
          SplitString(&_data0, &_numberOfParts, sapG);
          for (size_t i = 0; i <= _numberOfParts.toInt()-1; i++)
          {
            SplitString(&_data0, &_sms, sapG);
            SplitString(&_sms, &_partIndex, sapL);
            SplitString(&_sms, &TString, sapL);
            _pdulength = TString.toInt();
            Status = Sim900A.sendSms(_pdulength, _data0);
            counter = 0;
            while(counter<1){
              if(Status){
                _msgs = "SmsSendReport" + String(sapG) + _smsId + String(sapL)+_partIndex + String(sapL) +"Success";
                xQueueSend(SerialBuffer, &_msgs,(TickType_t) 0);
                break;
              }
              else
              {
                Status = Sim900A.sendSms(_pdulength, _pdu);
              }
              ++counter;
            }
            if(!Status)
            {
              _msgf = "SmsSendReport" + String(sapG) + _smsId + String(sapL)+_partIndex + String(sapL) + "Fail";
              xQueueSend(SerialBuffer, &_msgf,(TickType_t) 0); 
            }
          }
          
          /* code */
        }
        else
        {
          _msgs = "SmsSendReport" + String(sapG) + _smsId + String(sapL)+ "Erorr";
          xQueueSend(SerialBuffer, &_msgs,(TickType_t) 0);
        }
        
        // delay(100);
        Sim900A.delSendedSms();
      }
      xSemaphoreGive( Sim900ASemaphore );
    }
    vTaskDelay(10);
  }
  vTaskDelete(SmsSendH);
}


void ResaveSMS(void* pt)
{ 
  String data;
  String dataRedy;
  while(1)
  {
    if( xSemaphoreTake( Sim900ASemaphore, ( TickType_t ) portMAX_DELAY ) == pdTRUE )
    {
      data = Sim900A.readSms();
      
      if(data == "NoSMS")
      {
        ;
      }
      else if (data.length()>=40)
      {
        dataRedy = "NewSms"+String(sapG)+String(0)+data;
        xQueueSend(SerialBuffer, &dataRedy,(TickType_t) 0);
        //Sim900A.delReadedSms();
      }
      xSemaphoreGive( Sim900ASemaphore );
    }
    vTaskDelay(10);
  }
  vTaskDelete(ResaveSMSH);
}

void StartUp()
{
  xTaskCreatePinnedToCore(MainSerialHandler, "Main Ser Han", 10000, NULL, 1, &MainSerialH, 1);
  delay(50);
  xTaskCreatePinnedToCore(MainSerialResaver, "Main Ser Res", 10000, NULL, 1, &MainSerialR, 1);
  delay(50);
  xTaskCreatePinnedToCore(MainSerialSender, "Main Ser Sen", 10000, NULL, 1, &MainSerialS, 1);
  delay(50);
  xTaskCreatePinnedToCore(ResaveSMS, "Resave SMS", 10000, NULL, 1, &ResaveSMSH, 1);
  delay(50);
  xTaskCreatePinnedToCore(SmsSend, "Sms Send", 10000, NULL, 1, &SmsSendH, 1);
  delay(50);
  String data = "StatusReport" + String(sapG) + "System Online" + String(sapL) + "Success";
  delay(3000);
  xQueueSend(SerialBuffer, &data,(TickType_t) 0);
}

void setup() {
  Serial.begin(9600);
  Sim900ASemaphore = xSemaphoreCreateMutex();
  delay(10000);

  Sim900A.begin(9600);
  Sim900A.beginServe();
  StartUp();  
}

void loop() {
  delay(portMAX_DELAY);
}