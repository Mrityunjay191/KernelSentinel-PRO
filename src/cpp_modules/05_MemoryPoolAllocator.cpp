/**
 * ============================================================================
 * PROMPT 1 & 7: High-Performance Fixed Memory Pool Allocator
 * ============================================================================
 * Implements a cache-friendly, O(1) allocation & deallocation memory pool:
 * - Avoids heap fragmentation during high-frequency game loops
 * - Integrates with std::unique_ptr and RAII
 */

#include <iostream>
#include <vector>
#include <memory>
#include <cstddef>

template <typename T, size_t BlockCount = 512>
class FixedMemoryPool {
private:
    union Node {
        alignas(alignof(T)) char storage[sizeof(T)];
        Node* next;
    };

    std::vector<Node> pool;
    Node* freeListHead;
    size_t activeCount;

public:
    FixedMemoryPool() : pool(BlockCount), freeListHead(nullptr), activeCount(0) {
        for (size_t i = 0; i < BlockCount - 1; ++i) {
            pool[i].next = &pool[i + 1];
        }
        pool[BlockCount - 1].next = nullptr;
        freeListHead = &pool[0];
    }

    template <typename... Args>
    T* allocate(Args&&... args) {
        if (!freeListHead) throw std::bad_alloc();

        Node* node = freeListHead;
        freeListHead = freeListHead->next;
        ++activeCount;

        return new (node->storage) T(std::forward<Args>(args)...);
    }

    void deallocate(T* ptr) {
        if (!ptr) return;
        ptr->~T();

        Node* node = reinterpret_cast<Node*>(ptr);
        node->next = freeListHead;
        freeListHead = node;
        --activeCount;
    }

    size_t getActiveCount() const { return activeCount; }
};

struct GameParticle {
    float x, y, z;
    float life;

    GameParticle(float px, float py, float pz, float plife)
        : x(px), y(py), z(pz), life(plife) {}
};

int main() {
    std::cout << "=== GAME ENGINE MEMORY POOL ALLOCATOR ===\n";

    FixedMemoryPool<GameParticle, 1000> particlePool;
    auto deleter = [&particlePool](GameParticle* p) { particlePool.deallocate(p); };

    {
        std::vector<std::unique_ptr<GameParticle, decltype(deleter)>> particles;

        std::cout << "Allocating 100 particles via pool...\n";
        for (int i = 0; i < 100; ++i) {
            GameParticle* raw = particlePool.allocate(0.0f, 0.0f, 0.0f, 1.0f);
            particles.emplace_back(raw, deleter);
        }

        std::cout << "Active particle count: " << particlePool.getActiveCount() << "\n";
        std::cout << "Releasing scope...\n";
    }

    std::cout << "Active particle count after scope release: " << particlePool.getActiveCount() << "\n";
    return 0;
}
