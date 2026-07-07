#include <algorithm>
#include <vector>
#include <initializer_list>

using namespace std;

template<typename T, typename F>
void operator|(vector<T>& v, F func) {
    for_each(v.begin(), v.end(), func);
}

template<typename T, size_t N, typename F>
void operator|(T (&arr)[N], F func) {
    for_each(arr, arr + N, func);
}

template<typename T, typename F>
void operator|(initializer_list<T> lst, F func) {
    for_each(lst.begin(), lst.end(), func);
}
