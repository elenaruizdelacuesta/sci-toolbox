#ifndef DATAFRAME_HPP
#define DATAFRAME_HPP

#include <string>
#include <vector>
#include <variant>
#include <optional>
#include <cmath>

using DataType = std::variant<double, std::string>;
using ColumnType = std::vector<std::optional<DataType>>;

class DataFrame
{   
    
private:
    std::vector<std::string> column_names;
    std::vector<ColumnType> data;
public:
    DataFrame() = default; 
    ~DataFrame() = default;

    /* ---------------------------------------------------------------------- */
    /*                         DATA STRUCTURE METHODS                         */
    /* ---------------------------------------------------------------------- */

    // read from CSV file, specifing the separator (if necessary) and a bool to specify if
    // contains the header or not (if not it will provide a default header: Col1, Col2, ...)
    void read_csv(const std::string& filename, char separator = ',', bool has_header = true);

    // read from JSON file, passing the filename (with the extension)
    void read_json(const std::string& filename);

    // return data of the dataset (without the header)
    const std::vector<ColumnType>& get_data() const;

    // return the pair: number of rows, number of columns
    std::pair<unsigned int,unsigned int> shape() const;

    // return the data from a particular column, passing the index (number)
    const ColumnType& get_column(const size_t column) const;

    // return the header of the dataframe
    const std::vector<std::string>& get_header() const;
    
    // insert a column in the dataset, passing the name and the data
    void add_column(const std::string& column_name, const ColumnType& new_col);

    // set a custom header by passing as a vector of strings
    void set_header(const std::vector<std::string>& new_header);

    // drop a row by index (numeric)
    void drop_row(const unsigned int& row);

    // drop a column by name (string)
    void drop_col(const std::string& name);

    // extract the column index for a given attribute name
    // if not present throw an error
    unsigned int find_idx(const std::string& name) const;

    // check if the data in the column (by name) are numeric
    bool is_numeric(const std::string& name) const;

    // print the number of nans for each attribute
    void table_nan() const;

    // drop all the rows containing at least one nan
    void drop_row_nan();

    // get a vector of double from a numeric column (passed by name)  
    // its needed for every gsl method
    std::vector<double> get_double_column(const std::string& name) const;

    // get a vector of strings from a non numerical column (passed by name)
    std::vector<std::string> get_string_column(const std::string& name) const;

    // usefull method to display the output in a formatted style
    unsigned int formatting_width() const;

    /* ---------------------------------------------------------------------- */
    /*                               STATISTICS                               */
    /* ---------------------------------------------------------------------- */

    // statistics functions on numerical column (passed by name) 

    double mean(const std::string& name) const;

    double median(const std::string& name) const;
    
    double min(const std::string& name) const;
    
    double max(const std::string& name) const;

    // return the quantile of a column for a give value of probability
    double quantile(const std::string& name, const double& q) const;
    
    double var(const std::string& name) const;

    double sd(const std::string& name) const;
    
    double covariance(const std::string& name1, const std::string& name2) const;

    double correlation(const std::string& name1, const std::string& name2) const;

    // print the correlation matrix for some attributes (passed as a vector of strings)
    void correlation_matrix(std::vector<std::string>& names) const;

    // frequency table for the categorical data
    void table(const std::string& name) const;

    // print the first 5 rows of the dataframe
    void head() const;

    // print the histogram for a numerical column, for a given number of bins (default 10)
    void histogram(const std::string& name, int num_bins = 10) const;

    // print: min, 1st qu., median, mean, 3rd qu., max for all the numerical attributes
    void summary() const;

    /* ---------------------------------------------------------------------- */
    /*                           ROW ITERATOR CLASS                           */
    /* ---------------------------------------------------------------------- */

    class row_iterator {
    private:
        const DataFrame& dataframe;
        size_t current_row;
        size_t max_rows;

    public:
        // required for the iterator traits
        using iterator_category = std::forward_iterator_tag;
        using value_type = ColumnType;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type*;
        using reference = value_type&;

        row_iterator(const DataFrame& df, size_t row = 0);

        //dereferece operator
        value_type operator*() const;

        // pre-increment operator
        row_iterator& operator++();
        
        // Post-increment operator
        row_iterator operator++(int);

        // Addition operator
        row_iterator operator+(difference_type n) const;

        // Comparison operators
        bool operator<(const row_iterator& other) const;

        bool operator!=(const row_iterator& other) const;

        bool operator==(const row_iterator& other) const;
    };

    //useful methods to iterate over the dataframe class
    row_iterator begin() const;

    row_iterator end() const;

};

#endif // DATAFRAME_HPP
