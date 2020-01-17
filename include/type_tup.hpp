#ifndef TYPE_EXPR_TUP_HPP
#define TYPE_EXPR_TUP_HPP

#include "type_expr.hpp"

namespace type_expr {

namespace te = type_expr;
// FORWARD DECLARATION

// TUP_INST
template <typename Index, typename Type>
struct tup_inst;

// TUP_IMPL
template <typename... Zip_Indexes_Types>
struct tup_impl;

// TUP
template <typename... Ts>
struct tup;

// IMPLEMENTATION

// TUP_INST
template <int I, typename T>
struct tup_inst<te::i<I>, T> {
  tup_inst(T &&t) : data{std::forward<T>(t)} {}

 protected:
  T data;
};

// TUP_IMPL
template <typename... Is, typename... Ts, template <typename...> class TypeList>
struct tup_impl<TypeList<Is, Ts>...> : tup_inst<Is, Ts>... {
  tup_impl(Ts &&... ts) : tup_inst<Is, Ts>(std::forward<Ts>(ts))... {}
  template <unsigned int I>
  auto get() -> te::eval_pipe_<input<Ts...>, get_<I>> & {
    return te::eval_pipe_<input<tup_inst<Is, Ts>...>, te::get_<I>>::data;
  }
};

// TUP
template <typename... Ts>
using te_tup_metafunction =
    te::eval_pipe_<te::input<Ts...>, te::zip_index, te::quote_<tup_impl>>;

template <typename... Ts>
struct tup : te_tup_metafunction<Ts...> {
  tup(Ts &&... ts) : te_tup_metafunction<Ts...>(std::forward<Ts>(ts)...) {}
};

template <typename... Ts>
te::tup<Ts...> make_tup(Ts &&... ts) {
  return te::tup<Ts...>{std::forward<Ts>(ts)...};
};

template <unsigned int I, typename... Ts>
auto get(te::tup<Ts...> &t) -> decltype(t.template get<I>()) {
  return t.template get<I>();
}

template <class... Types>
tup<Types &&...> forward_as_tup(Types &&... args) noexcept {
  return te::tup<Types &&...>(std::forward<Types>(args)...);
}

namespace detail {
template <typename Ret, typename... Is, typename... Ks, typename Tuples>
Ret tuple_cat_(te::input<Is...>, te::input<Ks...>, Tuples tpls) {
  return Ret{
      std::move(tpls.template get<Is::value>().template get<Ks::value>())...};
}
};  // namespace detail
template <typename... Tups,
          typename Ret = te::eval_pipe_<input<Tups...>,
                                        te::transform_<te::pipe_<te::unwrap>>,
                                        te::flatten, quote_<te::tup>>>
Ret tup_cat(Tups &&... tups) {
  using zip_indexes = te::eval_pipe_<
      te::input<Tups...>,
      te::transform_<te::pipe_<te::unwrap, te::length, te::mkseq, te::listify>>,
      te::zip_index, te::transform_<te::product>, te::flatten, te::unzip>;
  using tup_index = te::eval_pipe_<zip_indexes, te::first>;
  using types_index = te::eval_pipe_<zip_indexes, te::second>;
  return detail::tuple_cat_<Ret>(
      tup_index{}, types_index{},
      te::forward_as_tup(std::forward<Tups>(tups)...));
};

};  // namespace type_expr

#endif  // TYPE_EXPR_TUP_HPP
