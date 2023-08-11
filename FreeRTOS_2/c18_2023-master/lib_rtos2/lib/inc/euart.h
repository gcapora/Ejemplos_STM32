/*
 * euart.h
 *
 *  Created on: Feb 15, 2023
 *      Author: seb
 */

#ifndef INC_EUART_H_
#define INC_EUART_H_

#include "eringbuffer.h"

#define EUART_HAL_BUFFER_SIZE           (16)

typedef struct
{
    eringbuffer_t rb;
    uint8_t pbuffer[EUART_HAL_BUFFER_SIZE];
} euart_buffer_t;

typedef struct
{
    void* phardware_handle;
    bool tx_free;
    euart_buffer_t tx;
    euart_buffer_t rx;
} euart_t;

void euart_hal_receive(void* phardware_handle, uint8_t* pbuffer, size_t size);

void euart_hal_send(void* phardware_handle, uint8_t* pbuffer, size_t size);

void euart_init(euart_t *phandle, void* phardware_handle, uint8_t* ptx_buffer, size_t tx_buffer_size, uint8_t* prx_buffer, size_t rx_buffer_size);

size_t euart_write_buffer_len(euart_t *phandle);

size_t euart_write(euart_t *phandle, const uint8_t *buffer, size_t size);

size_t euart_read_buffer_len(euart_t *phandle);

size_t euart_read(euart_t *phandle, uint8_t *buffer, size_t size);

void euart_rx_irq(euart_t *phandle, void *phardware_handle, size_t size);

void euart_tx_irq(euart_t *phandle, void *phardware_handle);

#endif /* INC_EUART_H_ */
