﻿/**
 * The MIT License (MIT)
 *
 * Webzash - Easy to use web based double entry accounting software
 *
 * Copyright (c) 2020 Remi123 <drolet.remi@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef TYPE_EXPR_HPP
#define TYPE_EXPR_HPP

#include <ratio>
#include <type_traits>
#include <utility>

// STD NAMESPACE FORWARD DECLARATION
// Here is all the types of std namespace that we will provide utilities
namespace std {
#if __cplusplus < 201403L
template <class T, T... Ints>
class integer_sequence;
#endif
template <typename T, std::size_t N>
class array;
};  // namespace std

namespace type_expr {

// -------------------------------------------------------
// FIRST CLASS CITIZEN OF TYPE_EXPR
// -------------------------------------------------------

// INPUT : Universal type container of types (aka typelist).
template <typename... Ts>
struct input_ {
  typedef input_ type;
  template <typename...>
  struct f {
    typedef input_<Ts...> type;
  };
};
template <typename T>
struct input_<T> {
  typedef T type;
  template <typename... Ts>
  struct f {
    typedef T type;
  };
};

// NOTHING : Universal representation of the concept of nothing
using nothing = input_<>;

// ERROR : Template used for debugging
template <typename... MSG>
struct error_ {
  template <typename...>
  struct f {
    typedef error_<MSG...> type;
  };
};

// LS_ : user-declared container
template <typename...>
struct ls_ {};

// IDENTITY : Continue with whatever the input_s were.
struct identity {
  template <typename... Ts>
  struct f {
    typedef input_<Ts...> type;
  };
  template <typename T>
  struct f<T> {
    typedef T type;
  };
};

// I : Universal integer type.
template <int V>
using i = std::integral_constant<int, V>;

// ZERO : User-specialized to retrieve the zero of their type.
template <typename T>
struct zero;

// First specialization of zero. The zero of any i<Num> is i<0>
template <typename T, T value>
struct zero<std::integral_constant<T, value>> {
  typedef std::integral_constant<T, 0> type;
};

// Second specilisation of zero. The zero of  any ratio<N,D> is ratio<0>
template <std::size_t num, std::size_t den>
struct zero<std::ratio<num, den>> {
  typedef std::ratio<0, den> type;
};

// Third specialization of zero. The zero of any std::integer_sequence is with
// the same amount of zero
template <typename T, T... value>
struct zero<std::integer_sequence<T, value...>> {
  typedef std::integer_sequence<T, (value * 0)...> type;
};

// B : Universal boolean type.
template <bool B>
using b = std::integral_constant<bool, B>;

// -------------------------------------------------------
// METAEXPRESSION
// -------------------------------------------------------

// LIFT_ : Universal customization point using template template. Get the ::type
// The Farming field of our library
template <template <typename...> class F>
struct lift_ {
  template <typename... Ts>
  struct f {
    typedef typename F<Ts...>::type type;
  };
};
// QUOTE_ : Universal wrapper. Doesn't get the ::type. Use with template alias
// The Other Farming field of our library
template <template <typename...> class F>
struct quote_ {
  template <typename... Ts>
  struct f {
    typedef F<Ts...> type;
  };
};
struct quote_std_integer_sequence {
  template <typename... Is>
  struct f {
    typedef std::integer_sequence<int, Is::value...> type;
  };
};
struct quote_std_array {
  template <typename T, typename N>
  struct f {
    typedef std::array<int, N::value> type;
  };
};
// FOLD_LEFT_ : Fold expression
// The Farmer of the library
template <typename F>
struct fold_left_ {
  template <typename... Ts>
  struct f {
    typedef error_<fold_left_<F>,Ts... > type;
  };
  template <typename A>
  struct f<A> {
    typedef A type;
  };

  template <typename... A>
  struct f<input_<A...>> {
    typedef typename input_<A...>::type type;
  };
  // helper alias to use with the monster below
  template <typename A, typename B>
  using f_impl = typename F::template f<A, B>::type;

  template <typename T0, typename T1, typename... Ts>
  struct f<T0, T1, Ts...> : f<f_impl<T0, T1>, Ts...> {};
  template <typename T0, typename T1, typename T2, typename... Ts>
  struct f<T0, T1, T2, Ts...> : f<f_impl<f_impl<T0, T1>, T2>, Ts...> {};
  template <typename T0, typename T1, typename T2, typename T3, typename... Ts>
  struct f<T0, T1, T2, T3, Ts...>
      : f<f_impl<f_impl<f_impl<T0, T1>, T2>, T3>, Ts...> {};
  template <typename T0, typename T1, typename T2, typename T3, typename T4,
            typename... Ts>
  struct f<T0, T1, T2, T3, T4, Ts...>
      : f<f_impl<f_impl<f_impl<f_impl<T0, T1>, T2>, T3>, T4>, Ts...> {};
  template <typename T0, typename T1, typename T2, typename T3, typename T4,
            typename T5, typename... Ts>
  struct f<T0, T1, T2, T3, T4, T5, Ts...>
      : f<f_impl<f_impl<f_impl<f_impl<f_impl<T0, T1>, T2>, T3>, T4>, T5>,
          Ts...> {};
  template <typename T0, typename T1, typename T2, typename T3, typename T4,
            typename T5, typename T6, typename... Ts>
  struct f<T0, T1, T2, T3, T4, T5, T6, Ts...>
      : f<f_impl<f_impl<f_impl<f_impl<f_impl<f_impl<T0, T1>, T2>, T3>, T4>, T5>,
                 T6>,
          Ts...> {};
  template <typename T0, typename T1, typename T2, typename T3, typename T4,
            typename T5, typename T6, typename T7, typename... Ts>
  struct f<T0, T1, T2, T3, T4, T5, T6, T7, Ts...>
      : f<f_impl<
              f_impl<f_impl<f_impl<f_impl<f_impl<f_impl<T0, T1>, T2>, T3>, T4>,
                            T5>,
                     T6>,
              T7>,
          Ts...> {};
};

// PIPE_EXPR : Internal-only. Take a type and send it as input_ to the next
// metafunction.
// The Flour and Yeast of the library.
template <typename...>
struct pipe_context {
  typedef nothing type;
};
template <typename T, typename G>
struct pipe_context<T, G> {
  typedef typename G::template f<T>::type type;
};
template <typename... Ts, typename G>
struct pipe_context<input_<Ts...>, G> {
  typedef typename G::template f<Ts...>::type type;
};
template <typename ErrorT, typename G>
struct pipe_context<error_<ErrorT>, G> {
  typedef error_<ErrorT> type;
};

// PIPE_ : Universal container of metafunction.
// The Bread and Butter of the library
template <typename... Cs>
struct pipe_;

// PIPE_ : Helper alias
template <typename... Fs>
using eval_pipe_ = typename pipe_<Fs...>::template f<>::type;
#if __cplusplus > 201403L
template <typename... Fs>
constexpr eval_pipe_<Fs...> eval_pipe_v = eval_pipe_<Fs...>::value;
#endif

template <typename... Cs>
struct pipe_ {
  template <typename... Ts>
  struct f {
    typedef
        typename fold_left_<lift_<pipe_context>>::template f<input_<Ts...>,
                                                             Cs...>::type type;
  };
  // No ::type. This is a problem since it's always instanciated even if not
  // asked. required to have an alias eval_pipe_ = typename
  // pipe_<Fs...>::template f<>::type; to instanciate to the result type;

  template <typename... Ts, typename... Us>
  constexpr pipe_<Cs..., Us...> const operator|(const pipe_<Us...> &) {
    return {};
  };
};

template <typename... Fs, typename... Args>
constexpr eval_pipe_<Fs...> eval(const pipe_<Fs...> &, Args &&... args) {
  return eval_pipe_<Fs...>{std::forward<Args>(args)...};
}

// FORK_ : Inputs are copied to each metafunctions
// The Peanut Butter of the library
template <typename... Cs>
struct fork_ {
  template <typename... Ts>
  struct f {
    typedef input_<typename Cs::template f<Ts...>::type...> type;
  };
};

// UNWRAP : Universal unwrapper.
struct unwrap {
  struct unwrappable_type {};
  template <typename... Ts>
  struct f {
    typedef error_<unwrappable_type> type;
  };
  template <template <typename... Ts> class F, typename... Ts>
  struct f<F<Ts...>> {
    typedef typename input_<Ts...>::type type;
  };
  template <template <typename... Ts> class F, typename... Ts>
  struct f<const F<Ts...>> {
    typedef typename input_<Ts...>::type type;
  };
  template <template <typename... Ts> class F, typename... Ts>
  struct f<F<Ts...> &> {
    typedef typename input_<Ts...>::type type;
  };
  template <template <typename... Ts> class F, typename... Ts>
  struct f<const F<Ts...> &> {
    typedef typename input_<Ts...>::type type;
  };
};

template <typename A, typename B>
struct is_same : std::false_type {};
template <typename A>
struct is_same<A, A> : std::true_type {};
template <typename A, typename B>
struct is_not_same : std::true_type {};
template <typename A>
struct is_not_same<A, A> : std::false_type {};

// IS_ : Comparaison metafunction.
template <typename... Ts>
struct is_ {
  template <typename... Us>
  struct f {
    typedef typename is_same<ls_<Ts...>, ls_<Us...>>::type type;
  };
};

// ISNT_
template <typename... Ts>
struct isnt_;
template <typename T, typename... Ts>
struct isnt_<T, Ts...> {
  template <typename... Us>
  struct f {
    typedef typename is_not_same<ls_<T, Ts...>, ls_<Us...>>::type type;
  };
};

// CONTAINER : Continue with a type that can contain a template template
// OR THE TYPE ITSELF IF NOT A TEMPLATE TEMPLATE;
struct container {
  struct sorry_not_implemented {};
  template <typename T>
  struct f : input_<T> {};
  template <template <typename...> class F, typename... Ts>
  struct f<F<Ts...>> : input_<quote_<F>> {};
};

template <template <typename...> class F>
struct container_is_ : pipe_<container, is_<quote_<F>>> {};

static_assert(eval_pipe_<input_<ls_<int>>, container, is_<quote_<ls_>>>::value,
              "");

// TRANSFORM_ : Similar to haskell's map. Also similar to std::transform
template <typename... Fs>
struct transform_ {
  template <typename... Ts>
  struct f {
    typedef input_<typename pipe_context<Ts, pipe_<Fs...>>::type...> type;
  };
};

// LISTIFY_ : wrap input_s into ls_
// Could have been implemented by struct listify : quote_<ls_>{}
struct listify {
  template <typename... Ts>
  struct f {
    typedef ls_<Ts...> type;
  };
};

// REVERSE : Reverse the order of the types
struct reverse {
  template <typename...>
  struct f_impl;
  template <template <typename...> class F, typename... Ts, typename T>
  struct f_impl<F<Ts...>, T> {
    typedef F<T, Ts...> type;
  };

  template <typename... Ts>
  struct f {
    typedef
        typename pipe_<input_<input_<>, Ts...>, fold_left_<lift_<f_impl>>>::type
            type;
  };
};

// CALL_F : The input_ is a metafunction. Call it.
struct call_f {
  template <typename... T>
  struct f : pipe_<T...> {};
};

// CONSTRUCT_FS : Construct a metafunction from the input_s. and call it.
template <typename... Fs>
struct construct_fs_ : pipe_<Fs..., call_f> {
  template <typename... Ts>
  struct f : pipe_<input_<Ts...>, Fs..., call_f> {};
};

template <typename... Predicate>
struct conditional {
 private:
  struct Type_not_a_predicate {};

 public:
  template <typename...>
  struct f : input_<error_<Type_not_a_predicate, Predicate...>> {};
};
template <>
struct conditional<std::true_type> {
  template <typename T, typename F>
  struct f {
    typedef T type;
  };
};
template <>
struct conditional<std::false_type> {
  template <typename T, typename F>
  struct f {
    typedef F type;
  };
};

// COND_ : Similar to std::conditional but only accept metafunctions
template <typename UnaryPredicate, typename True_Expression,
          typename False_Expression>
struct cond_ {
  template <typename... Ts>
  struct f {
    typedef
        typename conditional<typename UnaryPredicate::template f<Ts...>::type>::
            template f<True_Expression,
                       False_Expression>::type::template f<Ts...>::type type;
  };
};

// MKSEQ_ : Continue with i<0>, i<1>, ... , i<N-1>
// Implementation of mkseq -------------------------------------------
template <std::size_t... i>
struct index_sequence {
  typedef std::size_t value_type;

  typedef index_sequence<i...> type;
  typedef input_<std::integral_constant<int, i>...> to_types;
};

// this structure doubles index_sequence elements.
// s- is number of template arguments in IS.
template <std::size_t s, typename IS>
struct doubled_index_sequence;

template <std::size_t s, std::size_t... i>
struct doubled_index_sequence<s, index_sequence<i...>> {
  typedef index_sequence<i..., (s + i)...> type;
};

// this structure incremented by one index_sequence, iff NEED-is true,
// otherwise returns IS
template <bool NEED, typename IS>
struct inc_index_sequence;

template <typename IS>
struct inc_index_sequence<false, IS> {
  typedef IS type;
};

template <std::size_t... i>
struct inc_index_sequence<true, index_sequence<i...>> {
  typedef index_sequence<i..., sizeof...(i)> type;
};

// helper structure for make_index_sequence.
template <std::size_t N>
struct make_index_sequence_impl
    : inc_index_sequence<
          (N % 2 != 0),
          typename doubled_index_sequence<
              N / 2, typename make_index_sequence_impl<N / 2>::type>::type> {};

// helper structure needs specialization only with 0 element.
template <>
struct make_index_sequence_impl<0> {
  typedef index_sequence<> type;
};

// OUR make_index_sequence,  gcc-4.4.7 doesn't support `using`,
// so we use struct instead of it.
template <std::size_t N>
struct make_index_sequence : make_index_sequence_impl<N>::type {};

// index_sequence_for  any variadic templates
template <typename... T>
struct index_sequence_for : make_index_sequence<sizeof...(T)> {};

// MKSEQ actual implementation.
struct mkseq {
  struct not_integral_constant_int {};
  template <typename... Ts>
  struct f {
    typedef error_<not_integral_constant_int> type;
  };
  template <int I>
  struct f<i<I>> {
    typedef typename make_index_sequence<I>::to_types type;
  };
};

// ZIP : Join together two list of type in multiple ls_
struct zip {
  template <typename...>
  struct f {
    typedef error_<zip> type;
  };
  template <template <typename...> class F, typename... Ts, typename... Us>
  struct f<F<Ts...>, F<Us...>> {
    typedef input_<ls_<Ts, Us>...> type;
  };
};
static_assert(eval_pipe_<input_<ls_<int, int>, ls_<float, char>>, zip,
                         is_<ls_<int, float>, ls_<int, char>>>::value,
              "");

// ZIP_INDEX
struct zip_index {
  template <typename... Ts>
  struct f_impl {
    typedef nothing type;
  };
  template <typename... Is, typename... Ts>
  struct f_impl<input_<Is...>, input_<Ts...>> {
    typedef input_<input_<Is, Ts>...> type;
  };

  template <typename... Ts>
  struct f {
    typedef typename f_impl<typename mkseq::template f<i<sizeof...(Ts)>>::type,
                            input_<Ts...>>::type type;
  };
};

// UNZIP_INDEX
struct unzip_index {
  struct unzip_index_not_recognized {};
  template <typename... Ts>
  struct f {
    typedef error_<unzip_index_not_recognized> type;
  };
  template <typename... Is, typename... Ts>
  struct f<input_<Is, Ts>...> {
    typedef input_<Ts...> type;
  };
};

// UNZIP
struct unzip {
  template <typename...>
  struct f;
  template <template <typename...> class F, typename... Fs, typename... Gs>
  struct f<F<Fs, Gs>...> : input_<input_<Fs...>, input_<Gs...>> {};
};

// PUSH_FRONT_ : Add anything you want to the front of the inputs.
template <typename... Ts>
struct push_front_ {
  template <typename... Inputs>
  struct f {
    typedef input_<Ts..., Inputs...> type;
  };
};

// PUSH_BACK_ : Add anything you want to the back of the inputs
template <typename... Ts>
struct push_back_ {
  template <typename... Inputs>
  struct f {
    typedef input_<Inputs..., Ts...> type;
  };
};

// GET : Continue with the type a index N
template <int I>
struct get_ {
  struct index_out_of_range {};
  template <bool b, typename... Ts>
  struct fff {
    typedef error_<index_out_of_range> type;
  };
  template <typename... Ts>
  struct fff<true, Ts...> {
    template <typename...>
    struct f_impl {};
    template <typename T>
    struct f_impl<i<I>, T> {
      typedef T type;
    };
    template <typename... Is, typename... Us>
    struct f_impl<input_<Is...>, input_<Us...>> : f_impl<Is, Us>... {};

    typedef typename mkseq::template f<i<sizeof...(Ts)>>::type indexed_input_s;
    typedef typename f_impl<indexed_input_s, input_<Ts...>>::type type;
  };

  template <typename... Ts>
  struct f {
    typedef
        typename fff<(I < sizeof...(Ts)) && (0 < sizeof...(Ts)), Ts...>::type
            type;
  };
};

// FIRST : Continue with the first type
struct first {
  template <typename... Ts>
  struct f {
    typedef nothing type;
  };
  template <typename T, typename... Ts>
  struct f<T, Ts...> {
    typedef T type;
  };
};

// SECOND : Continue with the first type
struct second {
  template <typename... Ts>
  struct f {
    typedef nothing type;
  };
  template <typename T, typename T2, typename... Ts>
  struct f<T, T2, Ts...> {
    typedef T2 type;
  };
};

// THIRD : Continue with the third type
struct third {
  template <typename... Ts>
  struct f {
    typedef nothing type;
  };
  template <typename T, typename T2, typename T3, typename... Ts>
  struct f<T, T2, T3, Ts...> {
    typedef T3 type;
  };
};

// Nth : Continue with the Nth type
typedef first _1st;
typedef second _2nd;
typedef third _3rd;

// LAST : Continue with the last type
struct last {
  template <typename... Ts>
  struct f {
    typedef eval_pipe_<input_<Ts...>, get_<sizeof...(Ts) - 1>> type;
  };
};

template <typename... Ts>
struct flat;
template <typename... Ls>
struct flat<input_<Ls...>> {
  typedef input_<Ls...> type;
};
template <typename... Ls, typename T, typename... Ts>
struct flat<input_<Ls...>, T, Ts...> : flat<input_<Ls..., T>, Ts...> {};
template <typename... Ls, typename T, typename... Is, typename... Ts>
struct flat<input_<Ls...>, T, input_<Is...>, Ts...>
    : flat<input_<Ls..., T, Is...>, Ts...> {};

template <typename... Ls, typename... Fs, typename... Ts>
struct flat<input_<Ls...>, input_<Fs...>, Ts...>
    : flat<input_<Ls..., Fs...>, Ts...> {};
template <typename... Fs, typename... Gs, typename... Ls, typename... Ts>
struct flat<input_<Ls...>, input_<Fs...>, input_<Gs...>, Ts...>
    : flat<input_<Ls..., Fs..., Gs...>, Ts...> {};
template <typename... Fs, typename... Gs, typename... Hs, typename... Ls,
          typename... Ts>
struct flat<input_<Ls...>, input_<Fs...>, input_<Gs...>, input_<Hs...>, Ts...>
    : flat<input_<Ls..., Fs..., Gs..., Hs...>, Ts...> {};
template <typename... Fs, typename... Gs, typename... Hs, typename... Is,
          typename... Ls, typename... Ts>
struct flat<input_<Ls...>, input_<Fs...>, input_<Gs...>, input_<Hs...>,
            input_<Is...>, Ts...>
    : flat<input_<Ls..., Fs..., Gs..., Hs..., Is...>, Ts...> {};

template <typename T>
struct flat_impl {
  typedef input_<T> type;
};
template <typename... Ts>
struct flat_impl<input_<Ts...>> {
  typedef input_<Ts...> type;
};

// FLATTEN : Continue with only one input_. Sub-input_ are removed.
// The dirty but necessary tool of our library
struct flatten {
  template <typename... Ts>
  struct f {
    typedef typename flat<input_<>, Ts...>::type type;
  };
};

// LENGTH : Continue with the number of types in the input_s.
struct length {
  template <typename... Ts>
  struct f {
    typedef i<sizeof...(Ts)> type;
  };
};

// SIZE : Continue with the sizeof(T). T is one input_
struct size {
  template <typename T>
  struct f {
    typedef i<sizeof(T)> type;
  };
};

// ALIGNMENT : Continue with the alignment of one input_.
struct alignment {
  template <typename T>
  struct f {
    typedef i<alignof(T)> type;
  };
};

// NOT_ : Boolean metafunction are inversed
template <typename... UnaryPredicate>
struct not_ : cond_<pipe_<UnaryPredicate...>, input_<std::false_type>,
                    input_<std::true_type>> {};

template <>
struct not_<> {
  template <typename T>
  struct f : input_<std::integral_constant<decltype(!T::value), !T::value>> {};
};
// REMOVE_IF_ : Remove every type where the metafunction "returns"
// std::true_type
template <typename... UnaryPredicate>
struct remove_if_
    : pipe_<
          transform_<cond_<pipe_<UnaryPredicate...>, input_<>, quote_<input_>>>,
          flatten> {};

// PARTITION_ : Continue with two list. First predicate is true, Second
// predicate is false
template <typename... UnaryPredicate>
struct partition_ : fork_<pipe_<remove_if_<not_<UnaryPredicate...>>, listify>,
                          pipe_<remove_if_<UnaryPredicate...>, listify>> {};

// FILTER_ : Remove every type where the metafunction is false.
template <typename... UnaryPredicate>
struct filter_ : remove_if_<not_<UnaryPredicate...>> {};

// REPLACE_IF_ : Replace the type by another if the predicate is true
template <typename UnaryPredicate, typename F>
struct replace_if_ : transform_<cond_<UnaryPredicate, F, identity>> {};

// ALL_OF_ : Thanks to Roland Bock for the inspiration
template <typename... UnaryPredicate>
struct all_of_ {
  template <typename... Ts>
  struct f {
    typedef typename is_same<
        ls_<b<true>,
            typename pipe_<UnaryPredicate...>::template f<Ts>::type...>,
        ls_<typename pipe_<UnaryPredicate...>::template f<Ts>::type...,
            b<true>>>::type type;
  };
};

// ANY_OF_ : Thanks to Roland Bock for the inspiration
template <typename... UnaryPredicate>
struct any_of_ {
  template <typename... Ts>
  struct f {
    typedef typename is_not_same<
        ls_<b<false>,
            typename pipe_<UnaryPredicate...>::template f<Ts>::type...>,
        ls_<typename pipe_<UnaryPredicate...>::template f<Ts>::type...,
            b<false>>>::type type;
  };
};

// NONE_OF : Simply the inverse of any_of_
template <typename... UnaryPredicate>
struct none_of_ : not_<any_of_<UnaryPredicate...>> {};

// COUNT_IF_ : Count the number of type where the predicate is true
template <typename... F>
struct count_if_ : pipe_<remove_if_<not_<F...>>, length> {};

// FIND_IF_ : Return the first index that respond to the predicate, along with
// the type.
template <typename... F>
struct find_if_ : pipe_<zip_index, transform_<listify>,
                        filter_<unwrap, second, F...>, first, unwrap> {};

static_assert(eval_pipe_<input_<float, int, float, int>, find_if_<is_<int>>,
                         is_<i<1>, int>>::value,
              "");

// PRODUCT : Given two lists, continue with every possible lists of two types.
struct product {
 private:
  template <typename... Ts>
  struct impl;

 public:
  template <typename T, typename U>
  struct f {
    typedef input_<input_<input_<T, U>>> type;
  };
  template <typename As, typename... Bs>
  struct f<As, input_<Bs...>> {
    typedef input_<input_<As, Bs>...> type;
  };
  template <typename... As, typename Bs>
  struct f<input_<As...>, input_<Bs>> {
    typedef input_<input_<As, Bs>...> type;
  };
  template <typename A, typename... Bs>
  struct f<input_<A>, input_<Bs...>> {
    typedef input_<input_<A, Bs>...> type;
  };
  template <typename A, typename B>
  struct f<input_<A>, input_<B>> {
    typedef input_<A, B> type;
  };

  template <typename... As, typename... Bs>
  struct f<input_<As...>, input_<Bs...>> {
    typedef eval_pipe_<input_<input_<As, input_<Bs...>>...>,
                       transform_<product>,
                       flatten>  // TODO : Maybe flatten inside the transform
        type;
  };
};

static_assert(eval_pipe_<input_<input_<int[1]>, input_<float[1]>>, product,
                         is_<int[1], float[1]>>::value,
              "");
static_assert(
    eval_pipe_<input_<input_<int[1], int[2]>, input_<float[1]>>, product,
               is_<input_<int[1], float[1]>, input_<int[2], float[1]>>>::value, "");
static_assert(
    eval_pipe_<input_<input_<int[1]>, input_<float[1], float[2]>>, product,
               is_<input_<int[1], float[1]>, input_<int[1], float[2]>>>::value,
    "");
static_assert(
    eval_pipe_<input_<input_<int[1], int[2]>, input_<float[1], float[2]>>,
               product,
               is_<input_<int[1], float[1]>, input_<int[1], float[2]>,
                   input_<int[2], float[1]>, input_<int[2], float[2]>>>::value,
    "");

// ROTATE : rotate
// The implementation may rely on undefined behavior.
// But so far clang and gcc are compliant
template <int I>
struct rotate_ {
  template <int N, typename T, typename... Ts>
  struct f_impl : f_impl<N - 1, Ts..., T> {};
  template <typename T, typename... Ts>
  struct f_impl<0, T, Ts...> {
    typedef input_<T, Ts...> type;
  };

  template <typename... Ts>
  struct f : f_impl<I % sizeof...(Ts), Ts...> {};
};

static_assert(eval_pipe_<input_<int, float, short, int[2]>, rotate_<5>,
                         is_<float, short, int[2], int>>::value,
              "");

static_assert(eval_pipe_<input_<int, float, short, int[2]>, rotate_<-1>,
                         is_<int[2], int, float, short>>::value,
              "");

// IS_ZERO : return if the input_ is it's type_zero
struct is_zero {
  template <typename... T>
  struct f {
    typedef b<false> type;
  };
  template <typename T>
  struct f<T> {
    typedef typename std::is_same<T, typename zero<T>::type>::type type;
  };
};

static_assert(eval_pipe_<input_<b<0>>, is_zero>::value, "");
static_assert(eval_pipe_<input_<b<true>>, not_<is_zero>>::value, "");

// IF, AND , OR : WIP
// Not satisfying will revisit later
/*template<typename ... Ps>*/
// struct if_ : pipe_<Ps..., cond_<is_zero,input_<b<false>>,input_< b<true>>> >
//{};
// template<typename P>
// struct and_: pipe_<P,cond_<is_zero,input_<b<false>>,identity>>
//{};
// template<typename P>
// struct or_: pipe_<P,cond_<not_<is_zero>,input_<b<true>>, identity>>
//{};

// static_assert(pipe_<input_<b<false>>, if_< is_zero , and_<is_<b<true>>>
// >>::type::value,"");

template <typename UnaryPredicate, typename F>
struct if_then_ : cond_<UnaryPredicate, F, identity> {};

// ARITHMETIC METAFUNCTIONS

// std::integral_constant's boolean operator
template <typename T, T Tvalue, typename U, U Uvalue>
std::integral_constant<bool, (Tvalue < Uvalue)> operator<(
    std::integral_constant<T, Tvalue>, std::integral_constant<U, Uvalue>);
template <typename T, T Tvalue, typename U, U Uvalue>
std::integral_constant<bool, (Tvalue <= Uvalue)> operator<=(
    std::integral_constant<T, Tvalue>, std::integral_constant<U, Uvalue>);
template <typename T, T Tvalue, typename U, U Uvalue>
std::integral_constant<bool, (Tvalue > Uvalue)> operator>(
    std::integral_constant<T, Tvalue>, std::integral_constant<U, Uvalue>);
template <typename T, T Tvalue, typename U, U Uvalue>
std::integral_constant<bool, (Tvalue >= Uvalue)> operator>=(
    std::integral_constant<T, Tvalue>, std::integral_constant<U, Uvalue>);
template <typename T, T Tvalue, typename U, U Uvalue>
std::integral_constant<bool, (Tvalue == Uvalue)> operator==(
    std::integral_constant<T, Tvalue>, std::integral_constant<U, Uvalue>);

template <intmax_t nA, intmax_t dA, intmax_t nB, intmax_t dB>
std::ratio_less<std::ratio<nA, dA>, std::ratio<nB, dB>> operator<(
    const std::ratio<nA, dA> &, const std::ratio<nB, dB> &);
template <intmax_t nA, intmax_t dA, intmax_t nB, intmax_t dB>
std::ratio_less_equal<std::ratio<nA, dA>, std::ratio<nB, dB>> operator<=(
    const std::ratio<nA, dA> &, const std::ratio<nB, dB> &);
template <intmax_t nA, intmax_t dA, intmax_t nB, intmax_t dB>
std::ratio_greater<std::ratio<nA, dA>, std::ratio<nB, dB>> operator>(
    const std::ratio<nA, dA> &, const std::ratio<nB, dB> &);
template <intmax_t nA, intmax_t dA, intmax_t nB, intmax_t dB>
std::ratio_greater_equal<std::ratio<nA, dA>, std::ratio<nB, dB>> operator>=(
    const std::ratio<nA, dA> &, const std::ratio<nB, dB> &);
template <intmax_t nA, intmax_t dA, intmax_t nB, intmax_t dB>
std::ratio_equal<std::ratio<nA, dA>, std::ratio<nB, dB>> operator==(
    const std::ratio<nA, dA> &, const std::ratio<nB, dB> &);

// LESS
template <typename... Ts>
struct less_ {
  typedef error_<less_<Ts...>> type;
};
template <typename P>
struct less_<P> {
  template <typename B>
  struct f {
    typedef decltype(std::declval<B>() < std::declval<P>()) type;
  };
};
template <>
struct less_<> {
  template <typename P, typename B>
  struct f {
    typedef decltype(std::declval<P>() < std::declval<B>()) type;
  };
};
// LESS_EQ
template <typename... Ts>
struct less_eq_ {
  typedef error_<less_<Ts...>> type;
};
template <typename P>
struct less_eq_<P> {
  template <typename B>
  struct f {
    typedef decltype(std::declval<B>() <= std::declval<P>()) type;
  };
};
template <>
struct less_eq_<> {
  template <typename P, typename B>
  struct f {
    typedef decltype(std::declval<P>() <= std::declval<B>()) type;
  };
};

// GREATER
template <typename... Ts>
struct greater_ {
  typedef error_<greater_<Ts...>> type;
};
template <typename P>
struct greater_<P> {
  template <typename B>
  struct f {
    typedef decltype(std::declval<B>() > std::declval<P>()) type;
  };
};
template <>
struct greater_<> {
  template <typename P, typename B>
  struct f {
    typedef decltype(std::declval<P>() > std::declval<B>()) type;
  };
};
// GREATER_EQ
template <typename... Ts>
struct greater_eq_ {
  typedef error_<greater_eq_<Ts...>> type;
};
template <typename P>
struct greater_eq_<P> {
  template <typename B>
  struct f {
    typedef decltype(std::declval<B>() >= std::declval<P>()) type;
  };
};
template <>
struct greater_eq_<> {
  template <typename P, typename B>
  struct f {
    typedef decltype(std::declval<P>() >= std::declval<B>()) type;
  };
};

// PLUS
template <typename... Ts>
struct plus_ {
  typedef error_<plus_<Ts...>> type;
};
template <typename P>
struct plus_<P> {
  template <typename... Ts>
  struct f;
  template <typename I>
  struct f<I> {
    typedef decltype(std::declval<I>() + std::declval<P>()) type;
  };
};
template <typename T, T t>
struct plus_<std::integral_constant<T, t>> {
  template <typename>
  struct f;
  template <typename U, U u>
  struct f<std::integral_constant<U, u>>
      : std::integral_constant<decltype(t + u), t + u> {};
};
template <intmax_t nA, intmax_t dA>
struct plus_<std::ratio<nA, dA>> {
  template <typename>
  struct f;
  template <intmax_t nB, intmax_t dB>
  struct f<std::ratio<nB, dB>> {
    typedef std::ratio_add<std::ratio<nA, dA>, std::ratio<nB, dB>> type;
  };
};

template <>
struct plus_<> {
  template <typename P, typename B>
  struct f {
    typedef decltype(std::declval<P>() + std::declval<B>()) type;
  };
  template <typename T, typename U, T t, U u>
  struct f<std::integral_constant<T, t>, std::integral_constant<U, u>> {
    typedef std::integral_constant<decltype(t + u), t + u> type;
  };
  template <intmax_t nA, intmax_t dA, intmax_t nB, intmax_t dB>
  struct f<std::ratio<nA, dA>, std::ratio<nB, dB>> {
    typedef std::ratio_add<std::ratio<nA, dA>, std::ratio<nB, dB>> type;
  };
};

// MINUS
template <typename... Ts>
struct minus_ {
  typedef error_<minus_<Ts...>> type;
};
template <typename P>
struct minus_<P> {
  template <typename... Ts>
  struct f;
  template <typename I>
  struct f<I> {
    typedef decltype(std::declval<I>() - std::declval<P>()) type;
  };
};
template <typename T, T t>
struct minus_<std::integral_constant<T, t>> {
  template <typename>
  struct f;
  template <typename U, U u>
  struct f<std::integral_constant<U, u>>
      : std::integral_constant<decltype(t - u), t - u> {};
};
template <intmax_t nA, intmax_t dA>
struct minus_<std::ratio<nA, dA>> {
  template <typename>
  struct f;
  template <intmax_t nB, intmax_t dB>
  struct f<std::ratio<nB, dB>> {
    typedef std::ratio_subtract<std::ratio<nA, dA>, std::ratio<nB, dB>> type;
  };
};

template <>
struct minus_<> {
  template <typename P, typename B>
  struct f {
    typedef decltype(std::declval<P>() - std::declval<B>()) type;
  };
  template <typename T, typename U, T t, U u>
  struct f<std::integral_constant<T, t>, std::integral_constant<U, u>> {
    typedef std::integral_constant<decltype(t - u), t - u> type;
  };
  template <intmax_t nA, intmax_t dA, intmax_t nB, intmax_t dB>
  struct f<std::ratio<nA, dA>, std::ratio<nB, dB>> {
    typedef std::ratio_subtract<std::ratio<nA, dA>, std::ratio<nB, dB>> type;
  };
};

// Multiply
template <typename... Ts>
struct multiply_ {
  typedef error_<multiply_<Ts...>> type;
};
template <typename P>
struct multiply_<P> {
  template <typename... Ts>
  struct f;
  template <typename I>
  struct f<I> {
    typedef decltype(std::declval<I>() * std::declval<P>()) type;
  };
};
template <typename T, T t>
struct multiply_<std::integral_constant<T, t>> {
  template <typename>
  struct f;
  template <typename U, U u>
  struct f<std::integral_constant<U, u>>
      : std::integral_constant<decltype(t * u), t * u> {};
};
template <intmax_t nA, intmax_t dA>
struct multiply_<std::ratio<nA, dA>> {
  template <typename>
  struct f;
  template <intmax_t nB, intmax_t dB>
  struct f<std::ratio<nB, dB>> {
    typedef std::ratio_multiply<std::ratio<nA, dA>, std::ratio<nB, dB>> type;
  };
};

template <>
struct multiply_<> {
  template <typename P, typename B>
  struct f {
    typedef decltype(std::declval<P>() * std::declval<B>()) type;
  };
  template <typename T, typename U, T t, U u>
  struct f<std::integral_constant<T, t>, std::integral_constant<U, u>> {
    typedef std::integral_constant<decltype(t * u), t * u> type;
  };
  template <intmax_t nA, intmax_t dA, intmax_t nB, intmax_t dB>
  struct f<std::ratio<nA, dA>, std::ratio<nB, dB>> {
    typedef std::ratio_multiply<std::ratio<nA, dA>, std::ratio<nB, dB>> type;
  };
};

// Multiply
template <typename... Ts>
struct divide_ {
  typedef error_<divide_<Ts...>> type;
};
template <typename P>
struct divide_<P> {
  template <typename... Ts>
  struct f;
  template <typename I>
  struct f<I> {
    typedef decltype(std::declval<I>() / std::declval<P>()) type;
  };
};
template <typename T, T t>
struct divide_<std::integral_constant<T, t>> {
  template <typename>
  struct f;
  template <typename U, U u>
  struct f<std::integral_constant<U, u>>
      : std::integral_constant<decltype(t / u), t / u> {};
};
template <intmax_t nA, intmax_t dA>
struct divide_<std::ratio<nA, dA>> {
  template <typename>
  struct f;
  template <intmax_t nB, intmax_t dB>
  struct f<std::ratio<nB, dB>> {
    typedef std::ratio_divide<std::ratio<nA, dA>, std::ratio<nB, dB>> type;
  };
};

template <>
struct divide_<> {
  template <typename P, typename B>
  struct f {
    typedef decltype(std::declval<P>() / std::declval<B>()) type;
  };
  template <typename T, typename U, T t, U u>
  struct f<std::integral_constant<T, t>, std::integral_constant<U, u>> {
    typedef std::integral_constant<decltype(t / u), t / u> type;
  };
  template <intmax_t nA, intmax_t dA, intmax_t nB, intmax_t dB>
  struct f<std::ratio<nA, dA>, std::ratio<nB, dB>> {
    typedef std::ratio_divide<std::ratio<nA, dA>, std::ratio<nB, dB>> type;
  };
};

// EQUAL
template <typename... Ts>
struct equal_ {
  typedef error_<equal_<Ts...>> type;
};
template <typename P>
struct equal_<P> {
  template <typename... Ts>
  struct f;
  template <typename I>
  struct f<I> {
    typedef decltype(std::declval<I>() == std::declval<P>()) type;
  };
};
template <>
struct equal_<> {
  template <typename P, typename B>
  struct f {
    typedef decltype(std::declval<P>() == std::declval<B>()) type;
  };
};

// PLUS
template <typename... Ts>
struct modulo_ {
  typedef error_<modulo_<Ts...>> type;
};
template <typename P>
struct modulo_<P> {
  template <typename... Ts>
  struct f;
  template <typename I>
  struct f<I> {
    typedef decltype(std::declval<I>() % std::declval<P>()) type;
  };
};
template <typename T, T t>
struct modulo_<std::integral_constant<T, t>> {
  template <typename>
  struct f;
  template <typename U, U u>
  struct f<std::integral_constant<U, u>> {
    typedef std::integral_constant<decltype(u % t), u % t> type;
  };
};
template <intmax_t nA, intmax_t dA>
struct modulo_<std::ratio<nA, dA>> {
  template <typename...>
  struct f {
    typedef error_<plus_<std::ratio<nA, dA>>> type;
  };
};
template <>
struct modulo_<> {
  template <typename P, typename B>
  struct f {
    typedef decltype(std::declval<P>() % std::declval<B>()) type;
  };
  template <typename T, typename U, T t, U u>
  struct f<std::integral_constant<T, t>, std::integral_constant<U, u>> {
    typedef std::integral_constant<decltype(t % u), t % u> type;
  };
};

struct gcd {
  template <typename...>
  struct f {
    typedef error_<gcd> type;
  };
  template <typename T>
  struct f<T, typename zero<T>::type> {
    typedef T type;
  };
  template <typename T, typename U>
  struct f<T, U> : f<U, typename modulo_<>::template f<T, U>::type> {};
  template <typename T>
  struct f<T, T> {
    typedef T type;
  };
};

struct lcm {
  template <typename...>
  struct f;
  template <typename T, typename U>
  struct f<T, U> {
    typedef typename divide_<>::template f<
        typename multiply_<>::template f<T, U>::type,
        typename gcd::template f<T, U>::type>::type type;
  };
};

static_assert(eval_pipe_<input_<std::ratio<1, 4>>, plus_<std::ratio<2, 5>>,
                         equal_<std::ratio<26, 40>>>::value,
              "");

static_assert(eval_pipe_<input_<i<3>>, plus_<i<1>>,
                         if_then_<is_<i<4>>, plus_<i<2>>>, is_<i<6>>>::value,
              "");
static_assert(eval_pipe_<input_<i<3>>, plus_<i<1>>,
                         if_then_<is_<i<3>>, plus_<i<2>>>, is_<i<4>>>::value,
              "");

// SORT : Given a binary predicate, sort the types
// note : it's implicit that you receive two types, so you probably need to
// transform them. eg : sort by size : sort_<transform_<size>, greater_<> >

template <typename A, typename B>
struct eager {
  typedef input_<> type;
};

template <typename A>
struct eager<A, std::true_type> {
  typedef input_<A> type;
};

template <typename... BinaryPredicate>
struct sort_ {
  template <typename... L>
  struct sort_impl;
  template <typename T>
  struct sort_impl<T> {
    typedef T type;
  };

  template <typename... Ts, typename F>
  struct sort_impl<input_<F, Ts...>> {
    typedef typename sort_impl<typename flat<
        input_<>,
        typename eager<Ts, typename pipe_<BinaryPredicate...>::template f<
                               Ts, F>::type>::type...>::type>::type Yes_types;
    typedef typename sort_impl<typename flat<
        input_<>,
        typename eager<Ts, typename not_<pipe_<BinaryPredicate...>>::template f<
                               Ts, F>::type>::type...>::type>::type No_types;

    typedef typename flat<input_<>, Yes_types, F, No_types>::type type;
  };
  template <typename... Ts>
  struct f {
    typedef typename sort_impl<input_<Ts...>>::type type;
  };
};

// GROUP : Given a Unary Function, Gather those that give the same result
template <typename... Fs>
struct group_by_ : sort_<transform_<Fs...>, not_<lift_<std::is_same>>> {};

static_assert(
    eval_pipe_<
        input_<i<1>, i<2>, i<3>, i<4>>, group_by_<modulo_<i<2>>>,
        is_<std::integral_constant<int, 2>, std::integral_constant<int, 4>,
            std::integral_constant<int, 1>,
            std::integral_constant<int, 3>>>::value,
    "");

// PUSH_OUT
// A very strange algorithm that is technically half a quicksort.
template <typename... BinaryPredicate>
struct push_out_ {
  template <typename... L>
  struct sort_impl;
  template <typename T>
  struct sort_impl<T> {
    typedef T type;
  };

  template <typename... Ts, typename F>
  struct sort_impl<input_<F, Ts...>> {
    typedef typename sort_impl<typename flat<
        input_<>,
        typename eager<Ts, typename not_<pipe_<BinaryPredicate...>>::template f<
                               Ts, F>::type>::type...>::type>::type No_types;

    typedef typename flat<input_<>, F, No_types>::type type;
  };
  template <typename... Ts>
  struct f {
    typedef typename sort_impl<input_<Ts...>>::type type;
  };
};

// UNIQUE : Keep only one of each different types
// The implementation is special but work.
struct unique : push_out_<lift_<std::is_same>> {};

static_assert(eval_pipe_<input_<void, int, void, float, float, int>, unique,
                         is_<void, int, float>>::value,
              "");

// COPY_ : Copy N times the inputs.
// Implemented as a higher meta-expression
template <unsigned int N>
struct copy_ : eval_pipe_<input_<i<N>>, mkseq, transform_<input_<identity>>,
                          quote_<fork_>> {};

// REPEAT_ : Repeat N times the meta-expression
template <std::size_t N, typename... Es>
struct repeat_ : eval_pipe_<input_<Es...>, copy_<N>, flatten, quote_<pipe_>> {};

static_assert(
    eval_pipe_<input_<int>,
               repeat_<2, lift_<std::add_pointer>, lift_<std::add_const>>,
               is_<int *const *const>>::value,
    "");

// SWIZZLE : Restructure the inputs using the index
template <std::size_t... Is>
struct swizzle_ : fork_<get_<Is>...> {};

static_assert(
    eval_pipe_<input_<int, int *, int **, int ***>, swizzle_<2, 1, 0, 3, 1>,
               is_<int **, int *, int, int ***, int *>>::value,
    "");

// ON_ARGS_ : unwrap rewrap in the same template template.
//
template <typename... Es>
struct on_args_ {
  template <typename... Ts>
  struct f : input_<error_<on_args_<Es...>>> {};
  template <template <typename... Ts> class F, typename... Ts>
  struct f<F<Ts...>> {
    typedef eval_pipe_<input_<Ts...>, Es..., quote_<F>> type;
  };
  template <template <typename... Ts> class F, typename... Ts>
  struct f<F<Ts...> &> {
    typedef eval_pipe_<input_<Ts...>, Es..., quote_<F>> type;
  };
};
static_assert(eval_pipe_<input_<ls_<int, int[2], int[3]>>,
                         on_args_<sort_<transform_<size>, greater_<>>>,
                         is_<ls_<int[3], int[2], int>>>::value,
              "");

};  // namespace type_expr
#endif
