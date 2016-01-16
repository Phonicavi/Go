/**********************************************************************************************
* Copyright (C) 2016 <BadukGo Project>                                                        *
* All rights reserved                                                                         *
*                                                                                             *
* Permission is hereby granted, free of charge, to any person obtaining a copy of             *
* this software and associated documentation files (the “Software”), to deal in the Software  *
* without restriction, including without limitation the rights to use, copy, modify, merge,   *
* publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons  *
* to whom the Software is furnished to do so, subject to the following conditions:            *
*                                                                                             *
* The above copyright notice and this permission notice shall be included in all copies or    *
* substantial portions of the Software.                                                       *
*                                                                                             *
* THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,         *
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR    *
* PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE   *
* FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR        *
* OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER      *
* DEALINGS IN THE SOFTWARE.                                                                   *
**********************************************************************************************/
#ifndef FASTLOG_H
#define FASTLOG_H
#include <limits>
#include <cmath>
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
