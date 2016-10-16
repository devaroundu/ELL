////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     StlIndexValueIterator.h (linear)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// linear
#include "IndexValue.h"

// stl
#include <cstdint>
#include <type_traits>
#include <utility>
#include <vector>

namespace emll
{
namespace linear
{
    /// <summary> A read-only forward index-value iterator for an STL container </summary>

    template <typename IteratorType, typename ValueType = decltype(*std::declval<IteratorType>())>
    class StlIndexValueIterator : public IIndexValueIterator
    {
    public:
        /// <summary> Constructs an instance of StlIndexValueIterator. </summary>
        ///
        /// <param name="begin"> STL iterator pointing at beginning of range to iterate over </param>
        /// <param name="end"> STL iterator pointing at end of range to iterate over </param>
        StlIndexValueIterator(const IteratorType& begin, const IteratorType& end);

        /// <summary> Returns True if the iterator is currently pointing to a valid iterate. </summary>
        ///
        /// <returns> True if the iterator is currently pointing to a valid iterate. </returns>
        bool IsValid() const { return _begin != _end; }

        /// <summary> Proceeds to the Next iterate </summary>
        void Next();

        /// <summary> Returns The current index-value pair </summary>
        ///
        /// <returns> The current index-value pair </returns>
        linear::IndexValue Get() const { return linear::IndexValue{ _index, (double)*_begin }; }

    protected:
        IteratorType _begin;
        IteratorType _end;
        uint64_t _index = 0;
        void SkipZeros();
    };

    /// <summary> Handy type alias for a StlIndexValueIterator over a std::vector </summary>
    template <typename ValueType>
    using VectorIndexValueIterator = StlIndexValueIterator<typename std::vector<ValueType>::const_iterator>;

    /// <summary> Creates an StlIndexValueIteratorAdapter for a std::vector </summary>
    ///
    /// <param name="container"> A std::vector to iterate over </param>
    /// <returns> An StlIndexValueIteratorAdapter that iterates over the given vector </returns>
    template <typename ValueType>
    VectorIndexValueIterator<ValueType> MakeVectorIndexValueIterator(const std::vector<ValueType>& container);
}
}

#include "../tcc/StlIndexValueIterator.tcc"