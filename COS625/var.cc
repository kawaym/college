#include <functional>
#include <map>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>

class Var {
public:
    class Erro;
    class Undefined;
    class Int;
    class Double;
    class Bool;
    class Char;
    class String;
    class Object;
    class Function;

    Var();
    Var(int value);
    Var(double value);
    Var(bool value);
    Var(char value);
    Var(const char* value);
    Var(const std::string& value);

    Var(const Var&) = default;
    Var(Var&&) = default;
    Var& operator=(const Var&) = default;
    Var& operator=(Var&&) = default;

    Var& operator=(int value);
    Var& operator=(double value);
    Var& operator=(bool value);
    Var& operator=(char value);
    Var& operator=(const char* value);
    Var& operator=(const std::string& value);

    template<typename F,
             typename = std::enable_if_t<
                 !std::is_same_v<std::decay_t<F>, Var> &&
                 !std::is_integral_v<std::decay_t<F>> &&
                 !std::is_floating_point_v<std::decay_t<F>> &&
                 !std::is_same_v<std::decay_t<F>, std::string> &&
                 !std::is_same_v<std::decay_t<F>, const char*> &&
                 !std::is_same_v<std::decay_t<F>, char*>>>
    Var(F function) {
        *this = std::forward<F>(function);
    }

    template<typename F,
             typename = std::enable_if_t<
                 !std::is_same_v<std::decay_t<F>, Var> &&
                 !std::is_integral_v<std::decay_t<F>> &&
                 !std::is_floating_point_v<std::decay_t<F>> &&
                 !std::is_same_v<std::decay_t<F>, std::string> &&
                 !std::is_same_v<std::decay_t<F>, const char*> &&
                 !std::is_same_v<std::decay_t<F>, char*>>>
    Var& operator=(F function) {
        valor = std::make_shared<Function>(
            [fn = std::move(function)](const Var& arg) -> Var {
                return fn(arg);
            }
        );
        return *this;
    }

    Var& operator[](const std::string& key);
    Var& operator[](const char* key);
    Var operator[](const std::string& key) const;
    Var operator[](const char* key) const;

    Var& operator->*(const std::string& key);
    Var& operator->*(const char* key);
    Var operator->*(const std::string& key) const;
    Var operator->*(const char* key) const;

    Var operator()(const Var& arg) const;
    Var operator()() const;

    friend Var operator+(const Var& lhs, const Var& rhs);
    friend Var operator-(const Var& lhs, const Var& rhs);
    friend Var operator*(const Var& lhs, const Var& rhs);
    friend Var operator/(const Var& lhs, const Var& rhs);
    friend Var operator<(const Var& lhs, const Var& rhs);
    friend Var operator>=(const Var& lhs, const Var& rhs);
    friend Var operator&&(const Var& lhs, const Var& rhs);
    friend Var operator||(const Var& lhs, const Var& rhs);
    friend Var operator!(const Var& value);
    friend std::ostream& operator<<(std::ostream& out, const Var& value);

    static Var newObject();

private:
    std::shared_ptr<Undefined> valor;

    bool is_int() const;
    bool is_double() const;
    bool is_bool() const;
    bool is_char() const;
    bool is_string() const;
    bool is_object() const;
    bool is_function() const;
    bool is_numeric() const;

    int as_int() const;
    double as_double() const;
    char as_char() const;
    std::string as_string_value() const;
    bool truthy() const;

    Object* as_object_ptr();
    const Object* as_object_ptr() const;
    const Function* as_function_ptr() const;
};

class Var::Erro {
public:
    explicit Erro(std::string msg) : msg(std::move(msg)) {}

    std::string operator()() const { return msg; }

private:
    std::string msg;
};

class Var::Undefined {
public:
    virtual ~Undefined() = default;

    virtual std::string type_name() const { return "Undefined"; }
    virtual std::string str() const { return "undefined"; }
    virtual bool truthy() const { return false; }
};

class Var::Int : public Var::Undefined {
public:
    explicit Int(int value) : value(value) {}

    std::string type_name() const override { return "Int"; }
    std::string str() const override {
        return std::to_string(value);
    }
    bool truthy() const override { return value != 0; }

    int value;
};

class Var::Double : public Var::Undefined {
public:
    explicit Double(double value) : value(value) {}

    std::string type_name() const override { return "Double"; }
    std::string str() const override {
        std::ostringstream out;
        out << value;
        return out.str();
    }
    bool truthy() const override { return value != 0.0; }

    double value;
};

class Var::Bool : public Var::Undefined {
public:
    explicit Bool(bool value) : value(value) {}

    std::string type_name() const override { return "Bool"; }
    std::string str() const override { return value ? "true" : "false"; }
    bool truthy() const override { return value; }

    bool value;
};

class Var::Char : public Var::Undefined {
public:
    explicit Char(char value) : value(value) {}

    std::string type_name() const override { return "Char"; }
    std::string str() const override { return std::string(1, value); }
    bool truthy() const override { return value != '\0'; }

    char value;
};

class Var::String : public Var::Undefined {
public:
    explicit String(std::string value) : value(std::move(value)) {}

    std::string type_name() const override { return "String"; }
    std::string str() const override { return value; }
    bool truthy() const override { return !value.empty(); }

    std::string value;
};

class Var::Object : public Var::Undefined {
public:
    std::string type_name() const override { return "Object"; }
    std::string str() const override { return "object"; }
    bool truthy() const override { return true; }

    std::map<std::string, Var> attributes;
};

class Var::Function : public Var::Undefined {
public:
    using Callable = std::function<Var(const Var&)>;

    explicit Function(Callable callable) : callable(std::move(callable)) {}

    std::string type_name() const override { return "Function"; }
    std::string str() const override { return "function"; }
    bool truthy() const override { return true; }

    Var call(const Var& arg) const { return callable(arg); }

private:
    Callable callable;
};

Var::Var() : valor(std::make_shared<Undefined>()) {}

Var::Var(int value) : valor(std::make_shared<Int>(value)) {}

Var::Var(double value) : valor(std::make_shared<Double>(value)) {}

Var::Var(bool value) : valor(std::make_shared<Bool>(value)) {}

Var::Var(char value) : valor(std::make_shared<Char>(value)) {}

Var::Var(const char* value) : valor(std::make_shared<String>(value == nullptr ? "" : value)) {}

Var::Var(const std::string& value) : valor(std::make_shared<String>(value)) {}

Var& Var::operator=(int value) {
    valor = std::make_shared<Int>(value);
    return *this;
}

Var& Var::operator=(double value) {
    valor = std::make_shared<Double>(value);
    return *this;
}

Var& Var::operator=(bool value) {
    valor = std::make_shared<Bool>(value);
    return *this;
}

Var& Var::operator=(char value) {
    valor = std::make_shared<Char>(value);
    return *this;
}

Var& Var::operator=(const char* value) {
    valor = std::make_shared<String>(value == nullptr ? "" : value);
    return *this;
}

Var& Var::operator=(const std::string& value) {
    valor = std::make_shared<String>(value);
    return *this;
}

bool Var::is_int() const {
    return static_cast<bool>(std::dynamic_pointer_cast<Int>(valor));
}

bool Var::is_double() const {
    return static_cast<bool>(std::dynamic_pointer_cast<Double>(valor));
}

bool Var::is_bool() const {
    return static_cast<bool>(std::dynamic_pointer_cast<Bool>(valor));
}

bool Var::is_char() const {
    return static_cast<bool>(std::dynamic_pointer_cast<Char>(valor));
}

bool Var::is_string() const {
    return static_cast<bool>(std::dynamic_pointer_cast<String>(valor));
}

bool Var::is_object() const {
    return static_cast<bool>(std::dynamic_pointer_cast<Object>(valor));
}

bool Var::is_function() const {
    return static_cast<bool>(std::dynamic_pointer_cast<Function>(valor));
}

bool Var::is_numeric() const {
    return is_int() || is_double();
}

int Var::as_int() const {
    if (is_char()) {
        return static_cast<unsigned char>(std::dynamic_pointer_cast<Char>(valor)->value);
    }
    return std::dynamic_pointer_cast<Int>(valor)->value;
}

double Var::as_double() const {
    if (is_double()) {
        return std::dynamic_pointer_cast<Double>(valor)->value;
    }
    return static_cast<double>(as_int());
}

char Var::as_char() const {
    return std::dynamic_pointer_cast<Char>(valor)->value;
}

std::string Var::as_string_value() const {
    if (is_string()) {
        return std::dynamic_pointer_cast<String>(valor)->value;
    }
    if (is_char()) {
        return std::string(1, as_char());
    }
    return "";
}

bool Var::truthy() const {
    return valor->truthy();
}

Var::Object* Var::as_object_ptr() {
    return dynamic_cast<Object*>(valor.get());
}

const Var::Object* Var::as_object_ptr() const {
    return dynamic_cast<const Object*>(valor.get());
}

const Var::Function* Var::as_function_ptr() const {
    return dynamic_cast<const Function*>(valor.get());
}

Var& Var::operator[](const std::string& key) {
    if (!is_object()) {
        throw Erro("Essa variável não é um objeto");
    }
    return as_object_ptr()->attributes[key];
}

Var& Var::operator[](const char* key) {
    return (*this)[std::string(key)];
}

Var Var::operator[](const std::string& key) const {
    const Object* object = as_object_ptr();
    if (object == nullptr) {
        throw Erro("Essa variável não é um objeto");
    }
    auto it = object->attributes.find(key);
    if (it == object->attributes.end()) {
        return Var();
    }
    return it->second;
}

Var Var::operator[](const char* key) const {
    return (*this)[std::string(key)];
}

Var& Var::operator->*(const std::string& key) {
    return (*this)[key];
}

Var& Var::operator->*(const char* key) {
    return (*this)[key];
}

Var Var::operator->*(const std::string& key) const {
    return (*this)[key];
}

Var Var::operator->*(const char* key) const {
    return (*this)[key];
}

Var Var::operator()(const Var& arg) const {
    const Function* function = as_function_ptr();
    if (function == nullptr) {
        throw Erro("Essa variável não pode ser usada como função");
    }
    return function->call(arg);
}

Var Var::operator()() const {
    return (*this)(Var());
}

Var Var::newObject() {
    Var result;
    result.valor = std::make_shared<Object>();
    return result;
}

Var operator+(const Var& lhs, const Var& rhs) {
    if ((lhs.is_string() || lhs.is_char()) && (rhs.is_string() || rhs.is_char())) {
        return Var(lhs.as_string_value() + rhs.as_string_value());
    }
    if ((lhs.is_char() && rhs.is_int()) || (lhs.is_int() && rhs.is_char())) {
        return Var(lhs.as_int() + rhs.as_int());
    }
    if (!lhs.is_numeric() || !rhs.is_numeric()) {
        return Var();
    }
    double value = lhs.as_double() + rhs.as_double();
    if (lhs.is_int() && rhs.is_int()) {
        return Var(static_cast<int>(value));
    }
    return Var(value);
}

Var operator-(const Var& lhs, const Var& rhs) {
    if (!lhs.is_numeric() || !rhs.is_numeric()) {
        return Var();
    }
    double value = lhs.as_double() - rhs.as_double();
    if (lhs.is_int() && rhs.is_int()) {
        return Var(static_cast<int>(value));
    }
    return Var(value);
}

Var operator*(const Var& lhs, const Var& rhs) {
    if (!lhs.is_numeric() || !rhs.is_numeric()) {
        return Var();
    }
    double value = lhs.as_double() * rhs.as_double();
    if (lhs.is_int() && rhs.is_int()) {
        return Var(static_cast<int>(value));
    }
    return Var(value);
}

Var operator/(const Var& lhs, const Var& rhs) {
    if (!lhs.is_numeric() || !rhs.is_numeric()) {
        return Var();
    }
    if (lhs.is_int() && rhs.is_int()) {
        return Var(lhs.as_int() / rhs.as_int());
    }
    return Var(lhs.as_double() / rhs.as_double());
}

Var operator<(const Var& lhs, const Var& rhs) {
    if (lhs.is_numeric() && rhs.is_numeric()) {
        return Var(lhs.as_double() < rhs.as_double());
    }
    if (lhs.is_bool() && rhs.is_bool()) {
        return Var(std::dynamic_pointer_cast<Var::Bool>(lhs.valor)->value <
                   std::dynamic_pointer_cast<Var::Bool>(rhs.valor)->value);
    }
    if ((lhs.is_string() || lhs.is_char()) && (rhs.is_string() || rhs.is_char())) {
        return Var(lhs.as_string_value() < rhs.as_string_value());
    }
    return Var();
}

Var operator>=(const Var& lhs, const Var& rhs) {
    if (lhs.is_numeric() && rhs.is_numeric()) {
        return Var(lhs.as_double() >= rhs.as_double());
    }
    if ((lhs.is_int() && rhs.is_char()) || (lhs.is_char() && rhs.is_int())) {
        return Var(lhs.as_int() >= rhs.as_int());
    }
    if ((lhs.is_string() || lhs.is_char()) && (rhs.is_string() || rhs.is_char())) {
        return Var(lhs.as_string_value() >= rhs.as_string_value());
    }
    return Var();
}

Var operator&&(const Var& lhs, const Var& rhs) {
    if (!lhs.is_bool() || !rhs.is_bool()) {
        return Var();
    }
    return Var(lhs.truthy() && rhs.truthy());
}

Var operator||(const Var& lhs, const Var& rhs) {
    if (!lhs.is_bool() || !rhs.is_bool()) {
        return Var();
    }
    return Var(lhs.truthy() || rhs.truthy());
}

Var operator!(const Var& value) {
    if (!value.is_bool()) {
        return Var();
    }
    return Var(!value.truthy());
}

std::ostream& operator<<(std::ostream& out, const Var& value) {
    out << value.valor->str();
    return out;
}

inline Var newObject() {
    return Var::newObject();
}