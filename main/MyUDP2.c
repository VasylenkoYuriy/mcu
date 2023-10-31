/*
TCPIP task receive mail box size 32 -> 128 -
Maximum active TCP Connections 16 -> 64 +
Maximum listening TCP Connections 16 -> 64 +
Default send buffer size 5744 -> 11488 +
Default receive window size 5744 -> 11488 +
Default TCP receive mail box size 6 -> 24+
Maximum active UDP control blocks 16 -> 64+
Default UDP receive mail box size 6 -> 24+
TCP/IP Task Stack Size 3072 -> 12288+
*/





#include "esp_eth_netif_glue.h"
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

#include <esp_types.h>
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "esp_heap_caps.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "soc/soc.h"
#include "soc/periph_defs.h"
#if !CONFIG_FREERTOS_UNICORE
#include "esp_ipc.h"
#endif

#include "esp_eth_driver.h"
//#include "protocol_examples_common.h"
#include "sdkconfig.h"

#include "soc/soc_caps.h"
#include "soc/gpio_periph.h"
#include "esp_log.h"
#include "esp_check.h"
#include "hal/gpio_hal.h"
#include "esp_rom_gpio.h"

// #if (SOC_RTCIO_PIN_COUNT > 0)
// #include "hal/rtc_io_hal.h"
// #endif



#define CONFIG_ETH_PHY_ADDR 1
#define CONFIG_ETH_PHY_RST_GPIO -1
#define CONFIG_ETH_SPI_MISO_GPIO 19
#define CONFIG_ETH_SPI_MOSI_GPIO 23
#define CONFIG_ETH_SPI_SCLK_GPIO 18
#define CONFIG_ETH_SPI_CLOCK_MHZ 25
#define CONFIG_ETH_SPI_CS_GPIO 5
#define CONFIG_ETH_SPI_HOST VSPI_HOST
#define CONFIG_ETH_SPI_INT_GPIO 4

static const char *TAG2 = "LOG";


////////////////////////////
/** Event handler for Ethernet events */
static void IRAM_ATTR eth_event_handler(void *arg, esp_event_base_t event_base,
                              int32_t event_id, void *event_data)
{
    uint8_t mac_addr[6] = {};
    /* we can get the ethernet driver handle from event data */
    esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;
    switch (event_id) {
    case ETHERNET_EVENT_CONNECTED:
        esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac_addr);
        ESP_LOGI(TAG2, "Ethernet Link Up");
        ESP_LOGI(TAG2, "Ethernet HW Addr %02x:%02x:%02x:%02x:%02x:%02x",
                    mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
        break;
    case ETHERNET_EVENT_DISCONNECTED:
        ESP_LOGI(TAG2, "Ethernet Link Down");
        break;
    case ETHERNET_EVENT_START:
        ESP_LOGI(TAG2, "Ethernet Started");
        break;
    case ETHERNET_EVENT_STOP:
        ESP_LOGI(TAG2, "Ethernet Stopped");
        break;
    default:
        break;
    }
}

esp_eth_handle_t MyInitEth(){
eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();      // apply default common MAC configuration
mac_config.rx_task_stack_size = 8192;
mac_config.rx_task_prio = 15;  
eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();      // apply default PHY configuration
phy_config.phy_addr = CONFIG_ETH_PHY_ADDR;           // alter the PHY address according to your board design
phy_config.reset_gpio_num = CONFIG_ETH_PHY_RST_GPIO; // alter the GPIO used for PHY reset
// Install GPIO interrupt service (as the SPI-Ethernet module is interrupt-driven)
gpio_install_isr_service(0);
// SPI bus configuration
spi_device_handle_t spi_handle = NULL;
spi_bus_config_t buscfg = {
    .miso_io_num = CONFIG_ETH_SPI_MISO_GPIO,
    .mosi_io_num = CONFIG_ETH_SPI_MOSI_GPIO,
    .sclk_io_num = CONFIG_ETH_SPI_SCLK_GPIO,
    .quadwp_io_num = -1,
    .quadhd_io_num = -1,
};
ESP_ERROR_CHECK(spi_bus_initialize(CONFIG_ETH_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO));
// Configure SPI device
spi_device_interface_config_t spi_devcfg = {
    .mode = 0,
    .clock_speed_hz = CONFIG_ETH_SPI_CLOCK_MHZ * 1000 * 1000,
    .spics_io_num = CONFIG_ETH_SPI_CS_GPIO,
    .queue_size = 20
};
/* w5500 ethernet driver is based on spi driver */
eth_w5500_config_t w5500_config = ETH_W5500_DEFAULT_CONFIG(CONFIG_ETH_SPI_HOST, &spi_devcfg);
w5500_config.int_gpio_num = CONFIG_ETH_SPI_INT_GPIO;
esp_eth_mac_t *mac = esp_eth_mac_new_w5500(&w5500_config, &mac_config);
esp_eth_phy_t *phy = esp_eth_phy_new_w5500(&phy_config);


/////////////////////////////
esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy); // apply default driver configuration
esp_eth_handle_t eth_handle = NULL; // after the driver is installed, we will get the handle of the driver

esp_eth_driver_install(&config, &eth_handle); // install driver

uint8_t mac_addr[6] = {06,30,00,79,67,80};
esp_eth_ioctl(eth_handle, ETH_CMD_S_MAC_ADDR, mac_addr);



ESP_ERROR_CHECK(esp_event_loop_create_default());
esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, NULL); // register Ethernet event handler (to deal with user-specific stuff when events like link up/down happened)
    

    return eth_handle;
}

/** Event handler for IP_EVENT_ETH_GOT_IP */
static void got_ip_event_handler(void *arg, esp_event_base_t event_base,
                                 int32_t event_id, void *event_data)
{
    ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
    const esp_netif_ip_info_t *ip_info = &event->ip_info;

    ESP_LOGI(TAG2, "Ethernet Got IP Address");
    ESP_LOGI(TAG2, "~~~~~~~~~~~");
    ESP_LOGI(TAG2, "ETHIP:" IPSTR, IP2STR(&ip_info->ip));
    ESP_LOGI(TAG2, "ETHMASK:" IPSTR, IP2STR(&ip_info->netmask));
    ESP_LOGI(TAG2, "ETHGW:" IPSTR, IP2STR(&ip_info->gw));
    ESP_LOGI(TAG2, "~~~~~~~~~~~");
}


void nnetif(esp_eth_handle_t www){

esp_netif_config_t cfg = ESP_NETIF_DEFAULT_ETH(); // apply default network interface configuration for Ethernet
esp_netif_t *eth_netif = esp_netif_new(&cfg); // create network interface for Ethernet driver

esp_netif_attach(eth_netif, esp_eth_new_netif_glue(www)); // attach Ethernet driver to TCP/IP stack
esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, NULL); // register user defined IP event handlers
esp_eth_start(www); // start Ethernet driver state machine
}