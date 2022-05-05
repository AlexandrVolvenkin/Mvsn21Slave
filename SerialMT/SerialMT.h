//-----------------------------------------------------------------------------------------------------
// SerialMT - serial multi trans.
//-----------------------------------------------------------------------------------------------------
#ifndef SERIALMT_H_INCLUDED
#define SERIALMT_H_INCLUDED

//-------------------------------------------------------------------------------------------------
#include <stdint.h>
#include "Timers.h"
//#include "ATmega128platform.h"
//#include "modbus.h"
//#include "modbus-rtu.h"

extern uint8_t aui8DebugArray[16];
//-------------------------------------------------------------------------------------------------
class CSerialMT
{
public:
    // формат вложенного сообщения SerialMT: заголовок, данные.
    // заголовок сообщения: порт(адрес функции-задачи) - 1 байт, количество байт данных(без заголовка) - 1 байт.
    // данные: данные сообщения.

    struct TSerialMTData
    {
        // указатель на вложенное сообщение.
        uint8_t *pui8DataPointer;
        // длина вложенного сообщения.
        int16_t i16Length;
        // адрес функции-задачи.
        uint8_t ui8Address;
        // режим функции-задачи(функция-задача может выступать в роли сервера или клиента).
        uint8_t ui8Mode;
    };

    typedef enum
    {
        // длина заголовка вложенного сообщения.
        SERIAL_MT_HEADER_LENGTH = 2,
        // максимальная длина общего сообщения.
        SERIAL_MT_MESSAGE_BAG_MAX_LENGTH = 252,
        // передача и приём на шине SPI происходит в дуплексном режиме.
        // если длина передаваемого кадра меньше длины принимаемого кадра, то
        // принимаемый кадр будет потерян.
        // поэтому длина передаваемого кадра должна быть больше принимаемого кадра.
        // максимальная длина принимаемого кадра известна на стадии проектирования.
        // максимальная длина принимаемого кадра.
        MAX_RECEIVED_FRAME_LENGTH = 1
    };

//-------------------------------------------------------------------------------------------------

    // смещение на адрес функции-задачи в заголовке вложенного сообщения.
    static const uint8_t SERIAL_MT_PORT_OFFSET = 0;
    // смещение на длину вложенного сообщения в заголовке вложенного сообщения.
    static const uint8_t SERIAL_MT_MESSAGE_LENGTH_OFFSET = 1;
    // смещение на данные во вложенном сообщении.
    static const uint8_t SERIAL_MT_DATA_OFFSET = 2;
    // бит подтверждения приёма.
    static const uint8_t SERIAL_MT_HANDSHAKING_BIT = 7;
    static const uint8_t SERIAL_MT_WRITE = 0x80;
    static const uint8_t SERIAL_MT_READ = 0;

    uint8_t* ucGetBuffMessageBag(void);
    uint8_t GetLengthMessageBag(void);
    uint8_t IsWrite(void);
    uint8_t* MessagePushRequest(uint8_t , uint8_t );
    int8_t MessagePop(void);

    int8_t DataExchangeIsOccurred(void);
    void DataExchangeFlagSet(void);
    void DataExchangeFlagReset(void);
    virtual uint8_t* GetReceiveMessageBuff(void) = 0;
    virtual uint8_t* GetTransmitMessageBuff(void) = 0;
    virtual int8_t DataReceive(void) = 0;
    virtual int8_t DataTransmit(void) = 0;
    virtual void MessagePushInit(void) = 0;
    virtual void MessagePopInit(void) = 0;
    virtual uint8_t MessagePopCheck(void);

//-------------------------------------------------------------------------------------------------
    // длина общего сообщения.
    uint8_t ui8MessageBagLength;
    // индекс в буфере общего сообщения.
    // содержит смещение на место для следующего вложенного сообщения.
    uint8_t ui8MessageBagIndex;
    // указатель на буфер общего сообщения.
    uint8_t *pui8MessageBag;
    // указатель на вложенное сообщение.
    uint8_t *pui8DataPointer;
    // длина вложенного сообщения.
    uint8_t ui8Length;
    // адрес функции-задачи.
    uint8_t ui8Address;

    static uint8_t aui8ReceiveMessageBuff[SERIAL_MT_MESSAGE_BAG_MAX_LENGTH + 4];
    static uint8_t aui8TransmitMessageBuff[SERIAL_MT_MESSAGE_BAG_MAX_LENGTH + 4];
    static uint8_t nui8BuffByteCounter;
    // счётчик байтов для вычисления длительности пакета при обмене данными по SPI.
    static uint8_t ui8SpiReceivedByteCounter;
    static uint8_t ui8TempSPDR;
    uint8_t fui8DataExchangeIsOccurred;
    uint8_t fui8BuffOverflow;

//-------------------------------------------------------------------------------------------------
};



#define SPI_ONE_BYTE_RECEIVE_TIME ((1000000 / 150000) * 9)
// количество байт принятых за одну милисекунду при обмене данными по SPI.
#define SPI_ONE_MS_RECEIVED_BYTE_QUANTITY (1000 / SPI_ONE_BYTE_RECEIVE_TIME)

//-------------------------------------------------------------------------------------------------
class CSpiSerialMT : public CSerialMT
{
public:

// SpiMT
    // формат кадра SpiMT: команда - 1 байт, количество байт в кадре - 1 байт(команда, количество, данные),
    // данные, контрольная сумма(сумма содержимого всех байтов(команда, количество, данные)) - 2 байта.
    typedef enum
    {
        // максимальная длина сообщения.
        MESSAGE_BAG_MAX_LENGTH = 256,
        // код команды - обмен данными.
        DATA_EXCHANGE_COMMAND = 0x42,
        // 0 - смещение на команду в массиве RX SPI_MT.
        COMMAND_BYTE_OFFSET = 0,
        // 1 - смещение на размер сообщения в массиве RX SPI_MT.
        DATA_LENGTH_BYTE_OFFSET = 1,
        // 2 - смещение на данные в массиве RX SPI_MT.
        DATA_BYTE_OFFSET = 2,
        // длина заголовка сообщения.
        HEADER_LENGTH = 2,
        // длина контрольной суммы.
        CRC_LENGTH = 2,
        // передача и приём на шине SPI происходит в дуплексном режиме.
        // если длина передаваемого кадра меньше длины принимаемого кадра, то
        // принимаемый кадр будет потерян.
        // поэтому длина передаваемого кадра должна быть больше принимаемого кадра.
        // максимальная длина принимаемого кадра известна на стадии проектирования.
        // максимальная длина принимаемого кадра.
        MAX_RECEIVED_FRAME_LENGTH = 1
    };

//-------------------------------------------------------------------------------------------------
    uint8_t* GetReceiveMessageBuff(void);
    uint8_t* GetTransmitMessageBuff(void);
    void MessagePushInit(void);
    void MessagePopInit(void);
//    uint8_t MessagePopCheck(void);
    int8_t DataReceive(void);
    int8_t DataTransmit(void);

    void DataExchangeEnable(void);
    void DataExchangeDisable(void);
    void PrepareDataExchange(void);

    void BuffOverflowFlagSet(void);
    void BuffOverflowFlagReset(void);
    void BuffByteCounterIncrement(void);
    int16_t BuffByteCounterGet(void);
    void BuffByteCounterClear(void);

//-----------------------------------------------------------------------------------------------------------------
// корректирует системное время во время обмена данными по SPI.
// во время приёма SPI пакета прерывание таймера запрещено и переменная CTimers::ui16SystemTimer
// не инкрементируется. чтобы работали подпрограммы по переполнению системного
// таймера, изменим её значение здесь. приём одного байта на скорости 100000кгц занимает
// SPI_ONE_BYTE_RECEIVE_TIME ((1000000 / 100000) * 9).
    static void SpiFrameTimeCalculate(void)
    {
        // время приёма пакета при обмене данными по SPI превысило 1мс?
        if (ui8SpiReceivedByteCounter >= SPI_ONE_MS_RECEIVED_BYTE_QUANTITY)
        {
            // обнулим счётчик байтов для вычисления длительности пакета при обмене данными по SPI.
            ui8SpiReceivedByteCounter = 0;
            // инкрементируем системный таймер.
            CTimers::ui16SystemTimer++;
        }
    }


//-------------------------------------------------------------------------------------------------
};

//-------------------------------------------------------------------------------------------------
class CModbusSerialMT : public CSerialMT
{
public:

//-------------------------------------------------------------------------------------------------
// ModbusMT
    typedef enum
    {
        // максимальная длина сообщения.
        MESSAGE_BAG_MAX_LENGTH = 256,
        // код команды - обмен данными.
        DATA_EXCHANGE_COMMAND = 0x42,
        // 0 - смещение на команду в массиве RX SPI_MT.
        COMMAND_BYTE_OFFSET = 1,
        // 1 - смещение на размер сообщения в массиве RX SPI_MT.
        DATA_LENGTH_BYTE_OFFSET = 1,
        // 2 - смещение на данные в массиве RX SPI_MT.
        DATA_BYTE_OFFSET = 2,
        // длина заголовка сообщения.
        HEADER_LENGTH = 2,
        // длина контрольной суммы.
        CRC_LENGTH = 2
    };

//-------------------------------------------------------------------------------------------------
public:

//-------------------------------------------------------------------------------------------------
    uint8_t* GetReceiveMessageBuff(void);
    uint8_t* GetTransmitMessageBuff(void);
    void MessagePushInit(void);
    void MessagePopInit(void);
//    uint8_t MessagePopCheck(void);
    int8_t DataReceive(void);
    int8_t DataTransmit(void);

//-------------------------------------------------------------------------------------------------
};

extern CSpiSerialMT xCSpiSerialMT;
extern CModbusSerialMT xCModbusSerialMT;


//-----------------------------------------------------------------------------------------------------

#endif // SERIALMT_H_INCLUDED
