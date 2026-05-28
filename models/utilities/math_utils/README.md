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

A set of math utilities including methods for computing:
- transformation quaternions or matrices between frames, including:
  - inertial-LVLH
  - inertial-UVW
  - inertial-user-defined
  - inertial-VNC
  - pfix-ENU
  - ENU-pfix
- standard functions with built-in protections to avoid out-of-domain errors
- division protected against values close to zero
- values of polynomial functions
- computation of the roots for a quadratic equation
- cholesky decomposition of a square matrix
- simple backward-difference
- derivative of a unit vector
- comparison between floating-point values
- transforming a position-velocity covariance matrix between frames
- extract correlation coefficients from a covariance matrix
- vector and matrix algebra and utilities

## Documentation

This model is independently documented in the docs directory.
Current status: documentation is complete.

## Verification

This model contains independent verification test cases in the verif directory.
Current status: verification is complete.
