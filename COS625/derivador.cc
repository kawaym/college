#include <cmath>
#include <iostream>
#include <type_traits>

// ============================================================
// Forward declarations
// ============================================================
template<typename F, typename G> struct AddExpr;
template<typename F, typename G> struct SubExpr;
template<typename F, typename G> struct MulExpr;
template<typename F, typename G> struct DivExpr;
template<typename F, typename N> struct PowExpr;
template<typename F> struct ExpExpr;
template<typename F> struct LogExpr;
template<typename F> struct SinExpr;
template<typename F> struct CosExpr;

// ============================================================
// CRTP base — provê operator->* para todos os tipos de expressão
// ============================================================
template<typename Derived>
struct ExprBase {
    template<typename N>
    PowExpr<Derived, N> operator->*(N n) const {
        static_assert(std::is_integral_v<N>,
            "Operador de potenciação definido apenas para inteiros");
        return PowExpr<Derived, N>(static_cast<const Derived&>(*this), n);
    }
};

// ============================================================
// Variável simbólica
// ============================================================
struct Var : ExprBase<Var> {
    double e(double v) const { return v; }
    double dx(double)  const { return 1.0; }
};

// ============================================================
// Constante numérica (derivada zero)
// ============================================================
struct Const : ExprBase<Const> {
    double val;
    explicit Const(double v) : val(v) {}
    double e(double)  const { return val; }
    double dx(double) const { return 0.0; }
};

// ============================================================
// Expressões binárias
// ============================================================
template<typename F, typename G>
struct AddExpr : ExprBase<AddExpr<F, G>> {
    F f; G g;
    AddExpr(const F& f, const G& g) : f(f), g(g) {}
    double e(double v)  const { return f.e(v) + g.e(v); }
    double dx(double v) const { return f.dx(v) + g.dx(v); }
};

template<typename F, typename G>
struct SubExpr : ExprBase<SubExpr<F, G>> {
    F f; G g;
    SubExpr(const F& f, const G& g) : f(f), g(g) {}
    double e(double v)  const { return f.e(v) - g.e(v); }
    double dx(double v) const { return f.dx(v) - g.dx(v); }
};

template<typename F, typename G>
struct MulExpr : ExprBase<MulExpr<F, G>> {
    F f; G g;
    MulExpr(const F& f, const G& g) : f(f), g(g) {}
    double e(double v)  const { return f.e(v) * g.e(v); }
    double dx(double v) const { return f.dx(v) * g.e(v) + f.e(v) * g.dx(v); }
};

template<typename F, typename G>
struct DivExpr : ExprBase<DivExpr<F, G>> {
    F f; G g;
    DivExpr(const F& f, const G& g) : f(f), g(g) {}
    double e(double v)  const { return f.e(v) / g.e(v); }
    double dx(double v) const {
        double fv = f.e(v), gv = g.e(v);
        return (f.dx(v) * gv - fv * g.dx(v)) / (gv * gv);
    }
};

// ============================================================
// Potenciação inteira — n·f^(n-1)·f'  (regra da cadeia)
// ============================================================
template<typename F, typename N>
struct PowExpr : ExprBase<PowExpr<F, N>> {
    F f; N n;
    PowExpr(const F& f, N n) : f(f), n(n) {}
    double e(double v) const {
        return std::pow(f.e(v), static_cast<double>(n));
    }
    double dx(double v) const {
        return static_cast<double>(n)
             * std::pow(f.e(v), static_cast<double>(n - 1))
             * f.dx(v);
    }
};

// ============================================================
// Funções elementares com regra da cadeia  h'(x) = g'(f(x))·f'(x)
// ============================================================
template<typename F>
struct ExpExpr : ExprBase<ExpExpr<F>> {
    F f;
    explicit ExpExpr(const F& f) : f(f) {}
    double e(double v)  const { return std::exp(f.e(v)); }
    double dx(double v) const { return std::exp(f.e(v)) * f.dx(v); }
};

template<typename F>
struct LogExpr : ExprBase<LogExpr<F>> {
    F f;
    explicit LogExpr(const F& f) : f(f) {}
    double e(double v)  const { return std::log(f.e(v)); }
    double dx(double v) const { return f.dx(v) / f.e(v); }
};

template<typename F>
struct SinExpr : ExprBase<SinExpr<F>> {
    F f;
    explicit SinExpr(const F& f) : f(f) {}
    double e(double v)  const { return std::sin(f.e(v)); }
    double dx(double v) const { return std::cos(f.e(v)) * f.dx(v); }
};

template<typename F>
struct CosExpr : ExprBase<CosExpr<F>> {
    F f;
    explicit CosExpr(const F& f) : f(f) {}
    double e(double v)  const { return std::cos(f.e(v)); }
    double dx(double v) const { return -std::sin(f.e(v)) * f.dx(v); }
};

// ============================================================
// Operadores aritméticos: expr op expr, expr op double, double op expr
// ============================================================

template<typename F, typename G>
AddExpr<F,G> operator+(const ExprBase<F>& f, const ExprBase<G>& g) {
    return AddExpr<F,G>(static_cast<const F&>(f), static_cast<const G&>(g));
}
template<typename F>
AddExpr<F,Const> operator+(const ExprBase<F>& f, double c) {
    return AddExpr<F,Const>(static_cast<const F&>(f), Const(c));
}
template<typename G>
AddExpr<Const,G> operator+(double c, const ExprBase<G>& g) {
    return AddExpr<Const,G>(Const(c), static_cast<const G&>(g));
}

template<typename F, typename G>
SubExpr<F,G> operator-(const ExprBase<F>& f, const ExprBase<G>& g) {
    return SubExpr<F,G>(static_cast<const F&>(f), static_cast<const G&>(g));
}
template<typename F>
SubExpr<F,Const> operator-(const ExprBase<F>& f, double c) {
    return SubExpr<F,Const>(static_cast<const F&>(f), Const(c));
}
template<typename G>
SubExpr<Const,G> operator-(double c, const ExprBase<G>& g) {
    return SubExpr<Const,G>(Const(c), static_cast<const G&>(g));
}

template<typename F, typename G>
MulExpr<F,G> operator*(const ExprBase<F>& f, const ExprBase<G>& g) {
    return MulExpr<F,G>(static_cast<const F&>(f), static_cast<const G&>(g));
}
template<typename F>
MulExpr<F,Const> operator*(const ExprBase<F>& f, double c) {
    return MulExpr<F,Const>(static_cast<const F&>(f), Const(c));
}
template<typename G>
MulExpr<Const,G> operator*(double c, const ExprBase<G>& g) {
    return MulExpr<Const,G>(Const(c), static_cast<const G&>(g));
}

template<typename F, typename G>
DivExpr<F,G> operator/(const ExprBase<F>& f, const ExprBase<G>& g) {
    return DivExpr<F,G>(static_cast<const F&>(f), static_cast<const G&>(g));
}
template<typename F>
DivExpr<F,Const> operator/(const ExprBase<F>& f, double c) {
    return DivExpr<F,Const>(static_cast<const F&>(f), Const(c));
}
template<typename G>
DivExpr<Const,G> operator/(double c, const ExprBase<G>& g) {
    return DivExpr<Const,G>(Const(c), static_cast<const G&>(g));
}

// ============================================================
// Wrappers simbólicos para funções elementares
// ============================================================
template<typename F>
ExpExpr<F> exp(const ExprBase<F>& f) {
    return ExpExpr<F>(static_cast<const F&>(f));
}
template<typename F>
LogExpr<F> log(const ExprBase<F>& f) {
    return LogExpr<F>(static_cast<const F&>(f));
}
template<typename F>
SinExpr<F> sin(const ExprBase<F>& f) {
    return SinExpr<F>(static_cast<const F&>(f));
}
template<typename F>
CosExpr<F> cos(const ExprBase<F>& f) {
    return CosExpr<F>(static_cast<const F&>(f));
}

// ============================================================
// Variável global x
// ============================================================
Var x;

