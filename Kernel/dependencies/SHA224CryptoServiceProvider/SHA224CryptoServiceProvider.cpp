#include "SHA224CryptoServiceProvider.h"
#include <cstring>
#include <sstream>
#include <iomanip>
#include <iostream>

SHA224CryptoServiceProvider::SHA224CryptoServiceProvider()
{
    InitialState(H);
}

void SHA224CryptoServiceProvider::InitialState(uint32_t H[])
{
    lengthBits = 0;
    lengthBlock = 0;
    H[0] = 0xC1059ED8;
    H[1] = 0x367CD507;
    H[2] = 0x3070DD17;
    H[3] = 0xF70E5939;
    H[4] = 0xFFC00B31;
    H[5] = 0x68581511;
    H[6] = 0x64F98FA7;
    H[7] = 0xBEFA4FA4;
}
/*
 * Increases the total length of the
 * padded message multiple of 512.
*/
void SHA224CryptoServiceProvider::PadInputMessage()
{
    uint64_t i = lengthBlock;
    uint8_t end = lengthBlock < 56 ? 56 : 64;

    words[i++] = 0x80;
    while (i < end)
    {
        words[i++] = 0x00;
    }

    if(lengthBlock >= 56)
    {
        ProcessBlock();
        memset(words, 0, 56);
    }

    PadTotalInputMessageLength();
    ProcessBlock();
}
/*
 * Add message length at the end of the block.
*/
void SHA224CryptoServiceProvider::PadTotalInputMessageLength()
{
    lengthBits += lengthBlock * 8;
    words[63] = lengthBits;
    words[62] = lengthBits >> 8;
    words[61] = lengthBits >> 16;
    words[60] = lengthBits >> 24;
    words[59] = lengthBits >> 32;
    words[58] = lengthBits >> 40;
    words[57] = lengthBits >> 48;
    words[56] = lengthBits >> 56;
}

uint32_t SHA224CryptoServiceProvider::CircularRightRotate(uint32_t x, uint32_t n) {
    return (x >> n) | (x << (32 - n));
}
/*
 * CH, MAJ, SSIG0, SSIG1, BSIG0, BSIG1 - logical functions, each function
 * operates on 32-bit words, which are represented as x, y, and z.
 * The result of each function is a new 32-bit word.
*/
uint32_t SHA224CryptoServiceProvider::CH(uint32_t x, uint32_t y, uint32_t z)
{
    return (x & y) ^ (~x & z);
}

uint32_t SHA224CryptoServiceProvider::MAJ(uint32_t x, uint32_t y, uint32_t z)
{
    return (x & (y | z)) | (y & z);
}

uint32_t SHA224CryptoServiceProvider::BSIG1(uint32_t x)
{
    return CircularRightRotate(x, 6) ^ CircularRightRotate(x, 11) ^ CircularRightRotate(x, 25);
}

uint32_t SHA224CryptoServiceProvider::BSIG0(uint32_t x)
{
    return CircularRightRotate(x, 2) ^ CircularRightRotate(x, 13) ^ CircularRightRotate(x, 22);
}

uint32_t SHA224CryptoServiceProvider::SSIG0(uint32_t x)
{
    return CircularRightRotate(x, 7) ^ CircularRightRotate(x, 18) ^ (x >> 3);
}

uint32_t SHA224CryptoServiceProvider::SSIG1(uint32_t x)
{
    return CircularRightRotate(x, 17) ^ CircularRightRotate(x, 19) ^ (x >> 10);
}

void SHA224CryptoServiceProvider::ProcessBlock()
{
    uint32_t majRes, resFunc, buffer[64];
    uint32_t state[8];

    for (uint8_t i = 0, j = 0; i < 16; i++, j += 4)
    {
        buffer[i] = (words[j] << 24) | (words[j + 1] << 16) | (words[j + 2] << 8) | (words[j + 3]);
    }

    for (uint8_t k = 16 ; k < 64; k++)
    {
        buffer[k] = SSIG1(buffer[k - 2]) + buffer[k - 7] + SSIG0(buffer[k - 15]) + buffer[k - 16];
    }

    for(uint8_t i = 0 ; i < 8 ; i++)
    {
        state[i] = H[i];
    }

    for (uint8_t i = 0; i < 64; i++)
    {
        majRes   = MAJ(state[0], state[1], state[2]);
        resFunc  = buffer[i] + K[i] + state[7] + CH(state[4], state[5], state[6]) + BSIG1(state[4]);

        state[7] = state[6];
        state[6] = state[5];
        state[5] = state[4];
        state[4] = state[3] + resFunc;
        state[3] = state[2];
        state[2] = state[1];
        state[1] = state[0];
        state[0] = BSIG0(state[0]) + majRes + resFunc;
    }

    for(uint8_t i = 0 ; i < 8 ; i++)
    {
        H[i] += state[i];
    }
}

void SHA224CryptoServiceProvider::Update(const std::string &data)
{
    const auto* buffer = reinterpret_cast<const uint8_t*> (data.c_str());
    size_t length = data.size();

    for (size_t i = 0 ; i < length ; i++)
    {
        words[lengthBlock++] = buffer[i];
        if (lengthBlock == 64) {
            ProcessBlock();
            lengthBits += 512;
            lengthBlock = 0;
        }
    }
}

void SHA224CryptoServiceProvider::ToBigEndian(uint8_t * hash)
{
    for (uint8_t i = 0 ; i < 4 ; i++) {
        for(uint8_t j = 0 ; j < 8 ; j++) {
            hash[i + (j * 4)] = (H[j] >> (24 - i * 8)) & 0x000000ff;
        }
    }
}

std::string SHA224CryptoServiceProvider::FinalProcess()
{
    auto * finalHash = new uint8_t[32];
    PadInputMessage();
    ToBigEndian(finalHash);

    std::stringstream is;
    is << std::setfill('0') << std::hex;

    for(uint8_t i = 0 ; i < 28 ; i++)
    {
        is << std::setw(2) << (unsigned int) finalHash[i];
    }

    return is.str();
}

std::string SHA224CryptoServiceProvider::Hashing(const std::string& inputMessage)
{
    SHA224CryptoServiceProvider sha;
    sha.Update(inputMessage);
    return sha.FinalProcess();
}
