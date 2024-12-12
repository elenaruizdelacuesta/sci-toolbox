#ifndef INTERPOLATOR_HPP
#define INTERPOLATOR_HPP

#include <vector>

class Interpolator {
public:
    virtual ~Interpolator() = default;

    // Pure method for interpolating
    virtual double operator()(double x) const = 0;

    // Declaration for the data
    void set_data(const std::vector<double>& x, const std::vector<double>& y);

    // Method to check if a value x is within the range of the nodes
    void check_range(double x) const;

protected:
    std::vector<double> x_nodes;
    std::vector<double> y_nodes;
};

#endif // INTERPOLATOR_HPP

