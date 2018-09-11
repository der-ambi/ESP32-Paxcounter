/*

//////////////////////// ESP32-Paxcounter \\\\\\\\\\\\\\\\\\\\\\\\\\

Copyright  2018 Christian Ambach <christian.ambach@deutschebahn.com>

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

NOTICE:
Parts of the source files in this repository are made available under different
licenses. Refer to LICENSE.txt file in repository for more details.

*/

#include "spi.h"

#include "driver/spi_slave.h"
#include <sys/param.h>

static const char TAG[] = __FILE__;

QueueHandle_t SPISendQueue;

void spi_slave_task(void *param) {
  while (1) {

    MessageBuffer_t msg;
    size_t bufsize;
    uint8_t *txbuf, *rxbuf;

    if (xQueueReceive(SPISendQueue, &msg, portMAX_DELAY) != pdTRUE) {
      ESP_LOGE(TAG, "Premature return from xQueueReceive() with no data!");
      continue;
    }

    spi_slave_transaction_t spi_transaction;
    memset(&spi_transaction, 0, sizeof(spi_transaction));
    // SPI transaction size needs to be at least 8 bytes and dividable by 4, see
    // https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/peripherals/spi_slave.html
    bufsize = MAX(8, msg.MessageSize);
    bufsize += bufsize % 4;
    txbuf = (uint8_t *)heap_caps_malloc(bufsize, MALLOC_CAP_DMA);
    rxbuf = (uint8_t *)heap_caps_malloc(bufsize, MALLOC_CAP_DMA);
    // TODO check for OOM
    memset(txbuf, 0, bufsize);
    memset(rxbuf, 0, bufsize);
    memcpy(txbuf, &msg.Message, msg.MessageSize);
    spi_transaction.length = bufsize * 8;
    spi_transaction.tx_buffer = txbuf;
    spi_transaction.rx_buffer = rxbuf;

    ESP_LOGI(TAG, "Prepared SPI transaction for %zu bytes", bufsize);
    ESP_LOG_BUFFER_HEXDUMP(TAG, txbuf, bufsize, ESP_LOG_ERROR);
    esp_err_t ret =
        spi_slave_transmit(HSPI_HOST, &spi_transaction, portMAX_DELAY);
    ESP_LOG_BUFFER_HEXDUMP(TAG, rxbuf, bufsize, ESP_LOG_ERROR);
    free(txbuf);
    free(rxbuf);
    ESP_LOGI(TAG, "Transaction finished with size %zu bits",
             spi_transaction.trans_len);
  }
}

esp_err_t spi_init() {
#ifndef HAS_SPI
  return ESP_OK;
#else

  SPISendQueue = xQueueCreate(SEND_QUEUE_SIZE, sizeof(MessageBuffer_t));
  if (SPISendQueue == 0) {
    ESP_LOGE(TAG, "Could not create SPI send queue. Aborting.");
    return ESP_FAIL;
  }
  ESP_LOGI(TAG, "SPI send queue created, size %d Bytes",
           SEND_QUEUE_SIZE * PAYLOAD_BUFFER_SIZE);

  spi_bus_config_t spi_bus_cfg = {.mosi_io_num = PIN_SPI_MOSI,
                                  .miso_io_num = PIN_SPI_MISO,
                                  .sclk_io_num = PIN_SPI_SCLK,
                                  .quadwp_io_num = -1,
                                  .quadhd_io_num = -1,
                                  .max_transfer_sz = 0,
                                  .flags = 0};

  spi_slave_interface_config_t spi_slv_cfg = {.spics_io_num = PIN_SPI_CS,
                                              .flags = 0,
                                              .queue_size = 1,
                                              .mode = 0,
                                              .post_setup_cb = NULL,
                                              .post_trans_cb = NULL};

  gpio_set_pull_mode(PIN_SPI_MOSI, GPIO_PULLUP_ONLY);
  gpio_set_pull_mode(PIN_SPI_SCLK, GPIO_PULLUP_ONLY);
  gpio_set_pull_mode(PIN_SPI_CS, GPIO_PULLUP_ONLY);

  xTaskCreate(spi_slave_task, "spi_slave", 4096, (void *)NULL, 2, NULL);

  esp_err_t ret =
      spi_slave_initialize(HSPI_HOST, &spi_bus_cfg, &spi_slv_cfg, 1);
  return ret;

#endif
}

void spi_enqueuedata(uint8_t messageType, MessageBuffer_t *message) {
  // enqueue message in SPI send queue
#ifdef HAS_SPI
  // only report counter values for now
  if (messageType != COUNTERPORT) {
    return;
  }
  BaseType_t ret =
      xQueueSendToBack(SPISendQueue, (void *)message, (TickType_t)0);
  if (ret == pdTRUE) {
    ESP_LOGI(TAG, "%d bytes enqueued for SPI consumption", payload.getSize());
  } else {
    ESP_LOGW(TAG, "SPI sendqueue is full");
  }
#endif
}

void spi_queuereset(void) {
#ifdef HAS_SPI
  xQueueReset(SPISendQueue);
#endif
}
