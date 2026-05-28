/******************************** TRICK HEADER *********************************
PURPOSE:
  (Header for std::array<double, N> overloads. Useful for quick vector math.)

PROGRAMMERS:
  (((Matt Elmer) (OSR) (Jun 2023) (ANTARES V&V)))
*******************************************************************************/

#ifndef CML_STD_ARRAY_OPS_HH
#define CML_STD_ARRAY_OPS_HH

#ifndef SWIG

#include <stddef.h> // size_t
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <limits>
#include <ostream>
#include "math_utils.hh"

/*******************************************************************************
 * Insertion
 * std::cout << some_std_arr << std::endl;
 ******************************************************************************/
template<size_t N>
std::ostream& operator<<(std::ostream& out, const std::array<double, N>& rhs) {
  if (rhs.empty()) { return out; }
  out << "[" << rhs[0];
  for (size_t ii = 1; ii < rhs.size(); ii++) { out << ", " << rhs[ii];}
  out << "]";
  return out;
}

/*******************************************************************************
 * Copying/Casting
 * (usage given per copy/cast)
 *
 * These provide options for the programmer that would like to write clear code
 * but is interfacing with legacy code (or bad C++ code) that uses C-style
 * arrays. Note that only one operand per expression must be converted, since
 * overloads between std::array and C-style arrays are provided.
 ******************************************************************************/
/* Copy the C-style array into a std::array
 * temp = std_copy(c_arr);
 * other_result = temp + other_c_arr; */
template <size_t N>
std::array<double, N> std_copy(const double (&c_arr)[N]) {
  std::array<double, N> result;
  std::copy(std::begin(c_arr), std::end(c_arr), result.begin());
  return result;
}


/*******************************************************************************
 * Dot-wise addition assignment
 * lhs += rhs;
 * 
 * lhs must be a std::array.
 * rhs must be a std::array or C-style array.
 ******************************************************************************/
template <size_t N>
std::array<double, N>& operator+=(std::array<double, N>& lhs,
                                  const std::array<double, N>& rhs) {
  for (size_t ii = 0; ii < N; ii++) { lhs[ii] += rhs[ii]; }
  return lhs;
}

template <size_t N>
std::array<double, N>& operator+=(std::array<double, N>& lhs,
                                  const double (&rhs)[N]) {
  for (size_t ii = 0; ii < N; ii++) { lhs[ii] += rhs[ii]; }
  return lhs;
}

/*******************************************************************************
 * Dot-wise addition
 * result = lhs + rhs;
 * 
 * lhs and rhs must be a std::array lvalue, std::array rvalue, or C-style array.
 * Only one of lhs or rhs can be a C-style array per operation.
 ******************************************************************************/
template <size_t N>
std::array<double, N> operator+(const std::array<double, N>& lhs, const std::array<double, N>& rhs) {
  std::array<double, N> result = lhs;
  result += rhs;
  return result;
}

template <size_t N>
std::array<double, N> operator+(std::array<double, N>&& lhs, const std::array<double, N>& rhs) {
  lhs += rhs;
  return lhs;
}

template <size_t N>
std::array<double, N> operator+(const std::array<double, N>& lhs, std::array<double, N>&& rhs) {
  rhs += lhs;  // To avoid unnecessary copying, we take advantage of the fact
  return rhs;  // that rhs + lhs == lhs + rhs.
}

template <size_t N>
std::array<double, N> operator+(std::array<double, N>&& lhs, std::array<double, N>&& rhs) {
  lhs += rhs;
  return lhs;
}

template <size_t N>
std::array<double, N> operator+(const std::array<double, N>& lhs, const double (&rhs)[N]) {
  std::array<double, N> result = lhs;
  for (size_t ii = 0; ii < N; ii++) { result[ii] += rhs[ii]; }
  return result;
}

template <size_t N>
std::array<double, N> operator+(std::array<double, N>&& lhs, const double (&rhs)[N]) {
  for (size_t ii = 0; ii < N; ii++) { lhs[ii] += rhs[ii]; }
  return lhs;
}

template <size_t N>
std::array<double, N> operator+(const double (&lhs)[N], const std::array<double, N>& rhs) {
  std::array<double, N> result = rhs;
  for (size_t ii = 0; ii < N; ii++) { result[ii] += lhs[ii]; }
  return result;  // We again use rhs + lhs == lhs + rhs.
}

template <size_t N>
std::array<double, N> operator+(const double (&lhs)[N], std::array<double, N>&& rhs) {
  for (size_t ii = 0; ii < N; ii++) { rhs[ii] += lhs[ii]; }
  return rhs;  // We again use rhs + lhs == lhs + rhs.
}

/*******************************************************************************
 * Unary minus
 * result = -rhs;
 * 
 * rhs must be an std::array lvalue or std::array rvalue.
 ******************************************************************************/
template <size_t N>
std::array<double, N> operator-(const std::array<double, N>& rhs) {
  std::array<double, N> result = rhs;
  for (size_t ii = 0; ii < N; ii++) { result[ii] = -result[ii]; }
  return result;
}

template <size_t N>
std::array<double, N> operator-(std::array<double, N>&& rhs) {
  for (size_t ii = 0; ii < N; ii++) { rhs[ii] = -rhs[ii]; }
  return rhs;
}

/*******************************************************************************
 * Subtraction assignment
 * lhs -= rhs;
 * 
 * lhs must be a std::array.
 * rhs must be a std::array or C-style array.
 ******************************************************************************/
template <size_t N>
std::array<double, N>& operator-=(std::array<double, N>& lhs,
                                  const std::array<double, N>& rhs) {
  for (size_t ii = 0; ii < N; ii++) { lhs[ii] -= rhs[ii]; }
  return lhs;
}

template <size_t N>
std::array<double, N>& operator-=(std::array<double, N>& lhs,
                                  const double (&rhs)[N]) {
  for (size_t ii = 0; ii < N; ii++) { lhs[ii] -= rhs[ii]; }
  return lhs;
}

/*******************************************************************************
 * Subtraction
 * result = lhs - rhs;
 * 
 * lhs and rhs must be a std::array lvalue, std::array rvalue, or C-style array.
 * Only one of lhs or rhs can be a C-style array per operation.
 ******************************************************************************/
template <size_t N>
std::array<double, N> operator-(const std::array<double, N>& lhs, const std::array<double, N>& rhs) {
  std::array<double, N> result = lhs;
  result -= rhs;
  return result;
}

template <size_t N>
std::array<double, N> operator-(std::array<double, N>&& lhs, const std::array<double, N>& rhs) {
  lhs -= rhs;
  return lhs;
}

template <size_t N>
std::array<double, N> operator-(const std::array<double, N>& lhs, std::array<double, N>&& rhs) {
  rhs -= lhs;   // To avoid unnecessary copying, we take advantage of the fact
  return -rhs;  // that lhs - rhs == -(rhs - lhs).
}

template <size_t N>
std::array<double, N> operator-(std::array<double, N>&& lhs, std::array<double, N>&& rhs) {
  lhs -= rhs;
  return lhs;
}

template <size_t N>
std::array<double, N> operator-(const std::array<double, N>& lhs, const double (&rhs)[N]) {
  std::array<double, N> result = lhs;
  for (size_t ii = 0; ii < N; ii++) { result[ii] -= rhs[ii]; }
  return result;
}

template <size_t N>
std::array<double, N> operator-(std::array<double, N>&& lhs, const double (&rhs)[N]) {
  for (size_t ii = 0; ii < N; ii++) { lhs[ii] -= rhs[ii]; }
  return lhs;
}

template <size_t N>
std::array<double, N> operator-(const double (&lhs)[N], const std::array<double, N>& rhs) {
  std::array<double, N> result = rhs;
  for (size_t ii = 0; ii < N; ii++) { result[ii] -= lhs[ii]; }
  return -result;  // We again use lhs - rhs == -(rhs - lhs).
}

template <size_t N>
std::array<double, N> operator-(const double (&lhs)[N], std::array<double, N>&& rhs) {
  for (size_t ii = 0; ii < N; ii++) { rhs[ii] -= lhs[ii]; }
  return -rhs;  // We again use lhs - rhs == -(rhs - lhs).
}

/*******************************************************************************
 * Scalar multiplication assignment
 * lhs *= rhs;
 * 
 * lhs must be a std::array.
 * rhs must be a double.
 ******************************************************************************/
template <size_t N>
std::array<double, N>& operator*=(std::array<double, N>& lhs,
                                  const double& rhs) {
  for (size_t ii = 0; ii < N; ii++) { lhs[ii] *= rhs; }
  return lhs;
}

/*******************************************************************************
 * Scalar multiplication
 * result = lhs * rhs;
 * 
 * lhs and rhs must be a std::array lvalue, std::array rvalue, or double.
 ******************************************************************************/
template <size_t N>
std::array<double, N> operator*(const std::array<double, N>& lhs,
                                double rhs) {
  std::array<double, N> result = lhs;
  result *= rhs;
  return result;
}

template <size_t N>
std::array<double, N> operator*(std::array<double, N>&& lhs,
                                double rhs) {
  lhs *= rhs;
  return lhs;
}

template <size_t N>
std::array<double, N> operator*(double lhs,
                                const std::array<double, N>& rhs) {
  std::array<double, N> result = rhs;
  result *= lhs;  // To make use of our existing operator*=, we take advantage
  return result;  // of the fact that lhs * rhs == rhs * lhs.
}

template <size_t N>
std::array<double, N> operator*(double lhs,
                                std::array<double, N>&& rhs) {
  rhs *= lhs;  // To make use of our existing operator*=, we take advantage
  return rhs;  // of the fact that lhs * rhs == rhs * lhs.
}

/*******************************************************************************
 * Dot-wise vector multiplication assignment
 * lhs *= rhs;
 * 
 * lhs must be an std::array.
 * rhs must be a std::array or C-style array.
 ******************************************************************************/
template <size_t N>
std::array<double, N>& operator*=(std::array<double, N>& lhs,
                                  const std::array<double, N>& rhs) {
  for (size_t ii = 0; ii < N; ii++) { lhs[ii] *= rhs[ii]; }
  return lhs;
}

template <size_t N>
std::array<double, N>& operator*=(std::array<double, N>& lhs,
                                  const double (&rhs)[N]) {
  for (size_t ii = 0; ii < N; ii++) { lhs[ii] *= rhs[ii]; }
  return lhs;
}

/*******************************************************************************
 * Dot-wise vector multiplication
 * result = lhs * rhs;
 * 
 * lhs and rhs must be a std::array lvalue, std::array rvalue, or C-style array.
 * Only one of lhs or rhs can be a C-style array per operation.
 ******************************************************************************/
template <size_t N>
std::array<double, N> operator*(const std::array<double, N>& lhs, const std::array<double, N>& rhs) {
  std::array<double, N> result = lhs;
  result *= rhs;
  return result;
}

template <size_t N>
std::array<double, N> operator*(std::array<double, N>&& lhs, const std::array<double, N>& rhs) {
  lhs *= rhs;
  return lhs;
}

template <size_t N>
std::array<double, N> operator*(const std::array<double, N>& lhs, std::array<double, N>&& rhs) {
  rhs *= lhs;  // To avoid unnecessary copying, we take advantage of the fact
  return rhs;  // that lhs * rhs == rhs * lhs.
}

template <size_t N>
std::array<double, N> operator*(std::array<double, N>&& lhs, std::array<double, N>&& rhs) {
  lhs *= rhs;
  return lhs;
}

template <size_t N>
std::array<double, N> operator*(const std::array<double, N>& lhs, const double (&rhs)[N]) {
  std::array<double, N> result = lhs;
  for (size_t ii = 0; ii < N; ii++) { result[ii] *= rhs[ii]; }
  return result;
}

template <size_t N>
std::array<double, N> operator*(std::array<double, N>&& lhs, const double (&rhs)[N]) {
  for (size_t ii = 0; ii < N; ii++) { lhs[ii] *= rhs[ii]; }
  return lhs;
}

template <size_t N>
std::array<double, N> operator*(const double (&lhs)[N], const std::array<double, N>& rhs) {
  std::array<double, N> result = rhs;
  for (size_t ii = 0; ii < N; ii++) { result[ii] *= lhs[ii]; }
  return result;  // We again use lhs * rhs == rhs * lhs.
}

template <size_t N>
std::array<double, N> operator*(const double (&lhs)[N], std::array<double, N>&& rhs) {
  for (size_t ii = 0; ii < N; ii++) { rhs[ii] *= lhs[ii]; }
  return rhs;  // We again use lhs * rhs == rhs * lhs.
}

/*******************************************************************************
 * Scalar division assignment
 * lhs /= rhs;
 * 
 * lhs must be a std::array.
 * rhs must be a double.
 ******************************************************************************/
template <size_t N>
std::array<double, N>& operator/=(std::array<double, N>& lhs,
                                  const double& rhs) {
  for (size_t ii = 0; ii < N; ii++) {
    lhs[ii] = MathUtils::divide_protected( lhs[ii],
                                           rhs);
  }
  return lhs;
}

/*******************************************************************************
 * Scalar division
 * result = lhs / rhs;
 * 
 * lhs and rhs must be a std::array lvalue, std::array rvalue, or double.
 ******************************************************************************/
template <size_t N>
std::array<double, N> operator/(const std::array<double, N>& lhs,
                                const double& rhs) {
  std::array<double, N> result = lhs;
  result /= rhs;
  return result;
}

template <size_t N>
std::array<double, N> operator/(std::array<double, N>&& lhs, const double& rhs) {
  lhs /= rhs;
  return lhs;
}

template <size_t N>
std::array<double, N> operator/(const double& lhs,
                                const std::array<double, N>& rhs) {
  std::array<double, N> result = rhs;
  for (size_t ii = 0; ii < N; ii++) {
    result[ii] = MathUtils::divide_protected( lhs,
                                             result[ii]);
  }
  return result;  // Can't re-use operator/= in this case.
}

template <size_t N>
std::array<double, N> operator/(const double& lhs,
                                std::array<double, N>&& rhs) {
  for (size_t ii = 0; ii < N; ii++) {
    rhs[ii] = MathUtils::divide_protected( lhs,
                                          rhs[ii]);
  }
  return rhs;  // Can't re-use operator/= in this case.
}

/*******************************************************************************
 * Dot-wise vector division assignment
 * lhs /= rhs;
 * 
 * lhs must be a std::array.
 * rhs must be a std::array or C-style array.
 ******************************************************************************/
template <size_t N>
std::array<double, N>& operator/=(std::array<double, N>& lhs,
                                  const std::array<double, N>& rhs) {
  for (size_t ii = 0; ii < N; ii++) {
    lhs[ii] = MathUtils::divide_protected( lhs[ii],
                                          rhs[ii]);
  }
  return lhs;
}

template <size_t N>
std::array<double, N>& operator/=(std::array<double, N>& lhs,
                                  const double (&rhs)[N]) {
  for (size_t ii = 0; ii < N; ii++) {
    lhs[ii] = MathUtils::divide_protected( lhs[ii],
                                          rhs[ii]);
  }
  return lhs;
}

/*******************************************************************************
 * Dot-wise vector division
 * result = lhs / rhs;
 * 
 * lhs and rhs must be a std::array lvalue, std::array rvalue, or C-style array.
 * Only one of lhs or rhs can be a C-style array per operation.
 ******************************************************************************/
template <size_t N>
std::array<double, N> operator/(const std::array<double, N>& lhs, const std::array<double, N>& rhs) {
  std::array<double, N> result = lhs;
  result /= rhs;
  return result;
}

template <size_t N>
std::array<double, N> operator/(std::array<double, N>&& lhs, const std::array<double, N>& rhs) {
  lhs /= rhs;
  return lhs;
}

template <size_t N>
std::array<double, N> operator/(const std::array<double, N>& lhs, std::array<double, N>&& rhs) {
  std::array<double, N> result = lhs;
  result /= rhs;  // In this case, we have to choose between an extra copy and
  return result;  // an extra division operation, so we choose the latter.
}

template <size_t N>
std::array<double, N> operator/(std::array<double, N>&& lhs, std::array<double, N>&& rhs) {
  lhs /= rhs;
  return lhs;
}

template <size_t N>
std::array<double, N> operator/(const std::array<double, N>& lhs, const double (&rhs)[N]) {
  std::array<double, N> result;
  for (size_t ii = 0; ii < N; ii++) {
    result[ii] = MathUtils::divide_protected( lhs[ii],
                                             rhs[ii]);
  }
  return result;
}

template <size_t N>
std::array<double, N> operator/(std::array<double, N>&& lhs, const double (&rhs)[N]) {
  for (size_t ii = 0; ii < N; ii++) {
    lhs[ii] = MathUtils::divide_protected( lhs[ii],
                                          rhs[ii]);
  }
  return lhs;
}

template <size_t N>
std::array<double, N> operator/(const double (&lhs)[N], const std::array<double, N>& rhs) {
  std::array<double, N> result;// = std_copy(lhs) / rhs;
  for (size_t ii = 0; ii < N; ii++) {
    result[ii] = MathUtils::divide_protected( lhs[ii],
                                             rhs[ii]);
  }
  return result;
}

template <size_t N>
std::array<double, N> operator/(const double (&lhs)[N], std::array<double, N>&& rhs) {
  std::array<double, N> result;
  for (size_t ii = 0; ii < N; ii++) {
    result[ii] = MathUtils::divide_protected( lhs[ii],
                                             rhs[ii]);
  }
  return result;
}

#endif
#endif
