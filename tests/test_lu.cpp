#include <iostream>
#include "lu.hpp"

const real EPS = 1e-4;
const real allowedError = 1e-3;

using std::string;
using yalal::MatStructure;

bool hasErrors(cv::Mat_<real>& A, cv::Mat_<real>& L, cv::Mat_<real>& U, cv::Mat_<real>& P, bool success, bool pivoting, string matrix_type) {
    // Check that no internal errors occured.
    if (!success) {
        std::cout << "LU test FAIL for " << matrix_type << " matrix. Expected success" << std::endl;
        return 1;
    }

    // Check that A was decomposed correctly.
    real error = 0;

    if (pivoting) {
        error = cv::norm(L*U - P*A, cv::NORM_L2);
    } else {
        error = cv::norm(L*U - A, cv::NORM_L2);
    }

    if (error > allowedError) {
        std::cout << "LU test FAIL for " << matrix_type << " matrix. A matrix is wrong. Pivoting is " << pivoting << ". Error value is " << error << std::endl; 
        return 1;
    }

    // Check that L is lower triangular.
    for (int i = 0; i < L.rows; ++i) {
        for (int j = i + 1; j < L.cols; ++j) {
            if (std::abs(L.at<real>(i, j)) > EPS) {
                std::cout << "LU test FAIL for " << matrix_type << " matrix." << "L is not LOWER_TRI" << std::endl;
                return 1;
            }
        }
    }

    // Check that R is upper triangular. 
    for (int i = 0; i < U.rows; ++i) {
        for (int j = 0; j < i; ++j) {
            if (std::abs(U.at<real>(i, j)) > EPS) {
                std::cout << "LU test FAIL for " << matrix_type << " matrix." << "R is not UPPER_TRI" << std::endl;
                return 1;
            }
        }
    }

    return 0;
}

int main() {
    cv::Mat_<real> A, L, U, P;
    bool success;

    // Random matrix.
    A.create(179, 179);
    //float dummy_data[4] = { 4, 3, 6, 3};
    //float dummy_data[9] = { 1, -2, 3, 2, -5, 12, 0, 2, -10};
    //A = cv::Mat(3, 3, CV_32F, dummy_data);
    cv::randu(A, -10., 10.);
    success = yalal::LU(A, L, U, MatStructure::ARBITRARY);

    if(hasErrors(A, L, U, P, success, false, "random")) return 1; 

    // Random matrix. Decompose with pivoting.
    success = yalal::LU(A, L, U, P, MatStructure::ARBITRARY);

    if(hasErrors(A, L, U, P, success, true, "random")) return 1;


    // Diagonal matrix. 
    A = cv::Mat_<real>::zeros(117, 117);
    for (int i = 0; i < A.rows; ++i) {
        A.at<real>(i, i) = (cv::randu<real>() - 0.5) * 100;
    }
    success = yalal::LU(A, L, U, MatStructure::DIAGONAL);

    if(hasErrors(A, L, U, P, success, false, "diagonal")) return 1;

    // upper triangular
    A = cv::Mat_<real>::zeros(354, 354);
    for (int i = 0; i < 354; ++i) {
        for (int j = i; j < 354; ++j) {
            A.at<real>(i, j) = (cv::randu<real>() - 0.5) * 2.;
        }
    }

    success = yalal::LU(A, L, U, MatStructure::UPPER_TRI);

    if(hasErrors(A, L, U, P, success, false, "upper triangular")) return 1;

    // lower triangular
    cv::transpose(A, A);

    success = yalal::LU(A, L, U, MatStructure::LOWER_TRI);

    if(hasErrors(A, L, U, P, success, false, "lower triangular")) return 1;

    return 0;
}