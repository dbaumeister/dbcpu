#include <string>
#include <cassert>
#include <vector>
#include <sstream>
#include <string.h>

#include "../btree/BTree.h"

/* Comparator functor for uint64_t*/
struct MyCustomUInt64Cmp {
    bool operator()(uint64_t a, uint64_t b) const {
        return a < b;
    }
};

template<unsigned len>
struct Char {
    char data[len];
};

/* Comparator functor for char */
template<unsigned len>
struct MyCustomCharCmp {
    bool operator()(const Char<len> &a, const Char<len> &b) const {
        return memcmp(a.data, b.data, len) < 0;
    }
};

typedef std::pair<uint32_t, uint32_t> IntPair;

/* Comparator for IntPair */
struct MyCustomIntPairCmp {
    bool operator()(const IntPair &a, const IntPair &b) const {
        if (a.first < b.first)
            return true;
        else
            return (a.first == b.first) && (a.second < b.second);
    }
};

template<class T>
const T &getKey(const uint64_t &i);

template<>
const uint64_t &getKey(const uint64_t &i) { return i; }

std::vector<std::string> char20;

template<>
const Char<20> &getKey(const uint64_t &i) {
    std::stringstream ss;
    ss << i;
    std::string s(ss.str());
    char20.push_back(std::string(20 - s.size(), '0') + s);
    return *reinterpret_cast<const Char<20> *>(char20.back().data());
}

std::vector<IntPair> intPairs;

template<>
const IntPair &getKey(const uint64_t &i) {
    intPairs.push_back(std::make_pair(i / 3, 3 - (i % 3)));
    return intPairs.back();
}

uint64_t toUint64(TID tid){
    uint64_t out = 0;
    memcpy(&out, &tid, sizeof(uint64_t));
    return out;
}

TID toTID(uint64_t in){
    TID tid;
    memcpy(&tid, &in, sizeof(uint64_t));
    return tid;
}

template<class T, class CMP>
void test(uint64_t n) {
    // Set up stuff, you probably have to change something here to match to your interfaces
    BufferManager bm(4096);
    // ...
    uint64_t segment = 3;
    BTree<T, CMP> bTree(bm, segment);

    // Insert values
    for (uint64_t i = n-1; i >=0; --i){
        TID tid = toTID(i * i);
        bTree.insert(getKey<T>(i), tid);
        if(i == 0) break;
    }
    assert(bTree.size() == n);

    printf("Inserted\n");
    // Check if they can be retrieved
    for (uint64_t i = 0; i < n; ++i) {
        TID tid;

        if(!bTree.lookup(getKey<T>(i), tid)) printf("%lu\n", i);

        assert(bTree.lookup(getKey<T>(i), tid));
        assert(toUint64(tid) == i * i);
    }


    printf("LookedUp\n");

    // Delete some values
    for (uint64_t i = 0; i < n; ++i)
        if ((i % 7) == 0)
            bTree.erase(getKey<T>(i));

    // Check if the right ones have been deleted
    for (uint64_t i = 0; i < n; ++i) {
        TID tid;
        if ((i % 7) == 0) {
            assert(!bTree.lookup(getKey<T>(i), tid));
        } else {
            assert(bTree.lookup(getKey<T>(i), tid));
            assert(toUint64(tid) == i * i);
        }
    }


    std::cout << "Deleted" << std::endl;

    // Delete everything
    for (uint64_t i = 0; i < n; ++i)
        bTree.erase(getKey<T>(i));
    assert(bTree.size() == 0);
}

int main(int argc, char *argv[]) {
    // Get command line argument
    const uint64_t n = (argc == 2) ? strtoul(argv[1], NULL, 10) : 1000 * 1000ul;

    // Test index with 64bit unsigned integers
    test<uint64_t, MyCustomUInt64Cmp>(n);

    // Test index with 20 character strings
    test<Char<20>, MyCustomCharCmp<20>>(n);

    // Test index with compound key
    test<IntPair, MyCustomIntPairCmp>(n);
    return 0;
}
