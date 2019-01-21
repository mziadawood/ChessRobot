
#ifndef Stepper_h
#define Stepper_h

// library interface description
class Stepper {
  public:

    Stepper(int number_of_steps, int motor_pin_1, int motor_pin_2,
                                 int motor_pin_3, int motor_pin_4,int enable_pin);
 
    // speed setter method:
    void setSpeed(long whatSpeed);


    // mover method:
    void step(int number_of_steps);


  private:
    void stepMotor(int this_step);

    int direction;            // Direction of rotation
    int speed;  
	int enable_pin;              // Speed in RPMs
    unsigned long step_delay; // delay between steps, in ms, based on speed
    int number_of_steps;      // total number of steps this motor can take
    int pin_count;            // how many pins are in use.
    int step_number;          // which step the motor is on

    // motor pin numbers:
    int motor_pin_1;
    int motor_pin_2;
    int motor_pin_3;
    int motor_pin_4;
    int motor_pin_5;          // Only 5 phase motor

    unsigned long last_step_time; // time stamp in us of when the last step was taken
};

#endif

