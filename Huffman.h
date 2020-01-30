#pragma once


#include <vector>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Входной поток
class CInputStream : public IInputStream {
public:
    CInputStream(const vector<unsigned char>& _body) : index(0), body(_body) {}

    virtual bool Read(unsigned char& value);

private:
    unsigned int index;
    const vector<unsigned char>& body;
};

bool CInputStream::Read(unsigned char& value)
{
    if (index >= body.size()) {
        return false;
    }

    value = body[index++];
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Выходной поток
class COutputStream : public IOutputStream {
public:
    COutputStream(vector<unsigned char>& _body) : body(_body) { body.clear(); }
    virtual void Write(unsigned char value) { body.push_back(value); }

private:
    vector<unsigned char>& body;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Основное приложение

// Считываем все файлы, что перечислены во входе
void fillInputs(vector< vector<unsigned char> >& inputs)
{
    inputs.clear();

    string currentFileName;
    getline( cin, currentFileName );

        // Есть еще один файл, который следует закодировать
        inputs.push_back(vector<unsigned char>());

        ifstream file;
        file.open(currentFileName.c_str());

        unsigned char value;
        while (file >> value) {
            (*inputs.rbegin()).push_back(value);
        }

}

bool isEqual(const vector<unsigned char>& v1, const vector<unsigned char>& v2)
{
    if (v1.size() != v2.size()) {
/*cout << v1.size() << " ";
for( int i =v1.size() - 5; i < v1.size(); i++ ) { cout << v1[i]; }
cout << "\n" << v2.size() << " ";
for( int i =v2.size() - 5; i < v2.size(); i++ ) { cout << v2[i]; }*/
        return false;
    }

    for( unsigned int i = 0; i < v1.size(); i++ ) {
        if( v1[i] != v2[i] ) {
/*cout << "i: " << i << " ";
for( int j =i; j < i + 5; j++ ) { cout << v1[j]; }
cout << "\n";
for( int j =i; j < i + 5; j++ ) { cout << v2[j]; }*/
            return false;
        }
    }

    return true;
}

int calculateSize(const vector< vector<unsigned char> >& array)
{
    int result = 0;
    for ( unsigned int i  = 0; i < array.size(); i++ ) {
        const vector<unsigned char>& data = array[i];
        result += data.size();
    }

    return result;
}

int main()
{
    // Получаем данные, которые нужно закодировать
    vector< vector<unsigned char> > input;

    fillInputs(input);

    // Сжимаем данные
    vector< vector<unsigned char> > compressed;
    compressed.resize(input.size());
    for (unsigned int i = 0; i < input.size(); i++) {
        CInputStream iStream(input[i]);
        COutputStream oStream(compressed[i]);
        Encode(iStream, oStream);
    }

    // Распаковываем сжатые данные и проверяем, что они совпадают с оригиналом
    for (unsigned int i = 0; i < input.size(); i++) {
        vector<unsigned char> output;
        CInputStream iStream(compressed[i]);
        COutputStream oStream(output);
        Decode(iStream, oStream);
        if (!isEqual(input[i], output)) {
            cout << -1;
            return 0;
        }
    }

    // Вычисляем степень сжатия
    cout << (100. * calculateSize(compressed) / calculateSize(input));

    return 0;
}
