//
// Created by Omar on 12/11/2025.
//

#ifndef RMS_UNORDERED_MAP_H
#define RMS_UNORDERED_MAP_H
#include <vector>
#include <list>
#include <functional>
#include <utility>
#include <stdexcept>
#include <cstddef>
#include <limits>
template <typename Key, typename Value>
class unordered_map
{
private:
    struct Node
    {
        Key key;
        Value value;
        Node(const Key &k, const Value &v) : key(k), value(v) {}
        Node(Key &&k, Value &&v) : key(std::move(k)), value(std::move(v)) {}
    };

    std::vector<std::list<Node>> buckets;
    size_t elements = 0;
    float maxLoadFactor = 0.75f;

    size_t bucket_index(const Key &k) const
    {
        return std::hash<Key>{}(k) % buckets.size();
    }

    void check_rehash()
    {
        if (buckets.empty())
        {
            buckets.resize(8);
            return;
        }
        if ((float)(elements + 1) / buckets.size() > maxLoadFactor)
        {
            rehash(buckets.size() * 2);
        }
    }

public:
    // ITERATOR
    class iterator
    {
        using BucketList = std::list<Node>;
        unordered_map *map = nullptr;
        size_t bucketIdx = 0;
        typename BucketList::iterator listIt;
        friend class unordered_map;

        iterator(unordered_map *m, size_t b, typename BucketList::iterator it)
                : map(m), bucketIdx(b), listIt(it) {}

    public:
        iterator() = default;

        std::pair<const Key &, Value &> operator*() const
        {
            return {listIt->key, listIt->value};
        }

        iterator &operator++()
        {
            if (!map)
                return *this;

            ++listIt;

            while (bucketIdx < map->buckets.size() &&
                   listIt == map->buckets[bucketIdx].end())
            {
                ++bucketIdx;
                if (bucketIdx < map->buckets.size())
                    listIt = map->buckets[bucketIdx].begin();
            }

            return *this;
        }

        bool operator==(const iterator &other) const
        {
            if (map != other.map)
                return false;

            if (bucketIdx >= map->buckets.size() &&
                other.bucketIdx >= other.map->buckets.size())
                return true;

            if (bucketIdx >= map->buckets.size() ||
                other.bucketIdx >= other.map->buckets.size())
                return false;

            return bucketIdx == other.bucketIdx && listIt == other.listIt;
        }

        bool operator!=(const iterator &other) const
        {
            return !(*this == other);
        }
    };
    class const_iterator
    {
        using BucketList = std::list<Node>;
        const unordered_map* map = nullptr;
        size_t bucketIdx = 0;
        typename BucketList::const_iterator listIt;
        friend class unordered_map;

        const_iterator(const unordered_map* m, size_t b, typename BucketList::const_iterator it)
                : map(m), bucketIdx(b), listIt(it) {}

    public:
        const_iterator() = default;

        std::pair<const Key&, const Value&> operator*() const
        {
            return {listIt->key, listIt->value};
        }

        const_iterator& operator++()
        {
            if (!map) return *this;

            ++listIt;

            while (bucketIdx < map->buckets.size() &&
                   listIt == map->buckets[bucketIdx].end())
            {
                ++bucketIdx;
                if (bucketIdx < map->buckets.size())
                    listIt = map->buckets[bucketIdx].begin();
            }

            return *this;
        }

        bool operator==(const const_iterator& other) const
        {
            if (map != other.map) return false;

            if (bucketIdx >= map->buckets.size() &&
                other.bucketIdx >= other.map->buckets.size())
                return true;

            if (bucketIdx >= map->buckets.size() ||
                other.bucketIdx >= other.map->buckets.size())
                return false;

            return bucketIdx == other.bucketIdx && listIt == other.listIt;
        }

        bool operator!=(const const_iterator& other) const
        {
            return !(*this == other);
        }
    };

    unordered_map() { buckets.resize(8); }
    ~unordered_map() { clear(); }
    unordered_map(const unordered_map &other)
            : buckets(other.buckets), elements(other.elements), maxLoadFactor(other.maxLoadFactor)
    {
    }

    unordered_map &operator=(const unordered_map &other)
    {
        if (this != &other)
        {
            buckets = other.buckets;
            elements = other.elements;
            maxLoadFactor = other.maxLoadFactor;
        }
        return *this;
    }
    //  for = { } assginment
    unordered_map(std::initializer_list<std::pair<const Key, Value>> init)
    {
        buckets.resize(8);
        for (const auto &kv : init)
            insert(kv);
    }


    // ELEMENT ACCESS

    Value &operator[](const Key &k)
    {
        check_rehash();
        size_t idx = bucket_index(k);
        for (auto it = buckets[idx].begin(); it != buckets[idx].end(); ++it)
        {
            if (it->key == k)
                return it->value;
        }
        // Not found, insert with default value
        buckets[idx].emplace_back(k, Value{});
        ++elements;
        return buckets[idx].back().value;
    }
    unordered_map &operator=(unordered_map &&other) noexcept
    {
        if (this != &other)
        {
            buckets = std::move(other.buckets);
            elements = other.elements;
            maxLoadFactor = other.maxLoadFactor;
            other.elements = 0;
        }
        return *this;
    }


    Value &at(const Key &k)
    {
        size_t idx = bucket_index(k);
        for (auto &node : buckets[idx])
        {
            if (node.key == k)
                return node.value;
        }
        throw std::out_of_range("unordered_map::at: key not found");
    }

    const Value &at(const Key &k) const
    {
        size_t idx = bucket_index(k);
        for (const auto &node : buckets[idx])
        {
            if (node.key == k)
                return node.value;
        }
        throw std::out_of_range("unordered_map::at: key not found");
    }
    // CAPACITY
    bool empty() const noexcept { return elements == 0; }
    size_t size() const noexcept { return elements; }
    size_t max_size() const noexcept { return std::numeric_limits<size_t>::max() / 2; }
    // MODIFIERS
    void clear() noexcept
    {
        for (auto &bucket : buckets)
            bucket.clear();
        elements = 0;
    }

    std::pair<iterator, bool> insert(const std::pair<Key, Value> &kv)
    {
        check_rehash();
        size_t idx = bucket_index(kv.first);
        // Check if key exists
        for (auto it = buckets[idx].begin(); it != buckets[idx].end(); ++it)
        {
            if (it->key == kv.first)
                return {iterator(this, idx, it), false};
        }
        // Insert new element
        buckets[idx].emplace_back(kv.first, kv.second);
        ++elements;
        auto it = buckets[idx].end();
        --it;
        return {iterator(this, idx, it), true};
    }

    std::pair<iterator, bool> insert(std::pair<Key, Value> &&kv)
    {
        check_rehash();
        size_t idx = bucket_index(kv.first);
        for (auto it = buckets[idx].begin(); it != buckets[idx].end(); ++it)
        {
            if (it->key == kv.first)
                return {iterator(this, idx, it), false};
        }
        buckets[idx].emplace_back(std::move(kv.first), std::move(kv.second));
        ++elements;
        auto it = buckets[idx].end();
        --it;
        return {iterator(this, idx, it), true};
    }

    template <typename... Args>
    std::pair<iterator, bool> emplace(Args &&...args)
    {
        check_rehash();
        Node node(std::forward<Args>(args)...);
        size_t idx = bucket_index(node.key);

        for (auto it = buckets[idx].begin(); it != buckets[idx].end(); ++it)
        {
            if (it->key == node.key)
                return {iterator(this, idx, it), false};
        }

        buckets[idx].push_back(std::move(node));
        ++elements;
        auto it = buckets[idx].end();
        --it;
        return {iterator(this, idx, it), true};
    }

    size_t erase(const Key &k)
    {
        size_t idx = bucket_index(k);
        auto &bucket = buckets[idx];
        for (auto it = bucket.begin(); it != bucket.end(); ++it)
        {
            if (it->key == k)
            {
                bucket.erase(it);
                --elements;
                return 1;
            }
        }
        return 0;
    }

    iterator erase(iterator pos)
    {
        if (!pos.map || pos.bucketIdx >= buckets.size())
            return end();
        auto &bucket = buckets[pos.bucketIdx];
        auto nextIt = pos.listIt;
        ++nextIt;
        bucket.erase(pos.listIt);
        --elements;
        return iterator(this, pos.bucketIdx, nextIt);
    }

    void swap(unordered_map &other)
    {
        buckets.swap(other.buckets);
        std::swap(elements, other.elements);
        std::swap(maxLoadFactor, other.maxLoadFactor);
    }
    // LOOKUP
    iterator find(const Key &k)
    {
        size_t idx = bucket_index(k);
        auto &bucket = buckets[idx];
        for (auto it = bucket.begin(); it != bucket.end(); ++it)
        {
            if (it->key == k)
                return iterator(this, idx, it);
        }
        return end();
    }
    const_iterator find(const Key &k) const
    {
        size_t idx = bucket_index(k);
        const auto &bucket = buckets[idx];
        for (auto it = bucket.begin(); it != bucket.end(); ++it)
            if (it->key == k)
                return const_iterator(this, idx, it);
        return end();
    }
    size_t count(const Key &k) const
    {
        size_t idx = bucket_index(k);
        const auto &bucket = buckets[idx];
        for (const auto &node : bucket)
        {
            if (node.key == k)
                return 1;
        }
        return 0;
    }

    iterator lower_bound(const Key &k) { return end(); } // unused
    iterator upper_bound(const Key &k) { return end(); } // unused

    // REHASH
    void rehash(size_t n)
    {
        if (n == 0)
            return;
        std::vector<std::list<Node>> newBuckets(n);
        for (auto &bucket : buckets)
        {
            for (auto &node : bucket)
            {
                size_t idx = std::hash<Key>{}(node.key) % n;
                newBuckets[idx].emplace_back(std::move(node.key), std::move(node.value));
            }
        }
        buckets.swap(newBuckets);
    }

    void reserve(size_t n)
    {
        size_t requiredBuckets = static_cast<size_t>(n / maxLoadFactor) + 1;
        if (requiredBuckets > buckets.size())
            rehash(requiredBuckets);
    }
    // ITERATORS
    iterator begin()
    {
        for (size_t i = 0; i < buckets.size(); ++i)
            if (!buckets[i].empty())
                return iterator(this, i, buckets[i].begin());
        return end();
    }

    iterator end()
    {
        if (buckets.empty())
            return iterator(this, 0, typename std::list<Node>::iterator{});
        return iterator(this, buckets.size(), {});
    }
    const_iterator begin() const
    {
        for (size_t i = 0; i < buckets.size(); ++i)
            if (!buckets[i].empty())
                return const_iterator(this, i, buckets[i].cbegin());
        return end();
    }

    const_iterator end() const
    {
        return const_iterator(this, buckets.size(), {});
    }

};
#endif //RMS_UNORDERED_MAP_H
