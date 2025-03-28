#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <unordered_map>
#include <string>
#include <vector>
#include <mutex>

class MemoryManager {
public:
    MemoryManager(size_t size, const std::string& dumpFolder);
    int create(size_t size);
    bool set(int id, const char* data, size_t size);
    std::string get(int id);
    void increaseRef(int id);
    void decreaseRef(int id);
    void collectGarbage();  // Recolector de basura

private:
    struct MemoryBlock {
        std::vector<char> data;
        int refCount;
    };

    std::unordered_map<int, MemoryBlock> memoryPool;
    size_t totalSize;
    std::string dumpFolder;
    std::mutex memoryMutex;
};

#endif
