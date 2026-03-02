#ifndef GPIO_H_
#define GPIO_H_

#define XCLKOUT_GPIO_PIN        73U
#define LED1_GPIO_PIN           31U
#define LED2_GPIO_PIN           34U
#define OUTPUTXBAR4_GPIO_PIN    15U
#define OUTPUTXBAR3_GPIO_PIN    14U
#define ADC_ISR_TOGGLE_GPIO_PIN 25U

void gpio_init(void);

#endif
