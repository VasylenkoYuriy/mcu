/* BSD Socket API Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "MyUDP.c"
#include "MyUDP2.c"
#include "MySPISlave.c"
#include "MyBitBang.c"

#include "MyLOG.c"

#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
//#include "protocol_examples_common.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>


void app_main(void)
{
    xTaskCreate(My_bitbang, "My_bitbang", 4096, NULL, 5, NULL);  
 
     esp_eth_handle_t myEth = MyInitEth();
    
     esp_netif_init(); // Initialize TCP/IP network interface (should be called only once in application)
     
    nnetif(myEth);
    
    vTaskDelay(7000);
   
    xTaskCreate(My_SPI_Slave_task, "My_SPI_Slave", 4096, NULL, 5, NULL);
}

