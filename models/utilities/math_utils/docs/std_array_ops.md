std_array_ops
=============
Defines overloads and functions for operators commonly used in vector algebra. At a glance:
- Binary operators are provided for mixed array types i.e. `double[3]` + `std::array<double, 3>` works
- Care was taken to overload the operators for lvalues and rvalues to maximize performance
- Operator chaining works just as you would expect it to (PEMDAS respected, etc.)
- Copy/cast functions provide options for the programmer interfacing with C-style arrays
- In-place modification vs. returning a new object works as expected
  - e.g. `operator+` returns a new array while `operator+=` modifies the array in-place

Important things to note:
- In cases where explicitly handling rvalues wouldn't result in performance gains, it isn't done.
- **The existence of these overloads does not preclude the use of `std::array` with JEOD's `Vector3` library.** In fact, simply passing the address of a `std::array<double, 3>` to JEOD's `Vector3` library will result in performance parity between `std::array<double, 3>` and C-style arrays. Why might you want to use JEOD's `Vector3` library despite the existence of these overloads? Performance:
  - **Without optimization**, these overloads are about 5x slower than JEOD's Vector3 library. To put that in context:
    - **Without optimization**,  on average, a JEOD `sum`/`diff`/`scale` function takes about 27% the amount of time that calling `sin` on a scalar takes, and about 10% of the time that calling `pow` on two doubles takes.
    - **Without optimization**,  on average, an overload for `operator+`/`operator-`/`operator*`/`operator/` takes about 133% the amount of time that calling `sin` on a scalar takes, and about 50% of the time that calling `pow` on two doubles takes.
  - **With `-O` optimization**, these overloads are about 2x slower for in-place operations, and 16x slower for operations that return a new array. To put that in context:
    - **With `-O` optimization**, on average, a JEOD `sum`/`diff`/`scale` function takes about 100% the amount of time that calling `sin` on a scalar takes, and about 0.2% of the time that calling `pow` on two doubles takes.
    - **With `-O` optimization**, on average, an overload for `operator+`/`operator-`/`operator*`/`operator/` takes about 1600% the amount of time that calling `sin` on a scalar takes, and about 3.2% of the time that calling `pow` on two doubles takes.
