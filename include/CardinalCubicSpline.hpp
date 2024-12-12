#ifndef CARDINAL_CUBIC_SPLINE_HPP
#define CARDINAL_CUBIC_SPLINE_HPP

#include "Interpolator.hpp"
#include <boost/math/interpolators/cardinal_cubic_b_spline.hpp>

class CardinalCubicSpline : public Interpolator {
    public:
    // Constructor initialising the spline with Boost
    CardinalCubicSpline(const std::vector<double>& x, const std::vector<double>& y, double start, double step);
        
    // Destructor
    ~CardinalCubicSpline() override = default; 

    // Method for evaluating interpolation
    double operator()(double x) const override;

private:
    // Method to check if the steps between nodes are equal
    void check_steps(const std::vector<double>& x) const;

    boost::math::interpolators::cardinal_cubic_b_spline<double> spline; // Cubic spline using Boost

};

#endif 
