

#ifndef NAADSG_HASH_H
#define NAADSG_HASH_H

#include <string>
#include <vector>

namespace std {


/* Combination hash from Boost */
    template <class T>
    inline void hash_combine(size_t& seed, const T& v)
    {
        hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    template<typename S, typename T> struct hash<pair<S, T>>
    {
        inline size_t operator()(const pair<S, T>& v) const
        {
            size_t seed = 0;
            hash_combine(seed, v.first);
            hash_combine(seed, v.second);
            return seed;
        }
    };
/* End combination hash from Boost */



}

#endif //NAADSG_HASH_H
