//-----------------------------------------------------------------------------------------------------
//  Source      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------
#include "MeasurementChannel.h"
#include "Platform.h"

//-----------------------------------------------------------------------------------------------------
// ��������� ����� ������ �� ���.
uint8_t CMeasurementChannel::StateCheck(uint16_t uiValue)
{
    uint8_t uiState = CMeasurementChannel::SHORT_CIRCUIT;

    if(uiValue < CMeasurementChannel::WB_LEVEL)
    {
        uiState = CMeasurementChannel::WIRE_BREAK;
    }
    else if(uiValue < CMeasurementChannel::ON_LEVEL)
    {
        uiState = CMeasurementChannel::ON;
    }
    else if(uiValue < CMeasurementChannel::OFF_LEVEL)
    {
        return;		// ��������� � ������� ����
    }
    else if(uiValue < CMeasurementChannel::SC_LEVEL)
    {
        uiState = CMeasurementChannel::OFF;
    }

    return uiState;
};
