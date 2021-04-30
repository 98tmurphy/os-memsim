#include "pagetable.h"

PageTable::PageTable(int page_size)
{
    _page_size = page_size;
}

PageTable::~PageTable()
{
}

std::vector<std::string> PageTable::sortedKeys()
{
    std::vector<std::string> keys;

    std::map<std::string, int>::iterator it;
    for (it = _table.begin(); it != _table.end(); it++)
    {
        keys.push_back(it->first);
    }

    std::sort(keys.begin(), keys.end(), PageTableKeyComparator());

    return keys;
}

void PageTable::addEntry(uint32_t pid, int page_number)
{
    // Combination of pid and page number act as the key to look up frame number
    std::string entry = std::to_string(pid) + "|" + std::to_string(page_number);

    int frame = 0; 
    bool notFound = true;
    // Find free frame
    while(notFound){
        if(std::count(usedFrames.begin(), usedFrames.end(), frame) > 0){
            frame++;
        }
        else{
            notFound = false;
        }
    }
    _table[entry] = frame;
    usedFrames.push_back(frame);
}

int PageTable::getPhysicalAddress(uint32_t pid, uint32_t virtual_address)
{
    // Convert virtual address to page_number and page_offset
    int page_number = virtual_address / _page_size;
    int page_offset = virtual_address % _page_size;

    // Combination of pid and page number act as the key to look up frame number
    std::string entry = std::to_string(pid) + "|" + std::to_string(page_number);
    
    // If entry exists, look up frame number and convert virtual to physical address
    int address = -1;
    if (_table.count(entry) > 0)
    {
        int frame = _table[entry];
        address = frame * _page_size + page_offset;
    }

    return address;
}

void PageTable::print()
{
    int i;

    std::cout << " PID  | Page Number | Frame Number" << std::endl;
    std::cout << "------+-------------+--------------" << std::endl;

    std::vector<std::string> keys = sortedKeys();

    char * token;
    std::string temp = "";
    std::string hold = "";
    std::string pageNum;
    int len;
    int frame;

    for (i = 0; i < keys.size(); i++)
    {
        //prints the pid
        char holdPid[keys[i].size()];
        strcpy(holdPid,keys[i].c_str());
        token = strtok(holdPid, "|");
        std::cout << " " << token << " |";

        //prints the page number
        token = strtok(NULL,"|");
        temp = (std::string)token;
        hold = "";
        while(hold.size() + temp.size() != 12){
            hold += " ";
        }
        std::cout << hold << temp << " |";

        //prints the frame number
        temp = "";
        frame = _table[keys[i]];
        hold = std::to_string(frame);
        while(temp.size() + hold.size() != 13){
            temp = temp + " ";
        }
        std::cout << temp << hold << " " << std::endl;
    }
}

uint32_t PageTable::getPageNumber(uint32_t virtualAddr){
    int numOfOffsetBits = (int)log2(_page_size);
    int page = virtualAddr >> numOfOffsetBits;
    if(page == 0){
        return 0;
    }
    else{
        return page + 1;
    }
}

bool PageTable::keyExist(uint32_t pid, uint32_t pageNum){
    std::string entry = std::to_string(pid) + "|" + std::to_string(pageNum);
    if(_table.find(entry) == _table.end()){
        return false;
    }
    return true;
}
