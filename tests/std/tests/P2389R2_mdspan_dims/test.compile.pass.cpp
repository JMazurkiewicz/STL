// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <concepts>
#include <cstddef>
#include <mdspan>

using namespace std;

template <class T, class ExpectedIndexType, size_t ExpectedRank>
constexpr bool test_dims = false;

template <class IndexType, size_t... Extents, class ExpectedIndexType, size_t ExpectedRank>
constexpr bool test_dims<extents<IndexType, Extents...>, ExpectedIndexType, ExpectedRank> =
    ((Extents == dynamic_extent) && ...)
    && same_as<IndexType, ExpectedIndexType> && (sizeof...(Extents) == ExpectedRank);

static_assert(test_dims<dims<0, signed char>, signed char, 0>);
static_assert(test_dims<dims<1, short>, short, 1>);
static_assert(test_dims<dims<2, int>, int, 2>);
static_assert(test_dims<dims<3, long>, long, 3>);
static_assert(test_dims<dims<4, long long>, long long, 4>);

static_assert(test_dims<dims<5, unsigned char>, unsigned char, 5>);
static_assert(test_dims<dims<6, unsigned short>, unsigned short, 6>);
static_assert(test_dims<dims<7, unsigned int>, unsigned int, 7>);
static_assert(test_dims<dims<8, unsigned long>, unsigned long, 8>);
static_assert(test_dims<dims<9, unsigned long long>, unsigned long long, 9>);

static_assert(test_dims<dims<10>, size_t, 10>);
