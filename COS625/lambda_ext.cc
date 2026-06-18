// Código-alvo
// X x; <- functor que deveolver o valor que recebe, representando a variável simbólica
// int tab[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
// vector<int> v;
// tab | cout << x*x << '\n'; tarefa Filter: aplicar o functor(cout << x*x << '\n') a cada elemento da coleção
// tab | [ &v ]( int x ) { v.push_back( x )}; deve copiar os elementos de tab para v
// v | x % 2 == 0 | cout << x*x + 1 << '\n'; tarefa Filter encadeada
// v | x % 2 == 1 | x*x | cout << x << '\n'; tarefa Filter encadeada
// Checklist:
// tab | cout << x*x << '\n' imprime o quadrado de cada elemento?
// tab | [ &v ](...) copia corretamente os elementos para v ?
// O filtro x % 2 == 0 seleciona apenas os pares?
// O encadeamento v | pred | transf | saída funciona com três estágios?
// Compilação limpa com -Wall -Wextra -std=c++20 ?

#include <ostream>
#include <iostream>
#include <vector>
#include <functional>
#include <type_traits>
#include <iterator>
#include <utility>

#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wparentheses"
#endif

using namespace std;

template <typename T, typename U>
requires requires (vector<T> valores, U valor) {
    valores.push_back(valor);
}
auto operator+ ( const vector<T>& valores, U valor ) {
    vector<T> resultado { valores };
    resultado.push_back( valor );
    return resultado;
}

// Implementação de Filter
template <typename V, typename F>
auto operator| ( const V& vec, F function) {
    if constexpr ( is_same_v < invoke_result_t < F, decltype ( * std :: begin ( vec ) ) >, bool > ) {
        vector < decay_t < decltype ( * std :: begin ( vec ) ) > > aux;
        for ( auto x: vec)
            if ( invoke ( function, x ) )
                aux.push_back ( x );
        return aux;
    }

    else if constexpr ( is_same_v < invoke_result_t < F, decltype ( * std :: begin ( vec ) ) >, void > ) {
        for ( auto x: vec)
            invoke ( function, x );
    }

    else {
        vector < decay_t < invoke_result_t < F, decltype ( * std :: begin ( vec ) ) > > > aux;
        for ( auto x: vec)
            aux.push_back ( invoke ( function, x ) );
        return aux;
    }
}

template <typename F>
struct Expr {
    F func;

    template <typename T>
    decltype(auto) operator() (T&& valor) const {
        return func(std::forward<T>(valor));
    }

    template <typename I>
    auto operator[] (I indice) const {
        return ::Expr {
            [expr = *this, indice] (auto&& valor) -> decltype(auto) {
                return expr(std::forward<decltype(valor)>(valor))[indice];
            }
        };
    }
};

template <typename F>
Expr(F) -> Expr<F>;

template <typename F>
struct OutputExpr { 
    ostream* out;
    Expr<F> expr;

    template <typename T>
    void operator() (T&& valor) const {
        *out << expr(std::forward<T>(valor));
    }
};

template <typename F>
OutputExpr(ostream*, Expr<F>) -> OutputExpr<F>;

inline constexpr Expr x {
    [] (auto&& valor) -> decltype(auto) {
        return std::forward<decltype(valor)>(valor);
    }
};

template <typename F, typename G>
auto operator* (Expr<F> lhs, Expr<G> rhs) {
    return Expr {
        [lhs, rhs] (auto&& valor) {
            return lhs(valor) * rhs(valor);
        }
    };
}

template <typename F, typename T>
auto operator% (Expr<F> expr, T divisor) {
    return Expr {
        [expr, divisor] (auto&& valor) {
            return expr(valor) % divisor;
        }
    };
}

template <typename F, typename T>
auto operator+ (Expr<F> expr, T cte) {
    return Expr {
        [expr, cte] (auto&& valor) {
            return expr(valor) + cte;
        }
    };
}

template <typename F, typename G>
auto operator+ (Expr<F> lhs, Expr<G> rhs) {
    return Expr {
        [lhs, rhs] (auto&& valor) {
            return lhs(valor) + rhs(valor);
        }
    };
}

template <typename F, typename T>
auto operator== (Expr<F> expr, T cte) {
    return Expr {
        [expr, cte] (auto&& valor) {
            return expr(valor) == cte;
        }
    };
}

template <typename F, typename T>
auto operator!= (Expr<F> expr, T cte) {
    return Expr {
        [expr, cte] (auto&& valor) {
            return expr(valor) != cte;
        }
    };
}

template <typename F, typename G>
auto operator| (Expr<F> lhs, G rhs) {
    return Expr {
        [lhs, rhs] (auto&& valor) -> decltype(auto) {
            return lhs(std::forward<decltype(valor)>(valor)) | rhs;
        }
    };
}

template <typename F>
auto operator<< (ostream& out, Expr<F> expr) {
    return OutputExpr { &out, expr };
}

template <typename F, typename T>
auto operator<< (OutputExpr<F> output, T sufixo) {
    return [output, sufixo] (auto&& valor) {
        output(valor);
        *output.out << sufixo;
    };
}



int main() {
    int tab[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    vector<int> v;

    tab | cout << x*x << '\n';

    tab | [ &v ]( int valor ) { v.push_back( valor ); };

    v | ( x % 2 == 0 ) | cout << x*x + 1 << '\n';

    v | ( x % 2 == 1 ) | x*x | cout << x << '\n';

    v | (x + x + x) | cout << x << ' ';

    string arr[] = { "foo", "bar", "baz" };
    arr | (x + x) | cout << x << ' ';

    return 0;
}