#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <cassert>
#include <string>
#include <iostream>

#include "hash.h"

using namespace std;

namespace diagnostic {
#ifdef NDEBUG
    constexpr bool debug = false;
#else
    constexpr bool debug = true;
#endif

    enum InvFlag {
        invPointer = 0b001,
        invSize = 0b010,
        doesNotExist = 0b100,
    };

    void checkPointer(string const &functionName, void const *pointer) {
        if (debug) {
            if (pointer == NULL) {
                cerr << functionName << ": invalid pointer (NULL)\n";
            }
            assert(pointer != NULL);
        }
    }

    void oneArgFunction(string const &functionName, unsigned long num) {
        if (debug) {
            cerr << functionName << "(" << num << ")\n";
        }
    }

    void threeArgFunction(string const &functionName, unsigned long id,
                          uint64_t const *seq, size_t size) {
        if (debug) {
            cerr << functionName << "(" << id << ", ";

            if (seq != NULL) {
                cerr << "\"";
                if (size > 1) {
                    for (size_t i = 0; i < size - 1; i++) {
                        cerr << seq[i] << " ";
                    }
                }

                if (size != 0) {
                    cerr << seq[size - 1];
                }
                cerr << "\"";
            } else {
                cerr << "NULL";
            }
            cerr << ", " << size << ")\n";
        }
    }

    void invalidData(string const &functionName, unsigned long id,
                     int flag) {
        if (debug) {
            if (flag & invPointer) {
                cerr << functionName << ": invalid pointer (NULL)\n";
            }
            if (flag & invSize) {
                cerr << functionName << ": invalid size (0)\n";
            }
            if (flag & doesNotExist) {
                cerr << functionName << ": hash table #" << id << " does not exist\n";
            }
        }
    }

    void hash_create(void const *pointer, unsigned long id) {
        if (debug) {
            cerr << "hash_create(" << pointer << ")\n";
            if (pointer == NULL) {
                cerr << "hash_create: invalid pointer (NULL)\n";
            } else {
                cerr << "hash_create: hash table #" << id << " created\n";
            }
        }
    }

    void hash_delete(unsigned long id) {
        if (debug) {
            cerr << "hash_delete: hash table #" << id << " deleted\n";
        }
    }

    void hash_size(unsigned long id, size_t elements, bool exists) {
        if (debug) {
            if (exists) {
                cerr << "hash_size: " << "hash table #" << id << " contains "
                    << elements << " element(s)\n";
            } else {
                cerr << "hash_size: " << "hash table #" << id << " does not exist\n";
            }
        }
    }

    void hash_clear(unsigned long id, int operation) {
        if (debug) {
            switch (operation) {
                case 1:
                    cerr << "hash_clear: hash table #" << id << " was empty\n";
                    break;
                default:
                    cerr << "hash_clear: hash table #" << id << " cleared\n";
            }
        }
    }

    void hash_insert(unsigned long id, uint64_t const *seq,
                     size_t size, int operation) {
        if (debug) {
            switch (operation) {
                case 1:
                    cerr << "hash_insert: hash table #" << id << ", sequence \"";
                    for (size_t i = 0; i < size - 1; i++) {
                        cerr << seq[i] << " ";
                    }
                    cerr << seq[size - 1] << "\"";
                    cerr << " was present\n";
                    break;
                default:
                    cerr << "hash_insert: hash table #" << id << ", sequence \"";
                    for (size_t i = 0; i < size - 1; i++) {
                        cerr << seq[i] << " ";
                    }
                    cerr << seq[size - 1] << "\"";
                    cerr << " inserted\n";
            }
        }
    }

    void hash_remove(unsigned long id, uint64_t const *seq,
                     size_t size, int operation) {
        if (debug) {
            switch (operation) {
                case 1:
                    cerr << "hash_remove: hash table #" << id << ", sequence \"";
                    for (size_t i = 0; i < size - 1; i++) {
                        cerr << seq[i] << " ";
                    }
                    cerr << seq[size - 1] << "\"";
                    cerr << " was not present\n";
                    break;
                default:
                    cerr << "hash_remove: hash table #" << id << ", sequence \"";
                    for (size_t i = 0; i < size - 1; i++) {
                        cerr << seq[i] << " ";
                    }
                    cerr << seq[size - 1] << "\"";
                    cerr << " removed\n";
            }
        }
    }

    void hash_test(unsigned long id, uint64_t const *seq,
                   size_t size, int operation) {
        if (debug) {
            switch (operation) {
                case 1:
                    cerr << "hash_test: hash table #" << id << ", sequence \"";
                    for (size_t i = 0; i < size - 1; i++) {
                        cerr << seq[i] << " ";
                    }
                    cerr << seq[size - 1] << "\"";
                    cerr << " is not present\n";
                    break;
                default:
                    cerr << "hash_test: hash table #" << id << ", sequence \"";
                    for (size_t i = 0; i < size - 1; i++) {
                        cerr << seq[i] << " ";
                    }
                    cerr << seq[size - 1] << "\"";
                    cerr << " is present\n";
            }
        }
    }
}

namespace jnp1 {

// Custom struct for hashing in set
    struct customHash {

        hash_function_t localHashFunction;

        customHash(hash_function_t hash_function) {
            localHashFunction = hash_function;
        }

        size_t operator()(vector <uint64_t> const &vec) const {
            return localHashFunction(vec.data(), vec.size());
        }
    };

    unsigned long &maxId() {
        static unsigned long maxId = 0;
        return maxId;
    }

    unordered_map<unsigned long, unordered_set<vector < uint64_t>, customHash>> &

    map() {
        static
        unordered_map < unsigned long,
                unordered_set < vector < uint64_t >, customHash >> map;
        return map;
    }


    unsigned long hash_create(hash_function_t hash_function) {

        diagnostic::checkPointer("hash_create", (void const *) hash_function);

        unordered_set <vector<uint64_t>, customHash>
                set(10, customHash(hash_function));

        map().insert({maxId(), set});
        maxId()++;

        diagnostic::hash_create((void const *) hash_function,  maxId() - 1);

        return maxId() - 1;
    }


    void hash_delete(unsigned long id) {

        diagnostic::oneArgFunction("hash_delete", id);

        if (map().find(id) == map().end()) {
            diagnostic::invalidData("hash_delete", id, diagnostic::doesNotExist);
            return;
        }

        map().erase(id);
        diagnostic::hash_delete(id);
    }


    size_t hash_size(unsigned long id) {

        diagnostic::oneArgFunction("hash_size", id);

        size_t result;
        auto mapIterator = map().find(id);
        bool exists = true;;

        if (mapIterator == map().end()) {
            result = 0;
            exists = false;
        } else {
            unordered_set <vector<uint64_t>, customHash> &mySet = mapIterator->second;
            result = mySet.size();
        }

        diagnostic::hash_size(id, result, exists);
        return result;
    }


    bool hash_insert(unsigned long id, uint64_t const *seq, size_t size) {

        diagnostic::threeArgFunction("hash_insert", id, seq, size);
        int flag = 0;
        if (size == 0) {
            flag |= diagnostic::invSize;
        }
        if (seq == NULL) {
            flag |= diagnostic::invPointer;
        }
        if (flag) {
            diagnostic::invalidData("hash_insert", id, flag);
            return false;
        }

        auto mapIterator = map().find(id);

        if (mapIterator == map().end()) {
            diagnostic::invalidData("hash_insert", id, diagnostic::doesNotExist);
            return false;
        }

        unordered_set <vector<uint64_t>, customHash> &mySet = mapIterator->second;
        vector <uint64_t> v(seq, seq + size);

        if (mySet.count(v) > 0) {
            diagnostic::hash_insert(id, seq, size, 1);
            return false;
        }

        mySet.insert(v);

        diagnostic::hash_insert(id, seq, size, 0);
        return true;
    }


    bool hash_remove(unsigned long id, uint64_t const *seq, size_t size) {

        diagnostic::threeArgFunction("hash_remove", id, seq, size);

        int flag = 0;
        if (size == 0) {
            flag |= diagnostic::invSize;
        }
        if (seq == NULL) {
            flag |= diagnostic::invPointer;
        }
        if (flag) {
            diagnostic::invalidData("hash_remove", id, flag);
            return false;
        }

        auto mapIterator = map().find(id);

        if (mapIterator == map().end()) {
            diagnostic::invalidData("hash_remove", id, diagnostic::doesNotExist);
            return false;
        }

        unordered_set <vector<uint64_t>, customHash> &mySet = mapIterator->second;
        vector <uint64_t> v(seq, seq + size);

        if (mySet.count(v) == 0) {
            diagnostic::hash_remove(id, seq, size, 1);
            return false;
        }

        mySet.erase(v);

        diagnostic::hash_remove(id, seq, size, 0);
        return true;
    }


    void hash_clear(unsigned long id) {

        diagnostic::oneArgFunction("hash_clear", id);

        auto mapIterator = map().find(id);

        if (mapIterator == map().end()) {
            diagnostic::invalidData("hash_clear", id, diagnostic::doesNotExist);
            return;
        } else if (mapIterator->second.empty()) {
            diagnostic::hash_clear(id, 1);
            return;
        }

        mapIterator->second.clear();

        diagnostic::hash_clear(id, 0);
    }


    bool hash_test(unsigned long id, uint64_t const *seq, size_t size) {

        diagnostic::threeArgFunction("hash_test", id, seq, size);

        int flag = 0;
        if (size == 0) {
            flag |= diagnostic::invSize;
        }
        if (seq == NULL) {
            flag |= diagnostic::invPointer;
        }
        if (flag) {
            diagnostic::invalidData("hash_test", id, flag);
            return false;
        }

        auto mapIterator = map().find(id);

        if (mapIterator == map().end()) {
            diagnostic::invalidData("hash_test", id, diagnostic::doesNotExist);
            return false;
        }

        unordered_set <vector<uint64_t>, customHash> &mySet = mapIterator->second;
        vector <uint64_t> v(seq, seq + size);

        if (mySet.count(v) == 0) {
            diagnostic::hash_test(id, seq, size, 1);
            return false;
        }

        diagnostic::hash_test(id, seq, size, 0);
        return true;
    }

}