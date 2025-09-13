#ifndef MATRIX_H
#define MATRIX_H

#include <iterator>
#include <vector>
#include <cstddef>   // std::size_t
#include <iosfwd>    // std::ostream forward decl


class Matrix {
private:
    using Storage = std::vector<double>;
    Storage data_;
    std::size_t rows_{0};
    std::size_t cols_{0};
    struct LuResult factorizeGEPP(double eps = 1e-12) const;

public:
    Matrix() = default; // 0x0 valid empty

    // ctor we’ll implement in Matrix.cpp
    Matrix(std::size_t r, std::size_t c, double fill = 0.0);
    
    // copy ctor
    Matrix(const Matrix& other);
   
    // copy assignment
    Matrix& operator=(const Matrix& other);

    // move ctor and assignment
    Matrix(Matrix&& other) noexcept;

    Matrix& operator=(const Matrix&& other) noexcept;

    // accessors
    std::size_t rows() const noexcept { return rows_; }
    std::size_t cols() const noexcept { return cols_; }
    bool empty() const noexcept { return rows_ == 0 || cols_ == 0; }
    
    double& operator()(std::size_t i, std::size_t j) noexcept;
    const double& operator()(std::size_t i, std::size_t j) const noexcept;
    double& at(std::size_t i, std::size_t j);
    const double& at(std::size_t i, std::size_t j) const;
    void fill(double v);
    void swap(Matrix& other) noexcept;
    double det(double eps = 1e-12) const;
    Matrix solve(const Matrix& B, double eps = 1e-12) const;
    Matrix forward_substitution(const Matrix& L, Matrix& Pb) const; 
    Matrix backward_substitution(const Matrix& U, const Matrix& y, double eps = 1e-12) const;

    bool operator==(const Matrix& rhs) const;
    Matrix operator+(const Matrix&  rhs) const;
    Matrix operator*(const Matrix& rhs) const;
    Matrix operator*(double scalar) const;
    Matrix& operator*=(double scalar) noexcept;
};

std::ostream& operator<<(std::ostream& os, const Matrix& matrix);

struct LuResult {
    Matrix U;
    std::vector<std::size_t> piv;
    int swaps;
};

#endif // MATRIX_H
