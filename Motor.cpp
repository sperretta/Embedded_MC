#include <Motor.hpp>

MotorState phaseState( MotorState oldState, int8_t phase )
{
    MotorState newState = oldState;
    if( phase < 0 )
    {
        for( int8_t i = 0; i > phase; i-- )
        {
            --newState;
        }
    } else {
        for( int8_t i = 0; i < phase; i++ )
        {
            ++newState;
        }
    }
    return newState;
}

MotorState& operator++( MotorState& motorState )
{
    switch( motorState )
    {
        case W3Negative: motorState = W2Positive; break;
        case W2Positive: motorState = W1Negative; break;
        case W1Negative: motorState = W3Positive; break;
        case W3Positive: motorState = W2Negative; break;
        case W2Negative: motorState = W1Positive; break;
        case W1Positive: motorState = W3Negative; break;
    }

    return motorState;
}

MotorState operator++( MotorState& motorState, int )
{
    MotorState oldState( motorState );

    ++motorState;

    return oldState;
}

MotorState& operator--( MotorState& motorState )
{
    switch( motorState )
    {
        case W3Negative: motorState = W1Positive; break;
        case W2Positive: motorState = W3Negative; break;
        case W1Negative: motorState = W2Positive; break;
        case W3Positive: motorState = W1Negative; break;
        case W2Negative: motorState = W3Positive; break;
        case W1Positive: motorState = W2Negative; break;
    }

    return motorState;
}

MotorState operator--( MotorState& motorState, int )
{
    MotorState oldState( motorState );

    --motorState;

    return oldState;
}

Motor::Motor( 
            PinName m1L,
            PinName m1H,
            PinName m2L,
            PinName m2H,
            PinName m3L,
            PinName m3H
)
{
    // Set up the pins we're using.
    m_m1L = new DigitalOut( m1L );
    m_m1H = new DigitalOut( m1H );
    //m_m1H = new PwmOut( m1H );
    m_m2L = new DigitalOut( m2L );
    m_m2H = new DigitalOut( m2H );
    //m_m2H = new PwmOut( m2H );
    m_m3L = new DigitalOut( m3L );
    m_m3H = new DigitalOut( m3H );
    //m_m3H = new PwmOut( m3H );
    
    //m_m1H->period_us( 10 );
    //m_m2H->period_us( 10 );
    //m_m3H->period_us( 10 );
    
    m_power = 0.0f;
    
    // Start off in the freewheeling state.
    updateState( Freewheel );
}

Motor::~Motor()
{
    delete m_m1L;
    delete m_m1H;
    delete m_m2L;
    delete m_m2H;
    delete m_m3L;
    delete m_m3H;
}

MotorState Motor::updateState( MotorState nextState )
{
    switch( nextState )
    {
        case W3Negative:
            // L3 low
            m_m3L->write(1);
            m_m3H->write(1);
            // L1 high
            m_m1L->write(0);
            //m_m1H->write(m_power); // PWM
            m_m1H->write(0);
            // L2 HiZ
            m_m2L->write(0);
            m_m2H->write(1);
        break;
        
        case W2Positive:
            // L2 high
            m_m2L->write(0);
            //m_m2H->write(m_power); // PWM
            m_m2H->write(0);
            // L3 low
            m_m3L->write(1);
            m_m3H->write(1);
            // L1 HiZ
            m_m1L->write(0);
            m_m1H->write(1);
        break;
        
        case W1Negative:
            // L1 low
            m_m1L->write(1);
            m_m1H->write(1);
            // L2 high
            m_m2L->write(0);
            //m_m2H->write(m_power); // PWM
            m_m2H->write(0);
            // L3 HiZ
            m_m3L->write(0);
            m_m3H->write(1);
        break;
        
        case W3Positive:
            // L3 high
            m_m3L->write(0);
            //m_m3H->write(m_power); // PWM
            m_m3H->write(0);
            // L1 low
            m_m1L->write(1);
            m_m1H->write(1);
            // L2 HiZ
            m_m2L->write(0);
            m_m2H->write(1);
        break;
        
        case W2Negative:
            // L2 low
            m_m2L->write(1);
            m_m2H->write(1);
            // L3 high
            m_m3L->write(0);
            //m_m3H->write(m_power); // PWM
            m_m3H->write(0);
            // L1 HiZ
            m_m1L->write(0);
            m_m1H->write(1);
        break;
        
        case W1Positive:
            // L1 high
            m_m1L->write(0);
            //m_m1H->write(m_power); // PWM
            m_m1H->write(0);
            // L2 low
            m_m2L->write(1);
            m_m2H->write(1);
            // L3 HiZ
            m_m3L->write(0);
            m_m3H->write(1);
        break;
        
        default:
        case Freewheel:
            // All HiZ
            m_m1L->write(0);
            m_m1H->write(1);
            m_m2L->write(0);
            m_m2H->write(1);
            m_m3L->write(0);
            m_m3H->write(1);
        break;
        
        case Brake:
            // All low
            m_m1L->write(1);
            m_m1H->write(1);
            m_m2L->write(1);
            m_m2H->write(1);
            m_m3L->write(1);
            m_m3H->write(1);
        break;
    }
    
    return ( m_motorState = nextState );
}

void Motor::prepare( Sensors &sensors )
{
    updateState( W3Negative );
    sensors.reset();
    while( 1 )
    {
        wait_ms( 10 );
        // Do nothing until we're still!
        if( !sensors.moving() )
        {
            sensors.reset();
            return;
        }
    }
}

MotorState Motor::getState()
{
    return m_motorState;
}

void Motor::setPower( float power )
{
    m_power = 1.0f - power;
}

void Motor::setFrequency( float frequency )
{
    //m_m1H->period( 1.0f / frequency );
    //m_m2H->period( 1.0f / frequency );
    //m_m3H->period( 1.0f / frequency );
}

