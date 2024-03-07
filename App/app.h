#ifndef __APP_H
#define __APP_H

#ifdef __cplusplus
extern "C" {
#endif


#include "main.h"


void applicationMain(UART_HandleTypeDef* huart1, UART_HandleTypeDef* huart2, DMA_HandleTypeDef* hdma_usart2_rx);
void applicationErrorHandler();



#ifdef __cplusplus
}
#endif

#endif /* __APP_H */
