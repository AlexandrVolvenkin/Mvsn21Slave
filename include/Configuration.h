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
        MASTER_DATA_EXCHANGE_MAX_MESSAGE_LENGTH = 64,
        MEASURE_CHANNEL_NUMBER = 8,
        DISCRETE_INPUT_NUMBER = 24,
        MEASURE_CHANNEL_STATE_BIT_NUMBER = 2,
        CHANNELS_IN_BYTE = 4,
        CHIP_NUMBER = 3,
        SLAVE_1_CHIP_ADDRESS = 0,
        SLAVE_2_CHIP_ADDRESS = 1,
        MASTER_CHIP_ADDRESS = 2,
    };

    typedef enum
    {
        // длина заголовка вложенного сообщения.
        HEADER_LENGTH = 3,
        CRC_LENGTH = 2,
        TAIL_LENGTH = CRC_LENGTH,
        MESSAGE_BAG_MAX_LENGTH = 32,
        MIN_MESSAGE_LENGTH = 4,
    };


//-----------------------------------------------------------------------------------------------------
#endif // CONFIGURATION_H_INCLUDED
