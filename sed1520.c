#include <xc.h>
#include "sed1520.h"

#define _XTAL_FREQ 125000

#define A0 RA2
#define E1 RA5
#define E2 RA4
#define DATAPORT PORTC

char currentPage = 0;       // データ送信先の判断とページインクリメントの
char currentColumn = 0;     // ために現在のアドレスを保持

void Sed_Cmd(char c){       // コマンドは両方の制御ICに送信
    A0 = 0;
    DATAPORT = c;
    E1 = 1;
    E2 = 1;
    E1 = 0;
    E2 = 0;
    __delay_us(40);
}

void Sed_Dat(char c){
    A0 = 1;
    DATAPORT = c;
    
    if(currentColumn < 61){     // 送信先判断
        E1 = 1;
        E1 = 0;
    }
    else{
        E2 = 1;
        E2 = 0;
    }
    
    currentColumn++;            // コラム自動インクリメント
    if(currentColumn == 122){   // ページ手動インクリメント
        currentColumn = 0;
        currentPage = (currentPage + 1) % 4;
        
        Sed_SetAddress(currentPage, currentColumn);
    }
        __delay_us(40);
}

void Sed_SetAddress(char page, char column){
    currentPage = page;
    currentColumn = column;
    
    Sed_Cmd(0b10111000 + page); // 両方ともページセット
    
    if(column < 61){            // 前半の場合は後半のコラムを0にするのも
        DATAPORT = column;
        E1 = 1;
        E1 = 0;
        
        DATAPORT = 0;
        E2 = 1;
        E2 = 0;
    }
    else{
        DATAPORT = column-61;
        E2 = 1;
        E2 = 0;
    }
}

void Sed_Init(){
    Sed_Cmd(0b10101111);  //Display ON
    Sed_SetAddress(0,0);  //page 0, column 0
}