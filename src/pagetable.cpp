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
    // Find free frame
    // TODO: implement this!

    _table[entry] = frame;
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

void PageTable::terminateProcess(uint32_t pid){
    std::vector<std::string> keys = sortedKeys();
    std::string hold(std::to_string(pid));
    for(int i = 0; i < keys.size(); i++){
        if(keys[i].find(hold) != std::string::npos){
            _table.erase(keys[i]);
        }
    }
}

void PageTable::removeVariable(uint32_t pid, std::string var){
    std::string hold = std::to_string(pid) + "|" + var;
    _table.erase(hold);
}
