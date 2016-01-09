#ifndef FASTLOG_H
#define FASTLOG_H
#include <limits>
#define MAX_MANTISSA_BITS  23


class FastLog
{
public:
    FastLog(int mantissaBits)
    : m_mantissaBitsDiff(MAX_MANTISSA_BITS - mantissaBits)
    {
        m_lookupTable = new float[1 << mantissaBits];
        IntFloat x;
        x.m_int = 0x3F800000;
        int incr = (1 << m_mantissaBitsDiff);
        int p = static_cast<int>(pow(2.0f, mantissaBits));
        float invLogTwo = 1.f / log(2.f);
        for (int i = 0; i < p; ++i)
        {
            m_lookupTable[i] = log(x.m_float) * invLogTwo;
            x.m_int += incr;
        }
    }
	~FastLog()
    {
        delete[] m_lookupTable;
    }


    float Log(float val) const;

private:
    union IntFloat
    {
        int m_int;

        float m_float;
    };


    const int m_mantissaBitsDiff;

    float* m_lookupTable;
    // FastLog (const FastLog&);

};

inline float FastLog::Log(float val) const
{
    IntFloat x;
    x.m_float = val;
    int logTwo = ((x.m_int >> MAX_MANTISSA_BITS) & 255) - 127;
    x.m_int &= 0x7FFFFF;
    x.m_int >>= m_mantissaBitsDiff;
    return ((m_lookupTable[x.m_int] + float(logTwo)) * 0.69314718f);
}


#endif
