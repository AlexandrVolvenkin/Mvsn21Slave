
    NAME SpiInterrupt
#include <ioavr.h>

    PUBLIC SpiInterrupt

    RSEG   CODE

SpiInterrupt:

`?<Name CSpi 0>`  SYMBOL "CSpi"
??m_puiRxBuffer SYMBOL "m_puiRxBuffer", `?<Name CSpi 0>`
??m_puiTxBuffer SYMBOL "m_puiTxBuffer", `?<Name CSpi 0>`
??m_nuiBuffByteCounter SYMBOL "m_nuiBuffByteCounter", `?<Name CSpi 0>`
??m_uiReceivedByteCounter SYMBOL "m_uiReceivedByteCounter", `?<Name CSpi 0>`
??m_uiExchangeByte       SYMBOL "m_uiExchangeByte", `?<Name CSpi 0>`
??m_bfByteIsReceived       SYMBOL "m_bfByteIsReceived", `?<Name CSpi 0>`
??m_bfRxBuffOverflow       SYMBOL "m_bfRxBuffOverflow", `?<Name CSpi 0>`
SIG_SPI_STC         SYMBOL "SIG_SPI_STC"
`??SIG_SPI_STC::??INTVEC 68` SYMBOL "??INTVEC 68", SIG_SPI_STC

        EXTERN ??m_puiRxBuffer
        EXTERN ??m_puiTxBuffer
        EXTERN ??m_nuiBuffByteCounter
        EXTERN ??m_uiReceivedByteCounter
        EXTERN ??m_uiExchangeByte
        EXTERN ??m_bfByteIsReceived
        EXTERN ??m_bfRxBuffOverflow

        PUSH R16
        LDS     R16, ??m_uiExchangeByte
        OUT     0x0F, R16
        POP R16

        ST      -Y, R31
        ST      -Y, R30
        ST      -Y, R20
        ST      -Y, R19
        ST      -Y, R18
        ST      -Y, R17
        ST      -Y, R16
        IN      R20, 0x3F
;//  589     SPDR = CSpi::m_uiExchangeByte;
;        LDS     R16, ??m_uiExchangeByte
;        OUT     0x2E, R16
//  590     CSpi::m_puiRxBuffer[CSpi::m_nuiBuffByteCounter] = SPDR;
        IN      R16, 0x2E
        LDS     R18, ??m_nuiBuffByteCounter
        LDS     R19, (??m_nuiBuffByteCounter + 1)
        LDS     R30, ??m_puiRxBuffer
        LDS     R31, (??m_puiRxBuffer + 1)
        ADD     R30, R18
        ADC     R31, R19
        ST      Z, R16
//  591     CSpi::m_uiExchangeByte = CSpi::m_puiTxBuffer[CSpi::m_nuiBuffByteCounter];
        LDS     R16, ??m_nuiBuffByteCounter
        LDS     R17, (??m_nuiBuffByteCounter + 1)
        LDS     R30, ??m_puiTxBuffer
        LDS     R31, (??m_puiTxBuffer + 1)
        ADD     R30, R16
        ADC     R31, R17
        LD      R16, Z
        STS     ??m_uiExchangeByte, R16
//  592     // буфер приёма не переполнен?
//  593     if (CSpi::m_nuiBuffByteCounter <
//  594             SPI_MAX_BUFF_LENGTH)
        LDS     R16, ??m_nuiBuffByteCounter
        LDS     R17, (??m_nuiBuffByteCounter + 1)
        CPI     R16, 0
        LDI     R18, 1
        CPC     R17, R18
        BRCC    ??SIG_SPI_STC_1
//  595     {
//  596         CSpi::m_nuiBuffByteCounter++;
        LDI     R30, LOW(??m_nuiBuffByteCounter)
        LDI     R31, (??m_nuiBuffByteCounter) >> 8
        LD      R16, Z
        LDD     R17, Z+1
        SUBI    R16, 255
        SBCI    R17, 255
        ST      Z, R16
        STD     Z+1, R17
//  597         CSpi::m_bfByteIsReceived = 1;
        LDI     R16, 1
        STS     ??m_bfByteIsReceived, R16
        RJMP    ??SIG_SPI_STC_2
//  598     }
//  599     else
//  600     {
//  601         // не инкрементируем m_nuiBuffByteCounter, чтобы не выйти за границы буфера.
//  602         // установим флаг - произошел обмен данными по SPI.
//  603         CSpi::m_bfRxBuffOverflow = 1;
??SIG_SPI_STC_1:
        LDI     R16, 1
        STS     ??m_bfRxBuffOverflow, R16
//  604         CSpi::m_bfByteIsReceived = 1;
        LDI     R16, 1
        STS     ??m_bfByteIsReceived, R16
//  605     }
//  606 }
??SIG_SPI_STC_2:
        OUT     0x3F, R20
        LD      R16, Y+
        LD      R17, Y+
        LD      R18, Y+
        LD      R19, Y+
        LD      R20, Y+
        LD      R30, Y+
        LD      R31, Y+
        RETI


COMMON INTVEC(1)            ; Code in interrupt vector segment

ORG SPI_STC_vect               ; Place code at interrupt vector
    JMP   SpiInterrupt           ; Jump to assembler interrupt function

ENDMOD
END
