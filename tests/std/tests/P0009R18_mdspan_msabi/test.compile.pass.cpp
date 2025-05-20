// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Tests MSVC STL specific behavior on ABI.

#include <mdspan>

#include <test_mdspan_support.hpp>

using namespace std;

template <class T>
struct NuaBaseChecker : T {};

template <class T>
struct NuaMemberChecker {
    unsigned char object;
    _MSVC_NO_UNIQUE_ADDRESS T t;
};

template <class T>
concept nua_friendly = sizeof(T) == 1
#if !defined(__clang__) && !defined(__EDG__) // TODO Investigate
                    && sizeof(NuaBaseChecker<T>) == 1 && sizeof(NuaMemberChecker<T>) == 1
#endif
    ;

// When 'E::rank_dynamic()' is equal to 0 then 'nua_friendly<E>' should be true
static_assert(!nua_friendly<dextents<int, 2>>);
static_assert(!nua_friendly<extents<int, 3, dynamic_extent>>);
static_assert(nua_friendly<extents<int, 3, 3>>);
static_assert(nua_friendly<extents<int>>);

// When 'M::extents_type::rank_dynamic()' is equal to 0 then 'nua_friendly<M>' should be true
static_assert(!nua_friendly<layout_left::mapping<dextents<long long, 2>>>);
static_assert(!nua_friendly<layout_left::mapping<extents<long long, 3, dynamic_extent>>>);
static_assert(nua_friendly<layout_left::mapping<extents<long long, 3, 3>>>);
static_assert(nua_friendly<layout_left::mapping<extents<long long>>>);

// When 'M::extents_type::rank_dynamic()' is equal to 0 then 'nua_friendly<M>' should be true
static_assert(!nua_friendly<layout_right::mapping<dextents<long, 2>>>);
static_assert(!nua_friendly<layout_right::mapping<extents<long, 3, dynamic_extent>>>);
static_assert(nua_friendly<layout_right::mapping<extents<long, 3, 3>>>);
static_assert(nua_friendly<layout_right::mapping<extents<long>>>);

// When 'M::extents_type::rank()' is equal to 0 then 'nua_friendly<M>' should be true
static_assert(!nua_friendly<layout_stride::mapping<dextents<long long, 2>>>);
static_assert(!nua_friendly<layout_stride::mapping<extents<long long, 3, dynamic_extent>>>);
static_assert(!nua_friendly<layout_stride::mapping<extents<long long, 3, 3>>>);
static_assert(nua_friendly<layout_stride::mapping<extents<long long>>>);

// When
// * 'Mds::accessor_type' is a specialization of 'default_accessor', and
// * 'Mds::layout_type' is
//   * 'layout_left' or 'layout_right' and 'Mds::extents_type::rank_dynamic() == 0', or
//   * 'layout_stride' and 'Mds::extents_type::rank() == 0'
// then 'sizeof(Mds) == sizeof(void*)'.
#if !defined(__clang__) && !defined(__EDG__)
static_assert(sizeof(mdspan<int, extents<int, 3, 3, 3>, layout_left>) == sizeof(void*));
#endif
static_assert(sizeof(mdspan<int, dextents<int, 3>, layout_left>) > sizeof(void*));
static_assert(sizeof(mdspan<int, extents<int, 3, 3, 3>, layout_left, TrivialAccessor<int>>) > sizeof(void*));

#if !defined(__clang__) && !defined(__EDG__)
static_assert(sizeof(mdspan<long, extents<long, 2, 2, 2>, layout_right>) == sizeof(void*));
#endif
static_assert(sizeof(mdspan<long, dextents<long, 2>, layout_right>) > sizeof(void*));
static_assert(sizeof(mdspan<long, extents<long, 2, 2, 2>, layout_right, TrivialAccessor<long>>) > sizeof(void*));

#if !defined(__clang__) && !defined(__EDG__)
static_assert(sizeof(mdspan<short, extents<short>, layout_stride>) == sizeof(void*));
#endif
static_assert(sizeof(mdspan<short, extents<short, 4, 4, 4>, layout_stride>) > sizeof(void*));
static_assert(sizeof(mdspan<short, dextents<short, 4>, layout_stride>) > sizeof(void*));
static_assert(sizeof(mdspan<short, extents<short, 4, 4, 4>, layout_stride, TrivialAccessor<short>>) > sizeof(void*));
