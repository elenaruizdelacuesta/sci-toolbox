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
  - [Authors and contributions](#authors-and-contributions)

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
- **Linear Interpolation**
  Implements piecewise linear interpolation for a given set of data points. Suitable for quick approximations with moderate accuracy.

- **Polynomial Interpolation**  
  - **Lagrange Interpolation**: Uses the Lagrange form of the interpolating polynomial. Implemented using a third-party library (GSL)
  - **Newton Interpolation**: Implements the Newton divided difference formula for efficient computation of polynomials.
- **Cubic Spline Interpolation**  
  - **Cardinal Cubic B-Spline**: Implements smooth interpolation using a cubic spline basis, providing high accuracy and continuous second derivatives. Implemented using a third-party library (BOOST).

### Usage
After building the project, the interpolation module can be tested using the interpol_app. This application demonstrates the accuracy and efficiency of the implemented methods.

In this case, we select the exponential function y = e^x as the basis for our interpolation. In order to fulfil the specific requirements of the cardinal cubic B-spline interpolation using Boost, we generate equispaced nodes along the desired interval. From these equispaced nodes, we compute the corresponding values of y using the exponential function, thus providing the necessary data for the interpolation.
  
To carry out the interpolation, a specific interval was selected (x \in [-4,4] \). Within this interval, 20 test points were generated to evaluate the interpolated function.

### Results
-**Efficiency**: is measured in terms of interpolation time for all test points. 
  -**Lagrange** is the most efficient method in both cases (`n = 15` and `n = 30`), followed by Cubic Spline.  
  -**Newton** shows similar efficiency to Cubic Spline with `n = 15` and a higher efficiency with `n = 30`.

-**Accuracy**: is evaluated through Root Mean Square Error (RMSE)
  -**Lagrange** and **Newton** have the highest accuracy (lowest RMSE) in both (`n = 15` and `n = 30`).
  -**Cubic Spline**, while less accurate than Lagrange and Newton, performs well with manageable errors in both cases.  
  -**Linear** is significantly less accurate, especially with a lower number of points (`n = 30`).
  
Methods like Lagrange and Cubic Spline showed improved efficiency by taking advantage of third-party libraries that handle the complexities of interpolation more effectively.

Additionally, it's important to be cautious with polynomial interpolation, as the Runge phenomenon can occur, especially when the number of nodes becomes large. This phenomenon causes the interpolation to produce oscillatory behavior near the edges, which can lead to decreased accuracy, making the results less reliable.



## Authors and contributions

**Elena Ruiz de la Cuesta**: 
elenaruizdelacuestacastano@gmail.com

Interpolation module and documentation

**Giovanni Lucarelli**: 
giovanni.lucarelli@studenti.units.it

Statistics module and documentation.
