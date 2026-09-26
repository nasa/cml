# Information

<p align="center">
    <table>
        <tr>
            <th colspan=5>Model Information</th>
        </tr>
        <tr>
            <td>Rating</td>
            <td colspan=4 align=center>~maturity-4</td>
        </tr>
        <tr>
            <td>POC</td>
            <td>Gary Turner @gturner </td>
            <td>gary.w.turner@nasa.gov</td>
            <td>281.244.0668</td>
            <td>~ANTARES</td>
        </tr>
    </table>
</p>

## Synopsis

A one-stop shop for table interpolation capabilities, from simple single input
single-output linear interpolation to complex multi-input multi-output sets where
each output is a function of a subset of inputs.  Also includes wrap-around
capabilities on both input and output for interpreting and resolving cyclic
data such as angles.

## Documentation

This model is independently documented in the docs directory.

## Verification

This model contains independent verification test cases in the verif directory.

## Loading vectors without copying

`TableIndependentVariable::load_data` and `GenericMultiInputTable::load_data`
accept rvalue `DoubleVec` (`std::vector<double>`) arguments. Use `std::move` when
transferring a large data vector that the caller no longer needs:

```cpp
#include <utility>

double input = 0.5;
double output = 0.0;
TableIndependentVariable axis(input);
DoubleVec calibration{0.0, 1.0, 2.0};
if (!axis.load_data(std::move(calibration)) || !axis.initialize()) {
    return;
}

GenericMultiInputTable table(output);
table.add_independent(axis);
DoubleVec samples{10.0, 20.0, 30.0};
if (!table.load_data(std::move(samples), SizeVec{1, 3}) || !table.initialize()) {
    return;
}
table.update(); // output is 15.0
```

The same move overload is inherited by the single-input table classes. Existing
lvalue and pointer overloads continue to copy. Validation is shared between the
copy and move paths: independent data must be nonempty and monotonic, and
dependent data must match its dimensions and output count. Rejected vector
arguments are not moved from. After a successful move, the caller's vector is
valid but its contents are unspecified; reassign it before using its data again.

These methods keep the existing reload rules. Independent-variable data must
be cleared before reloading; a dependent table can replace previously loaded
data and retains its existing warning.
