#include <driver/uart.h>
#include "hw.h"
#include "pin.h"
#include "com.h"

#define UART_BAUD_RATE 115200

const uart_port_t uart_num = UART_NUM_2;

// Initialize the communication channel.
// Return zero if successful, or non-zero otherwise.
int32_t com_init(void){
    uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    // Configure UART parameters
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));

    // Configure transmit and receieve communication pins
    ESP_ERROR_CHECK(uart_set_pin(uart_num, HW_EX8, HW_EX7, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    
    // Install UART Driver
    ESP_ERROR_CHECK(uart_driver_install(uart_num, UART_HW_FIFO_LEN(PORT_NUM)*2, 0, 0, NULL, 0));
    pin_pullup(HW_EX7, 1);
    return 0;
}

// Free resources used for communication.
// Return zero if successful, or non-zero otherwise.
int32_t com_deinit(void){
    if(uart_is_driver_installed(uart_num)){
        uart_driver_delete(uart_num);
        return 0;
    }
    return 1;
}

// Write data to the communication channel. Does not wait for data.
// *buf: pointer to data buffer
// size: size of data in bytes to write
// Return number of bytes written, or negative number if error.
int32_t com_write(const void *buf, uint32_t size){
    return uart_tx_chars(uart_num, buf, size);
}

// Read data from the communication channel. Does not wait for data.
// *buf: pointer to data buffer
// size: size of data in bytes to read
// Return number of bytes read, or negative number if error.
int32_t com_read(void *buf, uint32_t size){
    return uart_read_bytes(uart_num, buf, size, 0);
}
