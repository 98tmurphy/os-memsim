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
uint32_t calculateSize(DataType type,uint32_t num_elements);
bool isNumber(std::string input);

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
        int pos = 0;

        //splits the string into smaller strings
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

        //allocate command
        else if(tokens[0] == "allocate"){
            if(!isNumber(tokens[1])){
                std::cout << "error: process not found" << std::endl;
            }
            else if(!mmu->containsPid(stoi(tokens[1],0,10))){
                std::cout << "error: process not found" << std::endl;
            }
            else if(mmu->getVirtualAddr(stoi(tokens[1],0,10),tokens[2]) != -1){
                std::cout << "error: variable already exists" <<std::endl;
            }
            else if(tokens[3] == "char"){
                allocateVariable(stoi(tokens[1],0,10),tokens[2],DataType::Char,stoi(tokens[4],0,10),mmu,page_table);
            }
            else if(tokens[3] == "short"){
                allocateVariable(stoi(tokens[1],0,10),tokens[2],DataType::Short,stoi(tokens[4],0,10),mmu,page_table);
            }
            else if(tokens[3] == "int"){
                allocateVariable(stoi(tokens[1],0,10),tokens[2],DataType::Int,stoi(tokens[4],0,10),mmu,page_table);
            }
            else if(tokens[3] == "float"){
                allocateVariable(stoi(tokens[1],0,10),tokens[2],DataType::Float,stoi(tokens[4],0,10),mmu,page_table);
            }
            else if(tokens[3] == "double"){
                allocateVariable(stoi(tokens[1],0,10),tokens[2],DataType::Double,stoi(tokens[4],0,10),mmu,page_table);
            }
            else if(tokens[3] == "long"){
                allocateVariable(stoi(tokens[1],0,10),tokens[2],DataType::Long,stoi(tokens[4],0,10),mmu,page_table);
            }
        }

        //set command
        else if(tokens[0] == "set"){//never implemented
            if(!isNumber(tokens[1])){
                std::cout << "error: process not found" << std::endl;
            }
            else if(!mmu->containsPid(stoi(tokens[1],0,10))){
                std::cout << "error: process not found" << std::endl;
            }
            else if(mmu->getVirtualAddr(stoi(tokens[1],0,10),tokens[2]) == -1){
                std::cout << "error: variable not found" <<std::endl;
            }
            else{
                for(int index = 3; index < tokens.size(); index++){
                    //setVariable(stoi(tokens[1],0,10),tokens[2],(##NEED THE OFFSET##),tokens[index],mmu,page_table);
                }
            }
        }

        // print command
        else if(tokens[0] == "print"){
            if(tokens[1] == "mmu"){
                mmu->print();
            }
            else if(tokens[1] == "page"){
                page_table->print();
            }
            else if(tokens[1] == "processes"){
                mmu->printProcessesIDs();
            }
            else if(tokens[1].find(":") != std::string::npos){ //"<PID:<var_name>", print the value of the variable for that process
                
            }
            else{
                std::cout << "error: command not recognized" << std::endl;
            }
        }

        //free command
        else if(tokens[0] == "free"){
            if(!isNumber(tokens[1])){
                std::cout << "error: process not found" << std::endl;
            }
            else if(!mmu->containsPid(stoi(tokens[1],0,10))){
                std::cout << "error: process not found" << std::endl;
            }
            else if(mmu->getVirtualAddr(stoi(tokens[1],0,10),tokens[2]) == -1){
                std::cout << "error: variable not found" <<std::endl;
            }
            else{
                freeVariable(stoi(tokens[1],0,10),tokens[2],mmu,page_table);
            }
        }

        //terminate command
        else if(tokens[0] == "terminate"){
            if(!isNumber(tokens[1])){
                std::cout << "error: process not found" << std::endl;
            }
            else{
                terminateProcess(stoi(tokens[1],0,10), mmu, page_table);
            }
        }

        //error checking
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
    uint32_t sumOfSpace = 0;
    uint32_t startPage;
    uint32_t endPage;

    int pid = mmu->createProcess(); //create process


    //create the new variables and add them to mmu
    int virAddr = mmu->getNextVirtualAddr(pid);
    mmu->addVariableToProcess(pid,"<TEXT>",DataType::Char,text_size,virAddr);
    sumOfSpace += text_size;
    startPage = 0;

    virAddr = mmu->getNextVirtualAddr(pid);
    mmu->addVariableToProcess(pid,"<GLOBALS>",DataType::Char,data_size,virAddr);
    sumOfSpace += data_size;

    virAddr = mmu->getNextVirtualAddr(pid);
    mmu->addVariableToProcess(pid,"<STACK>",DataType::Char,65536,virAddr);
    sumOfSpace += 65536;

    //add correct number of page numbers
    endPage = page_table->getPageNumber(sumOfSpace - 1);
    for(int i = 0; i <= endPage; i++){
        page_table->addEntry(pid,i);
    }

    mmu->removeFirstFree(pid);//remove the initial large free space

    mmu->addVariableToProcess(pid,"<FREE_SPACE>",DataType::FreeSpace,67108864-sumOfSpace,sumOfSpace);
    
    std::cout << pid << std::endl;
}

void allocateVariable(uint32_t pid, std::string var_name, DataType type, uint32_t num_elements, Mmu *mmu, PageTable *page_table)
{
    uint32_t sizeOfVariable = calculateSize(type,num_elements);
    uint32_t sizeOfType = calculateSize(type,1);
    uint32_t amountStored = 0;
    if(mmu->spaceInMemory(sizeOfVariable)){
        std::cout << mmu->getVirAddressOfFreeSpace(pid,sizeOfType) << std::endl;
        while(amountStored != num_elements){
            uint32_t virMemAddr = mmu->getVirAddressOfFreeSpace(pid,sizeOfType);
            uint32_t sizeOfFreeSpace = mmu->getSizeOfFreeSpace(pid,virMemAddr);
            uint32_t usableSpace = (sizeOfFreeSpace-(sizeOfFreeSpace % sizeOfType));
            uint32_t startPage = page_table->getPageNumber(virMemAddr);
            uint32_t endPage = 0;
            if(usableSpace < sizeOfVariable){
                mmu->removeFreeSpace(pid,virMemAddr,usableSpace,var_name,type);
                amountStored += usableSpace / sizeOfType;
                sizeOfVariable = sizeOfVariable - usableSpace;
                endPage = page_table->getPageNumber(virMemAddr + usableSpace -1);
            }
            else{
                mmu->removeFreeSpace(pid,virMemAddr,sizeOfVariable,var_name,type);
                amountStored = num_elements;
                endPage = page_table->getPageNumber(virMemAddr + sizeOfVariable -1);
            }
            for(int i = startPage; i <= endPage; i++){
                if(!page_table->keyExist(pid,i)){
                    page_table->addEntry(pid,i);
                }
            }
        }
    }
    else{
        std::cout << "error: allocation would exceed system memory" << std::endl;
    }
}

void setVariable(uint32_t pid, std::string var_name, uint32_t offset, void *value, Mmu *mmu, PageTable *page_table, void *memory)
{
    // TODO: implement this!
    //   - look up physical address for variable based on its virtual address / offset
    //   - insert `value` into `memory` at physical address
    //   * note: this function only handles a single element (i.e. you'll need to call this within a loop when setting
    //           multiple elements of an array)
    uint32_t virAddr = mmu->getVirtualAddr(pid,var_name);
    uint32_t phyAddr = page_table->getPhysicalAddress(pid,virAddr+offset);
    std::cout << typeid(value).name() << std::endl;
    //memcpy(memory,value,)
}

void freeVariable(uint32_t pid, std::string var_name, Mmu *mmu, PageTable *page_table)
{
    if(!mmu->pidExists(pid)){
        std::cout << "error: process not found" << std::endl;
    }
    else if(!mmu->varExists(pid,var_name)){
        std::cout << "error: variable not found" << std::endl;
    }
    else{
        uint32_t virtualAddr = mmu->getVirtualAddr(pid,var_name);
        mmu->removeVariable(pid,var_name);
        std::vector<uint32_t> freeSpaceRanges = mmu->getFreeSpaceRanges(pid);
        std::vector<uint32_t> pages;
        std::cout << freeSpaceRanges[0] << " " << freeSpaceRanges[1] << std::endl;
        for(int i = 0; i < freeSpaceRanges.size(); i++){
            pages.push_back(page_table->getPageNumber(freeSpaceRanges[i]));
        }
        std::cout << pages[0] << " " << pages[1] << std::endl;
        page_table->removeFreePages(pid,pages,freeSpaceRanges);
    }
}

void terminateProcess(uint32_t pid, Mmu *mmu, PageTable *page_table)
{
    if(mmu->pidExists(pid)){
        uint32_t largestPageNum = page_table->getPageNumber(mmu->getLargestVirtualAddress(pid));
        page_table->removeProcess(pid,largestPageNum);
        mmu->removeProcess(pid);
    }
    else{
        std::cout << "error: process not found" << std::endl;
    }
}

uint32_t calculateSize(DataType type, uint32_t num_elements){
    if(type == DataType::Char){
        return num_elements;
    }
    else if(type == DataType::Short){
        return 2 * num_elements;
    }
    else if(type == DataType::Int || type == DataType::Float){
        return 4 * num_elements;
    }
    else{
        return 8 * num_elements;
    }
}

bool isNumber(std::string input){
    for(int i = 0; i < input.size(); i++){
        if(!std::isdigit(input[i])){
            return false;
        }
    }
    return true;
}

