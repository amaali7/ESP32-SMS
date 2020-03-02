#include "BordX.h"



BordX::BordX(int rx, int tx){
    Sim900 gsm(rx, tx);
    _Sim900Semaphore = xSemaphoreCreateMutex();
    
}

void BordX::StartUp()
{
    xTaskCreatePinnedToCore(_MainSerialHandler, "Main Ser Han", 10000, NULL, 1, &_MainSerialH, 1);
    delay(50);
    /*
    xTaskCreatePinnedToCore(MainSerialResaver, "Main Ser Res", 10000, NULL, 1, &_MainSerialR, 1);
    delay(50);
    xTaskCreatePinnedToCore(MainSerialSender, "Main Ser Sen", 10000, NULL, 1, &_MainSerialS, 1);
    delay(50);
    xTaskCreatePinnedToCore(ResaveSMS, "Resave SMS", 10000, NULL, 1, &_ResaveSMSH, 1);
    delay(50);
    xTaskCreatePinnedToCore(SmsSend, "Sms Send", 10000, NULL, 1, &_SmsSendH, 1);
    delay(50);
    */
  String data = "StatusReport" + String(separatorG) + "System Online" + String(separatorL) + "Success";
  xQueueSend(_SerialBuffer, &data,(TickType_t) 0);
  delete(&data);
}

void BordX::_MainSerialHandler(void* pt)
{ 
  String _data0;
  while(1)
  {
    if(Serial.available() > 0)
    {
      _data0 = Serial.readString();
      if(!_data0.isEmpty())
      {
        xQueueSend(_SerialBufferfromPC, &_data0,(TickType_t) 0);
      }
    }
    vTaskDelay(10);
  }  
}

void BordX::_SplitString(String *data, String *token, String separator)
{
  *token = data->substring(0, data->indexOf(separator));
  data->remove(0, data->indexOf(separator) + separator.length());
}

void BordX::BordRestart()
{
  Serial.println("Bord Well Be Restart in 10s .....");
  delay(1000);
  ESP.restart();
}