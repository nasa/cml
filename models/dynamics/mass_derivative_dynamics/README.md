# Information

<p align="center">
    <table>
        <tr>
            <th colspan=5>Model Information</th>
        </tr>
        <tr>
            <td>Rating</td>
            <td colspan=4 align=center>~maturity-5</td>
        </tr>
        <tr>
            <td>POC</td>
            <td>Gary Turner</td>
            <td>gary.w.turner@nasa.gov</td>
            <td>281.244.0668</td>
            <td>~ANTARES</td>
        </tr>
    </table>
</p>

## Synopsis

Provides an approximation to the effects of changing mass within an integration
activity. State integrations proceed by using the mass and applied forces to
generate the accelerations, then the accelerations aare integrated.
However, if the mass is changing significantly, the mass used for each acceleration
will not be constant over an integration step. This cannot be easily avoided, but
this model generates a pseudo-force that approximates the value a force
would have to produce the same effect as the changing mass.
## Documentation

This model is independently documented in the docs directory.

## Verification

This model contains independent verification test cases in the verif directory.
