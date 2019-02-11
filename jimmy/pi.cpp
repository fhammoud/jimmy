#include "pi.h"
#include <wiringPi.h>
#include <softPwm.h>
#include <chrono>
#include <iostream>

// LED Pin - wiringPi pin 0 is BCM_GPIO 17.
// we have to use BCM numbering when initializing with wiringPiSetupSys
// when choosing a different pin number please use the BCM numbering, also
// update the Property Pages - Build Events - Remote Post-Build Event command 
// which uses gpio export for setup for wiringPiSetupSys
#define	LED	17
#define MOTOR 18
#define SERVO 19
#define TRIGGER 23
#define ECHO 24

// Initialize wiring pi
void pi_init()
{
	// Initialize wiring pi
	std::cout << "Initializing Raspberry Pi modules\n";
	wiringPiSetupGpio();

	// Set pin modes
	std::cout << "Setting pin modes\n";
	pinMode(LED, OUTPUT);
	pinMode(SERVO, OUTPUT);
	pinMode(TRIGGER, OUTPUT);
	pinMode(ECHO, INPUT);
	pinMode(MOTOR, PWM_OUTPUT);

	// Initialize servo
	std::cout << "Initializing servo\n";
	servo_init();
	
	// Initialize servo
	std::cout << "Initializing motor\n";
	motor_init();

	// Test ultrasonic sensor
	std::cout << "Testing ultrasonic sensor\n";
	const double distance = get_distance();
	std::cout << "Initial distance: " << distance << std::endl;

	// Test LED lights
	std::cout << "Testing LED lights\n";
	set_light(HIGH);
	delay(500);
	set_light(LOW);

	std::cout << "Initialization of Raspberry Pi modules complete!\n";
	std::cout << "Waiting" << std::endl;
	std::cin.get();
}

// Initialize servo
void servo_init()
{
	// Set SERVO pin as PWM pin
	softPwmCreate(SERVO, LOW, 200);

	// Set servo to minimum angle
	softPwmWrite(SERVO, 15);
	delay(1000);
}

// Initialize motor
void motor_init()
{
	pwmSetMode(PWM_MODE_MS);
	pwmSetClock(384); // Clock at 50kHz (20us tick)
	pwmSetRange(1000); // Range at 1000 ticks (20ms)

	// Set to neutral
	pwmWrite(MOTOR, 75);
	delay(20);

	// Set to max
	pwmWrite(MOTOR, 100);
	delay(20);

	// Set to min
	pwmWrite(MOTOR, 50);
	delay(20);

	// Set back to neautral
	pwmWrite(MOTOR, 75);
	delay(20);
}

// Turn LED on or off
void set_light(const int& mode)
{
	digitalWrite(LED, mode);
}

// Get distance reading from ultrasonic sensor
double get_distance()
{
	// Turn on trigger for 0.01 ms
	digitalWrite(TRIGGER, HIGH);
	delayMicroseconds(10);
	digitalWrite(TRIGGER, LOW);

	// Measure time
	auto start = std::chrono::steady_clock::now();
	auto stop = std::chrono::steady_clock::now();

	while (digitalRead(ECHO) == 0)
		start = std::chrono::steady_clock::now();

	while (digitalRead(ECHO) == 1)
		stop = std::chrono::steady_clock::now();

	const auto elapsed = std::chrono::duration<double>(stop - start).count();

	// Return distance
	return elapsed * 17000;
}

// Move car fwd/rev
void move()
{
	pwmWrite(MOTOR, 82);
}

// Stop car
void stop()
{
	pwmWrite(MOTOR, 75);
}

// Turn car left/right
void turn(const int& angle)
{
	softPwmWrite(SERVO, angle);
}
