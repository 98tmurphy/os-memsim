#include "mmu.h"

Mmu::Mmu(int memory_size)
{
    _next_pid = 1024;
    _max_size = memory_size;
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
    if (proc != NULL)
    {
        proc->variables.push_back(var);
    }
}

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

void Mmu::removeProcess(uint32_t pid){
    for (int i = 0; i < _processes.size(); i++){
        if(_processes[i]->pid == pid){
            _processes.erase(_processes.begin()+i);
        }
    }
}

void Mmu::removeVariable(uint32_t pid, std::string var){
    for (int i = 0; i < _processes.size(); i++){
        if(_processes[i]->pid == pid){
            for(int j = 0; j < _processes[i]->variables.size(); j++){
                if(_processes[i]->variables[j]->name == var){
                    _processes[i]->variables.erase(_processes[i]->variables.begin()+j);
                }
            }
        }
    }
}

bool Mmu::pidExists(uint32_t pid){
    for(int i = 0; i < _processes.size(); i++){
        if(_processes[i]->pid == pid){
            return true;
        }
    }
    return false;
}

bool Mmu::varExists(uint32_t pid, std::string var){
    for(int i = 0; i < _processes.size(); i++){
        if(_processes[i]->pid == pid){
            for(int j = 0; j < _processes[i]->variables.size(); j++){
                if(_processes[i]->variables[j]->name == var){
                    return true;
                }
            }
        }
    }
    return false;
}

void Mmu::printProcessesIDs(){
    for(int i = 0; i < _processes.size(); i++){
        std::cout << _processes[i]->pid << std::endl;
    }
}

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

uint32_t Mmu::getVirtualAddr(int pid, std::string var){
    for(int i = 0; i < _processes.size(); i++){
        if(_processes[i]->pid == pid){
            for(int j = 0; j < _processes[i]->variables.size();j++){
                if(_processes[i]->variables[j]->name == var){
                    return _processes[i]->variables[j]->virtual_address - _max_size;
                }
            }
        }
    }
    return -1;
}
