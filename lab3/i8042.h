#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_


#define BIT(n) (0x01<<(n))

#define STATUS_PORT         0x64
#define KBD_OUT_BUF         0x60
#define KBD_INPT_BUF        0x60
#define KBD_CMD_BUF         0x64
#define KBD_DATA_BUF        0x60
#define PORT_A              0x60
#define PORT_B              0x61
#define OUT_BUF_FULL        0x01
#define INPT_BUF_FULL       0x02
#define SYS_FLAG            0x04
#define CMD_DATA            0x08
#define KEYBD_INH           0x10
#define TRANS_TMOUT         0x20
#define RCV_TMOUT           0x40
#define PARITY_EVEN         0x80
#define INH_KEYBOARD        0x10
#define KBD_ENA             0xAE
#define KBD_DIS             0xAD


// Keyboard Commands
#define KB_MENU         EQU     0F1H
#define KB_ENABLE       EQU     0F4H
#define KB_MAKEBREAK    EQU     0F7H
#define KB_ECHO         EQU     0FEH
#define KB_RESET        EQU     0FFH
#define KB_LED_CMD      EQU     0EDH


// Keyboard responses
#define KB_OK           EQU     0AAH
#define KB_ACK          EQU     0FAH
#define KB_OVERRUN      EQU     0FFH
#define KB_RESEND       EQU     0FEH
#define KB_BREAK        EQU     0F0H
#define KB_FA           EQU     010H
#define KB_FE           EQU     020H
#define KB_PR_LED       EQU     040H



#define KB_IRQ   1

#endif /* _LCOM_I8042_H */
