#include <ostream>
#include <iostream>
#include <vector>
using namespace std;

template<int n, typename T>
class Vetor {
    public: 
        class MeioDoProdutoVetorial {
            private:
                MeioDoProdutoVetorial ( Vetor* v ) : vetor(v) {}
                friend class Vetor;
                Vetor* vetor;       
        };
    private:
        vector<T> data;

    public:
        // Default constructor 
        Vetor() {
            data.resize(n, T(0));
        }

        Vetor( const initializer_list<T>& list ) : data(list) {
            if (data.size() != n) {
                throw std::length_error("Initializer list size does not match the required size");
            }
        }

        // Sum operator
        Vetor operator+ (const Vetor& other) const {
            Vetor result = *this;
            for (size_t i = 0; i < n; ++i) {
                result.data[i] += other.data[i];
            }
            return result;
        }

        // Subtraction operator
        Vetor operator- (const Vetor& other) const {
            Vetor result = *this;
            for (size_t i = 0; i < n; ++i) {
                result.data[i] -= other.data[i];
            }
            return result;
        }

        // Scalar operators
        Vetor operator* (const T& scalar) const {
            Vetor result = *this;
            for (size_t i = 0; i < n; ++i) {
                result.data[i] *= scalar;
            }
            return result;
        }

        // Scalar * Vetor (permite ambas as ordens de multiplicação)
        friend Vetor operator* (const T& scalar, const Vetor& v) {
            return v * scalar;
        }

        Vetor operator/ (const T& scalar) const {
            if (scalar == T(0)) {
                throw std::invalid_argument("Division by zero");
            }
            Vetor result = *this;
            for (size_t i = 0; i < n; ++i) {
                result.data[i] /= scalar;
            }
            return result;
        }

        // Dot Product
        T operator* (const Vetor& other) const {
            if (data.size() != other.data.size()) {
                throw std::length_error("Vectors must be of the same size for dot product");
            }

            T result = T(0);
            for (size_t i = 0; i < n; ++i) {
                result += data[i] * other.data[i];
            }
            return result;
        }

        // Cross Product 
        MeioDoProdutoVetorial operator* () const {
            return MeioDoProdutoVetorial(const_cast<Vetor*>(this));
        }

        Vetor<3, T> operator* (const MeioDoProdutoVetorial& meio) const {
            if (n != 3) {
                throw std::length_error("Cross product is only defined for 3D vectors");
            }

            Vetor<3, T> result = { 
                data[1] * meio.vetor->data[2] - data[2] * meio.vetor->data[1],
                data[2] * meio.vetor->data[0] - data[0] * meio.vetor->data[2],
                data[0] * meio.vetor->data[1] - data[1] * meio.vetor->data[0]
            };
            return result;
        }

        // Print method
        void print( std::ostream& os ) const {
            os << "[ ";
            for (size_t i = 0; i < data.size(); ++i) {
                os << data[i];
                if (i < data.size() - 1) {
                    os << ", ";
                }
            }
            os << " ]";
        }

        // Print operator
        friend std::ostream& operator<< (std::ostream& os, const Vetor& vetor) {
            vetor.print(os);
            return os;
        }
};

int main() {
    Vetor<3, double> a = {1.0, 2.0, 3.0};
    Vetor<3, double> b = {4.0, 5.0, 6.0};

    cout << a << endl; 
    cout << a + b << endl; 
    {
          Vetor< 3, double> c;
          c = a ** b; 
          cout << c << endl;
           
    }
    cout << a * b << endl; 
    cout << a * 3.0 << endl; 
    cout << 2.1 * a << endl; 
    cout << a + b + a * 2.0 << endl; 
    
    return 0;
};