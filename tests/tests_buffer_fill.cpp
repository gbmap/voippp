

#include <iostream>
#include <catch.hpp>
#include "../src/buffer_fill.hpp"

using namespace std;

TEST_CASE("BufferFill swaps buffer", "[BufferFill]")
{
    const Uint32 bufferSize = 32;
    BufferFill bufferFill = BufferFill(bufferSize);

    const Uint8* pBuffer = bufferFill.Get();

    // Fill buffer with random data, go up to bufferSize+1
    // to make sure buffer fill swaps buffers.
    Uint8* pData = new Uint8[bufferSize];
    for (int i = 0; i < bufferSize + 1; i++)
    {
        pData[i] = i;
    }

    bufferFill.Push(pData, bufferSize+1);
    const Uint8* pBuffer2 = bufferFill.Get();

    REQUIRE(pBuffer != pBuffer2);
}

TEST_CASE("BufferFill data is correctly stored", "[BufferFill]")
{
    const Uint32 bufferSize = 32;
    BufferFill bufferFill = BufferFill(bufferSize);

    // Fill buffer with random data.
    Uint8* pData = new Uint8[bufferSize*2];
    for (int i = 0; i < bufferSize*2; i++)
    {
        pData[i] = i;
    }


    std::string valuesInData = "";
    std::string valuesInBuffer = "";

    bufferFill.Push(pData, bufferSize);
    const Uint8* pBuffer = bufferFill.Get();
    for (int i = 0; i < bufferSize; i++)
    {
        Uint8 valueInData   = pData[i];
        Uint8 valueInBuffer = pBuffer[i];

        valuesInData   += to_string(valueInData) + " ";
        valuesInBuffer += to_string(valueInBuffer) + " ";
        REQUIRE(valueInData == valueInBuffer);
    }
    cout << "Values in data: " << valuesInData << endl;
    cout << "Values in buffer: " << valuesInBuffer << endl;

    valuesInData = "";
    valuesInBuffer = "";

    bufferFill.Push(pData+(bufferSize), bufferSize);
    pBuffer = bufferFill.Get();
    for (int i = 0; i < bufferSize; i++)
    {
        Uint8 valueInData   = pData[i+bufferSize];
        Uint8 valueInBuffer = pBuffer[i];
        REQUIRE(valueInData == pBuffer[i]);

        // Just for logging purposes
        valuesInData   += to_string(valueInData) + " ";
        valuesInBuffer += to_string(valueInBuffer) + " ";
    }
    cout << "Values in data: " << valuesInData << endl;
    cout << "Values in buffer: " << valuesInBuffer << endl;

    delete pData;
}

TEST_CASE("Iteratively pushing data returns the correct pointer", "[BufferFill]")
{
    const Uint32 bufferSize = 32;
    const Uint32 sizeDenominator = 8; // = 4
    BufferFill bufferFill = BufferFill(bufferSize);

    Uint8 nChunks = bufferSize/sizeDenominator;

    // Fill buffer with random data.
    for (int h = 0; h < nChunks; h++)
    {
        Uint8* pData = new Uint8[sizeDenominator];
        for (int i = 0; i < sizeDenominator; i++)
            pData[i] = 1+i; // 1+i for testing purposes when checking for the first value.

        bufferFill.Push(pData, sizeDenominator);
        
        if (h == nChunks-1) // Returning first buffer, filled with 1+i previously.
            REQUIRE(bufferFill.Get()[0] == 1);
        else // Returning spare buffer, buffer not filled yet.
            REQUIRE(bufferFill.Get()[0] == 0);
    }
}