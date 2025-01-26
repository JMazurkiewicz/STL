// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <concepts>
#include <mdspan>
#include <span>
#include <type_traits>

using namespace std;

template <unsigned short V>
struct UShortConstant {
    static constexpr unsigned short value = V;

    constexpr operator unsigned short() const noexcept {
        return value;
    }
};

template <auto V>
struct AutoConstant {
    static constexpr auto value = V;

    constexpr operator decltype(value)() const noexcept {
        return value;
    }
};

template <int V>
struct NotConvertible {
    static constexpr int value = V;

    constexpr explicit operator int() const noexcept {
        return value;
    }
};

template <int V>
struct NotEqualityComparable {
    static constexpr int value = V;

    constexpr operator int() const noexcept {
        return value;
    }

    bool operator==(int) const = delete;
};

template <int V>
struct IncorrectConverionOp {
    static constexpr int value = V;

    constexpr operator int() const noexcept {
        return value + 1;
    }
};

using ElementType = int;
ElementType* ptr(); // not defined

template <class Span, size_t ExpectedExtent>
constexpr bool test_span = same_as<Span, span<ElementType, ExpectedExtent>>;

template <class Ext>
concept can_deduce_span_with_static_extent = requires(Ext e) {
    { span{ptr(), e} } -> same_as<span<ElementType, Ext::value>>;
};

static_assert(test_span<decltype(span{ptr(), 1}), dynamic_extent>);
static_assert(test_span<decltype(span{ptr(), 2U}), dynamic_extent>);
static_assert(test_span<decltype(span{ptr(), 3UL}), dynamic_extent>);
static_assert(test_span<decltype(span{ptr(), UShortConstant<4>{}}), 4>);
static_assert(test_span<decltype(span{ptr(), integral_constant<unsigned char, 5>{}}), 5>);
#if defined(__clang__) || defined(__EDG__) // TRANSITION, DevCom-10836688
static_assert(test_span<decltype(span{ptr(), AutoConstant<short{6}>{}}), 6>);
static_assert(test_span<decltype(span{ptr(), integral_constant<signed char, 7>{}}), 7>);
static_assert(test_span<decltype(span{ptr(), AutoConstant<long{8}>{}}), 8>);
static_assert(test_span<decltype(span{ptr(), integral_constant<long long, 9>{}}), 9>);
#endif // ^^^ no workaround ^^^
static_assert(test_span<decltype(span{ptr(), AutoConstant<10ULL>{}}), 10>);

static_assert(!can_deduce_span_with_static_extent<true_type>); // 'T::value' is bool
static_assert(!can_deduce_span_with_static_extent<AutoConstant<2.0f>>); // 'T::value' is not integral
static_assert(!can_deduce_span_with_static_extent<NotConvertible<4>>); // 'T' is not convertible
static_assert(
    !can_deduce_span_with_static_extent<NotEqualityComparable<5>>); // 'T::value' is not equality comparable with 'T'
static_assert(!can_deduce_span_with_static_extent<IncorrectConverionOp<6>>); // 'T() == T::value' is false

template <class Extents, size_t... ExpectedExtents>
constexpr bool test_extents = same_as<Extents, extents<size_t, ExpectedExtents...>>;

template <class Ext>
concept can_deduce_extents_with_static_values = requires(Ext e) {
    { extents{e} } -> same_as<extents<size_t, Ext::value>>;
};

static_assert(test_extents<decltype(extents{1, short{2}, 3}), dynamic_extent, dynamic_extent, dynamic_extent>);
static_assert(test_extents<decltype(extents{1, UShortConstant<2>{}, 3}), dynamic_extent, 2, dynamic_extent>);
static_assert(test_extents<decltype(extents{4, 5, AutoConstant<static_cast<unsigned char>(6)>{},
                               AutoConstant<static_cast<signed char>(7)>{}}),
    dynamic_extent, dynamic_extent, 6, 7>);
static_assert(test_extents<decltype(extents{integral_constant<long, 8>{}, integral_constant<unsigned long, 9>{},
                               integral_constant<long long, 10>{}, integral_constant<unsigned long long, 11>{},
                               integral_constant<unsigned int, 12>{}}),
    8, 9, 10, 11, 12>);

static_assert(!can_deduce_extents_with_static_values<true_type>); // 'T::value' is bool
static_assert(!can_deduce_extents_with_static_values<AutoConstant<2.0f>>); // 'T::value' is not integral
static_assert(!can_deduce_extents_with_static_values<NotConvertible<4>>); // 'T' is not convertible
static_assert(
    !can_deduce_extents_with_static_values<NotEqualityComparable<5>>); // 'T::value' is not equality comparable with 'T'
static_assert(!can_deduce_extents_with_static_values<IncorrectConverionOp<6>>); // 'T() == T::value'

template <class Mdspan, size_t... ExpectedExtents>
constexpr bool test_mdspan = same_as<Mdspan,
    mdspan<ElementType, extents<size_t, ExpectedExtents...>, layout_right, default_accessor<ElementType>>>;

template <class Ext>
concept can_deduce_mdspan_with_static_extents = requires(Ext e) {
    { mdspan{ptr(), e} } -> same_as<mdspan<ElementType, extents<size_t, Ext::value>>>;
};

static_assert(test_mdspan<decltype(mdspan{ptr(), 1, short{2}, static_cast<signed char>(3)}), dynamic_extent,
    dynamic_extent, dynamic_extent>);
static_assert(test_mdspan<decltype(mdspan{ptr(), 4, 5, UShortConstant<6>{}}), dynamic_extent, dynamic_extent, 6>);
static_assert(test_mdspan<decltype(mdspan{ptr(), 7, AutoConstant<long{8}>{}, AutoConstant<static_cast<long long>(9)>{},
                              10, AutoConstant<char{11}>{}}),
    dynamic_extent, 8, 9, dynamic_extent, 11>);
static_assert(test_mdspan<decltype(mdspan{ptr()})>);

static_assert(!can_deduce_mdspan_with_static_extents<true_type>); // 'T::value' is bool
static_assert(!can_deduce_mdspan_with_static_extents<AutoConstant<2.0f>>); // 'T::value' is not integral
static_assert(!can_deduce_mdspan_with_static_extents<NotConvertible<4>>); // 'T' is not convertible
static_assert(
    !can_deduce_mdspan_with_static_extents<NotEqualityComparable<5>>); // 'T::value' is not equality comparable with 'T'
static_assert(!can_deduce_mdspan_with_static_extents<IncorrectConverionOp<6>>); // 'T() == T::value' is false
