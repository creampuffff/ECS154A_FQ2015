/* 
Capacity: 512 = 8 * 64
4 ways 16 sets: 64 / 4 = 2^4(set)
Block Size: 8 = 2^3 (offset)
Tag Size: 2^(16 - 3 - 4) = 2^9(tag)

  tag   set   offset
|  9  |  4  |   3    |
*/

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <cmath>

using namespace std;

const int cacheSize = 64, way = 4, ramSize = 65536;

class CacheLine
{
  public:
    int tag;
    int count;
    bool dirty;
    string data[8];

    CacheLine()
    {
        int i = 0;
        count = 0;
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
        //cout << "current temp: " << temp << endl;
    }
    //cout << "i value: " << i << endl;
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
    CacheLine cache[cacheSize][way]; // 64
    Memory *ram = new Memory[ramSize]; // 65536
    string address;
    int ad = 0;
    stringstream str;
    string addressTemp;
    string readOrWrite;
    string data;
    int num = 0;
    int largest = 0;
    string tag;
    string set;
    string offs;
    int tagTemp = 0;
    int setTemp = 0;
    int offsetTemp = 0;
    bool dirty;

    ifstream infile;
    ofstream outfile;

    infile.open(argv[1]);
    outfile.open("sa-out.txt");

    
    while(infile >> address >> readOrWrite >> data)
    {  
        str << hex << address;
        str >> ad;

        addressTemp = hexTobinary(address);


        tag = addressTemp.substr(0, 9);
        set = addressTemp.substr(9, 4);
        offs = addressTemp.substr(13, 3);

        tagTemp = binaryTodec(tag);
        setTemp = binaryTodec(set);
        offsetTemp = binaryTodec(offs);
        
        /*cout << "address: " << address << endl;
        cout << "addressTemp: " << addressTemp << endl;
        cout << "Tag: "  << tag << endl;         
        cout << "set: "  << set << endl;
        cout << "offset: "  << offs << endl;
        cout << "tag temp: " <<  tagTemp << endl;         
        cout << "set temp: " << setTemp << endl;
        cout << "offset temp: " << offsetTemp << endl << endl;*/
        
        for(i = 0; i < way; i++)
        {
            cache[setTemp][i].count++;
            // cout << "count: " << cache[setTemp][i].count << " ";
        } // 0 0 0 0 -> 1 1 1 1
        //cout << endl;
 
        //cout << "give value" << endl;
        bool hit = false;

        for(i = 0; i < way; i++)
        {   
            if(cache[setTemp][i].tag == tagTemp)
            {
                hit = true;
                num = i;
                cache[setTemp][i].count = 0;
                break;
            }
        }

        if(hit == false)
        {
            largest = 0;
            num = 0;

            for(i = 0; i < way; i++)
            {
                if(largest <= cache[setTemp][i].count)
                {
                    largest = cache[setTemp][i].count;
                    num = i;
                }   
            }
            dirty = cache[setTemp][num].dirty;
            cache[setTemp][num].count = 0;
            //cout << "Done with LRU" << endl;

            if (cache[setTemp][num].dirty == true)
            {
                for(i = 0; i < 8; i++)
                {
                    ram[cache[setTemp][num].tag * 16 + setTemp].block[i] = cache[setTemp][num].data[i];
                    //cout << "(cache to ram) = " << "ram[" << cache[setTemp][num].tag << "].block[" << i << "] = " << ram[cache[setTemp][num].tag].block[i] << endl;
                }
                cache[setTemp][num].dirty = false; 
            }
            cache[setTemp][num].tag = tagTemp;
            
            for (i = 0; i < 8; i++)
            {
                cache[setTemp][num].data[i] = ram[cache[setTemp][num].tag * 16 + setTemp].block[i];
                //cout << "(ram to cache) = " << "cache[" << setTemp << "][" << num << "].data" << "[" << i << "] = " << cache[setTemp][num].data[i] << endl;
            }
        }
        else if(hit == true)
        {
            dirty = cache[setTemp][num].dirty;
        }

        if(readOrWrite == "FF")
        {
            cache[setTemp][num].data[offsetTemp] = data;
            cache[setTemp][num].dirty = true;
        }
        else if(readOrWrite == "00")
        {
            outfile << uppercase << address << " ";
    
            for(i = 7; i >= 0; i--)
                outfile << uppercase << setfill('0') << cache[setTemp][num].data[i];
    
            outfile << " " << hit << " " << dirty << endl;

            cout << uppercase << address << " ";
    
            for(i = 7; i >= 0; i--)
                cout << uppercase << setfill('0') << cache[setTemp][num].data[i];
    
            cout << " " << hit << " " << dirty << endl;

            /*cout << uppercase << address << " ";
    
            for(i = 7; i >= 0; i--)
                cout << uppercase << setfill('0') << "cache[" << setTemp << "][" << num << "].data" << "[" << i << "] = " << cache[setTemp][num].data[i] << endl;*/
        }   
    }
    infile.close(); 
    return 0;
}
