#include <iostream>
#include <cmath>
#include <chrono>
#include <map>
#include <functional>
#include "LinearInterpolator.hpp"
#include "GslPolynomialInterpolator.hpp"
#include "NewtonInterpolator.hpp"
#include "CardinalCubicSpline.hpp"

// Function to interpolate
double f1(double x) {
    return std::exp(x); //
}

// Generate equispaced nodes and corresponding values
void generate_nodes_and_values(double a, double b, int n, std::vector<double>& x_nodes, std::vector<double>& y_nodes, double (*func)(double)) {
    double delta_x = (b - a) / (n - 1);
    for (int i = 0; i < n; ++i) {
        double x = a + i * delta_x;
        x_nodes.push_back(x);
        y_nodes.push_back(func(x));
    }
}

// Structure for storing interpolators
struct Interpolators {
    std::unique_ptr<LinearInterpolator> linearInterp;
    std::unique_ptr<GslPolynomialInterpolator> lagrangeInterp;
    std::unique_ptr<NewtonInterpolator> newtonInterp;
    std::unique_ptr<CardinalCubicSpline> cubicSpline;
};


// Comparing interpolation methods using tables
Interpolators compareInterpolationMethods(const std::vector<double>& x_nodes, const std::vector<double>& y_nodes, double (*trueFunction)(double), const std::vector<double>& test_points) {
    
    // Create interpolators using smart pointers
    auto linearInterp = std::make_unique<LinearInterpolator>(x_nodes, y_nodes);
    auto lagrangeInterp = std::make_unique<GslPolynomialInterpolator>(x_nodes, y_nodes);
    auto newtonInterp = std::make_unique<NewtonInterpolator>(x_nodes, y_nodes);
    double delta_x = x_nodes[1] - x_nodes[0];
    auto cubicSpline = std::make_unique<CardinalCubicSpline>(x_nodes, y_nodes, x_nodes[0], delta_x);

    // Show header
    std::cout << std::left << std::setw(16) << "x (Test Points)"
            << std::setw(16) << "True Value"
            << std::setw(16) << "Linear"
            << std::setw(16) << "Lagrange"
            << std::setw(16) << "Newton"
            << std::setw(16) << "Cubic Spline"
            << std::setw(16) << "Error Linear"
            << std::setw(16) << "Error Lagrange"
            << std::setw(16) << "Error Newton"
            << std::setw(16) << "Error Cubic Spline" << "\n";
    std::cout << std::string(160, '-') << "\n";

    // Evaluating the methods for each test point
    for (double x : test_points) {
        double true_value = trueFunction(x);
        double linear_value = (*linearInterp)(x);
        double lagrange_value = (*lagrangeInterp)(x);
        double newton_value = (*newtonInterp)(x);
        double cubic_spline_value = (*cubicSpline)(x);

        double error_linear = std::abs(true_value - linear_value);
        double error_lagrange = std::abs(true_value - lagrange_value);
        double error_newton = std::abs(true_value - newton_value);
        double error_cubic_spline = std::abs(true_value - cubic_spline_value);

        std::cout << std::left << std::setw(16) << x
              << std::setw(16) << true_value
              << std::setw(16) << linear_value
              << std::setw(16) << lagrange_value
              << std::setw(16) << newton_value
              << std::setw(16) << cubic_spline_value
              << std::setw(16) << error_linear
              << std::setw(16) << error_lagrange
              << std::setw(16) << error_newton
              << std::setw(16) << error_cubic_spline << "\n";
    }

    return {std::move(linearInterp), std::move(lagrangeInterp), std::move(newtonInterp), std::move(cubicSpline)};
}

// Function to measure execution time
double measure_execution_time(std::function<void()> f) {
    // Get time before code execution
    auto start = std::chrono::high_resolution_clock::now();
    // Execute the code block passed as a parameter
    f();
    // Get time after code execution
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start; // Calculate time difference between start and end (in seconds)
    return duration.count();
}

void evaluateEfficiency(const Interpolators& interpolators, const std::vector<double>& test_points) {
    std::cout << std::left << std::setw(15) << "Method" 
              << std::setw(15) << "Time (seconds)" << "\n";
    std::cout << std::string(30, '-') << "\n";

    // Measure runtime for Linear Interpolation
    double linear_time = measure_execution_time([&]() {
        for (double x : test_points) (*interpolators.linearInterp)(x);
    });
    std::cout << std::left << std::setw(15) << "Linear"
              << std::setw(15) << linear_time << "\n";

    // Measure runtime for Lagrange Interpolation
    double lagrange_time = measure_execution_time([&]() {
        for (double x : test_points) (*interpolators.lagrangeInterp)(x);
    });
    std::cout << std::left << std::setw(15) << "Lagrange"
              << std::setw(15) << lagrange_time << "\n";

    // Measure runtime for Newton Interpolation
    double newton_time = measure_execution_time([&]() {
        for (double x : test_points) (*interpolators.newtonInterp)(x);
    });
    std::cout << std::left << std::setw(15) << "Newton"
              << std::setw(15) << newton_time << "\n";

    // Measure runtime for Cubic Spline
    double cubic_spline_time = measure_execution_time([&]() {
        for (double x : test_points) (*interpolators.cubicSpline)(x);
    });
    std::cout << std::left << std::setw(15) << "Cubic Spline"
              << std::setw(15) << cubic_spline_time << "\n";
}

// Function for calculating interpolation errors
std::vector<double> calculateInterpolationErrors(
    const Interpolators& interpolators,
    const std::vector<double>& test_points,
    double (*trueFunction)(double))
{

    std::cout << std::left << std::setw(15) << "Method"
              << std::setw(20) << "RMSE"
              << "\n";
    std::cout << std::string(30,'-') << "\n";

    // Define the interpolators on a map to process them automatically
    std::map<std::string, std::function<double(double)>> methods = {
        {"Linear", [&](double x) { return (*interpolators.linearInterp)(x); }},
        {"Lagrange", [&](double x) { return (*interpolators.lagrangeInterp)(x); }},
        {"Newton", [&](double x) { return (*interpolators.newtonInterp)(x); }},
        {"Cubic Spline", [&](double x) { return (*interpolators.cubicSpline)(x); }}
    };

    std::vector<std::string> ordered_methods = {"Linear", "Lagrange", "Newton", "Cubic Spline"};

    // Calculate errors for each method
    std::vector<double> rmse_values;
    for (const auto& method_name : ordered_methods) {
        auto interpolator = methods[method_name];
        double total_squared_error = 0.0;

        for (double x : test_points) {
            double true_value = trueFunction(x);
            double interpolated_value = interpolator(x);
            double error = std::abs(true_value - interpolated_value);

            total_squared_error += error * error;
        }

        double rmse = std::sqrt(total_squared_error / test_points.size());

        // Show errors
        std::cout << std::left << std::setw(15) << method_name
                  << std::setw(20) << rmse
                  << "\n";
        rmse_values.push_back(rmse);
    }
    return rmse_values;
}

void calculateConvergenceOrder(const std::vector<double>& rmse_n1, const std::vector<double>& rmse_n2, int n1, int n2) {
    
    double log_ratio = std::log(static_cast<double>(n2) / n1);

    // Methods in order
    std::vector<std::string> methods = {"Linear", "Lagrange", "Newton", "Cubic Spline"};

    std::cout << "\nOrder of Convergence:\n";
    std::cout << std::left << std::setw(15) << "Method" 
              << std::setw(15) << "Order (p)" << "\n";
    std::cout << std::string(30, '-') << "\n";

    // Calculate the order for each method
    for (size_t i = 0; i < rmse_n1.size(); ++i) {
        double p = std::log(rmse_n1[i] / rmse_n2[i]) / log_ratio;
        std::cout << std::left << std::setw(15) << methods[i]
                  << std::setw(15) << p << "\n";
    }
}



int main() {
    // Interval and number of nodes for f(x) = x^6
    double a_f1 = -4;
    double b_f1 = 4.0;
    int n_f1_1 = 15;
    int n_f1_2 = 30;

    // Vectors to store the nodes and values
    std::vector<double> x_nodes_f1_1, y_nodes_f1_1;
    std::vector<double> x_nodes_f1_2, y_nodes_f1_2;

    // Generate nodes and values for x^6
    generate_nodes_and_values(a_f1, b_f1, n_f1_1, x_nodes_f1_1, y_nodes_f1_1, f1);
    generate_nodes_and_values(a_f1, b_f1, n_f1_2, x_nodes_f1_2, y_nodes_f1_2, f1);

    // Show generated nodes
    std::cout << "\nNodes (x_i) and values (y_i) for f(x) = e^x with n = 15:\n";
    for (int i = 0; i < n_f1_1; ++i) {
        std::cout << "x[" << i << "] = " << x_nodes_f1_1[i] << ", y[" << i << "] = " << y_nodes_f1_1[i] << "\n";
    }

    std::cout << "\nNodes (x_i) and values (y_i) for f(x) = e^x with n = 30:\n";
    for (int i = 0; i < n_f1_2; ++i) {
        std::cout << "x[" << i << "] = " << x_nodes_f1_2[i] << ", y[" << i << "] = " << y_nodes_f1_2[i] << "\n";
    }

    // Test points
    
    // Test points
    std::vector<double> test_points_f1;
    int num_test_points = 20; 
    double step = ((b_f1 - a_f1) / (num_test_points - 1)); 

    for (int i = 0; i < num_test_points; ++i) {
        double x = a_f1 + i * step;
        // Asegurar que no coincida exactamente con a_f1 o b_f1
        if (i == 0) {
            x += 0.1; // Pequeña corrección al inicio
        } else if (i == num_test_points - 1) {
            x -= 0.1; // Pequeña corrección al final
        }
        test_points_f1.push_back(x);
    }
    
    // Compare interpolation methods for f(x) = x^6
    std::cout << "\nInterpolation of f(x) = e^x with n = 15: True Values, Approximations from Different Methods and Absolute Errors:\n";
    Interpolators interpolators_n1 = compareInterpolationMethods(x_nodes_f1_1, y_nodes_f1_1, f1, test_points_f1);
    std::cout << "\nInterpolation of f(x) = e^x with n = 30: True Values, Approximations from Different Methods and Absolute Errors:\n";
    Interpolators interpolators_n2 = compareInterpolationMethods(x_nodes_f1_2, y_nodes_f1_2, f1, test_points_f1);
    
    // Evaluate efficiency
    std::cout << "\nEfficiency (Time to interpolate all test points) with n = 15:\n";
    evaluateEfficiency(interpolators_n1, test_points_f1);

    std::cout << "\nEfficiency (Time to interpolate all test points) with n = 30:\n";
    evaluateEfficiency(interpolators_n2, test_points_f1);  

    // Evaluate Accuracy
    std::cout << "\nAccuracy with n = 15:\n";
    std::vector<double> rmse_n1 = calculateInterpolationErrors(interpolators_n1, test_points_f1, f1);

    std::cout << "\nAccuracy with n = 30:\n";
    std::vector<double> rmse_n2 = calculateInterpolationErrors(interpolators_n2, test_points_f1, f1);

    // Order of convergence
    calculateConvergenceOrder(rmse_n1, rmse_n2, n_f1_1, n_f1_2);

    return 0;
}