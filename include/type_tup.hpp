//     Copyright 2020 Remi Drolet (drolet.remi@gmail.com)
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE or copy at
//        http://www.boost.org/LICENSE_1_0.txt)

#ifndef TYPE_EXPR_TUP_HPP
#define TYPE_EXPR_TUP_HPP

#include <type_traits>
#include <utility>

#include "type_expr.hpp"

namespace te {

	template<typename T, typename ... Es>
		using sfinae_t =  std::enable_if<eval_pipe_<Es...>::value,T>;

	// FORWARD DECLARATION

	// TUP_INST
	template <typename Index, typename Type>
		struct tup_element;

	// TUP_IMPL
	template <typename... Zip_Indexes_Types>
		struct tup_impl;

	template <typename... Ts>
		using te_tup_metafunction =
		te::eval_pipe_<te::ts_<Ts...>, te::zip_index,te::transform_<wrap_<tup_element>>,/*te::sort_<sort_pred>,*/  te::wrap_<tup_impl>>;

	template<typename ... Ts>
		using tup = te_tup_metafunction<Ts...>;


}

namespace std{
	template<typename > struct tuple_size;
	template<typename ... Ts>
	struct tuple_size<te::tup_impl<Ts...>> : public std::integral_constant<std::size_t,te::tup_impl<Ts...>::size>{};
}

namespace te {
	// TUP
	/*template <typename... Ts>*/
	//struct tup;

	// IMPLEMENTATION

	// TUP_INST
	template <int I, typename T>
		struct tup_element<te::i<I>, T> {
	  		tup_element() : data{}{} 
	  		tup_element(const T& t) : data{t} {} // Ref
			template<typename U>
	  			tup_element(U &&t) : data{std::forward<U>(t)} {} // Move or copy
  			~tup_element() = default;

			tup_element(const tup_element<te::i<I>,T>& other) : data{other.data} {}
			tup_element(te::tup_element<te::i<I>,T>&& o) : data{std::forward<T>(o.data)}{}
			T& get()
			{
				return data;
			}

  			T data;
		};

	template<int Index,typename ... IElem>
		using nth_tup_element_ = te::eval_pipe_<te::ts_<IElem...>,te::zip_index,te::transform_<te::listify>
		,te::sort_<te::transform_<te::unwrap,te::second,te::size>,te::greater_<>>
		, te::at_c<Index>, te::unwrap,te::wrap_<tup_element>
		>;

	// TUP_IMPL
	template <typename... Is, typename... Ts>
		struct tup_impl<tup_element<Is, Ts>...> : nth_tup_element_<Is::value,Ts...>... {
			static constexpr std::size_t size = sizeof...(Ts);


			tup_impl()  : tup_element<Is,Ts>{}... {} 
  			tup_impl(const Ts& ... ts) : tup_element<Is, Ts>(ts)... {}
  			template<typename ... Us>
  				tup_impl(Ts&&... ts) : tup_element<Is, Ts>{std::forward<Ts>(ts)}... {}
  			~tup_impl() = default;

			tup_impl(const tup_impl<tup_element<Is,Ts>...>& o) : tup_element<Is, Ts>{o}... {}
			tup_impl(tup_impl<tup_element<Is,Ts>...>&& o) : tup_element<Is,Ts>{std::forward<tup_element<Is,Ts>>(o)}...{}

  			template <unsigned int I>
  				auto get() -> te::eval_pipe_<te::ts_<te::ls_<Is,Ts>...>,te::filter_<te::unwrap,te::first,te::same_as_<te::i<I>>>,te::unwrap,te::second> & {
					return te::eval_pipe_<te::ts_<te::ls_<Is,Ts>...>,te::filter_<te::unwrap,te::first,te::same_as_<i<I>>>,te::unwrap,te::wrap_<tup_element>>::data;
  				}
		};

	// TUP
	using sort_pred = te::pipe_<te::transform_<te::unwrap,te::second,te::size>,te::greater_<>>;
	
    template<typename ... Ts>
    	using tup_sequence = 
    	te::eval_pipe_<te::mkseq_c<sizeof...(Ts)>>;
    using dft_ctor = te::trait_<std::is_nothrow_default_constructible>;
		//template <typename... Ts>
	//struct tup : te_tup_metafunction<Ts...> {
	//tup() : te_tup_metafunction<Ts...>{}{ 
	//static_assert(te::eval_pipe_<te::ts_<Ts...>,te::all_of_<dft_ctor>>::value,"Type is not default constructible");
	//}
    //tup(const Ts& ... ts) : te_tup_metafunction<Ts...>(ts...) {}
    //template<typename ... Us>
    //tup(Us&&... us) : te_tup_metafunction<Ts...>(std::forward<Ts>(us)...) {}
    //~tup() = default;
	//tup(const te::tup<Ts...>& o) : te_tup_metafunction<Ts...>{(te_tup_metafunction<Ts...>)o} {}
	//tup(te::tup<Ts...>&& o) : te_tup_metafunction<Ts...>{std::forward<te_tup_metafunction<Ts...>>(o)} {}

	/*};*/

	template <class T>
		struct unwrap_refwrapper{using type = T;};
	template <class T>
		struct unwrap_refwrapper<std::reference_wrapper<T>> {using type = T&;};
	template <class T>
		using unwrap_decay_t = typename unwrap_refwrapper<typename std::decay<T>::type>::type;

	template <class... Types>
		static inline constexpr // since C++14
		te::tup<unwrap_decay_t<Types>...> make_tup(Types&&... args)
		{
    		return te::tup<unwrap_decay_t<Types>...>{std::forward<Types>(args)...};
		}

	template <unsigned int I, typename... Ts>
		auto get(te::tup<Ts...> &t) -> decltype(t.template get<I>()) {
  			return t.template get<I>();
		}

	template <class... Types>
		tup<Types &&...> forward_as_tup(Types &&... args) noexcept {
  			return te::tup<Types &&...>(std::forward<Types>(args)...);
		}

	template<typename ... Ts, std::size_t ... Is>
		te::eval_pipe_<te::input_<Ts...>,te::fork_<te::at_c<Is>...>,te::wrap_<tup>> tup_get(std::integer_sequence<std::size_t,Is...>,te::tup<Ts...>& tup)  
		{
			return te::make_tup(std::move(te::get<Is>(tup))...);
		}

	template<typename ... Ts>
		using indexes = 
		te::eval_pipe_<te::input_<Ts...>, te::zip_index
		,te::group_range_<te::second,te::size> 
		,te::sort_<transform_<te::second,te::size>,te::greater_<>>
		,te::transform_<te::first>
		,te::wrap_std_integer_sequence_<std::size_t>
		>;


	// TUP_CAT
	// Given multiple tup, concatenate them using the same expension trick than Eric
	// Niebler The trick is to get two std::integer_sequence which are the cartesian
	// cartesian of each indexes of the tups with their respective index in the
	// concatenation function. given tup< tup<A,B,C>, tup<D>, tup<>, tup<E,F> > //
	// Tup_of_Tups index_of_tup           = std::integer_sequence<int,0,0,0,1,3,3>
	// index inside each tup  = std::integer_sequence<int,0,1,2,0,0,1>
	//
	namespace detail {
		template <int... Is, int... Ks, typename... Ts, typename Tup_of_Tups>
			auto tup_cat_impl(std::integer_sequence<int, Is...>, 
					std::integer_sequence<int, Ks...>,
                    Tup_of_Tups &&tpls) 
            -> decltype(te::make_tup(tpls.template get<Is>().template get<Ks>()...))
            {
  				return te::make_tup(
      					// get<0>().get<0>(), get<0>().get<1>(), ...
      					tpls.template get<Is>().template get<Ks>()...);
			}
	};  // namespace detail

	template<typename ... Ts>
		using tup_cat_return = te::eval_pipe_<te::ts_<Ts...>,te::transform_<te::unwrap,te::transform_<te::unwrap,te::second>>,te::flatten,te::wrap_<te::tup>>;

	template <typename... Tups>
			tup_cat_return<Tups...> tup_cat(Tups &&... tups) {
  				// This do the magic of getting the cartesian cartesian of each tup's types
  				// with the index inside
  				using zip_indexes = te::eval_pipe_<
      				te::ts_<Tups...>, te::transform_<te::trait_<std::remove_reference>,te::trait_<std::tuple_size>, te::mkseq_<>>,
      			te::zip_index, transform_<te::cartesian_<te::listify>>
      				, te::flatten
		  			,transform_<te::unwrap>
		  			, te::unzip>;

  				using tup_index =
      				te::eval_pipe_<zip_indexes, te::first, te::wrap_std_integer_sequence_<int>>;
  				using types_index =
      				te::eval_pipe_<zip_indexes, te::second, te::wrap_std_integer_sequence_<int>>;
  				return detail::tup_cat_impl(
      					tup_index{},    // int_seq
      					types_index{},  // int_seq
      					te::forward_as_tup(std::forward<Tups>(tups)...));
			};

};  // namespace te


#endif  // TYPE_EXPR_TUP_HPP
