#pragma once

// Initialize wiring pi
void pi_init();

// Initialize servo
void servo_init();

// Initialize motor
void motor_init();

// Turn LED on or off
void set_light(const int& light, const int& mode);

// Get distance reading from ultrasonic sensor
double get_distance();