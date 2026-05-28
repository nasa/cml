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

Provides a subscriptions architecture whereby a model can subscribe and
unsubscribe to those models on which it depends.  A model that is subscribed
will be active.  When all subscriptions are removed, a model can become
inactive.

This allows a sim architecture in which only those models that are needed get
run, and the bookkeeping to determine which are needed is handled by the
top-level models rather than having to micro-manage every sub-model and
identify whether it can be switched off.

The model itself is intended to be used as a base class.  Any model that needs
subscription maintenenace can inherit from this model and pick up the
subscribe() unsubscribe(), active and enabled flags without replicating code.


## Documentation

This model is independently documented in the docs directory.

## Verification

This model contains independent verification test cases in the verif directory.
