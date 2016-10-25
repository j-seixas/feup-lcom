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
#define OBF   				BIT(0)

// Keyboard Commands
#define KB_MENU             0xF1
#define KB_ENABLE           0xF4
#define KB_MAKEBREAK        0xF7
#define KB_ECHO             0xFE
#define KB_RESET            0xFF
#define KB_LED_CMD          0xED


// Keyboard responses
#define KB_OK               0xAA
#define KB_ACK              0xFA
#define KB_OVERRUN          0xFF
#define KB_RESEND           0xFE
#define KB_BREAK            0xF0
#define KB_FA               0x10
#define KB_FE               0x20
#define KB_PR_LED           0x40


#define ESC_BREAK			0x81
#define TWO_BYTES			0xE0



#define KB_IRQ   1

#endif /* _LCOM_I8042_H */
