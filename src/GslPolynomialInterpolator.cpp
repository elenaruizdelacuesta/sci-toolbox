#include "GslPolynomialInterpolator.hpp"
#include <stdexcept>

// The gsl library is used to implement Lagrangian polynomial interpolation

GslPolynomialInterpolator::GslPolynomialInterpolator(const std::vector<double>& x, const std::vector<double>& y) {
    // Call setData to store the data
    set_data(x, y);  
    // Then, initialise the coefficients
    compute_coefficients();
} 

void GslPolynomialInterpolator::compute_coefficients() {
    std::size_t n = x_nodes.size();

    // Create GSL interpolator (polynomial) and accelerator
    interp = gsl_interp_alloc(gsl_interp_polynomial, n);
    acc = gsl_interp_accel_alloc();

    // Set the input data in the gsl interpolator
    int status = gsl_interp_init(interp, x_nodes.data(), y_nodes.data(), n);
    if (status != GSL_SUCCESS) {
        throw std::runtime_error("Error initialising gsl interpolator.");
    }  
}
    
double GslPolynomialInterpolator::operator()(double x) const {
    // We use GSL to calculate the interpolation at the point x
    check_range(x);
    return gsl_interp_eval(interp, x_nodes.data(), y_nodes.data(), x, acc);
}

GslPolynomialInterpolator::~GslPolynomialInterpolator() {
    // We release GSL resources if they have been initialised
    if (interp != nullptr) {
        gsl_interp_free(interp);
    }
    if (acc != nullptr) {
        gsl_interp_accel_free(acc);
    }
}
