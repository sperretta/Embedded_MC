#ifndef __SENSORS_HPP__
#define __SENSORS_HPP__

#include <mbed.h>

#define NUM_AVG 3

enum Direction
{
    ACW,
    CW
};

class Sensors
{
    public:
        Sensors(
            PinName eChA,
            PinName eChB,
            PinName pI1,
            PinName pI2,
            PinName pI3
        );
        
        ~Sensors();
        
        void startInterrupts();
        
        uint8_t getPhotoMask();
        int16_t getQuadCount();
        Direction getDirection();
        float getRPM();
        float getAvgRPM();
        int16_t getRevCount();
        void reset();
        
        bool moving();
            
    private:
        void readPhoto();
        
        void calcQuadCount();
        
        void syncRotations();
        void updateRevs();
        
        //void revUpdate();
        
        void isrRisingA();
        void isrFallingA();
        void isrRisingB();
        void isrFallingB();
        
        InterruptIn *m_eChA, *m_eChB;
        
        bool m_stateQuadA;
        bool m_stateQuadB;
        
        int16_t m_quadCount;
        
        int16_t m_currentRevs;
        
        bool m_revState;
        
        Direction m_direction;
        
        DigitalIn *m_pI1, *m_pI2, *m_pI3;
        bool m_statePI1;
        bool m_statePI2;
        bool m_statePI3;
        
        uint8_t m_prevState;
        
        uint32_t m_lastMoved;
        uint32_t m_lastRevved;
        float m_RPM;
        
        float m_avgRPM[NUM_AVG];
        uint8_t m_avrCountRPM;
};

#endif // __SENSORS_HPP__
