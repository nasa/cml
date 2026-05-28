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

Creates a vehicle out of nowhere.  Useful when needing to launch a distributed
set of studies fom a single initial condition; the vehicle can be replicated as
many times as desired with each replica slightly perturbed and all replicas
propagated from that point forward.

Also useful for siuations producing certain multi-body and tethered effects
where the detached body needs mass for its state but the parent body must also
retain that mass for state-propagation and vehicle response to effectors.

## Documentation

This model is independently documented in the docs directory.

## Verification

This model contains independent verification test cases in the verif directory.
