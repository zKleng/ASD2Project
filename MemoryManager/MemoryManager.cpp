#include "MemoryManager.h"
#include <iostream>

MemoryManager::MemoryManager(size_t size, const std::string& dumpFolder) 
    : totalSize(size), dumpFolder(dumpFolder) {}

int MemoryManager::create(size_t size) {
    std::lock_guard<std::mutex> lock(memoryMutex);
    
    static int idCounter = 0;
    int id = idCounter++;
    
    if (size > totalSize) {
        return -1;  // No hay suficiente memoria disponible.
    }

    memoryPool[id] = {std::vector<char>(size), 1};
    return id;
}

bool MemoryManager::set(int id, const char* data, size_t size) {
    std::lock_guard<std::mutex> lock(memoryMutex);

    auto it = memoryPool.find(id);
    if (it == memoryPool.end()) return false;

    if (size > it->second.data.size()) return false;  // Verificación de límite

    std::copy(data, data + size, it->second.data.begin());
    return true;
}

std::string MemoryManager::get(int id) {
    std::lock_guard<std::mutex> lock(memoryMutex);

    auto it = memoryPool.find(id);
    if (it == memoryPool.end()) return "";

    return std::string(it->second.data.begin(), it->second.data.end());
}

void MemoryManager::increaseRef(int id) {
    std::lock_guard<std::mutex> lock(memoryMutex);

    if (memoryPool.find(id) != memoryPool.end()) {
        memoryPool[id].refCount++;
    }
}

void MemoryManager::decreaseRef(int id) {
    std::lock_guard<std::mutex> lock(memoryMutex);

    auto it = memoryPool.find(id);
    if (it != memoryPool.end()) {
        it->second.refCount--;
        if (it->second.refCount <= 0) {
            memoryPool.erase(it);  // Elimina el bloque cuando ya no tiene referencias.
        }
    }
}

void MemoryManager::collectGarbage() {
    std::lock_guard<std::mutex> lock(memoryMutex);

    for (auto it = memoryPool.begin(); it != memoryPool.end();) {
        if (it->second.refCount <= 0) {
            it = memoryPool.erase(it);
        } else {
            ++it;
        }
    }
}

