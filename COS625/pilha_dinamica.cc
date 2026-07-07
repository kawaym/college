#include <ostream>
#include <iostream>
using namespace std;

// Classe a ser enviada no VPL

class PilhaInt {
    private:
        int* data;
        int top;
        int capacity;
    public:
        // Default constructor
        PilhaInt(int capacidade = 10) {
            data = (int*) malloc( capacidade * sizeof(int) );
            top = -1;

            if (data == nullptr) {
                throw std::bad_alloc();
            }

            for (int i = 0; i < capacidade; ++i) {
                data[i] = 0;
            }

            capacity = capacidade;
        }

        // Copy constructor
        PilhaInt(const PilhaInt& other) {
            data = (int*) malloc( other.capacity * sizeof(int) );
            top = other.top;
            capacity = other.capacity;

            if (data == nullptr) {
                throw std::bad_alloc();
            }

            for (int i = 0; i <= top; ++i) {
                data[i] = other.data[i];
            }
        }

        // Destructor
        ~PilhaInt() {
            free(data);
        }

        int capacidade() {
            return capacity;
        }

        void empilha(int value) {
            if (top >= capacidade() - 1) {
                this->duplica_tamanho();
            }
            data[++top] = value;
        }

        int desempilha() {
            if (empty()) {
                throw std::underflow_error("Stack underflow");
            }
            return data[top--];
        }

        int duplica_tamanho() {
            capacity *= 2;
            int* nova_data = (int*) realloc(data, capacity * sizeof(int));
            if (nova_data == nullptr) {
                throw std::bad_alloc();
            }
            data = nova_data;
            return capacity;
        }

        int redimensiona(int nova_capacidade) {
            if (nova_capacidade < 0) {
                throw std::invalid_argument("Capacity must be non-negative");
            }

            if (nova_capacidade == capacity) {
                return capacity;
            }

            if (nova_capacidade > capacity) {
                capacity = nova_capacidade;
                int* nova_data = (int*) realloc(data, capacity * sizeof(int));
                if (nova_data == nullptr) {
                    throw std::bad_alloc();
                }
                data = nova_data;
                return capacity;
            }
            
            if (nova_capacidade < capacity) {
                if (top >= nova_capacidade) {
                    top = nova_capacidade - 1;
                }

                for (int i = 0; i < nova_capacidade - capacity; ++i) {
                    this->desempilha();
                }

                capacity = nova_capacidade;
            }

            return capacity;
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
                free(data); // Free existing data

                data = (int*) malloc( other.capacity * sizeof(int) );
                top = other.top;
                capacity = other.capacity;

                if (data == nullptr) {
                    throw std::bad_alloc();
                }

                for (int i = 0; i <= top; ++i) {
                    data[i] = other.data[i];
                }
            }

            return *this;
        }

};

int main() {

    PilhaInt pilha = PilhaInt();
    pilha.empilha(10);
    pilha.empilha(20);
    pilha.empilha(30);

    pilha.print(cout);

    PilhaInt pilha2 = pilha;
    cout << "Pilha copiada: ";
    pilha2.print(cout);
    cout << endl;

    pilha2.empilha(40);
    cout << "Pilha copiada após empilhar 40: ";
    pilha2.print(cout);
    cout << endl;
    cout << "Pilha original após empilhar 40 na cópia: ";
    pilha.print(cout);
    cout << endl;
    pilha.redimensiona(2);
    pilha.print(cout);
    cout << endl;
    // int value2 = 40;
    // pilha << value2;
    // cout << "Empilhado: " << value2 << endl;
    pilha.empilha(50);
    pilha.empilha(50);
    pilha.empilha(50);
    pilha.empilha(50);
    pilha.empilha(50);
    pilha.print(cout);
    cout << endl;

    return 0;
}