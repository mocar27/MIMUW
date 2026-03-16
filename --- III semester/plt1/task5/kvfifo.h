#ifndef KVFIFO_H
#define KVFIFO_H

#include <cstddef>
#include <iterator>
#include <list>
#include <map>
#include <memory>
#include <utility>

using namespace std;

template<typename K, typename V>
class kvfifo {
protected:
    using elementsList = list<pair<K, V>>;
    using iteratorsList = list<typename elementsList::iterator>;
    using iteratorsMap = map<K, iteratorsList>;

    shared_ptr<elementsList> elements;
    shared_ptr<iteratorsMap> iterators;

    bool reference = false;

    constexpr void copyOnWrite() {
        if (!elements.unique()) {
            auto oldElementsPtr = elements;
            auto oldIteratorsPtr = iterators;

            try {
                elementsList newElements;
                newElements.insert(newElements.end(), elements->begin(),
                                   elements->end());
                elements = make_shared<elementsList>(newElements);
                iteratorsMap newIterators;
                for (auto it = elements->begin(); it != elements->end(); it++) {
                    newIterators[it->first].push_back(it);
                }
                iterators = make_shared<iteratorsMap>(newIterators);
                reference = false;
            } catch (...) {
                elements = oldElementsPtr;
                iterators = oldIteratorsPtr;
                throw;
            }
        }
    }

public:
    constexpr kvfifo() {
        try {
            this->elements = make_shared<elementsList>();
            this->iterators = make_shared<iteratorsMap>();
        } catch (...) {
            throw;
        }
    }

    constexpr kvfifo(kvfifo const &other) {
        if (this != &other) {
            auto oldElementsPtr = elements;
            auto oldIteratorsPtr = iterators;

            try {
                this->elements = other.elements;
                this->iterators = other.iterators;

                if (other.reference) {
                    copyOnWrite();
                }
            } catch (...) {
                elements = oldElementsPtr;
                iterators = oldIteratorsPtr;
                throw;
            }
        }
    }

    constexpr kvfifo(kvfifo &&other) noexcept {
        if (this != &other) {
            this->elements = move(other.elements);
            this->iterators = move(other.iterators);
            other.elements = make_shared<elementsList>();
            other.iterators = make_shared<iteratorsMap>();
        }
    }

    constexpr ~kvfifo() noexcept = default;

    constexpr size_t size() const noexcept {
        return elements->size();
    }

    constexpr bool empty() const noexcept {
        return elements->empty();
    }

    constexpr kvfifo &operator=(kvfifo other) {
        if (this != &other) {
            auto oldElementsPtr = elements;
            auto oldIteratorsPtr = iterators;

            try {
                elements = other.elements;
                iterators = other.iterators;
                if (other.reference) {
                    copyOnWrite();
                }
                return *this;
            } catch (...) {
                elements = oldElementsPtr;
                iterators = oldIteratorsPtr;
                throw;
            }
        } else {
            return *this;
        }
    }

    constexpr void push(K const &k, V const &v) {
        copyOnWrite();
        auto oldElementsPtr = elements;
        auto oldIteratorsPtr = iterators;

        try {
            pair<K, V> newPair = pair<K, V>(k, v);
            auto iterator = elements->insert(elements->end(), newPair);
            (*iterators)[k].push_back(iterator);
            reference = false;
        } catch (...) {
            elements = oldElementsPtr;
            iterators = oldIteratorsPtr;
            throw;
        }
    }

    constexpr void pop() {
        copyOnWrite();
        auto oldElementsPtr = elements;
        auto oldIteratorsPtr = iterators;

        try {
            if (empty()) {
                throw invalid_argument("Empty kvfifo");
            } else {
                auto it = elements->begin();
                iterators->find(it->first)->second.remove(it);

                if (iterators->find(it->first)->second.empty()) {
                    iterators->erase(it->first);
                }
                elements->pop_front();
                reference = false;
            }
        } catch (...) {
            elements = oldElementsPtr;
            iterators = oldIteratorsPtr;
            throw;
        }
    }

    constexpr void pop(K const &k) {
        copyOnWrite();
        auto oldElementsPtr = elements;
        auto oldIteratorsPtr = iterators;

        try {
            if (!iterators->contains(k)) {
                throw invalid_argument("k not in kvfifo");
            } else {
                auto it = iterators->find(k)->second.front();
                iterators->find(k)->second.remove(it);
                if (iterators->find(k)->second.empty()) {
                    iterators->erase(k);
                }
                elements->erase(it);
                reference = false;
            }
        } catch (...) {
            elements = oldElementsPtr;
            iterators = oldIteratorsPtr;
            throw;
        }
    }

    constexpr void move_to_back(K const &k) {
        copyOnWrite();
        auto oldElementsPtr = elements;
        auto oldIteratorsPtr = iterators;

        try {
            if (!iterators->contains(k)) {
                throw invalid_argument("k not in kvfifo");
            } else {
                iteratorsList newList;
                iteratorsList &toMove = iterators->find(k)->second;
                for (auto it = toMove.begin(); it != toMove.end(); it++) {
                    auto element = **it;
                    elements->remove(element);
                    auto iterator = elements->insert(elements->end(), element);
                    newList.push_back(iterator);
                }
                iterators->erase(k);
                K key = k;
                auto newIteratorPair = pair(key, newList);
                iterators->insert(newIteratorPair);
                reference = false;
            }
        } catch (...) {
            elements = oldElementsPtr;
            iterators = oldIteratorsPtr;
            throw;
        }
    }

    constexpr pair<K const &, V &> front() {
        if (empty()) {
            throw invalid_argument("Empty kvfifo");
        } else {
            copyOnWrite();

            pair<K, V> &ref = elements->front();
            K const &k = ref.first;
            V &v = ref.second;
            reference = true;
            return pair<K const &, V &>(k, v);
        }
    }

    constexpr pair<K const &, V const &> front() const {
        if (empty()) {
            throw invalid_argument("Empty kvfifo");
        } else {
            pair<K, V> &ref = elements->front();
            K const &k = ref.first;
            V const &v = ref.second;
            return pair<K const &, V const &>(k, v);
        }
    }

    constexpr pair<K const &, V &> back() {
        if (empty()) {
            throw invalid_argument("Empty kvfifo");
        } else {
            copyOnWrite();

            pair<K, V> &ref = elements->back();
            K const &k = ref.first;
            V &v = ref.second;
            reference = true;
            return pair<K const &, V &>(k, v);
        }
    }

    constexpr pair<K const &, V const &> back() const {
        if (empty()) {
            throw invalid_argument("Empty kvfifo");
        } else {
            pair<K, V> &ref = elements->back();
            K const &k = ref.first;
            V const &v = ref.second;
            return pair<K const &, V const &>(k, v);
        }
    }

    constexpr pair<K const &, V &> first(K const &key) {
        if (!iterators->contains(key)) {
            throw invalid_argument("Key not in kvfifo");
        } else {
            copyOnWrite();

            auto it = iterators->find(key)->second.front();
            pair<K, V> &ref = *it;
            K const &k = ref.first;
            V &v = ref.second;
            reference = true;
            return pair<K const &, V &>(k, v);
        }
    }

    constexpr pair<K const &, V const &> first(K const &key) const {
        if (!iterators->contains(key)) {
            throw invalid_argument("Key not in kvfifo");
        } else {
            auto it = iterators->find(key)->second.front();
            pair<K, V> &ref = *it;
            K const &k = ref.first;
            V const &v = ref.second;
            return pair<K const &, V const &>(k, v);
        }
    }

    constexpr pair<K const &, V &> last(K const &key) {
        if (!iterators->contains(key)) {
            throw invalid_argument("Key not in kvfifo");
        } else {
            copyOnWrite();

            auto it = iterators->find(key)->second.back();
            pair<K, V> &ref = *it;
            K const &k = ref.first;
            V &v = ref.second;
            reference = true;
            return pair<K const &, V &>(k, v);
        }
    }

    constexpr pair<K const &, V const &> last(K const &key) const {
        if (!iterators->contains(key)) {
            throw invalid_argument("Key not in kvfifo");
        } else {
            auto it = iterators->find(key)->second.back();
            pair<K, V> &ref = *it;
            K const &k = ref.first;
            V const &v = ref.second;
            return pair<K const &, V const &>(k, v);
        }
    }

    constexpr size_t count(K const &k) const noexcept {
        if (iterators->contains(k)) {
            return iterators->find(k)->second.size();
        } else {
            return 0;
        }
    }

    constexpr void clear() {
        copyOnWrite();
        elements->clear();
        iterators->clear();
    }

    class k_iterator {
    private:
        using iterator_category = bidirectional_iterator_tag;
        using difference_type = ptrdiff_t;
        using value_type = K;
        using pointer = const K *;
        using reference = const K &;

        typename iteratorsMap::iterator myIt;
    public:
        constexpr k_iterator(typename iteratorsMap::iterator const &m) : myIt(m) {}

        reference operator*() noexcept {
            return myIt->first;
        }

        pointer operator->() noexcept {
            return myIt;
        }

        const k_iterator &operator++() noexcept {
            ++myIt;
            return *this;
        }

        const k_iterator operator++(int) noexcept {
            k_iterator result(*this);
            ++(*this);
            return result;
        }

        const k_iterator &operator--() noexcept {
            --myIt;
            return *this;
        }

        const k_iterator operator--(int) noexcept {
            k_iterator result(*this);
            --(*this);
            return result;
        }

        constexpr friend bool operator==(const k_iterator &a, const k_iterator &b) noexcept {
            return a.myIt == b.myIt;
        }

        constexpr friend bool operator!=(const k_iterator &a, const k_iterator &b) noexcept {
            return !(a == b);
        }
    };

    constexpr k_iterator k_begin() const noexcept {
        return k_iterator(iterators->begin());
    }

    constexpr k_iterator k_end() const noexcept {
        return k_iterator(iterators->end());
    }
};

#endif  // KVFIFO_H