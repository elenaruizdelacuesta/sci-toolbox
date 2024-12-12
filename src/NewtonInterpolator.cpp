#include "NewtonInterpolator.hpp"

NewtonInterpolator::NewtonInterpolator(const std::vector<double>& x, const std::vector<double>& y) {
    // Call setData to store the data
    set_data(x, y);  
    // Then, initialise the coefficients
    compute_coefficients();
}

// Method for calculating Newton's coefficients (split differences)
void NewtonInterpolator::compute_coefficients() {
    std::size_t n = x_nodes.size();
    coefficients.resize(n); // These coefficients represent the split differences f[x0], f[x0,x1],...

// Copy the values of y into the first set of coefficients (P(x) = f[x0] + ...)
    for (std::size_t i = 0; i < n; ++i) {
        coefficients[i] = y_nodes[i]; // the values of yi are copied to the vector coefficients
    }

    // Calculate Newton's divided differences
    for (std::size_t j = 1; j < n; ++j) {
        for (std::size_t i = n - 1; i >= j; --i) {
            coefficients[i] = (coefficients[i] - coefficients[i - 1]) / (x_nodes[i] - x_nodes[i - j]);
        }
    }
}

// Operator overload to perform interpolation on the value ‘x’
double NewtonInterpolator::operator()(double x) const {
    check_range(x);
    std::size_t n = x_nodes.size();
    double result = coefficients[n - 1];

    // Evaluation of Newton's polynomial using Horner's formula
    for (int i = static_cast<int>(n) - 2; i >= 0; --i) {
        result = result * (x - x_nodes[i]) + coefficients[i];
    }

    return result;
}
