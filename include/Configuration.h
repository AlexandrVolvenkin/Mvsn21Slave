//-----------------------------------------------------------------------------------------------------
//  Sourse      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------
#ifndef CONFIGURATION_H_INCLUDED
#define CONFIGURATION_H_INCLUDED

// "Конроль" и "Квитирование" в течении 7 секунд возвращают заводские настройки на текущий сеанс.

//-----------------------------------------------------------------------------------------------------


enum
{
    MEASURE_CHANNEL_NUMBER = 8,
    MEASURE_CHANNEL_STATE_BIT_NUMBER = 2,
    CHANNELS_IN_BYTE = 4,
    CHIP_NUMBER = 3,
    SLAVE_1_CHIP_ADDRESS = 0,
    SLAVE_2_CHIP_ADDRESS = 1,
    MASTER_CHIP_ADDRESS = 2,
};

enum
{
    MODBUS_RTU_BAUD_RATE = 9600UL,
    MODBUS_RTU_MAX_ADU_LENGTH = 32,

    COILS_WORK_ARRAY_LENGTH = 8,
    DISCRETE_INPUTS_ARRAY_LENGTH = (MEASURE_CHANNEL_NUMBER * MEASURE_CHANNEL_STATE_BIT_NUMBER),
    HOLDING_REGISTERS_ARRAY_LENGTH = 8,
    INPUT_REGISTERS_ARRAY_LENGTH = 8,
};


//-----------------------------------------------------------------------------------------------------
#endif // CONFIGURATION_H_INCLUDED
