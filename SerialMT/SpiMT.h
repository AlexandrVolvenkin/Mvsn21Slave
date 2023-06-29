//-------------------------------------------------------------------------------------------------
// SpiMT - spi multi trans.
//-------------------------------------------------------------------------------------------------
#ifndef SPIMT_H_INCLUDED
#define SPIMT_H_INCLUDED

#include <stdint.h>
#include "SerialMT.h"
#include "ATmega128platform.h"
#include "Timers.h"


#define SPI_ONE_BYTE_RECEIVE_TIME ((1000000 / 150000) * 9)
// ���������� ���� �������� �� ���� ����������� ��� ������ ������� �� SPI.
#define SPI_ONE_MS_RECEIVED_BYTE_QUANTITY (1000 / SPI_ONE_BYTE_RECEIVE_TIME)

//-------------------------------------------------------------------------------------------------
class CSpiMT
{
public:
    // ������ ����� SpiMT: ������� - 1 ����, ���������� ���� � ����� - 1 ����,
    // ������, ����������� �����(����� ����������� ���� ������(�������, ����������, ������)) - 2 �����.
    typedef enum
    {
        // ������������ ����� ���������.
        MESSAGE_BAG_MAX_LENGTH = 256,
        // ��� ������� - ����� �������.
        DATA_EXCHANGE_COMMAND = 0x42,
        // 0 - �������� �� ������� � ������� RX SPI_MT.
        COMMAND_BYTE_OFFSET = 0,
        // 1 - �������� �� ������ ��������� � ������� RX SPI_MT.
        DATA_LENGTH_BYTE_OFFSET = 1,
        // 2 - �������� �� ������ � ������� RX SPI_MT.
        DATA_BYTE_OFFSET = 2,
        // ����� ��������� ���������.
        HEADER_LENGTH = 2,
        // ����� ����������� �����.
        CRC_LENGTH = 2,
        // �������� � ���� �� ���� SPI ���������� � ���������� ������.
        // ���� ����� ������������� ����� ������ ����� ������������ �����, ��
        // ����������� ���� ����� �������.
        // ������� ����� ������������� ����� ������ ���� ������ ������������ �����.
        // ������������ ����� ������������ ����� �������� �� ������ ��������������.
        // ������������ ����� ������������ �����.
        MAX_RECEIVED_FRAME_LENGTH = 1
    };

//-------------------------------------------------------------------------------------------------
public:
    static uint8_t fui8DataExchangeIsOccurred;
    static uint8_t fui8BuffOverflow;

    static uint8_t aui8ReceiveMessageBuff[CSpiMT::MESSAGE_BAG_MAX_LENGTH + 4];
    static uint8_t aui8TransmitMessageBuff[CSpiMT::MESSAGE_BAG_MAX_LENGTH + 4];
    static uint8_t nui8BuffByteCounter;
    static uint16_t nui16RxFrameLength;
    static uint16_t nui16TxFrameLength;
    // ������� ������ ��� ���������� ������������ ������ ��� ������ ������� �� SPI.
    static uint8_t ui8SpiReceivedByteCounter;
    static uint8_t ui8TempSPDR;

//-------------------------------------------------------------------------------------------------
    static uint8_t* GetReceiveMessageBuff(void);
    static uint8_t* GetTransmitMessageBuff(void);
    static int16_t GetMessageLength(void);
    static int16_t DataReceive(CSerialMT::TSerialMTData *pxData);
    static int16_t DataTransmit(CSerialMT::TSerialMTData *pxData);
    static void DataExchangeEnable(void);
    static void DataExchangeDisable(void);
    static void PrepareDataExchange(void);
    static int8_t DataExchangeIsOccurred(void);

    static uint8_t* GetSpiRxBuff(void)
    {
        // ����� ��������� �� ����� �����.
        return CSpiMT::aui8ReceiveMessageBuff;
    }
    static uint8_t* GetSpiTxBuff(void)
    {
        // ����� ��������� �� ����� ��������.
        return CSpiMT::aui8TransmitMessageBuff;
    }
    static void DataExchangeFlagSet(void)
    {
        // ��������� ���� - ��������� ����� ������� �� SPI.
        CSpiMT::fui8DataExchangeIsOccurred = 1;
    }
    static void DataExchangeFlagReset(void)
    {
        // ������� ���� - ��������� ����� ������� �� SPI.
        CSpiMT::fui8DataExchangeIsOccurred = 0;
    }
    static void BuffOverflowFlagSet(void)
    {
        // ��������� ���� - ��������� ����� ������� �� SPI.
        CSpiMT::fui8BuffOverflow = 1;
    }
    static void BuffOverflowFlagReset(void)
    {
        // ������� ���� - ��������� ����� ������� �� SPI.
        CSpiMT::fui8BuffOverflow = 0;
    }
    static void BuffByteCounterIncrement(void)
    {
        // �������������� ������� �������� ������ ��� ������ ������� �� SPI.
        CSpiMT::nui8BuffByteCounter++;
    }
    static int16_t BuffByteCounterGet(void)
    {
        // ������� ������� �������� ������ ��� ������ ������� �� SPI.
        return CSpiMT::nui8BuffByteCounter;
    }
    static void BuffByteCounterClear(void)
    {
        // ������� ������� ������ ��� ������ ������� �� SPI.
        CSpiMT::nui8BuffByteCounter = 0;
    }
    static void FrameLengthGet(void)
    {
        // ������� ����� ����� SpiMT ��� ������ ������� �� SPI.
        CSpiMT::nui16RxFrameLength = CSpiMT::nui8BuffByteCounter;
    }
//-----------------------------------------------------------------------------------------------------------------
// ������������ ��������� ����� �� ����� ������ ������� �� SPI.
// �� ����� ����� SPI ������ ���������� ������� ��������� � ���������� CTimers::ui16SystemTimer
// �� ����������������. ����� �������� ������������ �� ������������ ����������
// �������, ������� � �������� �����. ���� ������ ����� �� �������� 100000��� ��������
// SPI_ONE_BYTE_RECEIVE_TIME ((1000000 / 100000) * 9).
    static void SpiFrameTimeCalculate(void)
    {
        // ����� ����� ������ ��� ������ ������� �� SPI ��������� 1��?
        if (CSpiMT::ui8SpiReceivedByteCounter >= SPI_ONE_MS_RECEIVED_BYTE_QUANTITY)
        {
            // ������� ������� ������ ��� ���������� ������������ ������ ��� ������ ������� �� SPI.
            CSpiMT::ui8SpiReceivedByteCounter = 0;
            // �������������� ��������� ������.
            CTimers::ui16SystemTimer++;
        }
    }

private:
};

#endif // SPIMT_H_INCLUDED
