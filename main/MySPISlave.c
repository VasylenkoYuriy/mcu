#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "driver/spi_slave.h"
#include "driver/gpio.h"


#include <stdbool.h>

#include "esp_heap_caps.h"

#define GPIO_MOSI 13
#define GPIO_MISO 12
#define GPIO_SCLK 14
#define GPIO_CS 21//15
#define RCV_HOST    HSPI_HOST

spi_slave_transaction_t t;
spi_slave_transaction_t *loopbuf;

 uint8_t *sendbuf;
 uint8_t *recvbuf;



int n=0;
//Called after a transaction is queued and ready for pickup by master.
void IRAM_ATTR my_post_setup_cb(spi_slave_transaction_t *trans) {
}

//Called after transaction is sent/received. We use this to set the handshake line low.
void IRAM_ATTR my_post_trans_cb(spi_slave_transaction_t *trans) {
 }

//Main application
static void IRAM_ATTR  My_SPI_Slave_task(void *pvParameters){

     sendbuf = heap_caps_calloc(1,1500, MALLOC_CAP_DMA);
     recvbuf = heap_caps_calloc(1,1500, MALLOC_CAP_DMA);
      if (!sendbuf) {
            ESP_LOGI("Create pool:", "No enough memory");
            //return ESP_ERR_NO_MEM;
        }
     if (!recvbuf) {
            ESP_LOGI("Create pool:", "No enough memory");
            //return ESP_ERR_NO_MEM;
        }
    
    esp_err_t ret;

    //Configuration for the SPI bus
    spi_bus_config_t buscfg={
        .mosi_io_num=GPIO_MOSI,
        .miso_io_num=GPIO_MISO,
        .sclk_io_num=GPIO_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .isr_cpu_id = INTR_CPU_ID_AUTO,
    };

    //Configuration for the SPI slave interface
    spi_slave_interface_config_t slvcfg={
        .mode=0,
        .spics_io_num=GPIO_CS,
        .queue_size=3,
        .flags=0,
         .post_setup_cb=my_post_setup_cb,
        .post_trans_cb=my_post_trans_cb
    };

   
    gpio_set_pull_mode(GPIO_MOSI, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(GPIO_SCLK, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(GPIO_CS, GPIO_PULLUP_ONLY);
  
    ret=spi_slave_initialize(RCV_HOST, &buscfg, &slvcfg, SPI_DMA_CH_AUTO );
    assert(ret==ESP_OK);

    loopbuf = (spi_slave_transaction_t *) malloc (sizeof(spi_slave_transaction_t));
    
    
   t.length=1500*8;
   t.tx_buffer=sendbuf;
   t.rx_buffer=recvbuf;

  size_t item_size;

   while(1) {
    ret = spi_slave_transmit(RCV_HOST, &t, 1000);
}

}
