//
// Created by Victoria Gubanova on 30/11/2019.
//

#include "HaffmanCode.h"



void createCodeTable(std::map< unsigned char, std::vector<unsigned char> >& haffmanCodes, HaffmanNode* root, std::vector<unsigned char> bitCode){
    if(root->left != nullptr){
        //BitWriter ptrBit = bitCode;
        std::vector<unsigned char> ptrBit = bitCode;
        ptrBit.push_back('0');
        createCodeTable(haffmanCodes, root->left, ptrBit);
    }

    if(root->right != nullptr){
        std::vector<unsigned char> ptrBit = bitCode;
        ptrBit.push_back('1');
        createCodeTable(haffmanCodes, root->right, ptrBit);
    }

    if(root->left == nullptr && root->right == nullptr){
        haffmanCodes[root->value] = bitCode;
    }

}

void createTreeCode(HaffmanNode* root, BitWriter& bitWriter_){
    if(root->left == nullptr && root->right == nullptr){
        bitWriter_.WriteBit(1);
        bitWriter_.WriteByte(root->value);
    } else{
        if (root->left != nullptr){
            createTreeCode(root->left, bitWriter_);
        }
        if(root->right != nullptr){
            createTreeCode(root->right, bitWriter_);
        }
        bitWriter_.WriteBit(0);
    }
}

void clearTree(HaffmanNode* root){
    if(root->left != nullptr)
        clearTree(root->left);

    if(root->right != nullptr)
        clearTree(root->right);

    delete(root);
}


unsigned char getDecodeLetter(HaffmanNode* root, BitReader& reader){
    HaffmanNode* ptrNode = root;
    while (ptrNode->left != nullptr && ptrNode->right != nullptr)
    {
        unsigned char ptrLetter = reader.readBit();
        if(ptrLetter == 0){
            if(ptrNode->left != nullptr)
                ptrNode = ptrNode->left;
        }
        else if(ptrLetter  == 1){
            if(ptrNode->right != nullptr)
                ptrNode = ptrNode->right;
        }
    }

    return ptrNode->value;
}

void Encode(IInputStream& original, IOutputStream& compressed)
{
    /** 1. Один раз проходимся по входным данным, считая частотности букв алфавита.
       * Так как пройтись придется дважды, надо запомнить весь ввод. **/
    std::vector<unsigned char> textToEncode;
    unsigned char value;
    while (original.Read(value)){
        textToEncode.push_back(value);
    }
    if(textToEncode.empty())
        return;

    //// Считаем частотность символов
    std::map< unsigned char, int > byteFrequency;
    for (size_t i = 0; i < textToEncode.size(); ++i){
        unsigned char currentByte = textToEncode.at(i);

        //Если такой символ уже встречался увеличиваем частоту
        int counterOfLetters = byteFrequency.count(currentByte);
        if(counterOfLetters != 0){
            byteFrequency.at(currentByte) += 1;
        }
            //Иначе просто вставляем новый ключ с частотой 1
        else
        {
            std::pair<unsigned char, int> ptrPair(currentByte, 1);
            byteFrequency.insert(ptrPair);
        }
    }


    int byteFrequencySize = byteFrequency.size();
    ////Создаем узлы дерева Хаффмана и добавляем их в очередь с приоритетом

    std::priority_queue<HaffmanNode , std::vector<HaffmanNode>, comparatorOfHaffmanNodes > priorityQueue;
    for(auto i = byteFrequency.begin(); i != byteFrequency.end(); ++i){
        HaffmanNode node(i->first, i->second);
        priorityQueue.push(node);
    }

    byteFrequency.clear();

    while (priorityQueue.size() > 1){
        HaffmanNode* node1 = new HaffmanNode(
                priorityQueue.top().value,
                priorityQueue.top().frequency,
                priorityQueue.top().left,
                priorityQueue.top().right);

        priorityQueue.pop();
        HaffmanNode* node2 = new HaffmanNode(
                priorityQueue.top().value,
                priorityQueue.top().frequency,
                priorityQueue.top().left,
                priorityQueue.top().right);

        priorityQueue.pop();

        if(node1->frequency >= node2->frequency){
            HaffmanNode *node = new HaffmanNode(
                    ' ',
                    node1->frequency + node2->frequency,
                    node2,
                    node1
            );
            priorityQueue.push(*node);
            delete(node);
        }
        else{
            HaffmanNode *node = new HaffmanNode(
                    ' ',
                    node1->frequency + node2->frequency,
                    node1,
                    node2
            );
            priorityQueue.push(*node);
            delete(node);
        }
    }


    HaffmanNode *rootOfHafmanTree = new HaffmanNode(priorityQueue.top().value, priorityQueue.top().frequency, priorityQueue.top().left, priorityQueue.top().right);

    std::map< unsigned char, std::vector<unsigned char> > haffmanCodes;

    std::vector<unsigned char> path;
    createCodeTable(haffmanCodes, rootOfHafmanTree, path);


    BitWriter compressedData;

    ////Пишем байт с числом символов в алфавите
    compressedData.WriteByte(byteFrequencySize);
    createTreeCode(rootOfHafmanTree, compressedData);


    ////Закодируем само сообщение
    for (int i = 0; i < textToEncode.size(); ++i){
        std::vector<unsigned char> ptr = haffmanCodes.at(textToEncode.at(i));
        for(int m = 0; m < ptr.size(); ++m){
            unsigned char currentBit = ptr.at(m);
            compressedData.WriteBit(currentBit - '0');
        }
    }
    int usefullBits = 8 - (compressedData.GetBuffer().size()*8 - compressedData.GetBitCount());

    for(int i = 0; i < 8 - usefullBits; ++i){
        compressedData.WriteBit(0);
    }

    compressedData.WriteByte(usefullBits);

    for (int i = 0; i < compressedData.GetBuffer().size(); ++i){
        compressed.Write(compressedData.GetBuffer().at(i));
    }

    clearTree(rootOfHafmanTree);

}

void Decode(IInputStream& compressed, IOutputStream& original)
{
    /** 1. Читаем все данные **/
    std::vector<unsigned char> text;
    unsigned char value;
    while (compressed.Read(value)){
        text.push_back(value);
    }


    if(text.empty())
        return;

    int sizeOfAlph = text.at(0);
    int lettersReaded = 0;
    int usefullBits = text.back();

    std::vector<unsigned char> text2 = text;
    text2.pop_back();

    BitReader textToDecode(text2, text2.size()*8 - (8 - usefullBits));
    textToDecode.readByte();

    std::stack<HaffmanNode> nodesOfHaffmanTree;

    while (lettersReaded != sizeOfAlph || nodesOfHaffmanTree.size() != 1 )
    {
        unsigned char currentBit = textToDecode.readBit();

        if(currentBit == 0){

            HaffmanNode *node1 = new HaffmanNode(nodesOfHaffmanTree.top().value, 0,
                                                 nodesOfHaffmanTree.top().left,
                                                 nodesOfHaffmanTree.top().right);
            nodesOfHaffmanTree.pop();

            HaffmanNode *node2 = new HaffmanNode(nodesOfHaffmanTree.top().value, 0,
                                                 nodesOfHaffmanTree.top().left,
                                                 nodesOfHaffmanTree.top().right);
            nodesOfHaffmanTree.pop();

            HaffmanNode *superNode = new HaffmanNode(' ', 0, node2, node1);
            nodesOfHaffmanTree.push(*superNode);

            delete(superNode);


        }
        else if(currentBit == 1){

            unsigned char letter = textToDecode.readByte();
            HaffmanNode *node = new HaffmanNode(letter, 0);
            nodesOfHaffmanTree.push(*node);

            delete(node);
            lettersReaded++;
        }
    }

    HaffmanNode *rootOfHaffmanTree = new HaffmanNode(
            nodesOfHaffmanTree.top().value, 0,
            nodesOfHaffmanTree.top().left,
            nodesOfHaffmanTree.top().right);


    BitWriter decodedText;

    while(textToDecode.hasData())
    {
        decodedText.WriteByte(getDecodeLetter(rootOfHaffmanTree, textToDecode));
    }


    for (int i = 0; i < decodedText.GetBuffer().size(); ++i){
        original.Write(decodedText.GetBuffer().at(i));
    }

    clearTree(rootOfHaffmanTree);
}