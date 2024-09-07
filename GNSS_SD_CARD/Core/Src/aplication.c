/*
 * aplication.c
 *
 *  Created on: Sep 7, 2024
 *      Author: lucas
 */
#include "stm32f1xx_hal.h"
#include "ssd1306.h"
#include "fonts.h"
#include "stdlib.h"
#include "string.h"
#include "aplication.h"


displayConfig Display;

void initDisplay(displayConfig *disp){

	if(disp->configurado != NONE){
		SSD1306_Init();
		if(disp->configurado == VELOCIDADE){
			SSD1306_GotoXY (10,0);
			SSD1306_Puts ("Velocidade", &Font_11x18, 1);
			SSD1306_GotoXY (80, 36);
			SSD1306_Puts ("km/h", &Font_11x18, 1);
			SSD1306_UpdateScreen();
		}else if(disp->configurado == INFO){

		}
	}
}

void updateDisplay(displayConfig *disp){
	if(disp->configurado == VELOCIDADE){
		updateDisplayVelocidade(disp);
	}else if(disp->configurado == INFO){

	}
}

void updateDisplayVelocidade(displayConfig *disp){
	char _snum[4];
	itoa(disp->velocidade, _snum, 10);

	SSD1306_GotoXY (25, 30);
	if(disp->velocidade < 10) {// 1 DIGIT

		_snum[3] = 0;
		_snum[2] = _snum[0];
		_snum[1] = ' ';
		_snum[0] = ' ';

	}
	else if (disp->velocidade < 100 ) {// 2 DIGITS
		_snum[3] = 0;
		_snum[2] = _snum[1];
		_snum[1] = _snum[0];
		_snum[0] = ' ';

	}
	else if (disp->velocidade < 1000 ) {// 3 DIGITS
		_snum[3] = 0;
		_snum[2] = _snum[2];
		_snum[1] = _snum[1];
		_snum[0] = _snum[0];

	}
	else {
		SSD1306_GotoXY (30, 30);  // 4 DIGIS
	}

	SSD1306_Puts (_snum, &Font_16x26, 1);
	SSD1306_UpdateScreen();

}
