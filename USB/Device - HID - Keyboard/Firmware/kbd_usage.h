/* 
 * File:   kbd_usage.h
 *
 * Created on 2013/09/11, 18:48
 */

#ifndef KBD_USAGE_H
#define	KBD_USAGE_H

#ifdef	__cplusplus
extern "C" {
#endif

    #define  KEY_NONE             0
    #define  KEY_A                4
    #define  KEY_B                5
    #define  KEY_C                6
    #define  KEY_D                7
    #define  KEY_E                8
    #define  KEY_F                9
    #define  KEY_G                10
    #define  KEY_H                11
    #define  KEY_I                12
    #define  KEY_J                13
    #define  KEY_K                14
    #define  KEY_L                15
    #define  KEY_M                16
    #define  KEY_N                17
    #define  KEY_O                18
    #define  KEY_P                19
    #define  KEY_Q                20
    #define  KEY_R                21
    #define  KEY_S                22
    #define  KEY_T                23
    #define  KEY_U                24
    #define  KEY_V                25
    #define  KEY_W                26
    #define  KEY_X                27
    #define  KEY_Y                28
    #define  KEY_Z                29
    #define  KEY_1                30
    #define  KEY_2                31
    #define  KEY_3                32
    #define  KEY_4                33
    #define  KEY_5                34
    #define  KEY_6                35
    #define  KEY_7                36
    #define  KEY_8                37
    #define  KEY_9                38
    #define  KEY_0                39
    #define  KEY_ENTER            40
    #define  KEY_ESCAPE           41
    #define  KEY_BACKSPACE        42
    #define  KEY_TAB              43
    #define  KEY_SPACEBAR         44
    #define  KEY_UNDERSCORE       45
    #define  KEY_PLUS             46
//  #define  KEY_[ {              47
//  #define  KEY_] }              48
    #define  KEY_BACKSLASH        49
//  #define  KEY_# ~              50
//  #define  KEY_; :              51
//  #define  KEY_? "              52
    #define  KEY_TILDE            53
    #define  KEY_COMMA            54
    #define  KEY_DOT              55
    #define  KEY_SLASH            56
    #define  KEY_CAPSLOCK         57
    #define  KEY_F1               58
    #define  KEY_F2               59
    #define  KEY_F3               60
    #define  KEY_F4               61
    #define  KEY_F5               62
    #define  KEY_F6               63
    #define  KEY_F7               64
    #define  KEY_F8               65
    #define  KEY_F9               66
    #define  KEY_F10              67
    #define  KEY_F11              68
    #define  KEY_F12              69
    #define  KEY_PRINTSCREEN      70
    #define  KEY_SCROLL LOCK      71
    #define  KEY_PAUSE            72
    #define  KEY_INSERT           73
    #define  KEY_HOME             74
    #define  KEY_PAGEUP           75
    #define  KEY_DELETE           76
    #define  KEY_END              77
    #define  KEY_PAGEDOWN         78
    #define  KEY_RIGHT            79
    #define  KEY_LEFT             80
    #define  KEY_DOWN             81
    #define  KEY_UP               82
    #define  KEY_KEYPAD_NUM_LOCK  83
    #define  KEY_KEYPAD_DIVIDE    84
    #define  KEY_KEYPAD_AT        85
    #define  KEY_KEYPAD_MULTIPLY  85
    #define  KEY_KEYPAD_MINUS     86
    #define  KEY_KEYPAD_PLUS      87
    #define  KEY_KEYPAD_ENTER     88
    #define  KEY_KEYPAD_1         89
    #define  KEY_KEYPAD_2         90
    #define  KEY_KEYPAD_3         91
    #define  KEY_KEYPAD_4         92
    #define  KEY_KEYPAD_5         93
    #define  KEY_KEYPAD_6         94
    #define  KEY_KEYPAD_7         95
    #define  KEY_KEYPAD_8         96
    #define  KEY_KEYPAD_9         97
    #define  KEY_KEYPAD_0         98
    #define  KEY_VOLUME_MUTE     127
    #define  KEY_VOLUME_UP       128
    #define  KEY_VOLUME_DOWN     129

    #define  KEY_MODIFIER_NONE          0x00
    #define  KEY_MODIFIER_LEFT_CTRL     0x01
    #define  KEY_MODIFIER_LEFT_SHIFT    0x02
    #define  KEY_MODIFIER_LEFT_ALT      0x04
    #define  KEY_MODIFIER_LEFT_GUI      0x08
    #define  KEY_MODIFIER_RIGHT_CTRL    0x10
    #define  KEY_MODIFIER_RIGHT_SHIFT   0x20
    #define  KEY_MODIFIER_RIGHT_ALT     0x40
    #define  KEY_MODIFIER_RIGHT_GUI     0x80

#ifdef	__cplusplus
}
#endif

#endif	/* KBD_USAGE_H */

