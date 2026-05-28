# Information

<p align="center">
    <table>
        <tr>
            <th colspan=5>Model Information</th>
        </tr>
        <tr>
            <td>Rating</td>
            <td colspan=4 align=center>~maturity-3</td>
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

This model is an adaptation of the WGS84-to-EGM2008 model provided by NGA and
accessed from:
https://earth-info.nga.mil/GandG/wgs84/gravitymod/egm2008/egm08\_wgs84.html
on February 11, 2021.

It is being used to generate a geoid from the WGS-84 ellipsoid for comparing
against the MSL geoid defined in cml/models/environment/msl/src/msl\_alt\_dd.cc

The NGA model
  gridget\_2p5min.f
extracts the data from the binary file
  Und\_min2.5x2.5\_egm2008\_isw=82\_WGS84\_TideFree\_SE
and provides a grid output at 2.5' resolution across the entire globe.
This is much finer resolution than we need, so the code was modified in file
  gridget\_1deg.f
to output the data on a grid of 1-degree resolution, and those data are
integrated into the EGM-2008 model.

The data are further modified because the output provides 360 longitude values for
{0,1,...,359} degrees.  We add the first value to the end to support
interpolation between 359 and 360 degrees and to support the wrap-around
of the longitude table.

Because the binary is so large, it is not being committed, but is available
from the website identified above.
The modified and original gridget files are being committed to the repo.


## Documentation

This model is not documented.

## Verification

This model contains independent verification test cases in the verif directory.
