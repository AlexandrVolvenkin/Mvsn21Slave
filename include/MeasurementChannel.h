//-----------------------------------------------------------------------------------------------------
//  Source      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------
#ifndef CMEASUREMENTCHANNEL_H
#define CMEASUREMENTCHANNEL_H

#include <stdint.h>

//-----------------------------------------------------------------------------------------------------
class CMeasurementChannel
{
public:
    enum ADC_LEVELS
    {
        WB_LEVEL	=  23,			// ������� ������ �����
        ON_LEVEL 	= 143,			// ������� ������������
        OFF_LEVEL	= 161,			// ������� �����������
        SC_LEVEL	= 554			// ������� ��������� ���������
    };

    enum PREFERENCES
    {
        DEBOUNCE_LEVELS = 150,		// ������� ����������
        OUT_MASK	  = 0x00FFFFFF		// ����� �������� ������
    };

    enum SPECIAL_CODES
    {
        DEAD_ZONE	= 0xFF,			// ���� �����������������
        STOP_COUNT	= 0xFF,			// ������� �������� �����
        INVALID	= 0x0F			// ��������������� ��������	[##]
    };

    enum FLAGS
    {
        MASTER_VALID	= 0,			// ���� ������������� ������ �������� ��
        SLAVE_VALID	= 1			// ���� ������������� ������ �������� ��
    };

    enum INPUT_STATE_CODE
    {
        WIRE_BREAK	= 0x02,			// ����� �����
        ON = 0x01,			// ������������ �������
        OFF = 0x00,			// ���������� �������
        SHORT_CIRCUIT = 0x03			// �������� ���������
    };

    enum STATUS_CODE
    {
        NORM		= 0x00,			// ���������� ����������
        DATA_WAIT	= 0x01,			// �������� ���������� ������
    };

//    CMeasurementChannel();
//    virtual ~CMeasurementChannel();
    static uint8_t StateCheck(uint16_t );

protected:
private:
    uint8_t m_uiState;

    friend class CMvsn21;
};
//-----------------------------------------------------------------------------------------------------

#endif // CMEASUREMENTCHANNEL_H
