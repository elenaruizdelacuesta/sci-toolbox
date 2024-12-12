#include "CardinalCubicSpline.hpp"

// Constructor initialising the cubic spline of Boost
CardinalCubicSpline::CardinalCubicSpline(const std::vector<double>& x, const std::vector<double>& y, double start, double step)
    : spline(y.begin(), y.end(), start, step) {
    // Validates and stores the nodes in the base class
    set_data(x, y); 
    check_steps(x);
    //checkMinNodes(x); --> ponerlo en el readme, el método ya lo hace por si solo (el propio spline lanza un error suyo)
}

// Method to evaluate interpolation at a point x
double CardinalCubicSpline::operator()(double x) const {
    check_range(x); // Check if x is within the range
    return spline(x);
}

// Method to check if the steps between nodes are equal
void CardinalCubicSpline::check_steps(const std::vector<double>& x) const {
    double step = x[1] - x[0]; // Expected step between nodes
    double tolerance = 1e-9;  // Tolerance for comparison
    for (size_t i = 1; i < x.size() - 1; ++i) {
        double actualStep = x[i+1] - x[i];
        if (std::fabs(actualStep - step) > tolerance) {
            throw std::invalid_argument("The x-nodes must have equal spacing between them.");
        }
    }
}
