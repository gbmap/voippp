#pragma once

#include <SDL2/SDL.h>

/*
    This is a double buffer. Get() returns the currently filled buffer.
    When the spare buffer is full, the buffers are swapped.

    If Get() is called while no buffer has been filled yet, 
    the other empty buffer is returned and you'll get a bunch a zeroes.
*/
class BufferFill
{
public:
    BufferFill(Uint32 bufferSize);
    ~BufferFill();

    const Uint8* Get() const;
    const Uint32 GetBufferSize() const;
    void Push(Uint8* pData, Uint32 dataSize);

    void Clear();
private:

    void Swap();

    Uint32 m_bufferSize;
    Uint8* m_pCurrentBuffer;
    Uint8* m_pSpareBuffer; 

    Uint32 m_cursor;
};