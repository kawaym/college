#include <vector>
#include <mutex>
#include <cstddef>
#include <numeric>

class VetorSeguro {
private:
    std::vector<int> dados;
    mutable std::mutex mtx;

public:
    VetorSeguro() = default;

    VetorSeguro(std::size_t n, int valor) : dados(n, valor) {}

    VetorSeguro(const VetorSeguro&) = delete;
    VetorSeguro& operator=(const VetorSeguro&) = delete;
    VetorSeguro(VetorSeguro&&) = delete;
    VetorSeguro& operator=(VetorSeguro&&) = delete;

    void inserir(int valor) {
        std::lock_guard<std::mutex> lock(mtx);
        dados.push_back(valor);
    }

    bool removerUltimo() {
        std::lock_guard<std::mutex> lock(mtx);
        if (dados.empty()) return false;
        dados.pop_back();
        return true;
    }

    void redimensionar(std::size_t novoTamanho, int valor = 0) {
        std::lock_guard<std::mutex> lock(mtx);
        dados.resize(novoTamanho, valor);
    }

    std::size_t tamanho() const {
        std::lock_guard<std::mutex> lock(mtx);
        return dados.size();
    }

    long soma() const {
        std::lock_guard<std::mutex> lock(mtx);
        return std::accumulate(dados.begin(), dados.end(), 0L);
    }
};