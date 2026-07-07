#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

struct PlaceHolder {
};

inline constexpr PlaceHolder __ {};

template <typename F, typename Tuple>
class BindExpression;

template <typename T>
struct is_bind_placeholder : std::false_type {
};

template <>
struct is_bind_placeholder<PlaceHolder> : std::true_type {
};

template <typename T>
inline constexpr bool is_bind_placeholder_v = is_bind_placeholder<std::decay_t<T>>::value;

template <typename Tuple, std::size_t N>
struct trimmed_tuple_size {
	static constexpr std::size_t value = is_bind_placeholder_v<std::tuple_element_t<N - 1, Tuple>>
		? trimmed_tuple_size<Tuple, N - 1>::value
		: N;
};

template <typename Tuple>
struct trimmed_tuple_size<Tuple, 0> {
	static constexpr std::size_t value = 0;
};

template <std::size_t... I, typename Tuple>
auto tuple_take_impl(Tuple&& tuple, std::index_sequence<I...>) {
	return std::make_tuple(std::get<I>(std::forward<Tuple>(tuple))...);
}

template <std::size_t N, typename Tuple>
auto tuple_take(Tuple&& tuple) {
	return tuple_take_impl(std::forward<Tuple>(tuple), std::make_index_sequence<N> {});
}

template <typename Tuple>
auto trim_trailing_placeholders(Tuple&& tuple) {
	using tuple_t = std::decay_t<Tuple>;
	constexpr std::size_t size = trimmed_tuple_size<tuple_t, std::tuple_size_v<tuple_t>>::value;
	return tuple_take<size>(std::forward<Tuple>(tuple));
}

template <typename Tuple>
struct tuple_has_placeholder;

template <typename... T>
struct tuple_has_placeholder<std::tuple<T...>> : std::bool_constant<(is_bind_placeholder_v<T> || ...)> {
};

template <std::size_t Offset, typename Tuple, std::size_t... I>
auto tuple_drop_impl(Tuple&& tuple, std::index_sequence<I...>) {
	return std::make_tuple(std::get<Offset + I>(std::forward<Tuple>(tuple))...);
}

template <std::size_t Offset, typename Tuple>
auto tuple_drop(Tuple&& tuple) {
	using tuple_t = std::decay_t<Tuple>;
	constexpr std::size_t size = std::tuple_size_v<tuple_t>;
	return tuple_drop_impl<Offset>(std::forward<Tuple>(tuple), std::make_index_sequence<size - Offset> {});
}

template <std::size_t StoredIndex, std::size_t IncomingIndex, typename StoredTuple, typename IncomingTuple>
auto merge_arguments_impl(const StoredTuple& stored, const IncomingTuple& incoming) {
	if constexpr (StoredIndex == std::tuple_size_v<StoredTuple>) {
		return tuple_drop<IncomingIndex>(incoming);
	} else if constexpr (
		is_bind_placeholder_v<std::tuple_element_t<StoredIndex, StoredTuple>> &&
		IncomingIndex < std::tuple_size_v<IncomingTuple>
	) {
		return std::tuple_cat(
			std::make_tuple(std::get<IncomingIndex>(incoming)),
			merge_arguments_impl<StoredIndex + 1, IncomingIndex + 1>(stored, incoming)
		);
	} else {
		return std::tuple_cat(
			std::make_tuple(std::get<StoredIndex>(stored)),
			merge_arguments_impl<StoredIndex + 1, IncomingIndex>(stored, incoming)
		);
	}
}

template <typename StoredTuple, typename IncomingTuple>
auto merge_arguments(const StoredTuple& stored, IncomingTuple&& incoming) {
	return trim_trailing_placeholders(
		merge_arguments_impl<0, 0>(stored, std::forward<IncomingTuple>(incoming))
	);
}

template <typename F, typename Tuple, typename Sequence>
struct tuple_is_invocable;

template <typename F, typename Tuple, std::size_t... I>
struct tuple_is_invocable<F, Tuple, std::index_sequence<I...>>
	: std::bool_constant<std::is_invocable_v<F&, std::tuple_element_t<I, Tuple>...>> {
};

template <typename F, typename Tuple>
inline constexpr bool tuple_is_invocable_v = tuple_is_invocable<
	F,
	Tuple,
	std::make_index_sequence<std::tuple_size_v<Tuple>>
>::value;

template <typename F, typename Tuple>
decltype(auto) invoke_from_tuple(F&& function, Tuple&& tuple) {
	return std::apply(
		[&function](auto&&... values) -> decltype(auto) {
			return std::invoke(function, std::forward<decltype(values)>(values)...);
		},
		std::forward<Tuple>(tuple)
	);
}

template <typename F, typename Tuple>
auto make_bind_expression(F&& function, Tuple&& stored) {
	auto normalized = trim_trailing_placeholders(std::forward<Tuple>(stored));
	return BindExpression<std::decay_t<F>, std::decay_t<decltype(normalized)>> {
		std::forward<F>(function),
		std::move(normalized)
	};
}

template <typename F, typename Tuple>
class BindExpression {
	mutable F function;
	Tuple stored;

	template <typename BoundTuple>
	auto finish_or_defer(BoundTuple&& bound) const {
		using bound_t = std::decay_t<BoundTuple>;
		if constexpr (!tuple_has_placeholder<bound_t>::value && tuple_is_invocable_v<F, bound_t>) {
			return invoke_from_tuple(function, std::forward<BoundTuple>(bound));
		} else {
			return make_bind_expression(function, std::forward<BoundTuple>(bound));
		}
	}

public:
	BindExpression(F func, Tuple args)
		: function(std::move(func)), stored(std::move(args)) {
	}

	template <typename... Args>
	auto operator() (Args&&... args) const {
		auto merged = merge_arguments(stored, std::make_tuple(std::forward<Args>(args)...));
		return finish_or_defer(std::move(merged));
	}
};

template <typename F, typename... Args>
auto bind_impl(F&& function, Args&&... args) {
	return make_bind_expression(
		std::forward<F>(function),
		std::make_tuple(std::forward<Args>(args)...)
	);
}

#define bind bind_impl
