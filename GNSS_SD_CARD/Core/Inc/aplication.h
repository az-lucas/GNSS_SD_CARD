/*
 * aplication.h
 *
 *  Created on: Sep 7, 2024
 *      Author: lucas
 */

#ifndef INC_APLICATION_H_
#define INC_APLICATION_H_


typedef enum{
	NONE,
	VELOCIDADE,
	INFO,
	HORA,
	DATA
}tipoDisplay;


typedef struct{
	tipoDisplay configurado;
	uint8_t velocidade;

}displayConfig;

typedef struct{
	uint8_t RxDataSerial[13][100];
	uint8_t temp[2];
	uint8_t indx,indy;
	struct {
		float velocidadeFloat;
		uint8_t velocidade8bits;
		uint8_t velocidadeSTR[4];
	}velocidade;
	struct{
		uint8_t horaUTC, horaBR;
		uint8_t minuto;
		uint8_t segundo;
		uint8_t dia;
		uint8_t mes;
		uint16_t ano;

		uint8_t horaUTCStr[2], horaBRStr[2];
		uint8_t minutoStr[2];
		uint8_t segundoStr[2];
		uint8_t diaStr[2];
		uint8_t mesStr[2];
		uint16_t anoStr[4];
	}data;

	uint8_t numeroSatelitesStr[2];


}GNSS;

extern displayConfig Display;
extern GNSS gnss;
extern UART_HandleTypeDef huart1;

void initDisplay(displayConfig *disp);
void updateDisplay(displayConfig *disp, GNSS *gn);
void updateDisplayNumeroSatelites(displayConfig *disp, GNSS *gn);
void updateDisplayVelocidade(displayConfig *disp, GNSS *gn);
void updateDisplayHora(displayConfig *disp, GNSS *gn);

void decodeNMEA(uint8_t *str, GNSS *gn);
float converte4Bytes2float(uint8_t *str);
uint8_t converte2Bytes2uint8(uint8_t *str);
uint16_t converte4Bytes2uint16(uint8_t *str);

void GravaNMEASDCard(uint8_t *str);



#endif /* INC_APLICATION_H_ */
