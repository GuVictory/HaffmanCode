//
// Created by Victoria Gubanova on 30/11/2019.
//

#ifndef HAFFMANCODE_HAFFMANCODE_H
#define HAFFMANCODE_HAFFMANCODE_H

#include <iostream>
#include <map>
#include <stack>
#include <queue>
#include <bitset>
#include <string>
#include "BitReaderWriter.h"

// Входной поток
struct IInputStream {
    // Возвращает false, если поток закончился
    virtual bool Read(unsigned char& value) = 0;
};

// Выходной поток
struct IOutputStream {
    virtual void Write(unsigned char value) = 0;
};



struct HaffmanNode{
    int frequency;
    unsigned char value;
    HaffmanNode* left;
    HaffmanNode* right;

    HaffmanNode(unsigned char value_, int frequency_) :
            value(value_), frequency(frequency_), left(nullptr), right(nullptr) {}

    HaffmanNode(unsigned char value_, int frequency_, HaffmanNode* left_, HaffmanNode* right_) :
            value(value_), frequency(frequency_), left(left_), right(right_) {}
};

class comparatorOfHaffmanNodes
{
public:
    int operator() (HaffmanNode& p1, const HaffmanNode& p2)
    {
        return p1.frequency > p2.frequency;
    }
};

void Decode(IInputStream& compressed, IOutputStream& original);
void Encode(IInputStream& original, IOutputStream& compressed);
unsigned char getDecodeLetter(HaffmanNode* root, BitReader& reader);
void clearTree(HaffmanNode* root);
void createTreeCode(HaffmanNode* root, BitWriter& bitWriter_);
void createCodeTable(std::map< unsigned char, std::vector<unsigned char> >& haffmanCodes, HaffmanNode* root, std::vector<unsigned char> bitCode);


#endif //HAFFMANCODE_HAFFMANCODE_H
