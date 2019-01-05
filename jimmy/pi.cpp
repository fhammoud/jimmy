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
#define SERVO 18
#define TRIGGER 23
#define ECHO 24

using namespace std;
using namespace chrono;

// Initialize wiring pi
void pi_init()
{
	// Initialize wiring pi
	cout << "Initializing Raspberry Pi modules\n";
	wiringPiSetupSys();

	// Set pin modes
	cout << "Setting pin modes\n";
	pinMode(LED, OUTPUT);
	pinMode(SERVO, OUTPUT);
	pinMode(TRIGGER, OUTPUT);
	pinMode(ECHO, INPUT);

	// Initialize servo
	cout << "Initializing servo\n";
	servo_init();

	// Test ultrasonic sensor
	cout << "Testing ultrasonic sensor\n";
	const double distance = get_distance();
	cout << "Initial distance: " << distance << endl;

	// Test LED lights
	cout << "Testing LED lights\n";
	set_light(LED, HIGH);
	delay(500);
	set_light(LED, LOW);

	cout << "Initialization of Raspberry Pi modules complete!\n";
}

// Initialize servo
void servo_init()
{
	// Set SERVO pin as PWM pin
	softPwmCreate(SERVO, 0, 200);

	// Set servo to minimum angle
	softPwmWrite(SERVO, 5);
	delay(300);

	// Make servo rotate from minimum angle to maximum angle
	for (int i = 5; i <= 25; i++) {
		softPwmWrite(SERVO, i);
		delay(20);
	}
	delay(500);

	// Make servo rotate from maximum angle to minimum angle
	for (int i = 25; i >= 5; i--) {
		softPwmWrite(SERVO, i);
		delay(20);
	}
	delay(500);

	// Set servo to neutral position
	softPwmWrite(SERVO, 15);
	delay(300);
}

// Turn LED on or off
void set_light(int light, int mode)
{
	digitalWrite(light, mode);
}

// Get distance reading from ultrasonic sensor
double get_distance()
{
	// Turn on trigger for 0.01 ms
	digitalWrite(TRIGGER, HIGH);
	delayMicroseconds(10);
	digitalWrite(TRIGGER, LOW);

	// Measure time
	auto start = steady_clock::now();
	auto stop = steady_clock::now();

	while (digitalRead(ECHO) == 0)
		start = steady_clock::now();

	while (digitalRead(ECHO) == 1)
		stop = steady_clock::now();

	const auto elapsed = duration<double> (stop - start).count();

	return elapsed * 170;
}