#include "Matrix.hpp"
#include <cmath>
#include <cstddef>
#include <numeric>
#include <stdexcept>
#include <iostream>
#include <utility>

Matrix::Matrix(std::size_t r, std::size_t c, double fill) {
    if (r == 0 || c == 0) {
        throw std::invalid_argument("Matrix dimensions must be > 0");
    }
    rows_ = r;
    cols_ = c;
    data_.assign(rows_ * cols_, fill);
}

// copy constructor
Matrix::Matrix(const Matrix& other)
    :data_(other.data_), rows_(other.rows_), cols_(other.cols_) {
}

// copy assignment
Matrix& Matrix::operator=(const Matrix& other) {
    if (this != &other) {
       rows_ = other.rows_;
       cols_ = other.cols_;
       data_ = other.data_;
    }

    return *this;
}

Matrix::Matrix(Matrix&& other) noexcept {
    data_ = std::move(other.data_);
    rows_ = other.rows_;
    cols_ = other.cols_;
}

Matrix& Matrix::operator=(const Matrix&& other) noexcept {
    if (this != &other) {
        data_ = std::move(other.data_);
        rows_ = other.rows_;
        cols_ = other.cols_;
    }

    return *this;
}

double& Matrix::operator()(std::size_t i, std::size_t j) noexcept {
    return data_[i * cols_ + j];
}

const double& Matrix::operator()(std::size_t i, std::size_t j) const noexcept {
    return data_[i * cols_ + j];
}


// checked
double& Matrix::at(std::size_t i, std::size_t j) {
    if (i >= rows_ || j >= cols_) {
        throw std::out_of_range(
            "Matrix::at out of range: index (" + std::to_string(i) + "," +
            std::to_string(j) + ") size (" + std::to_string(rows_) + "," +
            std::to_string(cols_) + ")"
        );
    }
    return (*this)(i, j);
}

const double& Matrix::at(std::size_t i, std::size_t j) const {
    if (i >= rows_ || j >= cols_) {
        throw std::out_of_range(
            "Matrix::at out of range: index (" + std::to_string(i) + "," +
            std::to_string(j) + ") size (" + std::to_string(rows_) + "," +
            std::to_string(cols_) + ")"
        );
    }
    return (*this)(i, j);
}

void Matrix::fill(double v) {
    for(size_t r = 0; r < rows_; r++) {
        for(size_t c = 0; c < cols_; c++) {
            (*this)(r, c) = v;
        }
    }
}

void Matrix::swap(Matrix& other) noexcept {
    std::swap(rows_, other.rows_);
    std::swap(cols_, other.cols_);
    std::swap(data_, other.data_);
}

struct LuResult Matrix::factorizeGEPP(double eps) const {
    if(rows_ != cols_) throw std::invalid_argument("Dimensions should be equal for GEPP");

    const std::size_t n = rows_;

    LuResult out{*this, {}, 0};
    out.piv.resize(n);
    std::iota(out.piv.begin(), out.piv.end(), 0);

    for(std::size_t k = 0; k < n; ++k) {
        std::size_t p = k; // used to store the pivot
        double maxabs = std::fabs(out.U(k, k));
        for (std::size_t i = k+1; i < n; ++i) {
            double v = std::fabs(out.U(i, k));
            if (v > maxabs) { maxabs = v; p = i;}
        }
        
        if (maxabs < eps) return out;

        // Swap the pivot row if needed
        if (p != k) {
            for (std::size_t j = 0; j < n; ++j) std::swap(out.U(k, j), out.U(p, j));
            std::swap(out.piv[k], out.piv[p]);
            ++out.swaps;
        }

        // Eliminate the entries below
        const double piv = out.U(k,k);
        for (std::size_t i = k + 1; i < n; ++i) {
            const double m = out.U(i,k) / piv;
            for (std::size_t j = k; j < n; ++j) {
                out.U(i,j) -= m * out.U(k,j);
            }
        }

    }

   return out; 
}

double Matrix::det(double eps) const {
    if (rows_ != cols_) throw std::invalid_argument("det: matrix must be square");
    const std::size_t n = rows_;
    if (n == 0) return 1.0;
    if (n == 1) return (*this)(0,0);

    LuResult lu = factorizeGEPP(eps);

    // If any diagonal is ~0, det is 0; we can check explicitly:
    double prod = (lu.swaps % 2 ? -1.0 : 1.0);
    for (std::size_t i = 0; i < n; ++i) {
        const double d = lu.U(i,i);
        if (std::fabs(d) < eps) return 0.0;
        prod *= d;
    }
    return prod;
}

bool Matrix::operator==(const Matrix& rhs) const {
    if (rows_ != rhs.rows_ || cols_ != rhs.cols_) return false;
    for(size_t r = 0; r < rows_; r++) {
        for(size_t c = 0; c < cols_; c++) {
            if ((*this)(r,c) != rhs(r,c)) return false;
        }
    }
    return true;
}

Matrix Matrix::operator+(const Matrix&  rhs) const {
    if (rows_ != rhs.rows_ || cols_ != rhs.cols_) throw std::invalid_argument("Matrix+: shape mismathch"); 
    Matrix m(*this); // Initialization
    for(size_t r = 0; r < rows_; r++) {
        for(size_t c = 0; c < cols_; c++) {
            m(r, c) += rhs(r, c);   
        }
    }  
    return m;
}

Matrix Matrix::operator*(const Matrix& rhs) const {
    if (cols_ != rhs.rows_) {
        throw std::invalid_argument("Matrix*: shape mismatch");
    }
    Matrix out(rows_, rhs.cols_, 0.0);
    for (std::size_t i = 0; i < rows_; ++i) {
        for (std::size_t k = 0; k < cols_; ++k) {
            const double a = (*this)(i,k);
            for (std::size_t j = 0; j < rhs.cols_; ++j) {
                out(i,j) += a * rhs(k,j);
            }
        }
    }
    return out;
}

Matrix Matrix::operator*(double scalar) const {
    Matrix out(*this);
    for (std::size_t r = 0; r < rows_; ++r)
        for (std::size_t c = 0; c < cols_; ++c)
            out(r,c) *= scalar;
    return out;
}

Matrix& Matrix::operator*=(double scalar) noexcept {
    for (std::size_t r = 0; r < rows_; ++r)
        for (std::size_t c = 0; c < cols_; ++c)
            (*this)(r,c) *= scalar;
    return *this;
}

std::ostream& operator<<(std::ostream& os, const Matrix& m) {
    for (std::size_t r = 0; r < m.rows(); ++r) {
        for (std::size_t c = 0; c < m.cols(); ++c) {
            os << m(r, c);
            if (c + 1 < m.cols()) {
                os << ' ';  // space between elements
            }
        }
        os << '\n'; // newline after each row
    }
    return os;
}


int main() {
    try {
        Matrix m(2, 2, 1.5);
        
        m(0, 0) = 0;
        m(0, 1) = 1;
        m(1, 0) = 1;
        m(1, 1) = 0;


        std::cout << m.det();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << '\n';
        return 1; // exit code 1 for error
    }
    return 0; // success
}
