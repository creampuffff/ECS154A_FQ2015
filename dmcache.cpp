/* 
Capacity: 512 = 8 * 64
Index Size: 64 = 2^6 (index)
Block Size: 8 = 2^3 (offset)
Tag Size: 2^(16 - 3 - 6) = 2^7(tag)

  tag  index  offset
|  7  |  6  |   3   |
*/

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <cmath>

using namespace std;

const int cacheSize = 64, ramSize = 65536;

class CacheLine
{
  public:
    int tag;
    bool dirty;
    string data[8];

    CacheLine()
    {
        int i = 0;
        tag = 0;
        dirty = false;

        for (i = 0; i < 8; i++) 
            data[i] = "00";
    } 
};

class Memory
{
  public:
    string block[8];

    Memory()
    {
        int i = 0;

        for(i = 0; i < 8; i++)
            block[i] = "00";
    }
};

string hexTobinary(string hex)
{
    int i = 0;
    string temp = "";

    for(i = 0; i < hex.length(); i++)
    {
        switch(hex[i])
        {
            case '0': 
                temp.append("0000"); 
                break;
            case '1': 
                temp.append("0001"); 
                break;
            case '2': 
                temp.append("0010"); 
                break;
            case '3': 
                temp.append("0011"); 
                break;
            case '4': 
                temp.append("0100"); 
                break;
            case '5': 
                temp.append("0101"); 
                break;
            case '6': 
                temp.append("0110"); 
                break;
            case '7': 
                temp.append("0111"); 
                break;  
            case '8': 
                temp.append("1000"); 
                break;
            case '9': 
                temp.append("1001"); 
                break;
            case 'A': 
                temp.append("1010"); 
                break;
            case 'B': 
                temp.append("1011"); 
                break;
            case 'C': 
                temp.append("1100"); 
                break;
            case 'D': 
                temp.append("1101"); 
                break;
            case 'E': 
                temp.append("1110"); 
                break;
            case 'F': 
                temp.append("1111"); 
                break;
        }
    }
    return temp;
}

int binaryTodec(string bin)
{
    int temp;
    int rem, base = 1;
    int dec = 0;

    istringstream(bin) >> temp;

    while (temp > 0)
    {
        rem = temp % 10;
        dec = dec + rem * base;
        base = base * 2;
        temp = temp / 10;
    }

    return dec;
}

int main(int argc, char* argv[])
{
    int i = 0;
    CacheLine *cache = new CacheLine[cacheSize]; // 64
    Memory *ram = new Memory[ramSize]; // 65536
    string address;
    stringstream str;
    string addressTemp;
    string readOrWrite;
    string data;
    int num = 0;
    string tag;
    string index;
    string offs;
    int tagTemp;
    int indexTemp;
    int offsetTemp;
    int hit;
    bool dirty;

    ifstream infile;
    ofstream outfile;

    infile.open(argv[1]);
    outfile.open("dm-out.txt");

    
    while(infile >> address >> readOrWrite >> data)
    {      
        addressTemp = hexTobinary(address);

        tag = addressTemp.substr(0, 7);
        index = addressTemp.substr(7, 6);
        offs = addressTemp.substr(13, 3);

        tagTemp = binaryTodec(tag);
        indexTemp = binaryTodec(index);
        offsetTemp = binaryTodec(offs);
        
        if(cache[indexTemp].tag == tagTemp)
        {
            hit = 1;
        }
        else
        {
            hit = 0;
        }
        
        dirty = cache[indexTemp].dirty;

        if(hit == 0)
        {
            if (cache[indexTemp].dirty == true)
            {
                for(i = 0; i < 8; i++)
                {
                    ram[cache[indexTemp].tag].block[i] = cache[indexTemp].data[i];
                }
                cache[indexTemp].dirty = false; 
            }
            cache[indexTemp].tag = tagTemp;
            for (i = 0; i < 8; i++)
            {
                cache[indexTemp].data[i] = ram[cache[indexTemp].tag].block[i];
            }
        }

        if(readOrWrite == "FF")
        {
            cache[indexTemp].data[offsetTemp] = data;
            cache[indexTemp].dirty = true;
        }
        else if(readOrWrite == "00")
        {
            outfile << uppercase << address << " ";
    
            for(i = 7; i >= 0; i--)
                outfile << uppercase << setfill('0') << cache[indexTemp].data[i];
    
            outfile << " " << hit << " " << dirty << endl;
        }
    }

    infile.close(); 
    return 0;
}
