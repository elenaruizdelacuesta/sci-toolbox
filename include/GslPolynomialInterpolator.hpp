#ifndef GSL_POLYNOMIAL_INTERPOLATOR_HPP
#define GSL_POLYNOMIAL_INTERPOLATOR_HPP

#include "Interpolator.hpp"
#include <gsl/gsl_interp.h>
#include <gsl/gsl_errno.h>

class GslPolynomialInterpolator : public Interpolator {
public:
    // Constructor that initialises the data
    GslPolynomialInterpolator(const std::vector<double>& x, const std::vector<double>& y);

    // Destructor
    ~GslPolynomialInterpolator();

    // Operator overload for interpolation
    double operator()(double x) const override;

private:
    void compute_coefficients();
    gsl_interp* interp = nullptr;          // Pointer to gsl interpolator
    gsl_interp_accel* acc = nullptr;       // Pointer to gsl accelerator
};

#endif // GSL_POLYNOMIAL_INTERPOLATOR_HPP
