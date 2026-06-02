#include <iostream>
#include <iomanip>
using namespace std;
#include "derivador.cc"

int main(int argc, char**) {
    cout << fixed << setprecision(6);

    // Test 1
    {
        double v = 5.1;
        auto f = 3.0 * x * x;
        cout << "f(" << v << ")=" << f.e(v) << ", f'(" << v << ")=" << f.dx(v) << "\n";
    }

    // Test 2
    {
        double v = 1.1;
        auto f = x*x*( x + 8.0 ) + x;
        cout << "f(" << v << ")=" << f.e(v) << ", f'(" << v << ")=" << f.dx(v) << "\n";
    }

    // Test 3
    {
        double v = 0.5;
        auto f = sin( x * x * cos( 3.14 * x + 1.0 ) );
        cout << "f(" << v << ")=" << f.e(v) << ", f'(" << v << ")=" << f.dx(v) << "\n";
    }

    // Test 4
    {
        double v = 3.14;
        auto f = sin( x * x - cos( 3.14 * x + 1.0 ) );
        cout << "f(" << v << ")=" << f.e(v) << ", f'(" << v << ")=" << f.dx(v) << "\n";
    }

    // Test 5
    {
        double v = 3.14159;
        auto f = sin(x)/cos(x);
        cout << "f(" << v << ")=" << f.e(v) << ", f'(" << v << ")=" << f.dx(v) << "\n";
    }

    // Test 6
    {
        double v = -7.3;
        auto f = x->*3 + x->*2;
        cout << "f(" << v << ")=" << f.e(v) << ", f'(" << v << ")=" << f.dx(v) << "\n";
    }

    // Test 7
    {
        double v = -0.5;
        auto f = 1.0 / (sin(x)->*2 + cos(x)->*2)->*4;
        cout << "f(" << v << ")=" << f.e(v) << ", f'(" << v << ")=" << f.dx(v) << "\n";
    }

    // Test 8
    {
        double v = 0.1;
        auto f = 1.0 / (1.0 + exp( -2.0*( x - 1.0 )->*4 ) );
        cout << "f(" << v << ")=" << f.e(v) << ", f'(" << v << ")=" << f.dx(v) << "\n";
    }

    // Test 9
    {
        double v = 3;
        auto f = exp( x * log( x ) );
        cout << "f(" << v << ")=" << f.e(v) << ", f'(" << v << ")=" << f.dx(v) << "\n";
    }

    return 0;
}
