# Information

<p align="center">
    <table>
        <tr>
            <th colspan=5>Model Information</th>
        </tr>
        <tr>
            <td>Rating</td>
            <td colspan=4 align=center>~maturity-2</td>
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

Parachutes

## Documentation

This model has no documentation.

## Verification

This model originates from the Orion GN&C simulation, Ramtares, and contains limited independent verification test cases in the following verif directories **not included here but included on the cev side of the Ramtares repo**, not available to the public:

* `antares/cev/models/fhw/effectors/chute/verif_drg`
* `antares/cev/models/fhw/effectors/chute/verif_main`

**Note that these verif sims are not co-located with the CML chute model because they use CEV-specific data as part of the verification.**

## Known Issues

The construction of class members is not comprehensive in setting member
variables to consistent / functional values. The only known implementation of
this model is in ANTARES and this model relies on the ANTARES-specific
implementation to set those values.
