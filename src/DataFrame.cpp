#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <numeric>
#include <variant>
#include <optional>         
#include <gsl/gsl_statistics.h>
#include <boost/histogram.hpp>
#include <boost/json.hpp>
#include <map> 
#include "DataFrame.hpp"


/* -------------------------------------------------------------------------- */
/*                           DATA STRUCTURE METHODS                           */
/* -------------------------------------------------------------------------- */

void DataFrame::read_csv(const std::string& filename, char separator, bool has_header){
    // Clear existing data
    column_names.clear();
    data.clear();

    // Open the file
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    std::string line;
        
    // Read header if true
    if (has_header) {
        if (!std::getline(file, line)) {
            throw std::runtime_error("Empty CSV file");
        }

        std::istringstream header_stream(line);
        std::string column_name;
        
        // Parse header columns
        while (std::getline(header_stream, column_name, separator)) {
            column_names.push_back(column_name);
        }
    }

    // Read all rows and store in raw_data
    // raw_data is the transposed version of the row

    std::vector<ColumnType> raw_data;
    while (std::getline(file, line)) {
        std::istringstream row_stream(line);
        std::string cell;
        ColumnType row_cells;

        while (std::getline(row_stream, cell, separator)) {
            // If empy cell put a null option
            if (cell.empty()) {
                row_cells.push_back(std::nullopt);
            } 
            else 
            {
                // try to convert the cell into a double
                try {
                    row_cells.push_back(std::stod(cell));
                } catch (const std::invalid_argument&) {
                    // if fails pass it as a string
                    row_cells.push_back(cell);
                }
            }
        }
        // Add row to raw data
        raw_data.push_back(row_cells);
    }

    // If no header was specified, create default column names
    if (column_names.empty()) {
        for (size_t i = 0; i < raw_data[0].size(); ++i) {
            column_names.push_back("Column_" + std::to_string(i));
        }
    }

    // Transpose and convert raw_data to column-based storage
    if (!raw_data.empty()) {
        // Initialize columns
        data.resize(column_names.size());
        for (auto& column : data) {
            column.clear();
        }

        // Populate columns
        for (const auto& row : raw_data) {
            for (size_t col = 0; col < column_names.size(); ++col) {
                if (col < row.size()) {                    
                    data[col].push_back(row[col]);
                } else {
                    // If row is shorter, push null option
                    data[col].push_back(std::nullopt);
                }
            }
        }
    }
}

void DataFrame::read_json(const std::string& filename) {
    // Clear existing data
    column_names.clear();
    data.clear();

    // Read file contents
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    // Parse entire file contents
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string jsonStr = buffer.str();

    // Parse JSON 
    boost::json::value parsedJson = boost::json::parse(jsonStr);

    // Ensure it's an array
    if (!parsedJson.is_array()) {
        throw std::runtime_error("JSON must be an array of objects");
    }

    const boost::json::array& jsonArray = parsedJson.as_array();
    
    if (jsonArray.empty()) {
        throw::std::runtime_error("No data to load");
    }

    // Extract column names from first object
    const boost::json::object& firstObj = jsonArray[0].as_object();
    for (const auto& [key, value] : firstObj) {
        column_names.push_back(std::string(key));
    }

    // Prepare columns with the correct size
    data.resize(column_names.size());

    // Populate data
    for (const auto& jsonRow : jsonArray) {
        const boost::json::object& rowObj = jsonRow.as_object();
        
        // Iterate through expected columns
        for (size_t colIndex = 0; colIndex < column_names.size(); ++colIndex) {
            const std::string& colName = column_names[colIndex];
            
            // Find the value for this column
            auto it = rowObj.find(colName);
            if (it == rowObj.end()) {
                // Column not found, add null
                data[colIndex].push_back(std::optional<DataType>{std::nullopt});
                continue;
            }

            // Convert value based on type
            const boost::json::value& value = it->value();
            
            if (value.is_double()) {
                data[colIndex].push_back(value.as_double());
            }
            else if (value.is_string()) {
                std::string strValue = std::string(value.as_string());
                if (strValue.empty()) {
                    // Treat empty strings as null
                    data[colIndex].push_back(std::optional<DataType>{std::nullopt});
                } else {
                    data[colIndex].push_back(strValue);
                }
            }
            else {
                // Unsupported type, add null
                data[colIndex].push_back(std::optional<DataType>{std::nullopt});
            }
        }
    }
}

const std::vector<ColumnType>& DataFrame::get_data() const 
{
    return data;
}

// first = nrows, second = ncols
std::pair<unsigned int,unsigned int> DataFrame::shape() const 
{
    std::pair<unsigned int,unsigned int> dims=std::make_pair(data[0].size(),data.size());
    return dims;
}

const ColumnType& DataFrame::get_column(const size_t column) const 
{   
    if (column > column_names.size())
    {
        throw std::invalid_argument("Error in get_column: column index out of range");
    }
    
    return data[column];
}

const std::vector<std::string>& DataFrame::get_header() const 
{
    return column_names;
}

void DataFrame::add_column(const std::string& column_name, const ColumnType& new_col) {
// If data is not empty, check that the new column has the same length
    if (!data.empty() && new_col.size() != data[0].size()) {
        throw std::invalid_argument("Error in add_column: New column must have the same number of rows as existing data");
    }
    
    // Add the column name
    column_names.push_back(column_name);
    
    // Add the column data
    data.push_back(new_col);
}

void DataFrame::set_header(const std::vector<std::string>& new_header){
    if (new_header.size()>column_names.size())
    {
        std::cerr << "WARNING in set_header: the header has more columns than the data!" << std::endl;
    }
    
    column_names.clear();
    column_names = new_header;
}

void DataFrame::drop_row(const unsigned int& row)
{   
    if (row > data[0].size())
    {
        throw std::invalid_argument("Error in drop_row: index out of range");
    }
    for (auto &&col : data)
    {
        col.erase((col.begin()+row));
    }
    
}

unsigned int DataFrame::find_idx(const std::string& name) const {
    // Find the attribute
    auto it = std::find(column_names.begin(), column_names.end(), name);
    if (it != column_names.end()) {
        // Calculate the index by subtracting the beginning iterator from the found iterator
        return std::distance(column_names.begin(), it);
    } else {
        std::cout << "Attribute not found\n";
        throw std::invalid_argument("Attribute not found");
    }
}

void DataFrame::drop_col(const std::string& name)
{   
    // Find the index once and store it
    unsigned int idx = find_idx(name);
    
    // Erase the column name
    column_names.erase(column_names.begin() + idx);
    
    // Erase the corresponding column data
    data.erase(data.begin() + idx);
}

std::vector<double> DataFrame::get_double_column(const std::string& name) const{
    std::vector<double> double_values{};
    
    for (const auto &cell : data[find_idx(name)])
    {
        if (cell && std::holds_alternative<double>(*cell))
        {
            double_values.push_back(std::get<double>(*cell));
        }   
    }
    
    return double_values;
}

std::vector<std::string> DataFrame::get_string_column(const std::string& name) const{
    std::vector<std::string> string_values{};
    for (const auto &cell : data[find_idx(name)])
    {
        if (cell && std::holds_alternative<std::string>(*cell))
        {
            string_values.push_back(std::get<std::string>(*cell));
        }
    }
    return string_values;
}

bool DataFrame::is_numeric(const std::string& name) const{
    for (const auto &cell : data[find_idx(name)])
    {
        if (std::holds_alternative<std::string>(*cell))
        { 
            return false;
        } 
    }
    return true;
}

void DataFrame::table_nan() const
{
    unsigned int spacing{formatting_width() +3};
    for (auto &&name : column_names)
    {
        unsigned int cnt{0};
        for (auto &&value : data[find_idx(name)])
        {
            if (!value.has_value())
            {
                cnt++;
            }   
        }
        std::cout << std::setw(spacing) << std::left << name << std::setw(spacing) << std::left << cnt << std::endl;
    }
}

void DataFrame::drop_row_nan()
{
    if (data.empty()) {
        return;
    }

    // Number of rows (assuming all columns have the same number of rows)
    std::size_t num_rows = data[0].size();

    // Vector to mark rows that should be removed
    std::vector<unsigned int> rows_to_drop;

    // Identify rows with any `std::nullopt`
    for (const auto& column : data) {
        for (std::size_t i = 0; i < num_rows; ++i) {
            if (!column[i].has_value()) {
                rows_to_drop.push_back(i);
            }
        }
    }

    // sort is necessary to rescale the index correctly
    std::sort(rows_to_drop.begin(), rows_to_drop.end());
    unsigned int cnt{0};
    for (auto &&idx : rows_to_drop)
    {
        drop_row(idx-cnt);  // everytime a row is dropped, rescale the idx
        cnt +=1;    
    }
}

unsigned int DataFrame:: formatting_width() const
{
    // Determine the maximum length of name for formatting the output
    int max_name_len{0};
    for (const auto& name : column_names) {
        max_name_len = std::max(max_name_len, static_cast<int>(name.length()));
    }
    return max_name_len;
}

/* -------------------------------------------------------------------------- */
/*                                 STATISTICS                                 */
/* -------------------------------------------------------------------------- */

double DataFrame::mean(const std::string& name) const {
    // Convert the column into a std::vector of doubles in order to use the gls method
    std::vector<double> values= get_double_column(name);
    if (values.empty()) {
        throw std::runtime_error("ERROR in function mean(): vector is empty.");
    }

    return gsl_stats_mean(values.data(), 1, values.size());
}

double DataFrame::median(const std::string& name) const {
    // Convert the column into a std::vector of doubles in order to use the gls method
    std::vector<double> values= get_double_column(name);

    if (values.empty()) {
        throw std::runtime_error("ERROR in function median(): vector is empty.");
    }

    return gsl_stats_median(values.data(), 1, values.size());
}

double DataFrame::min(const std::string& name) const {
    // Convert the column into a std::vector of doubles in order to use the gls method
    std::vector<double> values= get_double_column(name);

    if (values.empty()) {
        throw std::runtime_error("ERROR in function min(): vector is empty.");
    }

    return gsl_stats_min(values.data(), 1, values.size());
}

double DataFrame::max(const std::string& name) const {
    // Convert the column into a std::vector of doubles in order to use the gls method
    std::vector<double> values= get_double_column(name);

    if (values.empty()) {
        throw std::runtime_error("ERROR in function max(): vector is empty.");
    }

    return gsl_stats_max(values.data(), 1, values.size());
}

double DataFrame::quantile(const std::string& name, const double& q) const {
    // Convert the column into a std::vector of doubles in order to use the gls method
    std::vector<double> values= get_double_column(name);

    if (values.empty()) {
        throw std::runtime_error("ERROR in function quantile(): vector is empty.");
    }
    std::sort(values.begin(), values.end());

    return gsl_stats_quantile_from_sorted_data(values.data(), 1, values.size(), q);
}

double DataFrame::var(const std::string& name) const {
    // Convert the column into a std::vector of doubles in order to use the gls method
    std::vector<double> values= get_double_column(name);

    if (values.empty()) {
        throw std::runtime_error("ERROR in function var(): vector is empty.");
    }

    return gsl_stats_variance(values.data(), 1, values.size());
}

double DataFrame::sd(const std::string& name) const {
    // Convert the column into a std::vector of doubles in order to use the gls method
    std::vector<double> values= get_double_column(name);

    if (values.empty()) {
        throw std::runtime_error("ERROR in function sd(): vector is empty.");
    }

    return gsl_stats_sd(values.data(), 1, values.size());
}

double DataFrame::covariance(const std::string& name1, const std::string& name2) const {
    // Convert the column into a std::vector of doubles in order to use the gls method
    std::vector<double> values1= get_double_column(name1);
    std::vector<double> values2= get_double_column(name2);

    if (values1.empty()||values2.empty()) {
        throw std::runtime_error("ERROR in function covariance(): one of the two vectors is empty");
    }
    // If sizes don't match, raise an error
    if (values1.size() != values2.size()) {
        throw std::runtime_error("ERROR in function covariance(): incompatible sizes to compute covariance.");
    }
    // If everything is ok, return the covariance
    return gsl_stats_covariance(values1.data(), 1, values2.data(),1,values1.size());
}

double DataFrame::correlation(const std::string& name1, const std::string& name2) const {
    // Convert the column into a std::vector of doubles in order to use the gls method
    std::vector<double> values1= get_double_column(name1);
    std::vector<double> values2= get_double_column(name2);

    if (values1.empty()||values2.empty()) {
        throw std::runtime_error("ERROR in function correlation(): vector is empty.");
    }
    // If vectors have different sizes raise an error
    if (values1.size() != values2.size()) {
        throw std::runtime_error("ERROR in function correlation(): incompatible sizes to compute covariance.");
    }
    // If everything is ok, return the covariance
    return gsl_stats_correlation(values1.data(), 1, values2.data(),1,values1.size());
}

void DataFrame::correlation_matrix(std::vector<std::string>& names) const {
    // Determine the maximum length of a numeric name for formatting
    unsigned int max_name_len{formatting_width() +3};

    // Print the header row with appropriate spacing
    for (const auto& name : names) {
        if (is_numeric(name)) {
            std::cout << std::setw(max_name_len) << std::right << name;
        }
    }
    std::cout << std::endl;

    // Print the correlation matrix with formatted output
    for (const auto& name1 : names) {
        if (is_numeric(name1)) {
            for (const auto& name2 : names) {
                if (is_numeric(name2)) {
                    std::cout << std::fixed << std::setprecision(4) << std::setw(max_name_len) << correlation(name1, name2);
                }
            }
            std::cout << std::endl;
        }
    }
}

void DataFrame::table(const std::string& name) const {
    std::map<std::string,unsigned int> table{};
    for (auto& val : get_string_column(name)) {
        table[val] ++; // Update the frequency
    }
    unsigned int spacing{formatting_width() +3};
    // print attributes
    for(auto &el : table){
        std::cout<<std::setw(spacing) << std::left <<el.first << " "; 
    }
    std::cout << std::endl;
    // print frequency counts
    for(auto &el : table){
        std::cout<<std::setw(spacing) << std::left<<el.second << " "; 
    }
    std::cout << std::endl;

}

void DataFrame::head() const
{
    // Determine the maximum length of a name for formatting
    unsigned int spacing{formatting_width() +3};
    
    // Calculate the total width of the table
    unsigned int total_width = spacing * column_names.size();

    // Create the header string, centered within the total width
    std::string header_str = " Dataset Head ";
    int padding = (total_width - header_str.length()) / 2;
    header_str = std::string(padding, '-') + header_str + std::string(padding, '-');

    std::cout << header_str << std::endl;

    // Print the header 
    for (const auto& name : column_names) {
        std::cout << std::setw(spacing) << std::left << name;
    }
    std::cout << std::endl;
    
    // Print the data in a formatted style by looking at all the cases (option/variant)
    for (auto rowIt = this->begin(); rowIt < std::min(this->begin()+5, this->end()); rowIt++)  
    {   
        for (const auto& el : *rowIt) {

            if(el.has_value())
            {   
                const auto& value = el.value();
                if (std::holds_alternative<double>(value)) {
                    std::cout << std::setw(spacing)<< std::get<double>(value);
                }
                else if (std::holds_alternative<std::string>(value)) {
                    std::cout << std::setw(spacing)<< std::get<std::string>(value);
                }
            } 
            else 
            {
                std::cout << std::setw(spacing)<< "nan";
            }
                
        }
        std::cout << std::endl;
    }
    std::cout << std::string(total_width, '-') << std::endl; 
}

void DataFrame::histogram(const std::string& name, int num_bins) const
{   
    std::vector<double> values{get_double_column(name)};
    
    if (values.empty())
    {
        std::cerr << "Empty column: " << name << std::endl;
        return;
    }

    // Determine the minimum and maximum values
    double min_value = *std::min_element(values.begin(), values.end());
    double max_value = *std::max_element(values.begin(), values.end());

    // Add a small epsilon to the maximum value to ensure it falls in the last bin
    double epsilon = std::numeric_limits<double>::epsilon();
    double max_value_adjusted = max_value + 10*epsilon;
    // the value 10 has been chosen empirically

    // Build a histogram with an inclusive upper bound
    auto hist = boost::histogram::make_histogram(
        boost::histogram::axis::regular<>(num_bins, min_value, max_value_adjusted)
    );

    // Fill the histogram
    for (double value : values)
    {
        hist(value);
    }

    // Print the histogram
    for (auto&& bin : boost::histogram::indexed(hist))
    {
        std::cout << "[" << bin.index() << "] " << bin.bin(0).lower() << " - " << bin.bin(0).upper()
                  << ": " << *bin << std::endl;
    }
}

void DataFrame::summary() const {
    const unsigned int spacing{formatting_width() + 3};

    // Calculate the total width of the table
    unsigned int n_numeric_feat{};
    for (auto &&col : column_names)
    {   
        if (is_numeric(col))
        {
            n_numeric_feat++;
        }     
    }

    unsigned int total_width = spacing * n_numeric_feat + 10;

    // Create the header string, centered within the total width
    std::string header_str = " Dataset Summary ";
    int padding = (total_width - header_str.length()) / 2;
    header_str = std::string(padding, '-') + header_str + std::string(padding, '-');

    std::cout << header_str << std::endl;

    // Headers for each column
    std::cout << std::setw(10) << "";
    for (const auto& name : column_names) {
        if (is_numeric(name)) {
            std::cout << std::setw(spacing) << std::left << name;
        }
    }
    std::cout << std::endl;

    // Print Min
    std::cout << std::setw(10) << "Min.";
    for (const auto& name : column_names) {
        if (is_numeric(name)) {
            std::cout << std::setw(spacing) << min(name);
        }
    }
    std::cout << std::endl;

    // Print 1st Quartile
    std::cout << std::setw(10) << "1st Qu.";
    for (const auto& name : column_names) {
        if (is_numeric(name)) {
            std::cout << std::setw(spacing) << quantile(name, 0.25);
        }
    }
    std::cout << std::endl;

    // Print Median
    std::cout << std::setw(10) << "Median";
    for (const auto& name : column_names) {
        if (is_numeric(name)) {
            std::cout << std::setw(spacing) << median(name);
        }
    }
    std::cout << std::endl;

    // Print Mean
    std::cout << std::setw(10) << "Mean";
    for (const auto& name : column_names) {
        if (is_numeric(name)) {
            std::cout << std::setw(spacing) << mean(name);
        }
    }
    std::cout << std::endl;

    // Print 3rd Quartile
    std::cout << std::setw(10) << "3rd Qu.";
    for (const auto& name : column_names) {
        if (is_numeric(name)) {
            std::cout << std::setw(spacing) << quantile(name, 0.75);
        }
    }
    std::cout << std::endl;

    // Print Max
    std::cout << std::setw(10) << "Max.";
    for (const auto& name : column_names) {
        if (is_numeric(name)) {
            std::cout << std::setw(spacing) << max(name);
        }
    }
    std::cout << std::endl;
    std::cout << std::string(total_width, '-') << std::endl; 
}


/* -------------------------------------------------------------------------- */
/*                             ROW ITERATOR CLASS                             */
/* -------------------------------------------------------------------------- */


DataFrame::row_iterator::row_iterator(const DataFrame& df, size_t row) 
    : dataframe(df), current_row(row) {
    // maximum rows number is the size of the shortest column
    max_rows = std::numeric_limits<size_t>::max();
    for (const auto& column : dataframe.data) {
        max_rows = std::min(max_rows, column.size());
    }
}

DataFrame::row_iterator::value_type DataFrame::row_iterator::operator*() const {
    value_type row;

    // fill the row by scanning all the columns for a fixed idex 
    for (const auto& column : dataframe.data) {
        if (current_row < column.size()) {
            row.push_back(column[current_row]);
        }
    }
    return row;
}

DataFrame::row_iterator& DataFrame::row_iterator::operator++() {
    ++current_row;
    return *this;
}

// Post-increment operator
DataFrame::row_iterator DataFrame::row_iterator::operator++(int) {
    row_iterator temp = *this;
    ++current_row;
    return temp;
}

// Addition operator
DataFrame::row_iterator DataFrame::row_iterator::operator+(difference_type n) const {
    return row_iterator(dataframe, current_row + n);
}

// Comparison operators
bool DataFrame::row_iterator::operator<(const row_iterator& other) const {
    return current_row < other.current_row;
}

bool DataFrame::row_iterator::operator!=(const row_iterator& other) const {
    return current_row != other.current_row;
}

bool DataFrame::row_iterator::operator==(const row_iterator& other) const {
    return current_row == other.current_row;
}

// Metodi nella classe DataFrame per supportare l'iterazione per righe
DataFrame::row_iterator DataFrame::begin() const { 
    return row_iterator(*this, 0); 
}

DataFrame::row_iterator DataFrame::end() const { 
    // maximum rows number is the size of the shortest column
    size_t max_rows = std::numeric_limits<size_t>::max();
    for (const auto& column : data) {
        max_rows = std::min(max_rows, column.size());
    }
    return row_iterator(*this, max_rows); 
}
