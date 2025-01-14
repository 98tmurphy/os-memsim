#include "mmu.h"

Mmu::Mmu(int memory_size)
{
    _next_pid = 1024;
    _max_size = memory_size;
    memoryUsed = 0;
}

Mmu::~Mmu()
{
}

uint32_t Mmu::createProcess()
{
    Process *proc = new Process();
    proc->pid = _next_pid;

    Variable *var = new Variable();
    var->name = "<FREE_SPACE>";
    var->type = DataType::FreeSpace;
    var->virtual_address = 0;
    var->size = _max_size;
    proc->variables.push_back(var);

    _processes.push_back(proc);

    _next_pid++;
    return proc->pid;
}

//add variable to the process
void Mmu::addVariableToProcess(uint32_t pid, std::string var_name, DataType type, uint32_t size, uint32_t address)
{
    int i;
    Process *proc = NULL;
    for (i = 0; i < _processes.size(); i++)
    {
        if (_processes[i]->pid == pid)
        {
            proc = _processes[i];
        }
    }

    Variable *var = new Variable();
    var->name = var_name;
    var->type = type;
    var->virtual_address = address;
    var->size = size;
    if(var_name != "<FREE_SPACE>"){
        memoryUsed += size;
    }
    if (proc != NULL)
    {
        proc->variables.push_back(var);
    }
}

//print
void Mmu::print()
{
    int i, j;

    std::cout << " PID  | Variable Name | Virtual Addr | Size" << std::endl;
    std::cout << "------+---------------+--------------+------------" << std::endl;
    for (i = 0; i < _processes.size(); i++)
    {
        for (j = 0; j < _processes[i]->variables.size(); j++)
        {
            if(_processes[i]->variables[j]->name != "<FREE_SPACE>"){
                std::cout << " " << _processes[i]->pid << " | ";

                std::string hold = _processes[i]->variables[j]->name;
                while(hold.size() != 14){
                    hold = hold + " ";
                }
                std::cout << hold << "|   ";

                printf("0x%08x",_processes[i]->variables[j]->virtual_address);
                std::cout << " | ";

                std::string temp = std::to_string(_processes[i]->variables[j]->size);
                hold = "";
                for(int k = 0; k < 11 - temp.size(); k++){
                    hold = hold + " ";
                }
                std:: cout << hold << temp << std::dec << std::endl;
            }
        }
    }
}

//used to terminate the process
void Mmu::removeProcess(uint32_t pid){
    for (int i = 0; i < _processes.size(); i++){
        if(_processes[i]->pid == pid){
            for(int j = 0; j < _processes[i]->variables.size(); j++){
                if(_processes[i]->variables[j]->name != "<FREE_SPACE>"){
                    memoryUsed = memoryUsed - _processes[i]->variables[j]->size; //decrease amount of space used
                }
            }
            _processes.erase(_processes.begin()+i);
        }
    }
}

//used in the creation of a process, removes the initial free space
void Mmu::removeFirstFree(uint32_t pid){
    for(int i=0; i < _processes.size(); i++){
        for(int j=0; j < _processes[i]->variables.size();j++){
            if(_processes[i]->variables[j]->virtual_address == 0 && _processes[i]->variables[j]->name == "<FREE_SPACE>"){
                _processes[i]->variables.erase(_processes[i]->variables.begin());
            }
        }
    }
}

//Replaces a free variable, and replaces it with a free space
void Mmu::removeVariable(uint32_t pid, std::string var){
    for (int i = 0; i < _processes.size(); i++){
        if(_processes[i]->pid == pid){
            uint32_t size = _processes[i]->variables.size();
            for(int j = 0; j < size; j++){
                //remove any 0 size free spaces
                if(_processes[i]->variables[j]->name == "<FREE_SPACE>" && _processes[i]->variables[j]->size == 0){
                    _processes[i]->variables.erase(_processes[i]->variables.begin() + j);
                    j--;
                }
                //change the variable
                else if(_processes[i]->variables[j]->name == var){
                    _processes[i]->variables[j]->name = "<FREE_SPACE>";
                    _processes[i]->variables[j]->type = DataType::FreeSpace;
                    memoryUsed = memoryUsed - _processes[i]->variables[j]->size;
                }
            }
        }
    }
}

//prints the ID's of all processes
void Mmu::printProcessesIDs(){
    for(int i = 0; i < _processes.size(); i++){
        std::cout << _processes[i]->pid << std::endl;
    }
}

//Address just virtual address when creating a new process
int Mmu::getNextVirtualAddr(int pid){
    int sumOfAddrs = -1 * _max_size;
    for(int i = 0; i < _processes.size(); i++){
        if(_processes[i]->pid == pid){
            for(int j = 0; j < _processes[i]->variables.size();j++){
                sumOfAddrs += _processes[i]->variables[j]->size;
            }
        }
    }
    return sumOfAddrs;
}

//Gets the virtual address of the inputted pid and variable
uint32_t Mmu::getVirtualAddr(int pid, std::string var){
    for(int i = 0; i < _processes.size(); i++){
        if(_processes[i]->pid == pid){
            for(int j = 0; j < _processes[i]->variables.size();j++){
                if(_processes[i]->variables[j]->name == var){
                    return _processes[i]->variables[j]->virtual_address;
                }
            }
        }
    }
    return -1;
}


//Get the virtual address of a free space that is big enough to hold the size of the variable
uint32_t Mmu::getVirAddressOfFreeSpace(uint32_t pid,uint32_t sizeOfVariable){
    uint32_t virAddress = -1;
    for(int i = 0; i < _processes.size(); i++){
        if(_processes[i]->pid == pid){
            for(int j = 0; j < _processes[i]->variables.size();j++){
                if(_processes[i]->variables[j]->name == "<FREE_SPACE>" && _processes[i]->variables[j]->size >= sizeOfVariable){
                    virAddress = _processes[i]->variables[j]->virtual_address;
                    break;
                }
            }
        }
    }
    return virAddress;
}

//Will input the new variable into the free space, and fill left over space with a free space
uint32_t Mmu::removeFreeSpace(uint32_t pid,uint32_t virMemAddr,uint32_t sizeOfVariable, std::string var_name, DataType type){
    memoryUsed += sizeOfVariable;
    std::vector<Variable*> newVariables;
    for(int i = 0; i < _processes.size(); i++){
        if(_processes[i]->pid == pid){
            for(int j = 0; j < _processes[i]->variables.size(); j++){
                uint32_t size = _processes[i]->variables[j]->size;

                //if found an open free space
                if(_processes[i]->variables[j]->virtual_address == virMemAddr){
                    _processes[i]->variables[j]->name = var_name;
                    _processes[i]->variables[j]->type = type;
                    _processes[i]->variables[j]->size = sizeOfVariable;

                    //create the new free space
                    Variable *newFree = new Variable();
                    newFree->name = "<FREE_SPACE>";
                    newFree->type = DataType::FreeSpace;
                    newFree->size = size - _processes[i]->variables[j]->size;
                    newFree->virtual_address = virMemAddr+sizeOfVariable;

                    newVariables.push_back(_processes[i]->variables[j]);
                    newVariables.push_back(newFree);
                }
                else{
                    newVariables.push_back(_processes[i]->variables[j]);
                }
            }
            _processes[i]->variables = newVariables;
        }
    }
    return 0;
}

//return the size of the free space with a virtual mem of virMemAddr, if not found return -1
uint32_t Mmu::getSizeOfFreeSpace(uint32_t pid, uint32_t virMemAddr){
    for(int i = 0; i < _processes.size(); i++){
        if(_processes[i]->pid == pid){
            for(int j = 0; j < _processes[i]->variables.size(); j++){
                if(_processes[i]->variables[j]->virtual_address == virMemAddr){
                    return _processes[i]->variables[j]->size;
                }
            }
        }
    }
    return -1;
}

//Find the process with the inputted pid that has the highest virtual address
uint32_t Mmu::getLargestVirtualAddress(uint32_t pid){
    int maxSoFar = 0;
    for(int i = 0; i < _processes.size(); i++){
        if(_processes[i]->pid == pid){
            for(int j = 0; j < _processes[i]->variables.size(); j++){
                if(maxSoFar < _processes[i]->variables[j]->virtual_address + _processes[i]->variables[j]->size){
                    maxSoFar = _processes[i]->variables[j]->virtual_address + _processes[i]->variables[j]->size;
                }
            }
        }
    }
    return maxSoFar;
}

//returns true if the mmu has the process number
bool Mmu::containsPid(uint32_t pid){
    for(int index = 0; index < _processes.size(); index++){
        if(_processes[index]->pid == pid){
            return true;
        }
    }
    return false;
}

//returns true if there is space in memory
bool Mmu::spaceInMemory(uint32_t size){
    return memoryUsed + size <= _max_size;
}


//returns the ranges of the free spaces in a vector of ints
//first index is the start virmem, and and the second index is the ending virmem
//this will include all free spaces
std::vector<uint32_t> Mmu::getFreeSpaceRanges(uint32_t pid){
    std::vector<uint32_t> result;
    uint32_t holdVirAddr;
    for(int i = 0; i < _processes.size(); i++){
        if(_processes[i]->pid == pid){
            for(int j = 0; j < _processes[i]->variables.size(); j++){
                if(_processes[i]->variables[j]->name == "<FREE_SPACE>" && j != _processes[i]->variables.size()-1){
                    result.push_back(_processes[i]->variables[j]->virtual_address);
                    holdVirAddr = _processes[i]->variables[j]->virtual_address + (_processes[i]->variables[j]->size - 1);
                    while(_processes[i]->variables[j]->name == "<FREE_SPACE>"){
                        if(j == _processes[i]->variables.size() - 1){
                            holdVirAddr += 32768;//max size of a page size
                            break;
                        }
                        else{
                            holdVirAddr += _processes[i]->variables[j]->size;
                            j++;
                        }
                    }
                    result.push_back(holdVirAddr);
                }
            }
        }
    }
    return result;
}

DataType Mmu::getDataType(uint32_t pid, std::string var){
    for(int i = 0; i < _processes.size(); i++){
        if(_processes[i]->pid == pid){
            for(int j = 0; j < _processes[i]->variables.size(); j++){
                if(_processes[i]->variables[j]->name == var){
                    return _processes[i]->variables[j]->type;
                }
            }
        }
    }
    return DataType::Char;
}
