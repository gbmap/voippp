
#pragma once

#include "buffer_fill.hpp"

BufferFill::BufferFill(Uint32 bufferSize)
{
    m_bufferSize = bufferSize;
    m_cursor = 0;

    m_pCurrentBuffer = new Uint8[m_bufferSize];
    m_pSpareBuffer = new Uint8[m_bufferSize];

    // memset(0)
    Clear();
}

BufferFill::~BufferFill()
{
    delete[] m_pCurrentBuffer;
    delete[] m_pSpareBuffer;
}

const Uint8* BufferFill::Get() const
{
    return m_pCurrentBuffer;
}

const Uint32 BufferFill::GetBufferSize() const
{
    return m_bufferSize;
}

void BufferFill::Clear()
{
    memset(m_pCurrentBuffer, 0, m_bufferSize);
    memset(m_pSpareBuffer, 0, m_bufferSize);
}

void BufferFill::Push(Uint8* pData, Uint32 dataSize)
{
    // There's still space at spare buffer after pushing data.
    if (m_cursor + dataSize < m_bufferSize)
    {
        memcpy(m_pSpareBuffer + m_cursor, pData, dataSize);
        m_cursor += dataSize;
    }
    else
    {
        int space_left = m_bufferSize - m_cursor;
        memcpy(m_pSpareBuffer + m_cursor, pData, space_left);

        int data_left = dataSize - space_left;
        memcpy(m_pCurrentBuffer, pData + space_left, data_left);

        Swap();
    }
}

void BufferFill::Swap()
{
    Uint8* pTemp = m_pCurrentBuffer;
    m_pCurrentBuffer = m_pSpareBuffer;
    m_pSpareBuffer = pTemp;
}