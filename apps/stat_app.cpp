#include "DataFrame.hpp"
#include <iostream>
#include <fstream>

int main(int argc, char* argv[])
{   
    try
    {
        if(argc<3)
        { 
            throw std::runtime_error("Not enough arguments provided.");
        }

        std::string in_dir_name="../../datasets/";
        std::string in_file_name=argv[1];

        std::string out_dir_name="../../output/";
        std::string out_file_name=argv[2];

        // Open a file for writing
        std::ofstream file(out_dir_name+out_file_name);
        // Check if the output file is opened correctly
        if (!file.is_open()) {
            std::cerr << "Error in opening output file." << std::endl;
            return 1; 
        }

        // redirect the output to the output file
        std::streambuf *original_buffer = std::cout.rdbuf();
        std::cout.rdbuf(file.rdbuf());

        /* ------------------------------------------------------------------ */
        /*               Dataframe Methods Testing: Iris Dataset              */
        /* ------------------------------------------------------------------ */
               
        
        DataFrame iris{};
        
        /* ------------------------ READING FROM FILE ----------------------- */
        // choose the appropriate reading function by commenting the other
        // remember to put the extension of the file in the terminal! 
        
        iris.read_csv(in_dir_name+in_file_name);
         
        //iris.read_json(in_dir_name+in_file_name);
         /* ----------------------------------------------------------------- */

        std::cout << "\nDATASET SHAPE:" <<"\n\n";
        std::cout<< "N. ROWS:\t" << iris.shape().first<<"\n";
        std::cout<< "N. COLUMNS:\t" << iris.shape().second<<"\n\n";

        iris.head();
    
        iris.summary();

        // print variance and standard deviation for each numerical feature
        std::cout << "\nTESTING VARIANCE AND STD DEVIATION" <<"\n\n";
        for (auto &&name : iris.get_header())
        {
            if (iris.is_numeric(name))
            {
                std::cout << name << "\tVariance: " << iris.var(name) << "\tSd: " << iris.sd(name) << std::endl;
            }    
        }
        
        // print the frequency count table
        std::cout << "\nFREQUENCY TABLE:    Species" <<"\n\n";
        iris.table("Species");
        
        // look for nans
        std::cout << "\nNANS TABLE" <<"\n\n";
        iris.table_nan();
        
        // drop rows containing nans in order to compute covariance etc
        std::cout << "\n... DROPPING THE ROWS CONTAINING NANS ..." <<"\n\n";
        iris.drop_row_nan();
        // check if everything is correct
        std::cout << "\nNOW THE DATASET SHOULD HAVE NO NANS" <<"\n\n";
        iris.head();
        std::cout << "\nNANS TABLE" <<"\n\n";
        iris.table_nan();
        
        // compute covariance and correlation between two numerical features
        std::cout << "\nTESTING COVAREIANCE AND CORRELATION" <<"\n\n";
        std::cout << "Covariance (SepalLengthCm,SepalWidthCm):\t" << iris.correlation("SepalWidthCm", "PetalWidthCm")<<"\n";
        std::cout << "Correlation (SepalLengthCm,SepalWidthCm):\t" << iris.correlation("SepalWidthCm", "PetalWidthCm")<<"\n\n";
        
        // print the correlation matrix between all the pairs of (numerical) features
        std::vector<std::string> att ={"SepalLengthCm",
                                        "SepalWidthCm",
                                        "PetalLengthCm",
                                        "PetalWidthCm"};
        std::cout << "CORRELATION MATRIX\n" << std::endl;
        iris.correlation_matrix(att);

        // print the histogram of a (numerical) feature
        // remark: the histogram for a categorical il just the frequency table
        std::cout << "\nTESTING HISTOGRAM" <<"\n\n";
        std::cout<<"HISTOGRAM (10 bins):   Sepal Width (Cm)"<<"\n\n";
        iris.histogram("SepalWidthCm");
        std::cout << "\n";
        std::cout<<"HISTOGRAM (5 bins):   Sepal Width (Cm)"<<"\n\n";
        iris.histogram("SepalWidthCm", 5);


        // testing the drop col method
        std::cout << "\n... DROPPING THE COLUMN SepalWidthCm ..." <<"\n\n";
        iris.drop_col("SepalWidthCm");
        std::cout<<"THE FOLLOWING TABLE SHOULD NOT HAVE THE COLUMN SepalWidthCm\n"<<std::endl;
        iris.head();   

        /* ------------------------------------------------------------------ */

        // Restore the original standard output
        std::cout.rdbuf(original_buffer);
        std::cout<<out_file_name<<" SAVED SUCCESFULLY IN "<<out_dir_name<<std::endl;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}
