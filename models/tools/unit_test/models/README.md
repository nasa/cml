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

Provides a C++ -based testing harness for unit-testing a model within a Trick
simulation framework.

Intent is to fire a series of a input value-sets into the model and record
the response back from the model.  The inputs are intended to be such that
each input set is effectively independent of every other input set.
It is not intended that this framework be used to test the dynamic response
of models over time.  Dynamic response should be tested with a dynamic
simulation.

## Documentation

This model is independently documented in the docs directory.

## Verification

This model contains independent verification test cases in the verif directory.
