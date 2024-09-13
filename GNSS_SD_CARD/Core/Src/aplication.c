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
#include "fatfs.h"


displayConfig Display;
GNSS gnss = {.numeroSatelitesStr[0] = '0', .numeroSatelitesStr[1] = '0',
		.data={	.horaUTCStr[0] = '0',	.horaUTCStr[1] = '0',
				.minutoStr[0] = '0',	.minutoStr[1] = '0',
				.segundoStr[0] = '0',	.segundoStr[1] = '0'}};
UART_HandleTypeDef huart1;


void initDisplay(displayConfig *disp){

	if(disp->configurado != NONE){
		SSD1306_Init();
		if(disp->configurado == VELOCIDADE){
			SSD1306_GotoXY (0,0);
			SSD1306_Puts ("Velocidade", &Font_11x18, 1);
			SSD1306_GotoXY (80, 36);
			SSD1306_Puts ("km/h", &Font_11x18, 1);

		}else if(disp->configurado == INFO){

		}else if(disp->configurado == HORA){
			SSD1306_GotoXY (0,0);
			SSD1306_Puts ("Hora", &Font_11x18, 1);
		}
		SSD1306_UpdateScreen();
	}
}

void updateDisplay(displayConfig *disp, GNSS *gn){
	if(disp->configurado == VELOCIDADE){
		updateDisplayVelocidade(disp, gn);
	}else if(disp->configurado == INFO){

	}else if(disp->configurado == HORA){
		updateDisplayHora(disp, gn);
	}
}

void updateDisplayNumeroSatelites(displayConfig *disp, GNSS *gn){
	SSD1306_GotoXY (113, 0);
	SSD1306_Putc(gn->numeroSatelitesStr[0], &Font_7x10, 1);
	SSD1306_Putc(gn->numeroSatelitesStr[1], &Font_7x10, 1);
}

void updateDisplayHora(displayConfig *disp, GNSS *gn){

	updateDisplayNumeroSatelites(disp, gn);

	SSD1306_GotoXY (0, 30);
	SSD1306_Putc(gn->data.horaUTCStr[0], &Font_16x26, 1);
	SSD1306_Putc(gn->data.horaUTCStr[1], &Font_16x26, 1);
	SSD1306_Putc(':', &Font_11x18, 1);
	SSD1306_Putc(gn->data.minutoStr[0], &Font_16x26, 1);
	SSD1306_Putc(gn->data.minutoStr[1], &Font_16x26, 1);
	SSD1306_Putc(':', &Font_11x18, 1);
	SSD1306_Putc(gn->data.segundoStr[0], &Font_16x26, 1);
	SSD1306_Putc(gn->data.segundoStr[1], &Font_16x26, 1);
	SSD1306_UpdateScreen();
}




void updateDisplayVelocidade(displayConfig *disp, GNSS *gn){
	char _snum[4];

	updateDisplayNumeroSatelites(disp, gn);
	itoa(gn->velocidade.velocidade8bits, _snum, 10);
	SSD1306_GotoXY (25, 30);
	if(gn->velocidade.velocidade8bits < 10) {// 1 DIGIT

		_snum[3] = 0;
		_snum[2] = _snum[0];
		_snum[1] = ' ';
		_snum[0] = ' ';

	}
	else if (gn->velocidade.velocidade8bits < 100 ) {// 2 DIGITS
		_snum[3] = 0;
		_snum[2] = _snum[1];
		_snum[1] = _snum[0];
		_snum[0] = ' ';

	}
	else if (gn->velocidade.velocidade8bits < 1000 ) {// 3 DIGITS
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

float converte4Bytes2float(uint8_t *str){

	if(*(str+1) == '.'){//x.xx
		return (((*(str+3)-'0')+10*((*(str+2)-'0'))+100*((*(str)-'0')))/100.0);
	}else if(*(str+2) == '.'){//xx.x
		return (((*(str+3)-'0')+10*((*(str+1)-'0'))+100*((*(str)-'0')))/10.0);
	}else{//xxx.
		return (((*(str+2)-'0')+10*((*(str+1)-'0'))+100*((*(str)-'0'))));
	}
}

uint8_t converte2Bytes2uint8(uint8_t *str){

	return ((*str)-'0')*10+(*(str+1)-'0');

}

uint16_t converte4Bytes2uint16(uint8_t *str){

	return ((*str)-'0')*1000+(*(str+1)-'0')*100+(*(str+2)-'0')*10+(*(str+3)-'0');

}


// 30min logando gera um arquivo de 61KB
void GravaNMEASDCard(uint8_t *str, GNSS *gn){
	static FATFS fs;
	static FIL fil;
	uint8_t aux = 0;
	char nomeArquivo[] = {"dd_mm_yyyy.nmea"};

	uint8_t *paux = str;

	while(*str != 0x0d){
		str++;
		aux++;
		if(aux >= 100)return;
	}
	str++;
	*str = 0;

	if(gn->data.diaStr[0] >= '0' && gn->data.diaStr[1] > '0' && gn->data.diaStr[0] <= '3' && gn->data.diaStr[1] <= '9'){// verifica se o dia é valido

		nomeArquivo[0] = gn->data.diaStr[0];
		nomeArquivo[1] = gn->data.diaStr[1];

		nomeArquivo[3] = gn->data.mesStr[0];
		nomeArquivo[4] = gn->data.mesStr[1];

		nomeArquivo[6] = gn->data.anoStr[0];
		nomeArquivo[7] = gn->data.anoStr[1];
		nomeArquivo[8] = gn->data.anoStr[2];
		nomeArquivo[9] = gn->data.anoStr[3];


		f_mount(&fs, "", 0);
		f_open(&fil, &nomeArquivo, FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
		f_lseek(&fil, f_size(&fil));
		f_puts(paux, &fil);
		f_close(&fil);
	}




}

void decodeNMEA(uint8_t *str, GNSS *gn){
	uint8_t *p;
	uint8_t contVirgulas = 0;


//GNZDA OK -> data e hora
//GPTXT
//GNGGA OK -> hora e numero de satelites
//BDGSA
//GPGSV
//GNRMC
//GNVTG OK -> velocidade
//GNGLL
//GPGSA
//BDGSV
	if(str[0] == '$'){
		GravaNMEASDCard(str,gn);
		if(str[1] == 'G'){//G
			if(str[2] == 'N'){//GN
				if(str[3] == 'V'){//GNV
					if(str[4] == 'T'){//GNVT
						if(str[5] == 'G'){//GNVTG
							if(str[6] == ','){//GNVTG
								p = &str[7];
								while(contVirgulas < 6){// velocidade
									if(*p == ','){
										contVirgulas++;
									}
									p++;
								}
								gn->velocidade.velocidadeSTR[0] = *p++;
								gn->velocidade.velocidadeSTR[1] = *p++;
								gn->velocidade.velocidadeSTR[2] = *p++;
								gn->velocidade.velocidadeSTR[3] = *p++;
								gn->velocidade.velocidadeFloat = converte4Bytes2float(gn->velocidade.velocidadeSTR);
								gn->velocidade.velocidade8bits = (uint8_t)gn->velocidade.velocidadeFloat;


							}
						}
					}
				}else if(str[3] == 'Z'){//GNZ
					if(str[4] == 'D'){//GNZD
						if(str[5] == 'A'){//GNZDA
							if(str[6] == ','){//GNZDA
								p = &str[7];

								gn->data.horaUTCStr[0] = *p++;
								gn->data.horaUTCStr[1] = *p++;
								gn->data.minutoStr[0] = *p++;
								gn->data.minutoStr[1] = *p++;
								gn->data.segundoStr[0] = *p++;
								gn->data.segundoStr[1] = *p++;
								p++;//.
								p++;//s
								p++;//s
								p++;//s
								p++;//,
								gn->data.diaStr[0] = *p++;
								gn->data.diaStr[1] = *p++;
								p++;//,
								gn->data.mesStr[0] = *p++;
								gn->data.mesStr[1] = *p++;
								p++;//,
								gn->data.anoStr[0] = *p++;
								gn->data.anoStr[1] = *p++;
								gn->data.anoStr[2] = *p++;
								gn->data.anoStr[3] = *p++;

							}
						}
					}
				}else if(str[3] == 'G'){//GNG
					if(str[4] == 'G'){//GNGG
						if(str[5] == 'A'){//GNGGA
							if(str[6] == ','){//GNGGA
								p = &str[7];
								gn->data.horaUTCStr[0] = *p++;
								gn->data.horaUTCStr[1] = *p++;
								gn->data.minutoStr[0] = *p++;
								gn->data.minutoStr[1] = *p++;
								gn->data.segundoStr[0] = *p++;
								gn->data.segundoStr[1] = *p++;

								while(contVirgulas < 6){// numero de satelites
									if(*p == ','){
										contVirgulas++;
									}
									p++;
								}
								gn->numeroSatelitesStr[0] = *p++;
								gn->numeroSatelitesStr[1] = *p++;

								//TODO: altitude



							}
						}
					}
				}
			}
		}
	}
	str[0] = 0;// ja foi lido


}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){

	HAL_UART_Receive_IT(&huart1, gnss.temp, 1);
	gnss.RxDataSerial[gnss.indy][gnss.indx] = gnss.temp[0];
	gnss.indx++;

	if (gnss.indx >= 100 || gnss.temp[0] == '\n'){
		//decodeNMEA(&gnss.RxDataSerial[gnss.indy][0], &gnss);

		gnss.indx = 0;
		gnss.indy++;
		if(gnss.indy >= 13)gnss.indy = 0;
	}

}
