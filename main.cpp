#include <Motor.hpp>
#include <Sensors.hpp>

#include <settings.h>

#include "PID.h"

// Did not have time to move serial to thread - this is how it would work.
// The parse function would use regex to extract the values from the serial input.
/*
#include "mbed.h"
#include "rtos.h"

#define BUFFER_SIZE 256

Thread serialThread;

RawSerial rs( SERIAL_TX, SERIAL_RX );

char serialBuffer[BUFFER_SIZE];
uint8_t bufferPos;

void serial_thread()
{
    while( 1 )
    {
        if( rs.readable() )
        {
            bufferPos = 0;
            while( rs.readable() )
            {
                serialBuffer[bufferPos++] = rs.getc();
            }
            serialBuffer[bufferPos] = '\0';
            parse( serialBuffer );
        }
    }
}

int main()
{
    rs.baud( 115200 );
    serialThread.start(serial_thread);
    
    while(1)
    {
        Thread::wait(500);
    }
}
*/

PID controller( 30.0, 1.5, 0.2, 0.1 );

int main()
{
    Motor motor( M1LPIN, M1HPIN, M2LPIN, M2HPIN, M3LPIN, M3HPIN );
    Sensors sensors( ECHAPIN, ECHBPIN, P1PIN, P2PIN, P3PIN );
    
    Serial serial( SERIAL_TX, SERIAL_RX );
    
    controller.setInputLimits(-100.0, 100.0);
    controller.setOutputLimits(-2.0, 2.0);
    // Change the value in the below line to set the speed in revs/second.
    controller.setSetPoint(10.0);
    controller.setBias(0);
    
    serial.baud( 115200 );
    
    us_ticker_init();
    
    sensors.startInterrupts();
    
    motor.prepare( sensors );
    
    int16_t oldQuad, newQuad;
    uint8_t oldMask, newMask;
    Direction newDirection;
    
    newQuad = oldQuad = 0;
    
    uint32_t lastUpdate = us_ticker_read();
    
    int8_t vvtable[] = { -1, 0, -2, 1, 2 };
    
    int8_t phase = 0;
    
    while( 1 )
    {
        newQuad = sensors.getQuadCount();
        newMask = sensors.getPhotoMask();
        newDirection = sensors.getDirection();
        
        if( oldQuad != newQuad )
        {
            oldQuad = newQuad;
        }
        if( oldMask != newMask )
        {
            switch( newMask )
            {
                case 0b011:
                    if( newDirection == ACW )
                    {
                        newQuad = 14;
                    } else {
                        newQuad = 81;
                    }
                    motor.updateState( phaseState( W3Positive, phase ) );
                break;
                
                case 0b001:
                    if( newDirection == ACW )
                    {
                        newQuad = 90;
                    } else {
                        newQuad = 161;
                    }
                    motor.updateState( phaseState( W2Negative, phase ) );
                break;
                
                case 0b101:
                    if( newDirection == ACW )
                    {
                        newQuad = 170;
                    } else {
                        newQuad = 237;
                    }
                    motor.updateState( phaseState( W1Positive, phase ) );
                break;
                
                case 0b100:
                    if( newDirection == ACW )
                    {
                        newQuad = 246;
                    } else {
                        newQuad = 317;
                    }
                    motor.updateState( phaseState( W3Negative, phase ) );
                break;
                
                case 0b110:
                    if( newDirection == ACW )
                    {
                        newQuad = 326;
                    } else {
                        newQuad = 393;
                    }
                    motor.updateState( phaseState( W2Positive, phase ) );
                break;
                
                case 0b010:
                    if( newDirection == ACW )
                    {
                        newQuad = 402;
                    } else {
                        newQuad = 5;
                    }
                    motor.updateState( phaseState( W1Negative, phase ) );
                break;
            }
            
            oldMask = newMask;
        }
        
        // Uncomment the following line to enable distance control.
        // $SPEED is the speed it needs to run at.
        // $NUMROTATIONS is the number of rotations that are to be performed.
        //controller.setSetPoint( $SPEED * ( 1 - ( sensors.getRevCount() / $NUMROTATIONS ) ) );
        
        controller.setProcessValue(sensors.getAvgRPM());
        
        float fOutput = controller.compute();
        uint8_t offset = rint( fOutput ) + 2;
        phase = vvtable[offset];
        
        if( ( us_ticker_read() - lastUpdate ) > 100000 )
        {
            serial.printf( "%f\t%d\r\n", sensors.getAvgRPM(), sensors.getRevCount());
            lastUpdate = us_ticker_read();
        }
    }
}
