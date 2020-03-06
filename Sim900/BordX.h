#ifndef Bordx
#define Bordx

#include "BordX.h"
#include "sim900.h"

#define separatorG ""
#define separatorL ""

class BordX
{
    
    private:

        TaskHandle_t _MainSerialH;
        TaskHandle_t _MainSerialR;
        TaskHandle_t _MainSerialS;
        TaskHandle_t _ResaveSMSH;
        TaskHandle_t _SmsSendH;

        String _TString;

        QueueHandle_t _SmsToBeSend0 = xQueueCreate(10, sizeof(String));
        QueueHandle_t _SerialBuffer  = xQueueCreate(100, sizeof(String));
        QueueHandle_t _SerialBufferfromPC = xQueueCreate(100, sizeof(String));

        String _tempBuffer;
        SemaphoreHandle_t _Sim900Semaphore;

        void _SplitString(String *data, String *token, String separator);
        static void _MainSerialHandler(void* pt);

    public:
        BordX(int rx, int tx);
        void BordRestart();
        void StartUp();
};

#endif