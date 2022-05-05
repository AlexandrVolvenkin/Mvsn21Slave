//-----------------------------------------------------------------------------------------------------
//  Sourse      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------
#ifndef DATATYPES_H_INCLUDED
#define DATATYPES_H_INCLUDED

#include "Configuration.h"

//-----------------------------------------------------------------------------------------------------
struct TChipChannelData
{
    CMeasurementChannel axMeasurementChannels[MEASURE_CHANNEL_NUMBER];
};

//-----------------------------------------------------------------------------------------------------
struct TChannelRemap
{
    uint8_t uiChip;
    uint8_t uiChannel;
};

//-----------------------------------------------------------------------------------------------------


#endif // DATATYPES_H_INCLUDED
