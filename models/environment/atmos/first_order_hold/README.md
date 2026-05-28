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

This model palces a first-order hold on the wind-velocity, delaying it by one
frame so that during the next frame, the wind can take the interpolated value
from the last 2 known values.
Note - the value is interpolated between the last two known values and delayed,
not extrapolated beyond the last known value.  So:

    v(t=0.5) = (v(t=-1.0) + v(t=0.0)) / 2

## Documentation

This model is independently documented in the docs directory.

## Verification

This model contains independent verification test cases in the verif directory.
