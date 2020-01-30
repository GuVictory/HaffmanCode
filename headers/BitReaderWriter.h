//
// Created by Victoria Gubanova on 30/11/2019.
//

#ifndef HAFFMANCODE_BITREADERWRITER_H
#define HAFFMANCODE_BITREADERWRITER_H

class BitReader
{
public:
    BitReader(std::vector<unsigned char> &data, size_t bitCount) : bitPos(0), bitCount(bitCount), data(data) {}

    unsigned char readBit()
    {
        auto val = (data[bitPos/8] >> (7 - bitPos % 8)) & 1;
        bitPos++;
        return val;
    }
    unsigned char readByte()
    {
        unsigned char result = 0;
        if (bitPos % 8 == 0)
        {
            result = data[bitPos/8];
        }
        else
        {
            result = data[bitPos/8] << (bitPos % 8);
            result |= data[bitPos/8 + 1] >> (8 - bitPos % 8);
        }

        bitPos += 8;
        return result;
    }

    bool hasData() const
    {
        return bitPos < bitCount;
    }

    int getBitPos(){
        return bitPos;
    }
private:
    size_t bitPos;
    size_t bitCount;
    std::vector<unsigned char> &data;
};

class BitWriter
{
public:
    BitWriter() : bitCount(0) {}
    void WriteBit(unsigned char bit)
    {
        if (bitCount % 8 == 0)
            buffer.push_back(0);
        if (bit)
        {
            buffer[bitCount/8] |= 1 << (7 - bitCount % 8);
        }
        bitCount++;
    }
    void WriteByte(unsigned char byte)
    {
        if (bitCount % 8 == 0)
            buffer.push_back(byte);
        else
        {
            int offset = bitCount % 8;
            buffer[bitCount/8] |= byte >> offset;
            buffer.push_back(byte << (8 - offset));
        }
        bitCount += 8;
    }

    std::vector<unsigned char> GetBuffer() const
    {
        return buffer;
    }

    size_t GetBitCount() const
    {
        return bitCount;
    }

private:
    std::vector<unsigned char> buffer;
    size_t bitCount;
};


#endif //HAFFMANCODE_BITREADERWRITER_H
