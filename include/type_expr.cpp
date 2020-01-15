﻿#include "type_expr.hpp"
#include <assert.h>
#include <type_traits>
  

int main() {
  using namespace type_expr;
  namespace te = type_expr;


  static_assert(std::is_same<i<(4 + 3 * 2 / 1)>, i<10>>::value, "");

  auto input_test = te::eval_pipe_<input<i<1>>, input<i<2>>>{};
  static_assert(std::is_same<decltype(input_test), i<2>>::value, "");

  auto sort_test =
      te::eval_pipe_<input<i<3>, i<1>, i<3>, i<4>, i<3>, i<1>, i<2>, i<3>, i<5>>,
            sort<less_<>>>{};
  sort_test = input<i<1>, i<1>, i<2>, i<3>, i<3>, i<3>, i<3>, i<4>, i<5>>{};

  auto pipe_test = te::eval_pipe_<input<i<3>>, plus_<i<1>>, plus_<i<2>>>{};
  static_assert(std::is_same<decltype(pipe_test), i<6>>::value, "");

  auto fold_test = te::eval_pipe_<input<i<1>, i<2>, i<3>>, fold_left_<plus_<>>>{};
  static_assert(std::is_same<decltype(fold_test), i<6>>::value, "");

  static_assert(std::is_same<decltype(pipe_test), decltype(fold_test)>::value,
                "");

  auto pipe_pipe_test =
      te::eval_pipe_<pipe_<input<i<3>>, plus_<i<1>>>, plus_<i<2>>>{};
  static_assert(std::is_same<decltype(pipe_pipe_test), i<6>>::value, "");
  // -------------------------------------------------------------------
  // -------------------------------------------------------------------
  // -------------------------------------------------------------------
  // UNWRAP TESTING
  // There was a problem with unwrap...
  // those test stay there because I don't want to debug this again.

  te::eval_pipe_<input<ls_<int, i<0>>>, unwrap, second> t11 = i<0>{};
  te::eval_pipe_<input<ls_<int, i<0>>>, unwrap, pipe_<identity, second>> t12 = i<0>{};
  te::eval_pipe_<input<ls_<int, i<0>>>, unwrap, pipe_<second>> t13 = i<0>{};
  te::eval_pipe_<input<ls_<int, i<0>>>, pipe_<unwrap, pipe_<second>>> t14 = i<0>{};

  te::eval_pipe_<input<ls_<int, i<0>>>, pipe_<unwrap, first>, is_<int>> t15 = b<true>{};

  constexpr te::eval_pipe_<input<int>, unwrap,
                  is_<error_<type_expr::unwrap::unwrappable_type>>> t = b<true>{};
  constexpr te::eval_pipe_<input<ls_<int>>, unwrap> pipe_unwrap = 0;
  constexpr te::eval_pipe_<input<ls_<int>>, pipe_<input<ls_<int>>, unwrap>>
      pipe_unwrap2 = 0;
  constexpr te::eval_pipe_<input<ls_<int>>, pipe_<unwrap>> pipe_unwrap3 = 0;
  constexpr te::eval_pipe_<input<ls_<int, float>>, pipe_<unwrap>, is_<int, float>>
      pipe_unwrap4 = b<true>{};
  constexpr te::eval_pipe_<input<ls_<int, float>>, pipe_<unwrap>, get_<0>>
      pipe_unwrap5 = 0;
  //
  // -------------------------------------------------------------------
  // -------------------------------------------------------------------
  // -------------------------------------------------------------------

  // Calculation tests
  // -------------------------------------------------------------------
  static_assert(te::eval_pipe_<input<double>, plus_<int>, is_<double>>::value, "");
  static_assert(te::eval_pipe_<input<i<21>, i<49>>, gcd, is_<i<7>>>::value, "");
  static_assert(te::eval_pipe_<input<i<21>, i<49>>, lcm, is_<i<147>>>::value, "");
  // -------------------------------------------------------------------

  auto pipe_less_ = te::eval_pipe_<input<i<1>, i<2>>, less_<>>{};
  auto pipe_less__2 = te::eval_pipe_<input<i<1>>, less_<i<2>>>{};

  static_assert(pipe_less_.value, "");
  static_assert(pipe_less__2.value, "");

  auto pipe_is_test = te::eval_pipe_<input<i<1>>, is_<i<1>>>{};
  static_assert(pipe_is_test.value, "");

  auto pipe_is_test_multi = te::eval_pipe_<input<i<1>, i<2>>, is_<i<1>, i<2>>>{};
  static_assert(pipe_is_test_multi.value, "");
  // This is such a good feature; instead of always listify then compare with
  // ls_<...>

  auto pipe_isnt_ = te::eval_pipe_<input<i<1>, i<2>>, isnt_<int>>{};
  static_assert(pipe_isnt_.value, "");
  // Isnt_ is the opposite of is_

  auto pipe_add =
      te::eval_pipe_<input<int>, pipe_<push_back_<float>>, is_<int, float>>{};
  static_assert(pipe_add.value, "");

  auto pipe_add2 = te::eval_pipe_<input<int, short>, pipe_<push_back_<float>>,
                         is_<int, short, float>>{};
  static_assert(pipe_add2.value, "");

  static_assert(te::eval_pipe_<input<int, float>, push_back_<ls_<>>,
                      is_<int, float, ls_<>>>::value,
                "");
  static_assert(te::eval_pipe_<input<int, float>, push_front_<ls_<>>,
                      is_<ls_<>, int, float>>::value,
                "");
  static_assert(te::eval_pipe_<push_back_<int>, is_<int>>::value, "");
  // pushing types has never been this easy

  static_assert(te::eval_pipe_<input<int, int, int>, length, is_<i<3>>>::value,
                "");
  // length continue with the number of elements in the inputs inside a i<>

  static_assert(te::eval_pipe_<input<char>, size, is_<i<1>>>::value, "");
  static_assert(
      te::eval_pipe_<input<char, char>, transform_<size>, is_<i<1>, i<1>>>::value,
      "");
  // size continue with the size of the type in the inputs inside a i<>

  static_assert(
      te::eval_pipe_<input<int, float, ls_<int>>, first, is_<int>>::value, "");
  static_assert(
      te::eval_pipe_<input<int, float, ls_<int>>, last, is_<ls_<int>>>::value, "");
  // Getting the first and the last.

  static_assert(te::eval_pipe_<input<int, input<int>, ls_<int, int>>, flatten,
                      is_<int, int, ls_<int, int>>>::value,
                "");
  // Flatten and join_list might be reworked

  static_assert(
      te::eval_pipe_<input<int, int, int>, cond_<is_<int, int>, input<b<false>>,
                                        is_<int, int, int>>>::value,
      "");
  // Conditional have a predicate and continue with the second or the third
  // function

  static_assert(
      te::eval_pipe_<input<int, float, short>, get_<1>, is_<float> >::value, "");
  static_assert(te::eval_pipe_<input<i<1>, i<2>>, transform_<plus_<i<3>>>,
                      is_<i<4>, i<5>>>::value,
                "");
  // Transform apply this metafunctor to the types one by one
  // Can also be called a map_ but I disgress transform_ is a better name

  static_assert(te::eval_pipe_<input<i<1>>, fork_<plus_<i<1>>, plus_<i<2>>>,
                      is_<i<2>, i<3>>>::value,
                "");
  // pipes and forks in a single expression ? Cool Stuff

  static_assert(te::eval_pipe_<input<int, float, short>, partition_<is_<int>>,
                      is_<ls_<int>, ls_<float, short>>>::value,
                "");
  // Partition , like the std counterpart, result in two
  // containers : first one with predicate == true,
  //              second one with predicate == false

  static_assert(
      te::eval_pipe_<input<int, float, int, short>, replace_if_<is_<int>, input<float>>,
            is_<float, float, float, short>>::value,
      "");
  // replace_if transform the type into another if the predicate is true

  static_assert(te::eval_pipe_<input<ls_<int, short>, ls_<float, char>>, product,
                      is_<ls_<int, float>, ls_<int, char>, ls_<short, float>,
                          ls_<short, char>>>::value,
                "");
  // product is a little bit special : given two lists, it return each
  // permutation possible while respecting the order

  static_assert(te::eval_pipe_<input<i<1>, i<2>, i<3>, i<4>, i<5>>, any_of_<is_<i<5>>>,
                      is_<std::true_type>>::value,
                "");
  static_assert(te::eval_pipe_<input<i<1>, i<0>, i<0>>, count_if_<is_<i<0>>>,
                      is_<i<2>>>::value,
                "");
  // Count_if does exactly what is name suggest.

  static_assert(te::eval_pipe_<input<int, int, int>, all_of_<is_<int>>>::value,
                "");
  static_assert(te::eval_pipe_<input<int, float, int>, any_of_<is_<float>>>::value,
                "");
  static_assert(te::eval_pipe_<input<int, float, int>, none_of_<is_<char>>>::value,
                "");
  // Does exactly the same as their std:: counterpart

  static_assert(
      te::eval_pipe_<input<int, int, int>, pipe_<get_<0>>, is_<int>>::value, "");
  static_assert(
      te::eval_pipe_<input<i<1>, i<2>, i<3>>, get_<-1>, is_<> >::value,
      "");
  static_assert(
      te::eval_pipe_<input<int, int, int>, pipe_<get_<3>>, is_<>>::value,
      "");

  static_assert(te::eval_pipe_<input<i<2>>, mkseq, is_<i<0>, i<1>>>::value, "");
  static_assert(te::eval_pipe_<input<i<1>>, mkseq, is_<i<0>>>::value, "");
  static_assert(te::eval_pipe_<input<i<0>>, mkseq, is_<>>::value, "");

  static_assert(te::eval_pipe_<input<int, i<0>>,
                      cond_<pipe_<first, is_<int>>, input<std::true_type>,
                            input<std::false_type>>>::value,
                "");
  static_assert(te::eval_pipe_<input<float, int, float, int>, find_if_<is_<int>>,
                      is_<i<1>, int>>::value,
                "");
  static_assert(te::eval_pipe_<input<int, float>, zip_index,
                      is_<ls_<i<0>, int>, ls_<i<1>, float>>>::value,
                "");
  static_assert(
      te::eval_pipe_<input<i<1>>, plus_<i<1>>, mkseq, is_<i<0>, i<1>>>::value, "");

  return 0;
}
