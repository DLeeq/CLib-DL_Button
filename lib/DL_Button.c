#include "DL_Button.h"

void DL_btnInit(DL_Button* button, GPIO_TypeDef* port, uint16_t pin, DL_BtnLevel active_level, void (*handler)(DL_BtnEvent))  //Инициализатор
{
	button->port = port;  //Порт подключения кнопки
	button->pin = pin;  //Пин подключения кнопки

	button->last_state = HAL_GPIO_ReadPin(port, pin);  //Последнее состояние кнопки

	button->active_level = active_level;  //Какой уровень кнопки считается активным

	button->handler = handler;  //Обработчик событий кнопки

	button->debounce_timer = 0;  //Таймер антидребезга
	button->dbc_timer = 0;  //Таймер двойного нажатия
	button->hold_timer = 0;  //Таймер удержания кнопки

	button->front_flag = 0;  //Флаг изменения состояния
	button->hold_flag = 0;  //Флаг удержания кнопки
}

void DL_btnTick(DL_Button* button)  //Тикер для ловли событий
{
	uint8_t state = HAL_GPIO_ReadPin(button->port, button->pin);  //Текущее состояние кнопки

	if(state != button->last_state)  //Если состояние изменилось
	{
		button->last_state = state;  //Заменить предыдущее состояние новым
		button->debounce_timer = HAL_GetTick();  //Сбросить таймер антидребезга
		button->front_flag = 1;  //Разрешение отловить фронт
	}
	else if(HAL_GetTick() - button->debounce_timer > 5)  //Если изменений состояния не было больше 5мс
	{
		if(state == button->active_level)  //Если состояние активное
		{
			button->handler(DL_BTN_PRESSING);  //Вызов обработчика с событием нажатия

			if(button->front_flag)  //Есди фронт
			{
				button->front_flag = 0;  //Сброс флага ловли фронта

				button->handler(DL_BTN_PRESS);  //Вызов обработчика с событием однократного нажатия
				button->handler(DL_BTN_TOGGLE); //Вызов обработчика с событием изменения

				if(HAL_GetTick() - button->dbc_timer < 250)  //Если с прошлого нажатия прошло меньше 250мс
					button->handler(DL_BTN_DOUBLECLICK);  //Вызов обработчика с событием двойного нажатия
				button->dbc_timer = HAL_GetTick();  //сброс таймера двойного нажатия

				button->hold_flag = 1;  //Разрешить удержание кнопки
				button->hold_timer = HAL_GetTick();  //Сброс таймера удержания кнопки
			}

			if(HAL_GetTick() - button->hold_timer > 500 && button->hold_flag)  //Если кнопка нажата больше 500мс и удержание разрешено
			{
				button->hold_flag = 0;  //Запретить удержание
				button->handler(DL_BTN_HOLD);  //Вызов обработчика с событием удержания кнопки
			}
		}
		else  //Иначе, если состояние не активное
		{
			button->handler(DL_BTN_RELEASING);  //Вызов обработчика с событием отпущенной кнопки
			if(button->front_flag)  //Если фронт
			{
				button->front_flag = 0;  //Сброс флага ловли фронта

				button->handler(DL_BTN_RELEASE); //Вызов обработчика с событием отпускания кнопки
				button->handler(DL_BTN_TOGGLE); //Вызов обработчика с событием изменения состояния кнопки
			}
		}
	}
}

