# sci-toolbox
This project is the second homework assignment for the *Advanced Programming* course at the University of Trieste. It implements a scientific toolbox with two main modules:

- **Statistics Module**  
- **Interpolation Module**  

Additionally, it includes two corresponding applications to test these modules.
## Table of contents

- [sci-toolbox](#sci-toolbox)
  - [Table of contents](#table-of-contents)
  - [Project structure](#project-structure)
  - [How to build](#how-to-build)
    - [Installing third part libraries](#installing-third-part-libraries)
    - [Building `sci-toolbox`](#building-sci-toolbox)
      - [Custom Builds](#custom-builds)
    - [Running the apps](#running-the-apps)
  - [Module A: Statistics](#module-a-statistics)
  - [Module B: Interpolation](#module-b-interpolation)
    - [Features](#features)
    - [Implementation](#implementation)
    - [Results](#results)
      - [Efficiency](#efficiency)
      - [Accuracy](#accuracy)
      - [Order of Convergence](#order-of-convergence)
  - [Authors and contributions](#authors-and-contributions)
  - [References](#references)

## Project structure

```
📂 project/
│ 
├── 📂 apps/
│   ├── 📝 CMakeLists.txt
│   ├── 📄 interopol_app.cpp
│   └── 📄 stats_app.cpp
│ 
├── 📂 datasets/
│   ├── 📊 iris.csv
│   └── 📊 iris.json
│
├── 📂 include/	
│   ├── 📄 CardinalCubicSpline.hpp
│   ├── 📄 GslPolynomialInterpolator.hpp
│   ├── 📄 Interpolator.hpp
│   ├── 📄 LinearInterpolator.hpp
│   ├── 📄 NewtonInterpolator.hpp
│   └── 📄 DataFrame.hpp
│ 
├── 📂 output/
│
├── 📂 src/
│   ├── 📄 CardinalCubicSpline.cpp
│   ├── 📄 GslPolynomialInterpolator.cpp
│   ├── 📄 Interpolator.cpp
│   ├── 📄 LinearInterpolator.cpp
│   ├── 📄 NewtonInterpolator.cpp
│   ├── 📝 CMakeLists.txt
│   └── 📄 DataFrame.cpp
│
├── 📒 .gitignore
├── 📝 CMakeLists.txt
└── 📰 README.md
```
## How to build

### Installing third part libraries

The two modules uses functions from *GSL-GNU Scientific Library* and *BOOST library*, so its necessary to install both libraries before building the toolbox.

You can install the third part library from the terminal writing

```bash
sudo apt-get install libboost-all-dev
```
for BOOST and

```bash
sudo apt-get install libgsl-dev
```
for GSL.

Here's a clearer and more concise version of the instructions:

---

### Building `sci-toolbox`

`sci-toolbox` uses CMake for its build process. To build both the shared libraries and the associated applications, run the following commands from the project's root directory:

```bash
cmake -B build
cmake --build build
```

#### Custom Builds

To build only one of the two libraries and its associated applications, specify the desired options with `ON` or `OFF`. For example, to build the `DataFrame` library and exclude the `Interpolation` library, use:

```bash
cmake -B build -DBUILD_LIB_DATAFRAME=ON -DBUILD_LIB_INTERPOLATION=OFF
cmake --build build
```

Adjust `ON` and `OFF` based on your requirements.

### Running the apps
After completing the build process, the applications will be available as executables in the `/build/apps` directory, named `stat_app` and `interpol_app`. To run them, simply type the following in the terminal:

```bash
./build/apps/stat_app iris.csv iris_analysis.txt 
./build/apps/interpol_app
```
For the `stat_app` you need to provide other two inputs to the terminal: the dataset name (with its extension) and the output filename (.txt). The dataset must be stored in the `datasets` folder and the output files will be saved in the `output` folder. Eventually this can be modified from the `stat_app.cpp` source file.

## Module A: Statistics
The `DataFrame` class serves as the core of the statistics module, offering a comprehensive framework for data storage, manipulation, and analysis. It includes functionalities for reading from file (CSV, JSON), handling tabular data and performing basic statistical operations.

The class stores its data in a **column-oriented** structure, which means the data is maintained as a **vector of column vectors**. This design choice has several implications for performance and flexibility, particularly for operations involving entire columns or statistical calculations.

Each element in the DataFrame can either be a `double` or a `std::string`, this is achieved using the C++ `std::variant` type. Each column is represented as a vector of `std::optional<DataType>`. The use of `std::optional` allows individual elements to be `null` or missing ( i.e. `std::nullopt`).

The class provides methods for reading data from both CSV and JSON files. For details on these methods, as well as the available statistical functions, refer to the corresponding `.hpp` and `.cpp` files.

Finally, the class also provides an iterator that allows for easy row-by-row traversal of the DataFrame, for example a range-based loop looks like:

```cpp
for (const auto& row : df) {
    // Each `row` is a vector of std::optional<DataType> representing a dataframe's row.
}
```


## Module B: Interpolation
The interpolation module provides tools to perform linear, polynomial and cubic spline interpolations. It is designed to handle data efficiently and produce accurate interpolated values for a given set of points. 

### Features
- **Linear Interpolation**: implements piecewise linear interpolation for a given set of data points. Suitable for quick approximations with moderate accuracy.

- **Polynomial Interpolation**  
  - **Lagrange Interpolation**: uses the Lagrange form of the interpolating polynomial. Implemented using a third-party library (GSL)
  - **Newton Interpolation**: implements the Newton divided difference formula for efficient computation of polynomials.

- **Cubic Spline Interpolation**: implements smooth interpolation using a cubic spline basis, providing high accuracy and continuous second derivatives. Implemented using a third-party library (BOOST).

### Implementation
After building the project, the interpolation module can be tested using the `interpol_app`. This application demonstrates the accuracy and efficiency of the implemented methods.

In this case, we select the exponential function $y = e^x$ as the basis for our interpolation. In order to fulfil the specific requirements of the cardinal cubic B-spline interpolation using BOOST, we generate equispaced nodes along the desired interval. From these equispaced nodes, we compute the corresponding values of $y$ using the exponential function, thus providing the necessary data for the interpolation.
  
To carry out the interpolation, a specific interval was selected ($x \in [-4,4]$). Within this interval, 20 test points were generated to evaluate the interpolated function.

### Results

We conducted a detailed analysis of both **accuracy** and **efficiency** for each implemented interpolation method, varying the number of nodes dynamically from 5 to 10. The results are summarized in tables, where each table corresponds to a specific node count and interpolation method.

#### Efficiency  
Efficiency is measured by the interpolation time for all test points.  
- **Lagrange** is the most efficient method, followed closely by **Newton**.  

#### Accuracy  
Accuracy is evaluated using the **Mean Absolute Error (MAE)**:  
- **Lagrange** and **Newton** methods provide the highest accuracy (i.e., the lowest MAE).  
- **Cubic Spline** offers good accuracy, though slightly less accurate compared to Lagrange and Newton.  
- **Linear** interpolation is the least accurate, particularly when using fewer nodes (e.g., `n = 5`).  

Additionally, caution is advised with polynomial interpolation, especially as the number of nodes increases. This can lead to [*Runge's phenomenon*](https://en.wikipedia.org/wiki/Runge%27s_phenomenon), where oscillatory behavior near the edges causes decreased accuracy, making the interpolation less reliable.

#### Order of Convergence

The **Order of Convergence** has been calculated using the ratio between the mean absolute errors at different sets of nodes. This typically involves a logarithmic comparison of the errors as the number of nodes increases, providing insight into how quickly the error decreases with additional nodes. The results for various interpolation methods are as follows:

- **Linear Interpolation**:  
  Exhibits a convergence order of approximately **0.8**, indicating slow convergence. The error decreases moderately as more nodes are added.

- **Lagrange and Newton Interpolation**:  
  Both methods achieve a convergence order of about **4.2**, reflecting very rapid convergence. The error decreases significantly with additional nodes.

- **Cubic Spline Interpolation**:  
  Has a convergence order of approximately **1.77**, indicating moderate convergence. It improves faster than the linear method but is slower compared to Lagrange and Newton.


## Authors and contributions

**Elena Ruiz de la Cuesta**: 
elenaruizdelacuestacastano@gmail.com

Interpolation module and documentation

**Giovanni Lucarelli**: 
giovanni.lucarelli@studenti.units.it

Statistics module and documentation.

## References

1. Meseguer, Alvaro. *Fundamentals of Numerical Mathematics for Physicists and Engineers*. United States: Wiley, 2020.

2. [Rate of Convergence - Order Estimation](https://en.wikipedia.org/wiki/Rate_of_convergence#Order_estimation). Wikipedia.

3. [C++ `std::variant`](https://en.cppreference.com/w/cpp/utility/variant). cppreference.com.

4. [C++ `std::optional`](https://en.cppreference.com/w/cpp/utility/optional). cppreference.com.
