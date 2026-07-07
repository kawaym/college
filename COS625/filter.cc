#include <algorithm>
#include <functional>
#include <initializer_list>
#include <type_traits>
#include <vector>

using namespace std;

template<typename Container, typename F,
         typename T = typename remove_reference_t<Container>::value_type>
auto operator|(Container c, F func) {
    using result_t = invoke_result_t<F, T>;
    if constexpr (is_void_v<result_t>) {
        for_each(c.begin(), c.end(), [&func](const T& x) { invoke(func, x); });
    } else if constexpr (is_same_v<result_t, bool>) {
        vector<T> result;
        copy_if(c.begin(), c.end(), back_inserter(result),
                [&func](const T& x) { return invoke(func, x); });
        return result;
    } else {
        vector<result_t> result;
        transform(c.begin(), c.end(), back_inserter(result),
                  [&func](const T& x) { return invoke(func, x); });
        return result;
    }
}

template<typename T, size_t N, typename F>
auto operator|(T (&arr)[N], F func) {
    return vector<T>(arr, arr + N) | func;
}
