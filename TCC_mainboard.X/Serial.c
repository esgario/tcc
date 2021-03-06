#include "Serial.h"
#include <xc.h>

Fila BufferWrite, BufferRead;

void Serial_9600_Init(char osc_freq)
{
    TXSTA = 0b00100100; // (2) BRGH 1 - HIGH SPEED | 0 - LOW SPEED
    RCSTA = 0b10010000;
    switch(osc_freq)
    {
        case 10:
            SPBRG = 64; // BRGH 1 SPBRG 64 | BRGH 0 SPBRG 15
        break;
        case 16:
            SPBRG = 103;
            break;
        case 20:
            SPBRG = 129;
            break;
    }
    //PIE1bits.TXIE = 1;
    PIE1bits.RCIE = 1; // interrup��o

    BufferWrite.primeiro = 0;
    BufferWrite.ultimo = 0;
    BufferWrite.nItens = 0;
    BufferRead.primeiro = 0;
    BufferRead.ultimo = 0;
    BufferRead.nItens = 0;

    TXREG = 0x00;
}

void Serial_Interrupt()
{
    if (PIR1bits.TXIF == 1 && TXSTAbits.TRMT == 1)
    {
        PIR1bits.TXIF = 0;
        if(!estaVazia(&BufferWrite))
        {
            TXREG = remover(&BufferWrite);
        }
    }

    if(RCSTAbits.OERR == 1)
    {
        char tmp = RCREG;
        tmp = RCREG;
        RCSTAbits.CREN = 0;
        RCSTAbits.CREN = 1;
    }

    if (PIR1bits.RCIF == 1)
    {
        //PIR1bits.RCIF = 0; //limpa sozinho quando l�
        if(!estaCheia(&BufferRead))
            inserir(&BufferRead, RCREG);
    }
}

void Serial_Write(char Valor)
{
    if(!estaCheia(&BufferWrite))
        inserir(&BufferWrite, Valor);
}

char Serial_Read()
{
    if(!estaVazia(&BufferRead))
        return remover(&BufferRead);
    else
        return 0;
}

char Serial_Available()
{
    return (!estaVazia(&BufferRead));
}

void Serial_Flush()
{
    BufferRead.primeiro = 0;
    BufferRead.ultimo = 0;
    BufferRead.nItens = 0;
}

//==================== Fila Circular =========================

void inserir(Fila *buf,char Valor)
{
    buf->dados[buf->ultimo] = Valor;
    buf->ultimo++;
    buf->nItens++;

    if(buf->ultimo == TAMANHO)
        buf->ultimo = 0;
}

char remover(Fila *buf)
{
    int temp = buf->dados[buf->primeiro++];

    if(buf->primeiro == TAMANHO)
        buf->primeiro = 0;

    buf->nItens--;
    return temp;
}

char estaVazia(Fila *buf)
{
    return (buf->nItens==0);
}

char estaCheia(Fila *buf)
{
    return (buf->nItens==TAMANHO);
}