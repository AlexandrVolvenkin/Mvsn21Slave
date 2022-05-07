//-----------------------------------------------------------------------------------------------------
//  Source      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------
#include "Platform.h"
#include "Mvsn21.h"

// ATMega48

bool CAdc::m_fbMeasureIsComlete;
////-----------------------------------------------------------------------------------------------------
//CAdc::CAdc()
//{
//
//}
//
////-----------------------------------------------------------------------------------------------------
//CAdc::~CAdc()
//{
//
//}

//-----------------------------------------------------------------------------------------------------
void CAdc::Init(void)
{
    // Отключаем цифровые входные буферы на линиях АЦП.
    DIDR0  = 0x3F;
    // SCK/128
    ADCSRA = CAdc::DIV128;
    ADCSRB = 0x00;
    // Внутренний источник опорного напряжения Vref = 1.1V
    ADMUX = (BIT(REFS1) | BIT(REFS0));
}

//-----------------------------------------------------------------------------------------------------
void CAdc::ChannelSelect(uint8_t uiChannel)
{
    ADMUX &= ~(0x0F);
    ADMUX |= (uiChannel & 0x0F);
}

//-----------------------------------------------------------------------------------------------------
void CAdc::StartSingle(void)
{
    // Отключаем режим пониженного энергопотребления.
    PRR &= ~BIT(PRADC);
    // Включаем АЦП.
    ADCSRA |= (BIT(ADEN) | BIT(ADIE) | BIT(ADSC));
}

//-----------------------------------------------------------------------------------------------------
void CAdc::Start(void)
{
    // Включаем АЦП.
////    ADCSRA |= (BIT(ADEN) | BIT(ADIE) | BIT(ADSC));
    ADCSRA |= (BIT(ADEN) | BIT(ADSC));
//    ADCSRA |= (BIT(ADSC));
}

//-----------------------------------------------------------------------------------------------------
void CAdc::Stop(void)
{
    // Выключаем АЦП.
    ADCSRA &= ~(BIT(ADEN) | BIT(ADSC));
//    ADCSRA &= ~(BIT(ADEN) | BIT(ADIE) | BIT(ADSC));
}

//-----------------------------------------------------------------------------------------------------
void CAdc::Disable(void)
{
    // Выключаем АЦП.
//    ADCSRA &= ~(BIT(ADEN) | BIT(ADIE) | BIT(ADSC));
    ADCSRA &= ~(BIT(ADEN) | BIT(ADSC));
    // Включаем режим пониженного энергопотребления.
    PRR |= BIT(PRADC);
}

//-----------------------------------------------------------------------------------------------------
void CAdc::Enable(void)
{
    // Отключаем режим пониженного энергопотребления.
    PRR &= ~BIT(PRADC);
    // Включаем АЦП.
//    ADCSRA |= (BIT(ADEN) | BIT(ADIE));
    ADCSRA |= BIT(ADEN);
}

//-----------------------------------------------------------------------------------------------------
uint16_t CAdc::GetMeasureValue(void)
{
    uint16_t uiData;
    uiData = (static_cast<uint16_t>(ADCL));
    uiData |= (static_cast<uint16_t>(ADCH) << 8);
    return uiData;
};

////-----------------------------------------------------------------------------------------------------
//#pragma vector = ADC_vect
//__interrupt void CAdc::SIG_ADC(void)
//{
//    m_fbMeasureIsComlete = true;
//}
////-----------------------------------------------------------------------------------------------------





volatile uint8_t* CUart::m_UBRRH;
volatile uint8_t* CUart::m_UBRRL;
volatile uint8_t* CUart::m_UCSRA;
volatile uint8_t* CUart::m_UCSRB;
volatile uint8_t* CUart::m_UCSRC;
volatile uint8_t* CUart::m_UDR;
volatile uint8_t* CUart::m_rs485ddr;
volatile uint8_t CUart::m_rs485ddpin;
volatile uint8_t* CUart::m_rs485port;
volatile uint8_t CUart::m_rs485pin;

uint8_t* CUart::m_puiTxBuffer;
uint16_t CUart::m_nuiTxBuffByteCounter;
uint8_t* CUart::m_puiRxBuffer;
uint16_t CUart::m_nuiRxBuffByteCounter;
uint8_t CUart::m_auiIntermediateBuff[UART_INTERMEDIATE_BUFF_LENGTH];
bool CUart::m_bfByteIsReceived;
bool CUart::m_bfFrameIsSended;
bool CUart::m_bfRxBuffOverflow;

////-----------------------------------------------------------------------------------------------------
//CUart::CUart()
//{
//
//}

////-----------------------------------------------------------------------------------------------------
//CUart::CUart(volatile uint8_t *ubrrh, volatile uint8_t *ubrrl,
//             volatile uint8_t *ucsra, volatile uint8_t *ucsrb,
//             volatile uint8_t *ucsrc, volatile uint8_t *udr,
//             volatile uint8_t *rs485ddr, volatile uint8_t rs485ddpin,
//             volatile uint8_t *rs485port, volatile uint8_t rs485pin) :
//    m_UBRRH(ubrrh), m_UBRRL(ubrrl), m_UCSRA(ucsra),
//    m_UCSRB(ucsrb), m_UCSRC(ucsrc), m_UDR(udr),
//    m_rs485ddr(rs485ddr), m_rs485ddpin(rs485ddpin), m_rs485port(rs485port), m_rs485pin(rs485pin)
//{
//    if (m_rs485ddr)
//    {
//        *m_rs485ddr |= Bit(m_rs485ddpin);
//        *m_rs485port &= ~(Bit(m_rs485pin));
//    }
//}

//-----------------------------------------------------------------------------------------------------
void CUart::UartBind(volatile uint8_t *ubrrh, volatile uint8_t *ubrrl,
                volatile uint8_t *ucsra, volatile uint8_t *ucsrb,
                volatile uint8_t *ucsrc, volatile uint8_t *udr,
                volatile uint8_t *rs485ddr, volatile uint8_t rs485ddpin,
                volatile uint8_t *rs485port, volatile uint8_t rs485pin)
{
    m_UBRRH = ubrrh;
    m_UBRRL = ubrrl;
    m_UCSRA = ucsra;
    m_UCSRB = ucsrb;
    m_UCSRC = ucsrc;
    m_UDR = udr;
    m_rs485ddr = rs485ddr;
    m_rs485ddpin = rs485ddpin;
    m_rs485port = rs485port;
    m_rs485pin = rs485pin;

    if (m_rs485ddr)
    {
        *m_rs485ddr |= Bit(m_rs485ddpin);
        *m_rs485port &= ~(Bit(m_rs485pin));
    }
}

////-----------------------------------------------------------------------------------------------------
//CUart::~CUart()
//{
//
//}

//-----------------------------------------------------------------------------------------------------
void CUart::Init(uint32_t ulBaudRate,
                 uint8_t ucParity,
                 uint8_t ucDataBits,
                 uint8_t ucStopBit,
                 uint8_t *puiRxBuffer,
                 uint8_t *puiTxBuffer)
{
    m_puiRxBuffer = puiRxBuffer;
    m_puiTxBuffer = puiTxBuffer;

    /* Set the baud rate */
    uint16_t uiBaudRateTemp = (F_CPU / (ulBaudRate * 16UL) - 1);
    *m_UBRRL = uiBaudRateTemp;
    *m_UBRRH = (uiBaudRateTemp >> 8);

    /* Set data bits (7, 8 bits) */
    switch (ucDataBits)
    {
    case 8:
        *m_UCSRC |= (1 << UCSZ00) | (1 << UCSZ01);
        break;
    case 7:
        *m_UCSRC |= (1 << UCSZ01);
        break;
    }

    /* Set parity */
    if (ucParity == 'N')
    {
        /* None */
    }
    else if (ucParity == 'E')
    {
        /* Even */
        *m_UCSRC |= (1 << UPM01);
    }
    else
    {
        /* Odd */
        *m_UCSRC |= (1 << UPM01) | (1 << UPM00);
    }

    /* Stop bit (1 or 2) */
    if (ucStopBit == 1)
    {

    }
    else if (ucStopBit == 2) /* 2 */
    {
        *m_UCSRC |= (1 << USBS0);
    }
}

//-----------------------------------------------------------------------------------------------------
void CUart::Reset(void)
{
    m_nuiRxBuffByteCounter = 0;
    m_bfByteIsReceived = 0;
    m_bfFrameIsSended = 0;
    m_bfRxBuffOverflow = 0;
}

//-----------------------------------------------------------------------------------------------------
void CUart::Enable(void)
{
    *m_UCSRA |= (1 << RXC0);
    *m_UCSRB |= (1 << RXEN0) | (1 << RXCIE0);
}

//-----------------------------------------------------------------------------------------------------
void CUart::Disable(void)
{
    *m_UCSRB &= ~((1 << TXEN0) | (1 << TXCIE0));
    *m_UCSRB &= ~((1 << RXEN0) | (1 << RXCIE0));
}

////-----------------------------------------------------------------------------------------------------
//void CUart::Rs485RtsOn(void)
//{
//    *m_rs485port |= (1 << m_rs485pin);
//}
//
////-----------------------------------------------------------------------------------------------------
//void CUart::Rs485RtsOff(void)
//{
//    *m_rs485port &= ~(1 << m_rs485pin);
//}

//-----------------------------------------------------------------------------------------------------
int16_t CUart::Write(uint8_t *puiSource, uint16_t uiLength)
{
    m_puiTxBuffer = (uint8_t*)puiSource;
    m_nuiTxBuffByteCounter = uiLength;

    *m_UCSRA &= ~(1 << RXC0);
    *m_UCSRB &= ~((1 << RXEN0) | (1 << RXCIE0));

//    if (m_rs485ddr)
//    {
//        Rs485RtsOn();
//    }
//    UDR0 = *pucUsartTxBuff++;
    //    UCSR0B |= (1 << TXEN0) | (1 << TXCIE0);
    *m_UCSRA |= (1 << TXC0) | (1 << UDRE0);
    *m_UCSRB |= (1 << TXEN0) | (1 << UDRIE0);

    return 1;
}

//-----------------------------------------------------------------------------------------------------
int16_t CUart::Read(uint8_t *puiDestination, uint16_t uiLength)
{
    m_bfByteIsReceived = 0;

    if (m_bfRxBuffOverflow)
    {
        return -1;
    }

    if (uiLength <= m_nuiRxBuffByteCounter)
    {
        return -1;
    }
    else if (m_nuiRxBuffByteCounter)
    {
//        *m_UCSRB &= ~((1 << RXEN0) | (1 << RXCIE0));
        CPlatform::InterruptDisable();
//
        for (int16_t i = 0; i < m_nuiRxBuffByteCounter; i++)
        {
            puiDestination[i] = m_auiIntermediateBuff[i];
        }

        uint8_t uiCounter = m_nuiRxBuffByteCounter;
        m_nuiRxBuffByteCounter = 0;

//        *m_UCSRB |= (1 << RXEN0) | (1 << RXCIE0);
        CPlatform::InterruptEnable();

        return uiCounter;
    }
    else if (0 == m_nuiRxBuffByteCounter)
    {
        return 0;
    }
    return 0;
}

////-----------------------------------------------------------------------------------------------------
//int16_t CUart::Read(uint8_t *puiDestination)
//{
//    if (m_bfByteIsReceived)
//    {
//        m_bfByteIsReceived = 0;
//
//        m_puiRxBuffer = puiDestination;
//
//        if (m_bfRxBuffOverflow)
//        {
//            return -1;
//        }
//
//        if ((UART_INTERMEDIATE_BUFFER_LENGTH - 1) <= m_nuiRxBuffByteCounter)
//        {
//            return -1;
//        }
//        else if (m_nuiRxBuffByteCounter)
//        {
//
////            for (int16_t i = 0; i < m_nuiRxBuffByteCounter; i++)
////            {
////                pucDestination[i] = m_auiIntermediateBuff[i];
////            }
////
////            uint8_t uiCounter = m_nuiRxBuffByteCounter;
////            m_nuiRxBuffByteCounter = 0;
////            return uiCounter;
//            return m_nuiRxBuffByteCounter;
//        }
//        else if (0 == m_nuiRxBuffByteCounter)
//        {
//            return 0;
//        }
//    }
//    return 0;
//}

////-----------------------------------------------------------------------------------------------------
//int16_t CUart::Read(void)
//{
//    if (m_bfByteIsReceived)
//    {
//        m_bfByteIsReceived = 0;
//
//        if (m_bfRxBuffOverflow)
//        {
//            return -1;
//        }
//
//        if ((UART_INTERMEDIATE_BUFFER_LENGTH - 1) <= m_nuiRxBuffByteCounter)
//        {
//            return -1;
//        }
//        else if (m_nuiRxBuffByteCounter)
//        {
//            return m_nuiRxBuffByteCounter;
//        }
//        else if (0 == m_nuiRxBuffByteCounter)
//        {
//            return 0;
//        }
//    }
//    return 0;
//}

//-----------------------------------------------------------------------------------------------------
void CUart::UdreInterruptHandler(void)
{
    if (m_nuiTxBuffByteCounter == 0)
    {
        *m_UCSRB &= ~((1 << UDRIE0));
        *m_UCSRA |= (1 << TXC0);
        *m_UCSRB |= (1 << TXCIE0);
        return;
    }
    *m_UDR = *m_puiTxBuffer++;
    m_nuiTxBuffByteCounter--;
}

//-----------------------------------------------------------------------------------------------------
void CUart::TxcInterruptHandler(void)
{
    *m_UCSRB &= ~((1 << TXEN0) | (1 << TXCIE0));

//    if (m_rs485ddr)
//    {
//        Rs485RtsOff();
//    }
    *m_UCSRA |= (1 << RXC0);
    *m_UCSRB |= (1 << RXEN0) | (1 << RXCIE0);
    m_bfFrameIsSended = 1;
    return;
}

//-----------------------------------------------------------------------------------------------------
void CUart::RecvInterruptHandler(void)
{
    // промежуточный буфер приёма переполнен?
    if (m_nuiRxBuffByteCounter >=
            UART_MAX_BUFF_LENGTH)
    {
        m_bfRxBuffOverflow = 1;
        // не инкрементируем pucUsartRxBuff, чтобы не выйти за границы буфера.
        m_bfByteIsReceived = 1;
    }
    else
    {
        m_puiRxBuffer[m_nuiRxBuffByteCounter++] = *m_UDR;
        m_bfByteIsReceived = 1;
    }
}

//-----------------------------------------------------------------------------------------------------
// Прерывание по пустому регистру данных UDR
#pragma vector = USART_UDRE_vect
__interrupt void SIG_UART0_DATA(void)
{
    CUart::UdreInterruptHandler();
}

//-----------------------------------------------------------------------------------------------------
// Прерывание по завершению передачи UART
#pragma vector = USART_TX_vect
__interrupt void SIG_UART0_TXC(void)
{
    CUart::TxcInterruptHandler();
}

//-----------------------------------------------------------------------------------------------------
// Прерывание по завершению приема UART
#pragma vector = USART_RX_vect
__interrupt void SIG_UART0_RECV(void)
{
    CUart::RecvInterruptHandler();
}

//-----------------------------------------------------------------------------------------------------





////----------------------------------------- EEPROM ----------------------------------------------------------------
//CEeprom::CEeprom()
//{
//
//}
//
////-----------------------------------------------------------------------------------------------------
//CEeprom::~CEeprom()
//{
//
//}

// SFR_W_N(0x21, EEAR, Dummy15, Dummy14, Dummy13, Dummy12, Dummy11, Dummy10, Dummy9, Dummy8, EEAR7, EEAR6, EEAR5, EEAR4, EEAR3, EEAR2, EEAR1, EEAR0)
// SFR_B_N(0x20, EEDR, EEDR7, EEDR6, EEDR5, EEDR4, EEDR3, EEDR2, EEDR1, EEDR0)
// SFR_B_N(0x1F, EECR, Dummy7, Dummy6, EEPM1, EEPM0, EERIE, EEMPE, EEPE, EERE)
//-----------------------------------------------------------------------------------------------------
uint8_t CEeprom::ReadByte(uint16_t ui16EepromSourse)
{
    __watchdog_reset();
    while (BitIsSet(EECR, EEPE))
    {
        __watchdog_reset();
    }
    __disable_interrupt();
    EEAR = ui16EepromSourse;
    SetBit(EECR, EERE);
    __enable_interrupt();
    return EEDR;
}

//-----------------------------------------------------------------------------------------------------
void CEeprom::WriteByte(uint16_t ui16EepromDestination, uint8_t ucData)
{
    __watchdog_reset();
    while (BitIsSet(EECR, EEPE))
    {
        __watchdog_reset();
    }
    __disable_interrupt();
    EEAR = ui16EepromDestination;
    EEDR = ucData;
    EECR |= (1<<EEMPE);
    EECR |= (1<<EEPE);
    __enable_interrupt();
}

//-----------------------------------------------------------------------------------------------------
uint8_t CEeprom::Read(uint8_t *pucRamDestination, uint16_t uiEepromSourse, uint16_t nuiLength)
{
    while (nuiLength != 0)
    {
        *pucRamDestination++ = ReadByte(uiEepromSourse++);
        nuiLength--;
    }
    return 1;
}

//-----------------------------------------------------------------------------------------------------
uint8_t CEeprom::Write(uint16_t uiEepromDestination, uint8_t *pucRamSourse, uint16_t nuiLength)
{
    while (nuiLength != 0)
    {
        WriteByte((uiEepromDestination++), (*pucRamSourse++));
        nuiLength--;
    }
    return 1;
}
//-----------------------------------------------------------------------------------------------------





////----------------------------------------- CSpi ----------------------------------------------------------------
//uint8_t CSpi::m_uiExchangeByte;
//uint8_t* CSpi::m_puiRxBuffer;
//uint8_t* CSpi::m_puiTxBuffer;
//uint16_t CSpi::m_nuiBuffByteCounter;
//uint16_t CSpi::m_uiReceivedByteCounter;
//bool CSpi::m_bfByteIsReceived;
//bool CSpi::m_bfByteIsTransmited;
//bool CSpi::m_bfDataExchangeInProgress;
//bool CSpi::m_bfDataExchangeIsOccur;
//bool CSpi::m_bfRxBuffOverflow;
//uint8_t CSpi::m_auiSpiRxBuffer[];
//uint8_t CSpi::m_auiSpiTxBuffer[];
//
////-----------------------------------------------------------------------------------------------------
//CSpi::CSpi()
//{
//
//}
//
////-----------------------------------------------------------------------------------------------------
//CSpi::~CSpi()
//{
//
//}
//
////-----------------------------------------------------------------------------------------------------
//void CSpi::Init(uint8_t *puiRxBuffer, uint8_t *puiTxBuffer)
//{
////    m_puiRxBuffer = m_auiSpiRxBuffer;
////    m_puiTxBuffer = m_auiSpiTxBuffer;
//    m_puiRxBuffer = puiRxBuffer;
//    m_puiTxBuffer = puiTxBuffer;
////    // Master mode.
////    SPCR = 0;
////    SPCR  |= (BIT(SPR1));		// Slave,  57600.
////
////    DDRB  |= (BIT(SPI_MOSI));
//
//    // Slave mode.
//    DDRB  |= (Bit(SPI_MISO) | Bit(DDB1));
//    SPCR |= (1 << SPR1);
//};
//
////-----------------------------------------------------------------------------------------------------
//void CSpi::Enable(void)
//{
////    // Slave mode.
////    DDRB  |= (Bit(SPI_MISO) | Bit(DDB1));
//    // разрешим SS.
//    PORTB &= ~Bit(PB1);
//    // разрешим SPI.
//    // разрешим прерывание SPI_STC.
//    SPCR |= ((1 << SPE) | (1 << SPIE));
//    SPDR = 0;
//}
//
////-----------------------------------------------------------------------------------------------------
//void CSpi::Disable(void)
//{
//    // Slave mode.
////    // запретим прерывание SPI_STC.
////    PORTB &= ~(Bit(SPI_MOSI) | Bit(SPI_MISO) | Bit(SPI_SCK));
////    DDRB  &= ~(Bit(SPI_MOSI) | Bit(SPI_MISO) | Bit(SPI_SCK));
//    // запретим SS.
//    PORTB |= Bit(PB1);
//    SPCR &= ~((1 << SPE) | (1 << SPIE));
//}
//
////-----------------------------------------------------------------------------------------------------
//void CSpi::Reset(void)
//{
//    m_nuiBuffByteCounter = 0;
//    m_uiReceivedByteCounter = 0;
//    m_bfByteIsReceived = 0;
//    m_bfByteIsTransmited = 0;
//    m_bfDataExchangeInProgress = 0;
//    m_bfDataExchangeIsOccur = 0;
//    m_bfRxBuffOverflow = 0;
//    m_uiExchangeByte = 0;
//}
//
////-----------------------------------------------------------------------------------------------------
//int16_t CSpi::Exchange(void)
//{
//    m_bfByteIsReceived = 0;
//
//    if (m_bfRxBuffOverflow)
//    {
//        return -1;
//    }
//    if (BUFFER_LENGTH <= m_nuiBuffByteCounter)
//    {
//        return -1;
//    }
//    else if (m_nuiBuffByteCounter)
//    {
////        CPlatform::InterruptDisable();
//
////        *puiDestination = m_uiExchangeByte;
////        m_uiExchangeByte = 0x78;//*puiSourse;
//
//        uint8_t uiCounter = m_nuiBuffByteCounter;
////        m_nuiBuffByteCounter = 0;
//
////        CPlatform::InterruptEnable();
//
//        return uiCounter;
//    }
//    else if (0 == m_nuiBuffByteCounter)
//    {
//        return 0;
//    }
//
//    return 0;
//}
//
////-----------------------------------------------------------------------------------------------------
//uint8_t CSpi::Read(uint8_t *pucRamDestination, uint16_t uiEepromSourse, uint16_t nuiLength)
//{
//
//    return 1;
//}
//
////-----------------------------------------------------------------------------------------------------
//void CSpi::RecvInterruptHandler(void)
//{
////    SPDR = m_uiExchangeByte;
////    m_uiExchangeByte = SPDR;
////    // буфер приёма не переполнен?
////    if (m_nuiBuffByteCounter <
////            SPI_MAX_BUFF_LENGTH)
////    {
////        m_nuiBuffByteCounter++;
////        m_bfByteIsReceived = 1;
////    }
////    else
////    {
////        // не инкрементируем m_nuiBuffByteCounter, чтобы не выйти за границы буфера.
////        // установим флаг - произошел обмен данными по SPI.
////        m_bfRxBuffOverflow = 1;
////        m_bfByteIsReceived = 1;
////    }
//
//
////    SPDR = m_uiExchangeByte;
////    m_puiRxBuffer[m_nuiBuffByteCounter] = SPDR;
////    m_uiExchangeByte = m_puiTxBuffer[m_nuiBuffByteCounter];
////    // буфер приёма не переполнен?
////    if (m_nuiBuffByteCounter <
////            SPI_MAX_BUFF_LENGTH)
////    {
////        m_nuiBuffByteCounter++;
////        m_bfByteIsReceived = 1;
////    }
////    else
////    {
////        // не инкрементируем m_nuiBuffByteCounter, чтобы не выйти за границы буфера.
////        // установим флаг - произошел обмен данными по SPI.
////        m_bfRxBuffOverflow = 1;
////        m_bfByteIsReceived = 1;
////    }
//}

////-----------------------------------------------------------------------------------------------------
//#pragma vector = SPI_STC_vect
//__interrupt void SIG_SPI_STC(void)
//{
////    if (CSpi::m_bfByteIsTransmited)
////    {
////        SPDR = CSpi::m_uiExchangeByte;
////    }
////    else
////    {
////        // Первый байт не передаём, чтобы прошло "эхо".
////        CSpi::m_bfByteIsTransmited = 1;
////        SPDR = SPDR;//0x91;//
////    }
//    SPDR = CSpi::m_uiExchangeByte;
//    CSpi::m_puiRxBuffer[CSpi::m_nuiBuffByteCounter] = SPDR;
//    CSpi::m_uiExchangeByte = CSpi::m_puiTxBuffer[CSpi::m_nuiBuffByteCounter + 1];
//
//    // буфер приёма не переполнен?
//    if (CSpi::m_nuiBuffByteCounter <
//            CSpi::BUFFER_LENGTH)
//    {
//        CSpi::m_nuiBuffByteCounter++;
//        CSpi::m_bfByteIsReceived = 1;
//    }
//    else
//    {
//        // не инкрементируем m_nuiBuffByteCounter, чтобы не выйти за границы буфера.
//        // установим флаг - произошел обмен данными по SPI.
//        CSpi::m_bfRxBuffOverflow = 1;
//        CSpi::m_bfByteIsReceived = 1;
//    }
//}

//-----------------------------------------------------------------------------------------------------





//-----------------------------------------------------------------------------------------------------
//// delay
//void delay_ms(uint16_t millisecs)
//{
//    while(millisecs--)
//    {
//        delay_us(1000);
//    }
//}
//void delay_s(uint16_t secs)
//{
//    while(secs--)
//    {
//        __delay_cycles(F_CPU);
//    }
//}
//void delay_mins(uint16_t minutes)
//{
//    while(minutes--)
//    {
//        __delay_cycles(60*F_CPU);
//    }
//}

//-----------------------------------------------------------------------------------------------------
uint16_t CPlatform::m_uiSystemTick;
CUart* CPlatform::m_pxUart0;
CUart* CPlatform::m_pxUart1;
//CSpi* CPlatform::m_pxSpi;
uint8_t CPlatform::uiSlaveSelectIsHigh;

////-----------------------------------------------------------------------------------------------------
//CPlatform::CPlatform()
//{
//
//}
//
////-----------------------------------------------------------------------------------------------------
//CPlatform::~CPlatform()
//{
//
//}

////-----------------------------------------------------------------------------------------------------
//// INT0
////-----------------------------------------------------------------------------------------------------
//void CPlatform::Int0InterruptEnable(void)
//{
//    // установим прерывание INT0 по переднему фронту(ожидание конца обмена данными по SPI).
//    EICRA &= ~(Bit(ISC01) | Bit(ISC00));
//    EICRA |= (Bit(ISC01) | Bit(ISC00));
//    // разрешение внешнего прерывания INT0.
//    EIMSK |= Bit(INT0);
//}
////-----------------------------------------------------------------------------------------------------
//void CPlatform::Int0InterruptDisable(void)
//{
//    // запретим внешнее прерывание INT0.
//    EIMSK &= ~Bit(INT0);
//}
//
////-----------------------------------------------------------------------------------------------------
//#pragma vector = INT0_vect
//__interrupt void SIG_INT0(void)
//{
//    // прерывание INT0 произошло по переднему фронту(закончен обмен данными по SPI)?
//    if (!(BIT_IS_SET(PIND, SPI_SS5)))
//    {
//        // прерывание INT0 произошло по переднему фронту(закончен обмен данными по SPI).
//        // установим флаг - вход SS0 переходил в 1(запущен демон ПАС).
//        CPlatform::uiSlaveSelectIsHigh = 1;
//        CSpi::DataExchangeInProgressClear();
//        // запретим SPI.
//        CSpi::Disable();
//        // установим прерывание INT0 по переднему фронту(ожидание конца обмена данными по SPI).
//        EICRA &= ~(Bit(ISC01) | Bit(ISC00));
//        EICRA |= (Bit(ISC01) | Bit(ISC00));
//        // установим флаг - произошел обмен данными по SPI.
//        CSpi::DataExchangeIsOccurSet();
//        CMvsn21::FlowControlSet(CMvsn21::FSM_IDDLE);
//        CPlatform::TxLedOff();
//    }
//    else
//    {
//        CSpi::DataExchangeInProgressSet();
//        CSpi::Enable();
//        // установим прерывание INT0 по заднему фронту(ожидание начала обмена данными по SPI).
//        EICRA &= ~(Bit(ISC01) | Bit(ISC00));
//        EICRA |= (Bit(ISC01));
//        CMvsn21::FlowControlSet(CMvsn21::FSM_START);
//        CMvsn21::MeasureFlowControlSet(CMvsn21::FSM_START);
//        CPlatform::TxLedOn();
//    }
//}

//-----------------------------------------------------------------------------------------------------
void SystemTickInit(void)
{
#define MAIN_TIMER_TICK_RATE_HZ 1000

//#define TIMER1_INTERRUPT
#define TIMER2_INTERRUPT


#ifdef TIMER2_INTERRUPT
    /* Hardware constants for timer 2. */
#define portCLEAR_COUNTER_ON_MATCH              ( ( unsigned char ) _BV(WGM21) )
#define portPRESCALE_64                         ( ( unsigned char ) (_BV(CS21) | _BV(CS20)) )
#define portCLOCK_PRESCALER                     ( ( unsigned long ) 64 )
#define portCOMPARE_MATCH_A_INTERRUPT_ENABLE    ( ( unsigned char ) _BV(OCIE2A) )
#endif //TIMER2_INTERRUPT

#ifdef TIMER1_INTERRUPT
    /* Hardware constants for timer 1. */
#define portCLEAR_COUNTER_ON_MATCH              ( ( unsigned char ) _BV(WGM12) )
#define portPRESCALE_64                         ( ( unsigned char ) (_BV(CS11) | _BV(CS10)) )
#define portCLOCK_PRESCALER                     ( ( unsigned long ) 64 )
#define portCOMPARE_MATCH_A_INTERRUPT_ENABLE    ( ( unsigned char ) _BV(OCIE1A) )
#endif //TIMER1_INTERRUPT

#ifdef TIMER2_INTERRUPT

    OCR2A = (((F_CPU / MAIN_TIMER_TICK_RATE_HZ) / 128) - 1);
    TCCR2A |= (Bit(WGM21));
    TCCR2B |= (Bit(CS22) | Bit(CS20));

    TIMSK2 |= Bit(OCIE2A);

#endif //TIMER2_INTERRUPT

#ifdef TIMER1_INTERRUPT

    unsigned long ulCompareMatch;
    unsigned char ucLowByte;

    /* Using 16bit timer 1 to generate the tick.  Correct fuses must be
    selected for the configCPU_CLOCK_HZ clock. */

    ulCompareMatch = CPlatform::F_CPU / MAIN_TIMER_TICK_RATE_HZ;

    /* We only have 16 bits so have to scale to get our required tick rate. */
    ulCompareMatch /= portCLOCK_PRESCALER;

    /* Adjust for correct value. */
    ulCompareMatch -= ( unsigned long ) 1;

    /* Setup compare match value for compare match A.  Interrupts are disabled
    before this is called so we need not worry here. */
    OCR1A = ulCompareMatch;

    /* Setup clock source and compare match behaviour. */
    TCCR1A &= ~(_BV(WGM11) | _BV(WGM10));
    ucLowByte = portCLEAR_COUNTER_ON_MATCH | portPRESCALE_64;
    TCCR1B = ucLowByte;

    /* Enable the interrupt - this is okay as interrupt are currently globally
    disabled. */
    ucLowByte = TIMSK;
    ucLowByte |= portCOMPARE_MATCH_A_INTERRUPT_ENABLE;
    TIMSK = ucLowByte;

#endif //TIMER1_INTERRUPT

}

//-----------------------------------------------------------------------------------------------------
#ifdef TIMER1_INTERRUPT
// Прерывание TIMER1_COMPA_vect
__interrupt void SystemTickInterrupt(void)
{
    CPlatform::IncrementSystemTick();
}
#endif //TIMER1_INTERRUPT
//-----------------------------------------------------------------------------------------------------
#ifdef TIMER2_INTERRUPT
// Прерывание TIMER2_COMP_vect
#pragma vector = TIMER2_COMPA_vect	// Прерывание по совпадению TMR2 с OCR2
__interrupt void SystemTickInterrupt(void)
{
    CPlatform::IncrementSystemTick();
}
#endif //TIMER2_INTERRUPT




//-----------------------------------------------------------------------------------------------------
void CPlatform::Init(void)
{
    SystemTickInit();
//    StatusLedSetPinOutput();
//    TxLedSetPinOutput();
}



//
//        PUSH R16
//        LDS     R16, ??m_uiExchangeByte
//        OUT     0x0F, R16
//        POP R16
//
//        ST      -Y, R31
//          CFI R31 Frame(CFA_Y, -1)
//          CFI CFA_Y Y+1
//        ST      -Y, R30
//          CFI R30 Frame(CFA_Y, -2)
//          CFI CFA_Y Y+2
//        ST      -Y, R19
//          CFI R19 Frame(CFA_Y, -3)
//          CFI CFA_Y Y+3
//        ST      -Y, R18
//          CFI R18 Frame(CFA_Y, -4)
//          CFI CFA_Y Y+4
//        ST      -Y, R17
//          CFI R17 Frame(CFA_Y, -5)
//          CFI CFA_Y Y+5
//        ST      -Y, R16
//          CFI R16 Frame(CFA_Y, -6)
//          CFI CFA_Y Y+6
//        IN      R19, 0x3F
//;        LDS     R16, ??m_uiExchangeByte
//;        OUT     0x2E, R16
//        IN      R16, 0x2E
//        STS     ??m_uiExchangeByte, R16
//        LDS     R16, ??m_nuiBuffByteCounter
//        LDS     R17, (??m_nuiBuffByteCounter + 1)
//        CPI     R16, 0
//        LDI     R18, 1
//        CPC     R17, R18
//        BRCC    ??SIG_SPI_STC_1
//        LDI     R30, LOW(??m_nuiBuffByteCounter)
//        LDI     R31, (??m_nuiBuffByteCounter) >> 8
//        LD      R16, Z
//        LDD     R17, Z+1
//        SUBI    R16, 255
//        SBCI    R17, 255
//        ST      Z, R16
//        STD     Z+1, R17
//        LDI     R16, 1
//        STS     ??m_bfByteIsReceived, R16
//        RJMP    ??SIG_SPI_STC_2
//??SIG_SPI_STC_1:
//        LDI     R16, 1
//        STS     ??m_bfRxBuffOverflow, R16
//        LDI     R16, 1
//        STS     ??m_bfByteIsReceived, R16
//??SIG_SPI_STC_2:
//        OUT     0x3F, R19
//        LD      R16, Y+
//          CFI R16 SameValue
//          CFI CFA_Y Y+5
//        LD      R17, Y+
//          CFI R17 SameValue
//          CFI CFA_Y Y+4
//        LD      R18, Y+
//          CFI R18 SameValue
//          CFI CFA_Y Y+3
//        LD      R19, Y+
//          CFI R19 SameValue
//          CFI CFA_Y Y+2
//        LD      R30, Y+
//          CFI R30 SameValue
//          CFI CFA_Y Y+1
//        LD      R31, Y+
//          CFI R31 SameValue
//          CFI CFA_Y Y+0
//        RETI
