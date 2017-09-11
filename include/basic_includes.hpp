#ifndef BASIC_INCLUDES_HPP
#define BASIC_INCLUDES_HPP

#include <iostream>
#include <cassert>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <functional>
#include <utility>

// depending on size of individual box
typedef uint32_t size_type;
typedef std::unordered_set<size_type> faults_type;
typedef std::vector<size_type> vec;


//first element: boxid   second element: id
typedef std::pair<size_type, size_type> position;



enum direction { right = 0 , up = 1, back = 2, left = 3 , down = 4 , front = 5 };



// correction needed to make tuples hashable
// simply copied and pasted
namespace std{
    namespace
    {
        // Code from boost
        // Reciprocal of the golden ratio helps spread entropy
        //     and handles duplicates.
        // See Mike Seymour in magic-numbers-in-boosthash-combine:
        //     http://stackoverflow.com/questions/4948780

        template <class T>
        inline void hash_combine(std::size_t& seed, T const& v)
        {
            seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
        }

        // Recursive template code derived from Matthieu M.
        template <class Tuple, size_t Index = std::tuple_size<Tuple>::value - 1>
        struct HashValueImpl
        {
          static void apply(size_t& seed, Tuple const& tuple)
          {
            HashValueImpl<Tuple, Index-1>::apply(seed, tuple);
            hash_combine(seed, std::get<Index>(tuple));
          }
        };

        template <class Tuple>
        struct HashValueImpl<Tuple,0>
        {
          static void apply(size_t& seed, Tuple const& tuple)
          {
            hash_combine(seed, std::get<0>(tuple));
          }
        };
    }

    template <typename ... TT>
    struct hash<std::tuple<TT...>> 
    {
        size_t
        operator()(std::tuple<TT...> const& tt) const
        {                                              
            size_t seed = 0;                             
            HashValueImpl<std::tuple<TT...> >::apply(seed, tt);    
            return seed;                                 
        }                                              
    };
}



namespace std
{
    template<>
    struct hash<std::pair<size_type, size_type>>
    {
        size_t operator () (std::pair<size_type, size_type> const& p) const
        {
            // A bad example of computing the hash, 
            // rather replace with something more clever
            return std::hash<int>()(p.first) + std::hash<int>()(p.second);
        }
    };
}


#endif