#include <ostream>
#include <iostream>
#include <vector>

template <typename Container, typename F>
void apply(Container& container, F func) {
    for (auto& element : container) {
        func(element);
    }
}

int main() {
    // Gere um array de inteiros
    std::vector<int> arr = {1, 2, 3, 4, 5};

    // Função lambda que recebe um inteiro e o imprime 
    
    ::apply(arr, [](int& element) {
        std::cout << element << " ";
    });
}