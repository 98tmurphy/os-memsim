#include <iostream>
#include <string>
#include <cstring>
#include "mmu.h"
#include "pagetable.h"

void printStartMessage(int page_size);
void createProcess(int text_size, int data_size, Mmu *mmu, PageTable *page_table);
void allocateVariable(uint32_t pid, std::string var_name, DataType type, uint32_t num_elements, Mmu *mmu, PageTable *page_table);
void setVariable(uint32_t pid, std::string var_name, uint32_t offset, void *value, Mmu *mmu, PageTable *page_table, void *memory);
void freeVariable(uint32_t pid, std::string var_name, Mmu *mmu, PageTable *page_table);
void terminateProcess(uint32_t pid, Mmu *mmu, PageTable *page_table);

int main(int argc, char **argv)
{
    // Ensure user specified page size as a command line parameter
    if (argc < 2)
    {
        fprintf(stderr, "Error: you must specify the page size\n");
        return 1;
    }

    // Print opening instuction message
    int page_size = std::stoi(argv[1]);
    printStartMessage(page_size);

    // Create physical 'memory'
    uint32_t mem_size = 67108864;
    void *memory = malloc(mem_size); // 64 MB (64 * 1024 * 1024)

    // Create MMU and Page Table
    Mmu *mmu = new Mmu(mem_size);
    PageTable *page_table = new PageTable(page_size);

    // Prompt loop
    std::string command;
    std::cout << "> ";
    std::getline (std::cin, command);
    std::vector<std::string> tokens;
    std::string token;
    
    while (command != "exit") {
        // Handle command
        // TODO: implement this!

        //splits the command into tokens in the vector, probably should find a better way
        //there could be a lot of errors with this method
        int pos = 0;
        while((pos = command.find(" ")) != std::string::npos){
            token = command.substr(0,pos);
            tokens.push_back(token);
            command.erase(0,pos+1);
        }
        tokens.push_back(command);

        // create command
        if(tokens[0] == "create"){
            int text_size = stoi(tokens[1],0,10);
            int data_size = stoi(tokens[2],0,10);
            createProcess(text_size, data_size, mmu, page_table);
        }

        else if(tokens[0] == "allocate"){
            //allocateVariable(tokens[1],tokens[2],)
        }

        else if(tokens[0] == "set"){

        }

        // print command
        else if(tokens[0] == "print"){
            if(tokens[1] == "mmu"){
                mmu->print();
            }
            else if(tokens[1] == "page"){
                page_table->print();
            }
            else if(tokens[1] == "process"){
                mmu->printProcessesIDs();
            }
            else if(false){ //"<PID:<var_name>", print the value of the variable for that process

            }
            else{
                std::cout << "error: command not recognized" << std::endl;
            }
        }

        else if(tokens[0] == "free"){
            freeVariable(stoi(tokens[1],0,10),tokens[2],mmu,page_table);
        }

        else if(tokens[0] == "terminate"){
            terminateProcess(stoi(tokens[1],0,10), mmu, page_table);
        }

        else{
            std::cout << "error: command not recognized" << std::endl;
        }

        // Get next command
        tokens.clear();
        std::cout << "> ";
        std::getline (std::cin, command);
    }

    // Cean up
    free(memory);
    delete mmu;
    delete page_table;

    return 0;
}

void printStartMessage(int page_size)
{
    std::cout << "Welcome to the Memory Allocation Simulator! Using a page size of " << page_size << " bytes." << std:: endl;
    std::cout << "Commands:" << std:: endl;
    std::cout << "  * create <text_size> <data_size> (initializes a new process)" << std:: endl;
    std::cout << "  * allocate <PID> <var_name> <data_type> <number_of_elements> (allocated memory on the heap)" << std:: endl;
    std::cout << "  * set <PID> <var_name> <offset> <value_0> <value_1> <value_2> ... <value_N> (set the value for a variable)" << std:: endl;
    std::cout << "  * free <PID> <var_name> (deallocate memory on the heap that is associated with <var_name>)" << std:: endl;
    std::cout << "  * terminate <PID> (kill the specified process)" << std:: endl;
    std::cout << "  * print <object> (prints data)" << std:: endl;
    std::cout << "    * If <object> is \"mmu\", print the MMU memory table" << std:: endl;
    std::cout << "    * if <object> is \"page\", print the page table" << std:: endl;
    std::cout << "    * if <object> is \"processes\", print a list of PIDs for processes that are still running" << std:: endl;
    std::cout << "    * if <object> is a \"<PID>:<var_name>\", print the value of the variable for that process" << std:: endl;
    std::cout << std::endl;
}

void createProcess(int text_size, int data_size, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    //   - create new process in the MMU
    //   - allocate new variables for the <TEXT>, <GLOBALS>, and <STACK>
    //   - print pid

    int pid = mmu->createProcess();
    int virAddr = mmu->getNextVirtualAddr(pid);
    mmu->addVariableToProcess(pid,"<TEXT>",DataType::Char,text_size,virAddr);
    page_table->addEntry(pid,0);

    virAddr = mmu->getNextVirtualAddr(pid);
    mmu->addVariableToProcess(pid,"<GLOBALS>",DataType::Char,data_size,virAddr);
    page_table->addEntry(pid,1);

    virAddr = mmu->getNextVirtualAddr(pid);
    mmu->addVariableToProcess(pid,"<STACK>",DataType::Char,65536,virAddr);
    page_table->addEntry(pid,2);
    
    std::cout << pid << std::endl;
}

void allocateVariable(uint32_t pid, std::string var_name, DataType type, uint32_t num_elements, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    //   - find first free space within a page already allocated to this process that is large enough to fit the new variable
    //   - if no hole is large enough, allocate new page(s)
    //   - insert variable into MMU
    //   - print virtual memory address
}

void setVariable(uint32_t pid, std::string var_name, uint32_t offset, void *value, Mmu *mmu, PageTable *page_table, void *memory)
{
    // TODO: implement this!
    //   - look up physical address for variable based on its virtual address / offset
    //   - insert `value` into `memory` at physical address
    //   * note: this function only handles a single element (i.e. you'll need to call this within a loop when setting
    //           multiple elements of an array)
}

void freeVariable(uint32_t pid, std::string var_name, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    //   - remove entry from MMU
    //   - free page if this variable was the only one on a given page
    if(!mmu->pidExists(pid)){
        std::cout << "error: process not found" << std::endl;
    }
    else if(!mmu->varExists(pid,var_name)){
        std::cout << "error: variable not found" << std::endl;
    }
    else{
        mmu->removeVariable(pid,var_name);
        page_table->removeVariable(pid,var_name);
    }
}

void terminateProcess(uint32_t pid, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    //   - remove process from MMU
    //   - free all pages associated with given process
    if(mmu->pidExists(pid)){
        mmu->removeProcess(pid);
        page_table->terminateProcess(pid);
    }
    else{
        std::cout << "error: process not found" << std::endl;
    }
}
