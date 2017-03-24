#ifndef __MOTOR_HPP__
#define __MOTOR_HPP__

#include <mbed.h>

#include <Sensors.hpp>

// Motor states
enum MotorState
{
    W3Negative = 0,
    W2Positive,
    W1Negative,
    W3Positive,
    W2Negative,
    W1Positive,
    NUM_MOTOR_STATES,
    Freewheel,
    Brake
};

// Prefix increment operator.
MotorState& operator++( MotorState& motorState );

// Postfix increment operator.
MotorState operator++( MotorState& motorState, int );

// Prefix decrement operator.
MotorState& operator--( MotorState& motorState );

// Postfix decrement operator.
MotorState operator--( MotorState& motorState, int );

MotorState phaseState( MotorState oldState, int8_t phase );

class Motor
{
    public:
        // Constructor.
        Motor( 
            PinName m1L,
            PinName m1H,
            PinName m2L,
            PinName m2H,
            PinName m3L,
            PinName m3H
        );
        
        // Destructor
        ~Motor();
        
        // Start the motor in the initial (turning) state, 0 degrees.
        void prepare( Sensors &sensors );
        
        // Function to update the motor's state with nextState.
        MotorState updateState( MotorState nextState );
        
        // m_motorState getter.
        MotorState getState();
        
        void setPower( float power );
        void setFrequency( float frequency );
        
        void reset();
    protected:
    private:
        void init();
        void outputState();
    
        MotorState m_motorState;
        DigitalOut *m_m1L, *m_m2L, *m_m3L;
        //PwmOut *m_m1H, *m_m2H, *m_m3H;
        DigitalOut *m_m1H, *m_m2H, *m_m3H;
        
        float m_power;
};

#endif // __MOTOR_HPP__
