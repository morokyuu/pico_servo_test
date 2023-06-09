#include <stdio.h>
#include <pico/stdlib.h>
#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"

const uint LED_PIN = 25;
const uint PWM_PORT = 2;
const uint PWM_PERIOD = 25000;

void init(void){
    uint slice_num = pwm_gpio_to_slice_num(PWM_PORT);
    gpio_set_function(PWM_PORT,GPIO_FUNC_PWM);

    //gpio_set_slew_rate(PWM_PORT,GPIO_SLEW_RATE_FAST);
    //gpio_set_slew_rate(PWM_PORT,GPIO_SLEW_RATE_SLOW);
    //gpio_set_drive_strength(PWM_PORT,GPIO_DRIVE_STRENGTH_12MA);

    pwm_config config;
    pwm_config_set_phase_correct(&config, false);
    pwm_config_set_clkdiv_int(&config, 100);
    pwm_config_set_clkdiv_mode(&config, PWM_DIV_FREE_RUNNING);
    pwm_config_set_output_polarity(&config, false, false);
    pwm_config_set_wrap(&config, PWM_PERIOD);
    pwm_init(slice_num, &config, true);

    adc_init();
    adc_gpio_init(26);
    adc_select_input(0);
}

int getDuty(){
    uint16_t ad = adc_read();
    const int ADRANGE = (1<<12)/2;
    uint16_t result = PWM_PERIOD/20 * (1.6 + 0.85 * (ad - ADRANGE)/ADRANGE);
    printf("ad %d, duty %d\n",ad,result);
    return result;
}

bool timer_callback(repeating_timer_t *rt){
    static int led = 0;

    gpio_put(LED_PIN, led);

    if(led){
        led = 0;
    }else{
        led = 1;
    }
}

int main(void){
    bi_decl(bi_program_description("This is a mosfet drive test"));
    
    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    gpio_put(LED_PIN, 1);
    sleep_ms(80);
    gpio_put(LED_PIN, 0);
    sleep_ms(80);
    gpio_put(LED_PIN, 1);

    //repeating_timer_t timer;
    //add_repeating_timer_us(-2000000, timer_callback, NULL, &timer);
    
    init();

    int duty=0;
    while(1){
        //pwm_set_gpio_level(PWM_PORT, (int)(0.3 * PWM_PERIOD));
        pwm_set_gpio_level(PWM_PORT, duty);
        duty = getDuty();
        
        if(duty & 0x01)
            gpio_put(LED_PIN, 1);
        else
            gpio_put(LED_PIN, 0);
    }
}

