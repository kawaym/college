#include <ostream>
#include <iostream>
using namespace std;

// Classe a ser enviada no VPL

class PilhaInt {
    private:
        static const int CAPACITY = 100;
        int data[CAPACITY];
        int top;
    public:
        PilhaInt(): top(-1) {}

        void empilha(int value) {
            if (top >= CAPACITY - 1) {
                throw std::overflow_error("Stack overflow");
            }
            data[++top] = value;
        }

        int desempilha() {
            if (empty()) {
                throw std::underflow_error("Stack underflow");
            }
            return data[top--];
        }

        bool empty() const {
            return top == -1;
        }

        int size() const {
            return top + 1;
        }

        void print( std::ostream& os ) const {
            os << "[ ";
            for (int i = 0; i <= top; ++i) {
                os << data[i];
                if (i < top) {
                    os << ", ";
                }
            }
            os << " ]";
        }

        PilhaInt& operator<< (int value) {
            empilha(value);
            return *this;
        }

        PilhaInt& operator= (const PilhaInt& other) {
            if (this != &other) {
                top = other.top;
                for (int i = 0; i <= top; ++i) {
                    data[i] = other.data[i];
                }
            }

            return *this;
        }
};

int main() {

    PilhaInt pilha;
    pilha.empilha(10);
    pilha.empilha(20);
    pilha.empilha(30);

    pilha.print(cout);

    int value2 = 40;
    pilha << value2;
    cout << "Empilhado: " << value2 << endl;

    return 0;
}