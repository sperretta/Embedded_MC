#include <Sensors.hpp>

Sensors::Sensors(
    PinName eChA,
    PinName eChB,
    PinName pI1,
    PinName pI2,
    PinName pI3
)
{
    // Setup quadrature interrupts.
    m_eChA = new InterruptIn( eChA );
    m_eChB = new InterruptIn( eChB );
    
    // Initialise quadrature state.
    m_stateQuadA = false;
    m_stateQuadB = false;
    
    reset();
    
    // Setup photointerruptors.
    m_pI1 = new DigitalIn( pI1 );
    m_pI2 = new DigitalIn( pI2 );
    m_pI3 = new DigitalIn( pI3 );
    
    m_avrCountRPM = 0;
    for( uint8_t i = 0; i < NUM_AVG; i++ )
    {
        m_avgRPM[i] = 0.0f;
    }
    
    // Initialise photointerruptor state.
    readPhoto();
    // if change since last time
    // syncRotations()
}

void Sensors::startInterrupts()
{
    // Start quadrature interrupts.
    m_eChA->rise( callback( this, &Sensors::isrRisingA ) );
    m_eChA->fall( callback( this, &Sensors::isrFallingA ) );
    m_eChB->rise( callback( this, &Sensors::isrRisingB ) );
    m_eChB->fall( callback( this, &Sensors::isrFallingB ) );
}

Sensors::~Sensors()
{
    delete m_eChA;
    delete m_eChB;
    
    delete m_pI1;
    delete m_pI2;
    delete m_pI3;
}

void Sensors::readPhoto()
{
    m_statePI1 = m_pI1->read();
    m_statePI2 = m_pI2->read();
    m_statePI3 = m_pI3->read();
}

void Sensors::isrRisingA()
{
    if( m_stateQuadB )
    {
        --m_quadCount;
        m_direction = CW;
        m_lastMoved = us_ticker_read();
    } else {
        ++m_quadCount;
        m_direction = ACW;
        m_lastMoved = us_ticker_read();
    }
    
    if( m_quadCount > 467 )
    {
        m_quadCount -= 468;
    } else if( m_quadCount < 0 )
    {
        m_quadCount += 468;
    }
    if( m_quadCount == 0 )
    {
        if( m_revState )
        {
            if( m_direction == CW )
            {
                ++m_currentRevs;
            } else {
                --m_currentRevs;
            }
            // Time for one rev:
            uint32_t currentTick = us_ticker_read();
            
            m_RPM = 1000000.0f / ( currentTick - m_lastRevved );
            
            if( m_direction == ACW )
            {
                m_RPM *= -1.0f;
            }
            
            if( m_avrCountRPM == ( NUM_AVG - 1 ) )
            {
                m_avrCountRPM = 0;
            }
            
            m_avgRPM[m_avrCountRPM++] = m_RPM;
            
            m_lastRevved = currentTick;
            m_revState = 0;
        }
    } else if( m_quadCount == 233 )
    {
        m_revState = 1;
    }
    
    readPhoto();
    //syncRotations();
    
    m_stateQuadA = true;
}

void Sensors::isrFallingA()
{
    if( m_stateQuadB )
    {
        ++m_quadCount;
        m_direction = ACW;
        m_lastMoved = us_ticker_read();
    } else {
        --m_quadCount;
        m_direction = CW;
        m_lastMoved = us_ticker_read();
    }
    
    if( m_quadCount > 467 )
    {
        m_quadCount -= 468;
    } else if( m_quadCount < 0 )
    {
        m_quadCount += 468;
    }
    if( m_quadCount == 0 )
    {
        if( m_revState )
        {
            if( m_direction == CW )
            {
                ++m_currentRevs;
            } else {
                --m_currentRevs;
            }
            // Time for one rev:
            uint32_t currentTick = us_ticker_read();
            
            m_RPM = 1000000.0f / ( currentTick - m_lastRevved );
            
            if( m_direction == ACW )
            {
                m_RPM *= -1.0f;
            }
            
            if( m_avrCountRPM == ( NUM_AVG - 1 ) )
            {
                m_avrCountRPM = 0;
            }
            
            m_avgRPM[m_avrCountRPM++] = m_RPM;
            
            m_lastRevved = currentTick;
            m_revState = 0;
        }
    } else if( m_quadCount == 233 )
    {
        m_revState = 1;
    }
    
    
    //readPhoto();
    //syncRotations();
    
    m_stateQuadA = false;
}

void Sensors::isrRisingB()
{
    if( m_stateQuadA )
    {
        ++m_quadCount;
        m_direction = ACW;
        m_lastMoved = us_ticker_read();
    } else {
        --m_quadCount;
        m_direction = CW;
        m_lastMoved = us_ticker_read();
    }
    
    
    if( m_quadCount > 467 )
    {
        m_quadCount -= 468;
    } else if( m_quadCount < 0 )
    {
        m_quadCount += 468;
    }
    if( m_quadCount == 0 )
    {
        if( m_revState )
        {
            if( m_direction == CW )
            {
                ++m_currentRevs;
            } else {
                --m_currentRevs;
            }
            // Time for one rev:
            uint32_t currentTick = us_ticker_read();
            
            m_RPM = 1000000.0f / ( currentTick - m_lastRevved );
            
            if( m_direction == ACW )
            {
                m_RPM *= -1.0f;
            }
            
            if( m_avrCountRPM == ( NUM_AVG - 1 ) )
            {
                m_avrCountRPM = 0;
            }
            
            m_avgRPM[m_avrCountRPM++] = m_RPM;
            
            m_lastRevved = currentTick;
            m_revState = 0;
        }
    } else if( m_quadCount == 233 )
    {
        m_revState = 1;
    }
    
    
    //readPhoto();
    //syncRotations();
    
    m_stateQuadB = true;
}

void Sensors::isrFallingB()
{
    if( m_stateQuadA )
    {
        --m_quadCount;
        m_direction = CW;
        m_lastMoved = us_ticker_read();
    } else {
        ++m_quadCount;
        m_direction = ACW;
        m_lastMoved = us_ticker_read();
    }
    
    if( m_quadCount > 467 )
    {
        m_quadCount -= 468;
    } else if( m_quadCount < 0 )
    {
        m_quadCount += 468;
    }
    if( m_quadCount == 0 )
    {
        if( m_revState )
        {
            if( m_direction == CW )
            {
                ++m_currentRevs;
            } else {
                --m_currentRevs;
            }
            // Time for one rev:
            uint32_t currentTick = us_ticker_read();
            
            m_RPM = 1000000.0f / ( currentTick - m_lastRevved );
            
            if( m_direction == ACW )
            {
                m_RPM *= -1.0f;
            }
            
            if( m_avrCountRPM == ( NUM_AVG - 1 ) )
            {
                m_avrCountRPM = 0;
            }
            
            m_avgRPM[m_avrCountRPM++] = m_RPM;
            
            m_lastRevved = currentTick;
            m_revState = 0;
        }
    } else if( m_quadCount == 233 )
    {
        m_revState = 1;
    }
    
    //readPhoto();
    //syncRotations();
    
    m_stateQuadB = false;
}

uint8_t Sensors::getPhotoMask()
{
    return ( m_statePI1 ) + ( m_statePI2 * 2 ) + ( m_statePI3 * 4 );
}

int16_t Sensors::getQuadCount()
{
    return m_quadCount;
}

Direction Sensors::getDirection()
{
    return m_direction;
}

int16_t Sensors::getRevCount()
{
    return m_currentRevs;
}

void Sensors::reset()
{
    // Mutex?
    m_quadCount = 0;
    
    m_currentRevs = 0;
    m_revState = 0;
    m_RPM = 0.0f;
    
    m_lastMoved = us_ticker_read();
}

bool Sensors::moving()
{
    if( ( us_ticker_read() - m_lastMoved ) > 1000000 ) // If no motion within 1s
    {
        return false;
    }
    return true;
}

float Sensors::getRPM()
{
    return m_RPM;
}/*

void Sensors::syncRotations()
{
    uint8_t newState = getPhotoMask();
    switch( newState )
    {
        case 0b011:
            if( m_direction == ACW )
            {
                m_quadCount = 14;
            } else {
                m_quadCount = 81;
            }
        break;
        
        case 0b001:
            if( m_direction == ACW )
            {
                m_quadCount = 90;
            } else {
                m_quadCount = 161;
            }
        break;
        
        case 0b101:
            if( m_direction == ACW )
            {
                m_quadCount = 170;
            } else {
                m_quadCount = 237;
            }
        break;
        
        case 0b100:
            if( m_direction == ACW )
            {
                m_quadCount = 246;
            } else {
                m_quadCount = 317;
            }
        break;
        
        case 0b110:
            if( m_direction == ACW )
            {
                m_quadCount = 326;
            } else {
                m_quadCount = 393;
            }
        break;
        
        case 0b010:
            if( m_direction == ACW )
            {
                m_quadCount = 402;
            } else {
                m_quadCount = 5;
            }
        break;
    }
    
    if( newState != m_prevState )
    {
        updateRevs();
    }
}

void Sensors::updateRevs()
{
    if( m_quadCount > 467 )
    {
        m_quadCount -= 468;
    } else if( m_quadCount < 0 )
    {
        m_quadCount += 468;
    }
    if( m_quadCount == 0 )
    {
        if( m_revState )
        {
            if( m_direction == ACW )
            {
                ++m_currentRevs;
            } else {
                --m_currentRevs;
            }
            // Time for one rev:
            uint32_t currentTick = us_ticker_read();
            
            m_RPM = 60000000 / ( currentTick - m_lastRevved );
            
            m_lastRevved = currentTick;
            m_revState = 0;
        }
    } else if( m_quadCount == 233 )
    {
        m_revState = 1;
    }
}
*/
float Sensors::getAvgRPM()
{
    //if( ( us_ticker_read() - m_lastRevved ) > 1000000 )
    if( !moving() )
    {
        return 0;
    }
    float avgrpm = 0.0f;
    for( uint8_t i = 0; i < NUM_AVG; i++ )
    {
        avgrpm += m_avgRPM[i];
    }
    avgrpm /= NUM_AVG;
    return avgrpm;
}