//-----------------------------------------------------------------------------------------------------
// SerialMT - serial multi trans.
//-----------------------------------------------------------------------------------------------------
#include "SerialMT.h"
#include "Crc.h"
#include "MindPasConfig.h"
#include "HD44780.h"


CSpiSerialMT xCSpiSerialMT;
CModbusSerialMT xCModbusSerialMT;

//-----------------------------------------------------------------------------------------------------
uint8_t CSerialMT::aui8ReceiveMessageBuff[];
uint8_t CSerialMT::aui8TransmitMessageBuff[];
uint8_t CSerialMT::nui8BuffByteCounter;
// счётчик байтов для вычисления длительности пакета при обмене данными по SPI.
uint8_t CSerialMT::ui8SpiReceivedByteCounter;
uint8_t CSerialMT::ui8TempSPDR;


uint8_t aui8DebugArray[16];
//-----------------------------------------------------------------------------------------------------
// возвращает тип запроса. прочитать, если ноль. или записать, если единица.
uint8_t CSerialMT::IsWrite(void)
{
    if ((ui8Address & CSerialMT::SERIAL_MT_WRITE) != 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//-----------------------------------------------------------------------------------------------------
// готовит буфер общего сообщения к извлечению вложенных сообщений.
void CSerialMT::MessagePushInit(void)
{
    // получим указатель на буфер общего сообщения.
    pui8MessageBag = GetTransmitMessageBuff();
    // перейдём к началу общего сообщения.
    ui8MessageBagLength = 0;
    // перейдём к первому вложенному сообщению.
    ui8MessageBagIndex = 0;
}

//-----------------------------------------------------------------------------------------------------
// формирует заголовок сообщения.
// возвращает указатель на свободное место в буфере общего сообщения.
uint8_t* CSerialMT::MessagePushRequest(uint8_t ui8Length, uint8_t ui8Address)
{
    // для вложенного сообщения нет места?
    if ((ui8MessageBagLength +
            (ui8Length) +
            SERIAL_MT_HEADER_LENGTH) >
            SERIAL_MT_MESSAGE_BAG_MAX_LENGTH)
    {
        // для вложенного сообщения нет места.
        return 0;
    }
    else
    {
        uint8_t* pui8MessageBagTemp;
        // получим указатель на буфер общего сообщения.
        pui8MessageBagTemp = &pui8MessageBag[ui8MessageBagLength];
        // сформируем заголовок вложенного сообщения - поместим адрес функции-задачи исполнителя.
        *pui8MessageBagTemp++ = ui8Address;
        // сформируем заголовок вложенного сообщения - поместим длину вложенного сообщения.
        *pui8MessageBagTemp++ = ui8Length;
        // изменим длину общего сообщения на длину вложенного плюс заголовок.
        ui8MessageBagLength += (ui8Length + SERIAL_MT_HEADER_LENGTH);
        // вернём указатель на свободное место в буфере общего сообщения.
        return pui8MessageBagTemp;
    }
}

//-----------------------------------------------------------------------------------------------------
// готовит буфер общего сообщения к извлечению вложенных сообщений.
void CSerialMT::MessagePopInit(void)
{
    // получим указатель на буфер принятого общего сообщения.
    pui8MessageBag = GetReceiveMessageBuff();
    // получим длину принятого общего сообщения.
    ui8MessageBagLength = ui8Length;
    // перейдём к первому вложенному сообщению.
    ui8MessageBagIndex = 0;
}

//-----------------------------------------------------------------------------------------------------
// готовит буфер общего сообщения к извлечению вложенных сообщений.
uint8_t CSerialMT::MessagePopCheck(void)
{
    // длина общего сообщения меньше минимальной длины?
    if (ui8MessageBagLength < SERIAL_MT_HEADER_LENGTH)
    {
        // сообщим о том, что нет сообщения.
        ui8Length = 0;
        return 0;
    }

    uint8_t *pui8MessageBagTemp;
    pui8MessageBagTemp = pui8MessageBag;
    // вычислим сумму длин всех вложенных сообщений с их заголовками.
    uint8_t i = 0;
    while (i < ui8MessageBagLength)
    {
        // заголовок вложенного сообщения.
        ui8Address = *pui8MessageBagTemp++;
        ui8Length = *pui8MessageBagTemp++;
        // перейдём к заголовку следующего вложенного сообщения.
        i += (ui8Length + SERIAL_MT_HEADER_LENGTH);
        pui8MessageBagTemp += ui8Length;
    }
    // сумма длин всех вложенных сообщений и длин их заголовков равна длине общего сообщения?
    if (ui8MessageBagLength == i)
    {
        return 1;
    }
    else
    {
        // ошибка: сумма длин всех вложенных сообщений и длин их заголовков не равна длине общего сообщения.
        // сообщим о том, что нет сообщения.
        ui8Length = 0;
        return 0;
    }
}

//-----------------------------------------------------------------------------------------------------
// извлекает вложенное сообщение из общего.
// возвращает -1, если:
// длина общего сообщения меньше минимальной длины,
// извлечены все вложенные сообщения,
// сумма длин всех вложенных сообщений и длин их заголовков не равна длине общего сообщения.
// возвращает адрес функции-задачи, указатель на вложенное сообщение, размер сообщения.
int8_t CSerialMT::MessagePop(void)
{
    // извлечены все вложенные сообщения?
    if (ui8MessageBagLength <= ui8MessageBagIndex)
    {
        // извлечены все вложенные сообщения.
        // сообщим о том, что нет сообщения.
        ui8Length = 0;
        return 0;
    }

    uint8_t *pui8MessageBagTemp;

    pui8MessageBagTemp = (pui8MessageBag + ui8MessageBagIndex);
    // вернём адрес функции-задачи, для которой поступило вложенное сообщение.
    ui8Address = *pui8MessageBagTemp++;
    // вернём длину вложенного сообщения.
    ui8Length = *pui8MessageBagTemp++;
    // вернём указатель на вложенное сообщение.
    pui8DataPointer = pui8MessageBagTemp;
    // перейдём к следующему вложенному сообщению.
    ui8MessageBagIndex += (ui8Length + SERIAL_MT_HEADER_LENGTH);
    return 1;
}

//-------------------------------------------------------------------------------------------------
int8_t CSerialMT::DataExchangeIsOccurred(void)
{
    // произошел обмен данными по Modbus?
    return fui8DataExchangeIsOccurred;
}

//-------------------------------------------------------------------------------------------------
void CSerialMT::DataExchangeFlagSet(void)
{
    // установим флаг - произошел обмен данными по Modbus.
    fui8DataExchangeIsOccurred = 1;
}

//-------------------------------------------------------------------------------------------------
void CSerialMT::DataExchangeFlagReset(void)
{
    // сбросим флаг - произошел обмен данными по Modbus.
    fui8DataExchangeIsOccurred = 0;
}

//-----------------------------------------------------------------------------------------------------
// Spi

//-------------------------------------------------------------------------------------------------



//-----------------------------------------------------------------------------------------------------
// готовит буфер общего сообщения к извлечению вложенных сообщений.
void CSpiSerialMT::MessagePushInit(void)
{
    // получим указатель на буфер общего сообщения.
    pui8MessageBag = (GetTransmitMessageBuff() +
                      HEADER_LENGTH);
    // перейдём к началу общего сообщения.
    ui8MessageBagLength = 0;
    // перейдём к первому вложенному сообщению.
    ui8MessageBagIndex = 0;
}

//-----------------------------------------------------------------------------------------------------
// готовит буфер общего сообщения к извлечению вложенных сообщений.
void CSpiSerialMT::MessagePopInit(void)
{
    // получим указатель на буфер принятого общего сообщения.
    pui8MessageBag = (GetReceiveMessageBuff() + HEADER_LENGTH);
    // получим длину принятого общего сообщения.
    ui8MessageBagLength = ui8Length;
    // перейдём к первому вложенному сообщению.
    ui8MessageBagIndex = 0;
}

//-------------------------------------------------------------------------------------------------
uint8_t* CSpiSerialMT::GetReceiveMessageBuff(void)
{
    return aui8ReceiveMessageBuff;
}

//-------------------------------------------------------------------------------------------------
uint8_t* CSpiSerialMT::GetTransmitMessageBuff(void)
{
    // вернём указатель на буфер передачи.
    // оставим место для заголовка. его создадим позже в _FC_DATA_EXCHANGE.
    return aui8TransmitMessageBuff;
}

//-------------------------------------------------------------------------------------------------
int8_t CSpiSerialMT::DataReceive(void)
{
    uint8_t ui8LengthLocal;
    uint8_t *pui8ReceiveMessageBuff;
    pui8ReceiveMessageBuff = (GetReceiveMessageBuff());
//    // получим длину кадра SpiMT при обмене данными по SPI.
//    ui8LengthLocal = aui8ReceiveMessageBuff[DATA_LENGTH_BYTE_OFFSET];

    // принято сообщение с командой не SPI_DATA_EXCHANGE_COMMAND?
    if ((*pui8ReceiveMessageBuff) !=
            DATA_EXCHANGE_COMMAND)
    {
        return -1;
    }
    pui8ReceiveMessageBuff++;
    // произошло переполнение буфера?
    if (fui8BuffOverflow)
    {
        return -1;
    }
    // получим длину кадра SpiMT при обмене данными по SPI.
    ui8LengthLocal = *pui8ReceiveMessageBuff++;
    // длина сообщения слишком мала?
    if (ui8LengthLocal <=
            (HEADER_LENGTH))
    {
        return -1;
    }
    // сообщение повреждено?
    if(iCrcSummTwoByteCompare(((GetReceiveMessageBuff())),
                              (ui8LengthLocal)) == -1)
    {
        // очистим длину кадра SpiMT при обмене данными по SPI, чтобы не принимать старое сообщение из буфера.
        aui8ReceiveMessageBuff[DATA_LENGTH_BYTE_OFFSET] = 0;
        return -1;
    }
    else
    {
        // очистим длину кадра SpiMT при обмене данными по SPI, чтобы не принимать старое сообщение из буфера.
        aui8ReceiveMessageBuff[DATA_LENGTH_BYTE_OFFSET] = 0;
        // получим указатель на принятое общее сообщение в кадре SpiMT.
        pui8DataPointer = pui8ReceiveMessageBuff;
        // получим длину принятого общего сообщения в кадре SpiMT.
        ui8Length = (int16_t)(ui8LengthLocal -
                              HEADER_LENGTH);
        return 0;
    }
}

//-------------------------------------------------------------------------------------------------
int8_t CSpiSerialMT::DataTransmit(void)
{
    uint16_t ui16Data;
    uint8_t ui8LengthLocal;
    uint8_t *pui8TransmitMessageBuff;

    // перейдём на место команды в кадре SpiMT.
    pui8TransmitMessageBuff = aui8TransmitMessageBuff;
    // получим размер общего сообщения.
    ui8LengthLocal = ui8MessageBagLength;
    ui8MessageBagLength = 0;
    // длина сообщения слишком мала?
    if (ui8LengthLocal <=
            HEADER_LENGTH)
    {
        // поместим команду кадра SpiMT.
        *pui8TransmitMessageBuff++ = 7;
        // поместим длину кадра SpiMT.
        *pui8TransmitMessageBuff++ = 5;
        return -1;
    }

    // поместим команду кадра SpiMT.
    *pui8TransmitMessageBuff++ = DATA_EXCHANGE_COMMAND;
    // поместим длину кадра SpiMT.
    *pui8TransmitMessageBuff++ =
        (ui8LengthLocal +
         HEADER_LENGTH);

    // перейдём на место CRC в кадре SpiMT.
    pui8TransmitMessageBuff += ui8LengthLocal;
    // вычислим CRC кадра SpiMT.
    ui16Data = usCrcSummTwoByteCalculation((unsigned char*)aui8TransmitMessageBuff,
                                           (ui8LengthLocal +
                                            HEADER_LENGTH));
    // поместим младший байт CRC кадра SpiMT.
    *pui8TransmitMessageBuff++ = (unsigned char)ui16Data;
    // поместим старший байт CRC кадра SpiMT.
    *pui8TransmitMessageBuff = (unsigned char)(ui16Data >> 8);

    return 0;
}


//-----------------------------------------------------------------------------------------------------
void CSpiSerialMT::DataExchangeEnable(void)
{
    // сбросим флаг - произошел обмен данными по SPI.
    fui8DataExchangeIsOccurred = 0;
    vSpiDataExchangeEnable();
}

//-----------------------------------------------------------------------------------------------------
void CSpiSerialMT::DataExchangeDisable(void)
{
    vSpiDataExchangeDisable();
}

//-------------------------------------------------------------------------------------------------
void CSpiSerialMT::PrepareDataExchange(void)
{
    nui8BuffByteCounter = 0;
    fui8BuffOverflow = 0;
}

//-----------------------------------------------------------------------------------------------------
void CSpiSerialMT::BuffOverflowFlagSet(void)
{
    // установим флаг - буфер переполнен.
    fui8BuffOverflow = 1;
}

//-----------------------------------------------------------------------------------------------------
void CSpiSerialMT::BuffOverflowFlagReset(void)
{
    // сбросим флаг - буфер переполнен.
    fui8BuffOverflow = 0;
}

//-----------------------------------------------------------------------------------------------------
void CSpiSerialMT::BuffByteCounterIncrement(void)
{
    // инкрементируем счётчик принятых байтов при обмене данными по SPI.
    nui8BuffByteCounter++;
}

//-----------------------------------------------------------------------------------------------------
int16_t CSpiSerialMT::BuffByteCounterGet(void)
{
    // получим счётчик принятых байтов при обмене данными по SPI.
    return nui8BuffByteCounter;
}

//-----------------------------------------------------------------------------------------------------
void CSpiSerialMT::BuffByteCounterClear(void)
{
    // обнулим счётчик байтов при обмене данными по SPI.
    nui8BuffByteCounter = 0;
}


//-----------------------------------------------------------------------------------------------------
// ModbusMT



//-----------------------------------------------------------------------------------------------------
// готовит буфер общего сообщения к извлечению вложенных сообщений.
void CModbusSerialMT::MessagePushInit(void)
{
    // получим указатель на буфер общего сообщения.
    // оставим место для заголовка. его создадим позже в _FC_DATA_EXCHANGE.
    pui8MessageBag = (GetTransmitMessageBuff() +
                      HEADER_LENGTH +
                      DATA_LENGTH_BYTE_OFFSET);
    // перейдём к началу общего сообщения.
    ui8MessageBagLength = 0;
    // перейдём к первому вложенному сообщению.
    ui8MessageBagIndex = 0;
}

//-----------------------------------------------------------------------------------------------------
// готовит буфер общего сообщения к извлечению вложенных сообщений.
void CModbusSerialMT::MessagePopInit(void)
{
    // получим указатель на буфер принятого общего сообщения.
    pui8MessageBag = pui8DataPointer;
    // получим длину принятого общего сообщения.
    ui8MessageBagLength = ui8Length;
    // перейдём к первому вложенному сообщению.
    ui8MessageBagIndex = 0;
}

//-------------------------------------------------------------------------------------------------
int8_t CModbusSerialMT::DataReceive(void)
{
    // длина сообщения слишком мала?
    if (ui8Length <=
            (HEADER_LENGTH))
    {
        return -1;
    }
    return 0;
}

//-------------------------------------------------------------------------------------------------
int8_t CModbusSerialMT::DataTransmit(void)
{
    // длина сообщения слишком мала?
    if (ui8MessageBagLength <=
            HEADER_LENGTH)
    {
        return -1;
    }
    return 0;
}

//-------------------------------------------------------------------------------------------------
uint8_t* CModbusSerialMT::GetReceiveMessageBuff(void)
{
    return aui8ReceiveMessageBuff;
}

//-------------------------------------------------------------------------------------------------
uint8_t* CModbusSerialMT::GetTransmitMessageBuff(void)
{
    // вернём указатель на буфер передачи.
    return aui8TransmitMessageBuff;
}


//-----------------------------------------------------------------------------------------------------
