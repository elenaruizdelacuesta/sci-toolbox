#include "LinearInterpolator.hpp"

// Constructor: initialises the data using setData
LinearInterpolator::LinearInterpolator(const std::vector<double>& x, const std::vector<double>& y) {
    set_data(x, y); // Verify and store data
}

double LinearInterpolator::operator()(double x) const {
    check_range(x);

    // Finds the interval [x_nodes[i], x_nodes[i+1]] where x is located
    for (size_t i = 0; i < x_nodes.size() - 1; ++i) {
        if (x >= x_nodes[i] && x <= x_nodes[i + 1]) {
            // Calculates the slope and returns the interpolated value
            double slope = (y_nodes[i + 1] - y_nodes[i]) / (x_nodes[i + 1] - x_nodes[i]);
            return y_nodes[i] + slope * (x - x_nodes[i]); // y = y_i + slope * (x - x_i)
        }
    }

     throw std::runtime_error("Interpolation failed. x should be within the data range.");
}
