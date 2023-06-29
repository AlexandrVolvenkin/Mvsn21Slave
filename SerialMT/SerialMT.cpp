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
// ������� ������ ��� ���������� ������������ ������ ��� ������ ������� �� SPI.
uint8_t CSerialMT::ui8SpiReceivedByteCounter;
uint8_t CSerialMT::ui8TempSPDR;


uint8_t aui8DebugArray[16];
//-----------------------------------------------------------------------------------------------------
// ���������� ��� �������. ���������, ���� ����. ��� ��������, ���� �������.
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
// ������� ����� ������ ��������� � ���������� ��������� ���������.
void CSerialMT::MessagePushInit(void)
{
    // ������� ��������� �� ����� ������ ���������.
    pui8MessageBag = GetTransmitMessageBuff();
    // ������� � ������ ������ ���������.
    ui8MessageBagLength = 0;
    // ������� � ������� ���������� ���������.
    ui8MessageBagIndex = 0;
}

//-----------------------------------------------------------------------------------------------------
// ��������� ��������� ���������.
// ���������� ��������� �� ��������� ����� � ������ ������ ���������.
uint8_t* CSerialMT::MessagePushRequest(uint8_t ui8Length, uint8_t ui8Address)
{
    // ��� ���������� ��������� ��� �����?
    if ((ui8MessageBagLength +
            (ui8Length) +
            SERIAL_MT_HEADER_LENGTH) >
            SERIAL_MT_MESSAGE_BAG_MAX_LENGTH)
    {
        // ��� ���������� ��������� ��� �����.
        return 0;
    }
    else
    {
        uint8_t* pui8MessageBagTemp;
        // ������� ��������� �� ����� ������ ���������.
        pui8MessageBagTemp = &pui8MessageBag[ui8MessageBagLength];
        // ���������� ��������� ���������� ��������� - �������� ����� �������-������ �����������.
        *pui8MessageBagTemp++ = ui8Address;
        // ���������� ��������� ���������� ��������� - �������� ����� ���������� ���������.
        *pui8MessageBagTemp++ = ui8Length;
        // ������� ����� ������ ��������� �� ����� ���������� ���� ���������.
        ui8MessageBagLength += (ui8Length + SERIAL_MT_HEADER_LENGTH);
        // ����� ��������� �� ��������� ����� � ������ ������ ���������.
        return pui8MessageBagTemp;
    }
}

//-----------------------------------------------------------------------------------------------------
// ������� ����� ������ ��������� � ���������� ��������� ���������.
void CSerialMT::MessagePopInit(void)
{
    // ������� ��������� �� ����� ��������� ������ ���������.
    pui8MessageBag = GetReceiveMessageBuff();
    // ������� ����� ��������� ������ ���������.
    ui8MessageBagLength = ui8Length;
    // ������� � ������� ���������� ���������.
    ui8MessageBagIndex = 0;
}

//-----------------------------------------------------------------------------------------------------
// ������� ����� ������ ��������� � ���������� ��������� ���������.
uint8_t CSerialMT::MessagePopCheck(void)
{
    // ����� ������ ��������� ������ ����������� �����?
    if (ui8MessageBagLength < SERIAL_MT_HEADER_LENGTH)
    {
        // ������� � ���, ��� ��� ���������.
        ui8Length = 0;
        return 0;
    }

    uint8_t *pui8MessageBagTemp;
    pui8MessageBagTemp = pui8MessageBag;
    // �������� ����� ���� ���� ��������� ��������� � �� �����������.
    uint8_t i = 0;
    while (i < ui8MessageBagLength)
    {
        // ��������� ���������� ���������.
        ui8Address = *pui8MessageBagTemp++;
        ui8Length = *pui8MessageBagTemp++;
        // ������� � ��������� ���������� ���������� ���������.
        i += (ui8Length + SERIAL_MT_HEADER_LENGTH);
        pui8MessageBagTemp += ui8Length;
    }
    // ����� ���� ���� ��������� ��������� � ���� �� ���������� ����� ����� ������ ���������?
    if (ui8MessageBagLength == i)
    {
        return 1;
    }
    else
    {
        // ������: ����� ���� ���� ��������� ��������� � ���� �� ���������� �� ����� ����� ������ ���������.
        // ������� � ���, ��� ��� ���������.
        ui8Length = 0;
        return 0;
    }
}

//-----------------------------------------------------------------------------------------------------
// ��������� ��������� ��������� �� ������.
// ���������� -1, ����:
// ����� ������ ��������� ������ ����������� �����,
// ��������� ��� ��������� ���������,
// ����� ���� ���� ��������� ��������� � ���� �� ���������� �� ����� ����� ������ ���������.
// ���������� ����� �������-������, ��������� �� ��������� ���������, ������ ���������.
int8_t CSerialMT::MessagePop(void)
{
    // ��������� ��� ��������� ���������?
    if (ui8MessageBagLength <= ui8MessageBagIndex)
    {
        // ��������� ��� ��������� ���������.
        // ������� � ���, ��� ��� ���������.
        ui8Length = 0;
        return 0;
    }

    uint8_t *pui8MessageBagTemp;

    pui8MessageBagTemp = (pui8MessageBag + ui8MessageBagIndex);
    // ����� ����� �������-������, ��� ������� ��������� ��������� ���������.
    ui8Address = *pui8MessageBagTemp++;
    // ����� ����� ���������� ���������.
    ui8Length = *pui8MessageBagTemp++;
    // ����� ��������� �� ��������� ���������.
    pui8DataPointer = pui8MessageBagTemp;
    // ������� � ���������� ���������� ���������.
    ui8MessageBagIndex += (ui8Length + SERIAL_MT_HEADER_LENGTH);
    return 1;
}

//-------------------------------------------------------------------------------------------------
int8_t CSerialMT::DataExchangeIsOccurred(void)
{
    // ��������� ����� ������� �� Modbus?
    return fui8DataExchangeIsOccurred;
}

//-------------------------------------------------------------------------------------------------
void CSerialMT::DataExchangeFlagSet(void)
{
    // ��������� ���� - ��������� ����� ������� �� Modbus.
    fui8DataExchangeIsOccurred = 1;
}

//-------------------------------------------------------------------------------------------------
void CSerialMT::DataExchangeFlagReset(void)
{
    // ������� ���� - ��������� ����� ������� �� Modbus.
    fui8DataExchangeIsOccurred = 0;
}

//-----------------------------------------------------------------------------------------------------
// Spi

//-------------------------------------------------------------------------------------------------



//-----------------------------------------------------------------------------------------------------
// ������� ����� ������ ��������� � ���������� ��������� ���������.
void CSpiSerialMT::MessagePushInit(void)
{
    // ������� ��������� �� ����� ������ ���������.
    pui8MessageBag = (GetTransmitMessageBuff() +
                      HEADER_LENGTH);
    // ������� � ������ ������ ���������.
    ui8MessageBagLength = 0;
    // ������� � ������� ���������� ���������.
    ui8MessageBagIndex = 0;
}

//-----------------------------------------------------------------------------------------------------
// ������� ����� ������ ��������� � ���������� ��������� ���������.
void CSpiSerialMT::MessagePopInit(void)
{
    // ������� ��������� �� ����� ��������� ������ ���������.
    pui8MessageBag = (GetReceiveMessageBuff() + HEADER_LENGTH);
    // ������� ����� ��������� ������ ���������.
    ui8MessageBagLength = ui8Length;
    // ������� � ������� ���������� ���������.
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
    // ����� ��������� �� ����� ��������.
    // ������� ����� ��� ���������. ��� �������� ����� � _FC_DATA_EXCHANGE.
    return aui8TransmitMessageBuff;
}

//-------------------------------------------------------------------------------------------------
int8_t CSpiSerialMT::DataReceive(void)
{
    uint8_t ui8LengthLocal;
    uint8_t *pui8ReceiveMessageBuff;
    pui8ReceiveMessageBuff = (GetReceiveMessageBuff());
//    // ������� ����� ����� SpiMT ��� ������ ������� �� SPI.
//    ui8LengthLocal = aui8ReceiveMessageBuff[DATA_LENGTH_BYTE_OFFSET];

    // ������� ��������� � �������� �� SPI_DATA_EXCHANGE_COMMAND?
    if ((*pui8ReceiveMessageBuff) !=
            DATA_EXCHANGE_COMMAND)
    {
        return -1;
    }
    pui8ReceiveMessageBuff++;
    // ��������� ������������ ������?
    if (fui8BuffOverflow)
    {
        return -1;
    }
    // ������� ����� ����� SpiMT ��� ������ ������� �� SPI.
    ui8LengthLocal = *pui8ReceiveMessageBuff++;
    // ����� ��������� ������� ����?
    if (ui8LengthLocal <=
            (HEADER_LENGTH))
    {
        return -1;
    }
    // ��������� ����������?
    if(iCrcSummTwoByteCompare(((GetReceiveMessageBuff())),
                              (ui8LengthLocal)) == -1)
    {
        // ������� ����� ����� SpiMT ��� ������ ������� �� SPI, ����� �� ��������� ������ ��������� �� ������.
        aui8ReceiveMessageBuff[DATA_LENGTH_BYTE_OFFSET] = 0;
        return -1;
    }
    else
    {
        // ������� ����� ����� SpiMT ��� ������ ������� �� SPI, ����� �� ��������� ������ ��������� �� ������.
        aui8ReceiveMessageBuff[DATA_LENGTH_BYTE_OFFSET] = 0;
        // ������� ��������� �� �������� ����� ��������� � ����� SpiMT.
        pui8DataPointer = pui8ReceiveMessageBuff;
        // ������� ����� ��������� ������ ��������� � ����� SpiMT.
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

    // ������� �� ����� ������� � ����� SpiMT.
    pui8TransmitMessageBuff = aui8TransmitMessageBuff;
    // ������� ������ ������ ���������.
    ui8LengthLocal = ui8MessageBagLength;
    ui8MessageBagLength = 0;
    // ����� ��������� ������� ����?
    if (ui8LengthLocal <=
            HEADER_LENGTH)
    {
        // �������� ������� ����� SpiMT.
        *pui8TransmitMessageBuff++ = 7;
        // �������� ����� ����� SpiMT.
        *pui8TransmitMessageBuff++ = 5;
        return -1;
    }

    // �������� ������� ����� SpiMT.
    *pui8TransmitMessageBuff++ = DATA_EXCHANGE_COMMAND;
    // �������� ����� ����� SpiMT.
    *pui8TransmitMessageBuff++ =
        (ui8LengthLocal +
         HEADER_LENGTH);

    // ������� �� ����� CRC � ����� SpiMT.
    pui8TransmitMessageBuff += ui8LengthLocal;
    // �������� CRC ����� SpiMT.
    ui16Data = usCrcSummTwoByteCalculation((unsigned char*)aui8TransmitMessageBuff,
                                           (ui8LengthLocal +
                                            HEADER_LENGTH));
    // �������� ������� ���� CRC ����� SpiMT.
    *pui8TransmitMessageBuff++ = (unsigned char)ui16Data;
    // �������� ������� ���� CRC ����� SpiMT.
    *pui8TransmitMessageBuff = (unsigned char)(ui16Data >> 8);

    return 0;
}


//-----------------------------------------------------------------------------------------------------
void CSpiSerialMT::DataExchangeEnable(void)
{
    // ������� ���� - ��������� ����� ������� �� SPI.
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
    // ��������� ���� - ����� ����������.
    fui8BuffOverflow = 1;
}

//-----------------------------------------------------------------------------------------------------
void CSpiSerialMT::BuffOverflowFlagReset(void)
{
    // ������� ���� - ����� ����������.
    fui8BuffOverflow = 0;
}

//-----------------------------------------------------------------------------------------------------
void CSpiSerialMT::BuffByteCounterIncrement(void)
{
    // �������������� ������� �������� ������ ��� ������ ������� �� SPI.
    nui8BuffByteCounter++;
}

//-----------------------------------------------------------------------------------------------------
int16_t CSpiSerialMT::BuffByteCounterGet(void)
{
    // ������� ������� �������� ������ ��� ������ ������� �� SPI.
    return nui8BuffByteCounter;
}

//-----------------------------------------------------------------------------------------------------
void CSpiSerialMT::BuffByteCounterClear(void)
{
    // ������� ������� ������ ��� ������ ������� �� SPI.
    nui8BuffByteCounter = 0;
}


//-----------------------------------------------------------------------------------------------------
// ModbusMT



//-----------------------------------------------------------------------------------------------------
// ������� ����� ������ ��������� � ���������� ��������� ���������.
void CModbusSerialMT::MessagePushInit(void)
{
    // ������� ��������� �� ����� ������ ���������.
    // ������� ����� ��� ���������. ��� �������� ����� � _FC_DATA_EXCHANGE.
    pui8MessageBag = (GetTransmitMessageBuff() +
                      HEADER_LENGTH +
                      DATA_LENGTH_BYTE_OFFSET);
    // ������� � ������ ������ ���������.
    ui8MessageBagLength = 0;
    // ������� � ������� ���������� ���������.
    ui8MessageBagIndex = 0;
}

//-----------------------------------------------------------------------------------------------------
// ������� ����� ������ ��������� � ���������� ��������� ���������.
void CModbusSerialMT::MessagePopInit(void)
{
    // ������� ��������� �� ����� ��������� ������ ���������.
    pui8MessageBag = pui8DataPointer;
    // ������� ����� ��������� ������ ���������.
    ui8MessageBagLength = ui8Length;
    // ������� � ������� ���������� ���������.
    ui8MessageBagIndex = 0;
}

//-------------------------------------------------------------------------------------------------
int8_t CModbusSerialMT::DataReceive(void)
{
    // ����� ��������� ������� ����?
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
    // ����� ��������� ������� ����?
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
    // ����� ��������� �� ����� ��������.
    return aui8TransmitMessageBuff;
}


//-----------------------------------------------------------------------------------------------------
