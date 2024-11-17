#ifndef DL_BUTTON_H
#define DL_BUTTON_H

#include "stm32f1xx_hal.h"  //Подключение библиотеки HAL, соответствующей вашему МК

typedef enum {DL_BTN_PRESS, DL_BTN_RELEASE, DL_BTN_PRESSING, DL_BTN_RELEASING, DL_BTN_TOGGLE,
			  DL_BTN_DOUBLECLICK, DL_BTN_HOLD} DL_BtnEvent;  //Перечисление событий кнопки

typedef enum {DL_BTN_LOW, DL_BTN_HIGH} DL_BtnLevel;  //Перечисление уровней кнопки

typedef struct
{
	GPIO_TypeDef* port;  //На каком порту кнопка
	uint16_t pin;  //Пин подключения кнопки

	uint8_t last_state : 1;  //Предыдущее состояние кнопки

	uint8_t active_level : 1;  //Тип активного уровня кнопки

	uint8_t front_flag : 1;  //Флаг изменения состояния
	uint8_t hold_flag : 1;  //Флаг удержания кнопки

	void (*handler)(DL_BtnEvent);  //Обработчик событий кнопки

	uint32_t debounce_timer;  //Таймер антидребезга
	uint32_t dbc_timer;  //Таймер двойного нажатия
	uint32_t hold_timer;  //Таймер удержания
} DL_Button;

void DL_btnInit(DL_Button* button, GPIO_TypeDef* port, uint16_t pin, DL_BtnLevel active_level, void (*handler)(DL_BtnEvent)); //Функция инициализации кнопки
void DL_btnTick(DL_Button* button);  //Функция-Тикер для отслеживания событий

#endif
