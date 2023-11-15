/**
 * @file tca9535.h
 * @brief TCA9535 driver header.
 * @version 0.1
 * @date 2022-12-01
 *
 * @copyright Copyright 2022 Seeed Technology Co. Ltd.
 *
 *      Licensed under the Apache License, Version 2.0 (the "License");
 *      you may not use this file except in compliance with the License.
 *      You may obtain a copy of the License at
 *
 *               http://www.apache.org/licenses/LICENSE-2.0
 *
 *      Unless required by applicable law or agreed to in writing, software
 *      distributed under the License is distributed on an "AS IS" BASIS,
 *      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *      See the License for the specific language governing permissions and
 *      limitations under the License.
 */

#pragma once

#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


esp_err_t tca9535_init(uint8_t i2c_addr);
esp_err_t tca9535_set_direction(uint8_t pin, bool is_output);
esp_err_t tca9535_set_level(uint8_t pin, bool level);
esp_err_t tca9535_read_output_pins(uint16_t *pin_val);
esp_err_t tca9535_read_input_pins(uint16_t *pin_val);

esp_err_t tca9535_multi_write_start(void);
esp_err_t tca9535_multi_write_new_level(int pin, bool new_level);
esp_err_t tca9535_multi_write_end(void);

#ifdef __cplusplus
}
#endif
