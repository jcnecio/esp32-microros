#include <stdio.h>
#include <unistd.h>
#include <inttypes.h>
#include <math.h>

#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <geometry_msgs/msg/twist.h>

#include <driver/gpio.h>
#include <driver/ledc.h>

#ifdef ESP_PLATFORM
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#endif

#define constrain(amt, low, high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){printf("Failed status on line %d: %d. Aborting.\n",__LINE__,(int)temp_rc);vTaskDelete(NULL);}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){printf("Failed status on line %d: %d. Continuing.\n",__LINE__,(int)temp_rc);}}


#define FRAME_TIME 100 // 1000 / FRAME_TIME = FPS
#define SLEEP_TIME 10

// PINS
#define PIN_LEFT_FORWARD 26
#define PIN_LEFT_BACKWARD 25
#define PIN_RIGHT_FORWARD 32
#define PIN_RIGHT_BACKWARD 33

#define PWM_FREQUENCY 50
#define PWM_RESOLUTION LEDC_TIMER_12_BIT
#define PWM_TIMER LEDC_TIMER_1
#define PWM_MODE LEDC_HIGH_SPEED_MODE

geometry_msgs__msg__Twist msg;

void setupPins();
void setupRos();
void cmd_vel_callback(const void *msgin);
void timer_callback(rcl_timer_t *timer, int64_t last_call_time);
float fmap(float val, float in_min, float in_max, float out_min, float out_max);

void appMain(void *arg) {
    setupPins();
    setupRos();
}

void setupPins() {
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = PWM_RESOLUTION,
        .freq_hz = PWM_FREQUENCY,
        .speed_mode = PWM_MODE,
        .timer_num = PWM_TIMER,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&ledc_timer);

    gpio_reset_pin(PIN_LEFT_FORWARD);
    gpio_set_direction(PIN_LEFT_FORWARD, GPIO_MODE_OUTPUT);
	
    gpio_reset_pin(PIN_LEFT_BACKWARD);
    gpio_set_direction(PIN_LEFT_BACKWARD, GPIO_MODE_OUTPUT);
	
    gpio_reset_pin(PIN_RIGHT_FORWARD);
    gpio_set_direction(PIN_RIGHT_FORWARD, GPIO_MODE_OUTPUT);
	
    gpio_reset_pin(PIN_RIGHT_BACKWARD);
    gpio_set_direction(PIN_RIGHT_BACKWARD, GPIO_MODE_OUTPUT);
}

void setupRos() {
    rcl_allocator_t allocator = rcl_get_default_allocator();
    rclc_support_t support;

    RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

    rcl_node_t node;
    RCCHECK(rclc_node_init_default(&node, "ros_esp32_bot", "", &support));

    rcl_subscription_t subscriber;
    RCCHECK(rclc_subscription_init_default(
        &subscriber,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Twist),
        "/cmd_vel"));

    rcl_timer_t timer;
    RCCHECK(rclc_timer_init_default(
        &timer,
        &support,
        RCL_MS_TO_NS(FRAME_TIME),
        timer_callback));

    rclc_executor_t executor;
    RCCHECK(rclc_executor_init(&executor, &support.context, 2, &allocator));
    RCCHECK(rclc_executor_add_subscription(&executor, &subscriber, &msg, &cmd_vel_callback, ON_NEW_DATA));
    RCCHECK(rclc_executor_add_timer(&executor, &timer));

    while (1) {
        rclc_executor_spin_some(&executor, RCL_MS_TO_NS(SLEEP_TIME));
        usleep(SLEEP_TIME * 1000);
    }

    RCCHECK(rcl_subscription_fini(&subscriber, &node));
    RCCHECK(rcl_node_fini(&node));

    vTaskDelete(NULL);
}

void cmd_vel_callback(const void *msgin) {
}

void timer_callback(rcl_timer_t *timer, int64_t last_call_time) {
    if (timer == NULL) {
        return;
    }

    float linear = msg.linear.x;
    float angular = msg.angular.z;
    printf("%f, %f\n", linear, angular);
    if (fabs(linear) > fabs(angular)) {
        if (linear > 0) {
            gpio_set_level(PIN_LEFT_BACKWARD, 0);
            gpio_set_level(PIN_RIGHT_BACKWARD, 0);
            gpio_set_level(PIN_LEFT_FORWARD, 1);
            gpio_set_level(PIN_RIGHT_FORWARD, 1);
        } else {
            gpio_set_level(PIN_LEFT_FORWARD, 0);
            gpio_set_level(PIN_RIGHT_FORWARD, 0);
            gpio_set_level(PIN_LEFT_BACKWARD, 1);
            gpio_set_level(PIN_RIGHT_BACKWARD, 1);
        }
    } else if(fabs(linear) < fabs(angular)) {
        if (angular > 0) {
            gpio_set_level(PIN_LEFT_FORWARD, 0);
            gpio_set_level(PIN_RIGHT_BACKWARD, 0);
            gpio_set_level(PIN_RIGHT_FORWARD, 1);
            gpio_set_level(PIN_LEFT_BACKWARD, 1);
        } else {
            gpio_set_level(PIN_RIGHT_FORWARD, 0);
            gpio_set_level(PIN_LEFT_BACKWARD, 0);
            gpio_set_level(PIN_LEFT_FORWARD, 1);
            gpio_set_level(PIN_RIGHT_BACKWARD, 1);
        }
    } else {
        gpio_set_level(PIN_LEFT_FORWARD, 0);
        gpio_set_level(PIN_RIGHT_FORWARD, 0);
        gpio_set_level(PIN_LEFT_BACKWARD, 0);
        gpio_set_level(PIN_RIGHT_BACKWARD, 0);
    }
}