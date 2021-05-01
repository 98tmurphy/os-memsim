#ifndef __MMU_H_
#define __MMU_H_

#include <iostream>
#include <string>
#include <vector>

enum DataType : uint8_t {FreeSpace, Char, Short, Int, Float, Long, Double};

typedef struct Variable {
    std::string name;
    DataType type;
    uint32_t virtual_address;
    uint32_t size;
} Variable;

typedef struct Process {
    uint32_t pid;
    std::vector<Variable*> variables;
} Process;

class Mmu {
private:
    uint32_t _next_pid;
    uint32_t _max_size;
    std::vector<Process*> _processes;
    uint32_t memoryUsed;

public:
    Mmu(int memory_size);
    ~Mmu();

    uint32_t createProcess();
    void addVariableToProcess(uint32_t pid, std::string var_name, DataType type, uint32_t size, uint32_t address);
    void print();
    void removeProcess(uint32_t pid);
    void removeVariable(uint32_t pid, std::string var);
    void printProcessesIDs();
    int getNextVirtualAddr(int pid);
    uint32_t getVirtualAddr(int pid, std::string var);
    uint32_t getVirMemUsed(uint32_t pid);
    uint32_t getVirAddressOfFreeSpace(uint32_t pid,uint32_t sizeOfVariable);
    uint32_t removeFreeSpace(uint32_t pid,uint32_t virMemAddr,uint32_t sizeOfVariable, std::string var_name, DataType type);
    uint32_t getSizeOfFreeSpace(uint32_t pid,uint32_t virMemAddr);
    void removeFirstFree(uint32_t pid);
    uint32_t getLargestVirtualAddress(uint32_t pid);
    bool containsPid(uint32_t pid);
    bool spaceInMemory(uint32_t size);
    std::vector<uint32_t> getFreeSpaceRanges(uint32_t pid);
};

#endif // __MMU_H_
