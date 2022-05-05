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
CMeasurementChannel::CMeasurementChannel()
{
    //ctor
}

//-----------------------------------------------------------------------------------------------------
CMeasurementChannel::~CMeasurementChannel()
{
    //dtor
}

//-----------------------------------------------------------------------------------------------------
// Обработка блока данных от АЦП.
uint8_t CMeasurementChannel::StatusCheck(uint16_t uiValue)
{
    uint8_t uiState = CMeasurementChannel::SHORT_CIRCUIT;

    if(uiValue < CMeasurementChannel::WB_LEVEL)
    {
        uiState = CMeasurementChannel::WIRE_BREAK;	// Определение кода состояния входа
    }
    else if(uiValue < CMeasurementChannel::ON_LEVEL)
    {
        uiState = CMeasurementChannel::ON;
    }
    else if(uiValue < CMeasurementChannel::OFF_LEVEL)
    {
        return;		// Попадание в мертвую зону
    }
    else if(uiValue < CMeasurementChannel::SC_LEVEL)
    {
        uiState = CMeasurementChannel::OFF;
    }

//    m_uiState = uiState;

//    TDChannel *DCPtr = &CMeasurementChannel::DChannel[Index];
//
//    if(DCPtr->Current == uiState)
//    {
//        if(DCPtr->Counter != CMeasurementChannel::STOP_COUNT)
//        {
//            if((++DCPtr->Counter) >= CMeasurementChannel::DEBOUNCE_LEVELS)
//            {
//                DCPtr->uiState   = uiState;
//                DCPtr->Counter = CMeasurementChannel::STOP_COUNT;
//            }
//        }
//    }
//    else
//    {
//        DCPtr->Current = uiState;
//        DCPtr->Counter = 0;
//    }
//
    return uiState;
};
