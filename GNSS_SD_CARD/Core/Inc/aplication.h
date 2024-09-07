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
	INFO
}tipoDisplay;


typedef struct{
	tipoDisplay configurado;
	uint8_t velocidade;

}displayConfig;

extern displayConfig Display;


void initDisplay(displayConfig *disp);
void updateDisplay(displayConfig *disp);
void updateDisplayVelocidade(displayConfig *disp);



#endif /* INC_APLICATION_H_ */
