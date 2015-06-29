#include <iostream>
#include <cstdlib>
#include <atomic>
#include <tbb/tbb.h>
#include <tbb/mutex.h>
#include <unordered_map>

using namespace tbb;
using namespace std;

inline uint64_t hashKey(uint64_t k) {
    // MurmurHash64A
    const uint64_t m = 0xc6a4a7935bd1e995;
    const int r = 47;
    uint64_t h = 0x8445d61a4e774912 ^(8 * m);
    k *= m;
    k ^= k >> r;
    k *= m;
    h ^= k;
    h *= m;
    h ^= h >> r;
    h *= m;
    h ^= h >> r;
    return h | (1ull << 63);
}

// Chained tuple entry
struct ChainedEntry {
    uint64_t key;
    uint64_t value;
    ChainedEntry *next;
};

class ChainingLockingHT { //TODO duplicates
public:
    // Constructor
    ChainingLockingHT(uint64_t size) : htsize(size), entries(htsize), locks(htsize){}

    // Destructor
    ~ChainingLockingHT() {}

    inline ChainedEntry *lookup(uint64_t key) {

        uint64_t index = key % htsize;
        ChainedEntry* entry = entries[index];

        while(entry != nullptr && entry->key != key){
            entry = entry->next;
        }
        return entry; //entry is either nullptr or the correct entry
    }

    inline void insert(ChainedEntry* entry) {

        uint64_t index = entry->key % htsize;

        locks[index].lock();

        entry->next = entries[index];
        entries[index] = entry;

        locks[index].unlock();
    }

private:
    uint64_t htsize;
    vector<ChainedEntry*> entries;
    vector<mutex> locks;
};



// Chained tuple entry
struct ChainedAtomicEntry {
    uint64_t key;
    uint64_t value;
    std::atomic<ChainedAtomicEntry*> next;
};


class ChainingHT {
public:

    // Constructor
    ChainingHT(uint64_t size) : htsize(size), entries(htsize){
    }

    // Destructor
    ~ChainingHT() {
    }

    inline ChainedEntry *lookup(uint64_t key) {
        uint64_t index = key % htsize;
        ChainedEntry* entry = entries[index];

        while(entry != nullptr && entry->key != key){
            entry = entry->next;
        }
        return entry; //entry is either nullptr or the correct entry
    }

    inline void insert(ChainedEntry *entry) {
        uint64_t index = entry->key % htsize;

        //compare and exchange
        //this.( expected , desired)
        // if this == expected
        //      this <= desired
        // else expected <= this;

        // goal: entry->next = entries[index]
        // and entries[index] = entry

        //TODO: check if working
        ChainedEntry* oldEntry = entries[index];
        entries[index].compare_and_swap(oldEntry, entry);
        entry->next = oldEntry;

    }

private:
    uint64_t htsize;
    vector<tbb::atomic<ChainedEntry*>> entries;
};

class LinearProbingHT {
public:
    // Entry
    struct Entry {
        uint64_t key;
        uint64_t value;
        std::atomic<bool> marker;
    };

    // Constructor
    LinearProbingHT(uint64_t size) {
    }

    // Destructor
    ~LinearProbingHT() {
    }

    inline Entry *lookup(uint64_t key) {
    }

    inline void insert(uint64_t key) {
    }
};

int main(int argc, char **argv) {
    uint64_t sizeR = atoi(argv[1]);
    uint64_t sizeS = atoi(argv[2]);
    unsigned threadCount = atoi(argv[3]);

    task_scheduler_init init(threadCount);

    // Init build-side relation R with random data
    uint64_t *R = static_cast<uint64_t *>(malloc(sizeR * sizeof(uint64_t)));
    parallel_for(blocked_range<size_t>(0, sizeR), [&](const blocked_range<size_t> &range) {
        unsigned int seed = range.begin();
        for (size_t i = range.begin(); i != range.end(); ++i){

            R[i] = rand_r(&seed) % sizeR;
//            std::cout << "R[" << i << "]: " << R[i] << std::endl;
        }
    });

    std::cout << std::endl;

    // Init probe-side relation S with random data
    uint64_t *S = static_cast<uint64_t *>(malloc(sizeS * sizeof(uint64_t)));
    parallel_for(blocked_range<size_t>(0, sizeS), [&](const blocked_range<size_t> &range) {
        unsigned int seed = range.begin();
        for (size_t i = range.begin(); i != range.end(); ++i){

            S[i] = rand_r(&seed) % sizeR;
//            std::cout << "S[" << i << "]: " << S[i] << std::endl;
        }
    });

    // STL
    {
        // Build hash table (single threaded)
        tick_count buildTS = tick_count::now();
        unordered_multimap<uint64_t, uint64_t> ht(sizeR);
        for (uint64_t i = 0; i < sizeR; i++)
            ht.emplace(R[i], 0);
        tick_count probeTS = tick_count::now();
        cout << "STL:" << endl << " - build: " << (sizeR / 1e6) / (probeTS - buildTS).seconds() << " MT/s " << endl;

        // Probe hash table and count number of hits
        std::atomic<uint64_t> hitCounter;
        hitCounter = 0;
        parallel_for(blocked_range<size_t>(0, sizeS), [&](const blocked_range<size_t> &range) {
            uint64_t localHitCounter = 0;
            for (size_t i = range.begin(); i != range.end(); ++i) {
                auto range = ht.equal_range(S[i]);
                for (unordered_multimap<uint64_t, uint64_t>::iterator it = range.first; it != range.second; ++it)
                    localHitCounter++;
            }
            hitCounter += localHitCounter;
        });
        tick_count stopTS = tick_count::now();
        cout << " - probe: " << (sizeS / 1e6) / (stopTS - probeTS).seconds() << " MT/s " << endl
        << " - total: " << ((sizeR + sizeS) / 1e6) / (stopTS - buildTS).seconds() << " MT/s " << endl
        << " - count: " << hitCounter << endl << endl;
    }

    // Test your implementation here... (like the STL test above)

    // ChainingLockingHT
    {
        // Build hash table (single threaded) //TODO later multithreaded with parallel_for
        tick_count buildTS = tick_count::now();
        ChainingLockingHT ht(sizeR);
        for (uint64_t i = 0; i < sizeR; i++)
        {
            ChainedEntry* e = new ChainedEntry();
            e->value = R[i];
            e->key = hashKey(R[i]);
            ht.insert(e);
        }
        tick_count probeTS = tick_count::now();
        cout << "ChainingLockingHT: " << endl << " - build: " << (sizeR / 1e6) / (probeTS - buildTS).seconds() << " MT/s " << endl;

        // Probe hash table and count number of hits
        std::atomic<uint64_t> hitCounter;
        hitCounter = 0;
        parallel_for(blocked_range<uint64_t>(0, sizeS), [&](const blocked_range<uint64_t> &range) {
            uint64_t localHitCounter = 0;
            for (size_t i = range.begin(); i != range.end(); ++i) {
                if(ht.lookup(hashKey(S[i])) != nullptr) ++localHitCounter;
            }
            hitCounter += localHitCounter;
        });
        tick_count stopTS = tick_count::now();
        cout << " - probe: " << (sizeS / 1e6) / (stopTS - probeTS).seconds() << " MT/s " << endl
        << " - total: " << ((sizeR + sizeS) / 1e6) / (stopTS - buildTS).seconds() << " MT/s " << endl
        << " - count: " << hitCounter << endl << endl;
    }

    return 0;
}
