#ifndef NEWTON_INTERPOLATOR_HPP
#define NEWTON_INTERPOLATOR_HPP

#include "Interpolator.hpp"

class NewtonInterpolator : public Interpolator {
public:
    // Constructor that initialises the data and calculates the Newton coefficients
    NewtonInterpolator(const std::vector<double>& x, const std::vector<double>& y);

    // Operator overload to perform interpolation on the value ‘x’
    double operator()(double x) const override;

private:
    // Method for calculating Newton's coefficients
    void compute_coefficients();

    std::vector<double> coefficients; // Stores Newton's coefficients
    
};

#endif // NEWTON_INTERPOLATOR_HPP 
