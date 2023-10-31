/* 
    Example Connection Configuration SPI clock speed (MHz) 40Mhz
    ESP System Settings   CPU frequency   240Mhz
    LWIP    Enable LWIP IRAM optimization   1
    Enable IPV4 Link-Local Addressing (AUTOIP)  ???
    TCP Maximum Segment Size (MSS) 1440???
    Checksums   Enable LWIP UDP checksums 1
    FreeRTOS 1000tick
    lwip TCPIP task receive mail box siz 16? to 64
    UDP Default UDP receive mail box size 16? to 64

*/

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


#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>


#include "freertos/ringbuf.h"
 char data_buffer[3000]="DAFI";
extern uint8_t *recvbuf;
//static const char *TAG = "example";

char recgvn[3000];
 struct  sockaddr_storage raddr;

extern uint8_t *recvbuf;

void My_udp_server_task(void){

  int socket_fd;

  
 
  socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if ( socket_fd < 0 )
    {
        printf("socket call failed");
        exit(0);
    }

    struct sockaddr_in myAdr;
    struct sockaddr q123;
    myAdr.sin_addr.s_addr = inet_addr("0.0.0.0");
    myAdr.sin_family = AF_INET;
    myAdr.sin_port = htons(1234);

   if (bind(socket_fd, &myAdr, sizeof(struct sockaddr_in)) == -1)
    {
      printf("Bind to Port Number  ,IP address  failed\n");
      close(socket_fd);
      //exit(1);
    }
    printf("Bind to Port Number  ,IP address  SUCCESS!!!\n");
    
  
    struct sockaddr_in sDestAddr;
   
    sDestAddr.sin_addr.s_addr = inet_addr("255.255.255.255");
    sDestAddr.sin_family = AF_INET;
    sDestAddr.sin_port = htons(3333);
    
    int sent_data=4;

   
    
    //size_t item_size=666;

    TickType_t xLastWakeTime;
BaseType_t xWasDelayed;
const TickType_t xFrequency = 1;
xLastWakeTime = xTaskGetTickCount ();


memset(&recgvn, 0, sizeof(recgvn));
memset(&raddr, 0, sizeof(raddr));
socklen_t socklen = sizeof(raddr);


    while(1){
          
          sent_data = sendto(socket_fd, recvbuf,1000,0,(struct sockaddr *)&sDestAddr, sizeof(sDestAddr));
     
          if(sent_data < 0){
            //  ESP_LOGI("Create pool:", "No enough memory");
         }
      
      }
    close(socket_fd); 
}


// void app_main(void)
// {
   
//     ESP_ERROR_CHECK(nvs_flash_init());
//     ESP_ERROR_CHECK(esp_netif_init());
//     ESP_ERROR_CHECK(esp_event_loop_create_default());
//     ESP_ERROR_CHECK(example_connect());
//     xTaskCreate(My_udp_server_task, "My_udp_server", 4096, (void*)AF_INET, 5, NULL);
// }

