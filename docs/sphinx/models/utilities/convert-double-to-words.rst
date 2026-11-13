Convert Double Precision Data to Unsigned Integer Words
+++++++++++++++++++++++++++++++++++++++++++++++++++++++

.. list-table:: Revision History
   :widths: 15 30 30 50
   :header-rows: 1

   * - Version
     - Date
     - Author
     - Purpose
   * - 1
     - April 2020
     - Brent Caughron
     - Initial version
   * - 1.1
     - June 2021
     - Tony Varesic
     - Added ``bit_size`` test information
   * - 2
     - September 2026
     - Nino Tarantino
     - Converted to reStructuredText and major overhaul of content

.. contents:: Table of Contents
   :local:
   :class: this-will-duplicate-information-and-it-is-still-useful-here

________________________________________________________

.. _introduction:

Introduction
============

This model provides several interfaces for converting a double-precision value to a vector of unsigned integer words.
The bit-resolution (i.e. significance of each bit), number of bits per word, and number of words to use are all
user-configurable.

Consider a simple case, where a double-precision value must be stored in a 4-bit register with each bit representing
a resolution of 0.1. This is a common case for flight software telemetry, which tends to pack floating point numbers into
smaller buffers of known resolution per bit due to limited bandwidth.

.. graphviz::

   digraph register {
       node [shape=plaintext];
       reg [label=<
         <TABLE BORDER="0" CELLBORDER="1" CELLSPACING="0" CELLPADDING="0">
           <TR>
             <TD BORDER="0"><FONT POINT-SIZE="11"><B>Most Significant Bit</B></FONT></TD>
             <TD BORDER="0"></TD><TD BORDER="0"></TD>
             <TD BORDER="0"><FONT POINT-SIZE="11"><B>Least Significant Bit</B></FONT></TD>
           </TR>
           <TR>
             <TD FIXEDSIZE="TRUE" WIDTH="150" HEIGHT="60">
               <TABLE BORDER="0" CELLBORDER="0" CELLSPACING="0" CELLPADDING="6">
                 <TR><TD SIDES="B"><B>Bit 3</B></TD></TR>
                 <TR><TD>0.1 &#215; 2&#179; = 0.8</TD></TR>
               </TABLE>
             </TD>
             <TD FIXEDSIZE="TRUE" WIDTH="150" HEIGHT="60">
               <TABLE BORDER="0" CELLBORDER="0" CELLSPACING="0" CELLPADDING="6">
                 <TR><TD SIDES="B"><B>Bit 2</B></TD></TR>
                 <TR><TD>0.1 &#215; 2&#178; = 0.4</TD></TR>
               </TABLE>
             </TD>
             <TD FIXEDSIZE="TRUE" WIDTH="150" HEIGHT="60">
               <TABLE BORDER="0" CELLBORDER="0" CELLSPACING="0" CELLPADDING="6">
                 <TR><TD SIDES="B"><B>Bit 1</B></TD></TR>
                 <TR><TD>0.1 &#215; 2&#185; = 0.2</TD></TR>
               </TABLE>
             </TD>
             <TD FIXEDSIZE="TRUE" WIDTH="150" HEIGHT="60">
               <TABLE BORDER="0" CELLBORDER="0" CELLSPACING="0" CELLPADDING="6">
                 <TR><TD SIDES="B"><B>Bit 0</B></TD></TR>
                 <TR><TD>0.1 &#215; 2&#8304; = 0.1</TD></TR>
               </TABLE>
             </TD>
           </TR>
           <TR>
             <TD COLSPAN="4" BORDER="0">
               <FONT POINT-SIZE="11"><I>4-bit register &#8212; resolution 0.1 per LSB (full scale = 1.5)</I></FONT>
             </TD>
           </TR>
         </TABLE>
       >];
   }

In this case, the value 0.9 would be stored as:

.. graphviz::

   digraph value_0_9 {
       node [shape=plaintext];
       reg [label=<
         <TABLE BORDER="0" CELLBORDER="1" CELLSPACING="0" CELLPADDING="0">
           <TR>
             <TD FIXEDSIZE="TRUE" WIDTH="150" HEIGHT="60">
               <TABLE BORDER="0" CELLBORDER="0" CELLSPACING="0" CELLPADDING="6">
                 <TR><TD SIDES="B"><B>Bit 3</B></TD></TR>
                 <TR><TD><B>1</B></TD></TR>
               </TABLE>
             </TD>
             <TD FIXEDSIZE="TRUE" WIDTH="150" HEIGHT="60">
               <TABLE BORDER="0" CELLBORDER="0" CELLSPACING="0" CELLPADDING="6">
                 <TR><TD SIDES="B"><B>Bit 2</B></TD></TR>
                 <TR><TD><B>0</B></TD></TR>
               </TABLE>
             </TD>
             <TD FIXEDSIZE="TRUE" WIDTH="150" HEIGHT="60">
               <TABLE BORDER="0" CELLBORDER="0" CELLSPACING="0" CELLPADDING="6">
                 <TR><TD SIDES="B"><B>Bit 1</B></TD></TR>
                 <TR><TD><B>0</B></TD></TR>
               </TABLE>
             </TD>
             <TD FIXEDSIZE="TRUE" WIDTH="150" HEIGHT="60">
               <TABLE BORDER="0" CELLBORDER="0" CELLSPACING="0" CELLPADDING="6">
                 <TR><TD SIDES="B"><B>Bit 0</B></TD></TR>
                 <TR><TD><B>1</B></TD></TR>
               </TABLE>
             </TD>
           </TR>
           <TR>
             <TD COLSPAN="4" BORDER="0">
               <FONT POINT-SIZE="11"><I>0.9 stored as 1001 &#8212; (0.8 + 0.1 = 0.9)</I></FONT>
             </TD>
           </TR>
         </TABLE>
       >];
   }

This model extends this concept to allow for a user-defined word count and bits per word. Consider the case of a 4-word
register with 4 bits per word and a resolution of 0.001 bits per word.

.. graphviz::

   digraph word_layout {
       node [shape=plaintext];
       reg [label=<
         <TABLE BORDER="0" CELLBORDER="1" CELLSPACING="0" CELLPADDING="0">

           <!-- Most/Least-Significant Word row -->
           <TR>
             <TD BORDER="0"><FONT POINT-SIZE="11"><B>Most-Significant Word</B></FONT></TD>
             <TD BORDER="0"></TD><TD BORDER="0"></TD>
             <TD BORDER="0"><FONT POINT-SIZE="11"><B>Least-Significant Word</B></FONT></TD>
           </TR>

           <!-- The 4 word cells -->
           <TR>
             <TD FIXEDSIZE="TRUE" WIDTH="190" HEIGHT="60">
               <TABLE BORDER="0" CELLBORDER="0" CELLSPACING="0" CELLPADDING="6">
                 <TR><TD SIDES="B"><B>Word 3</B></TD></TR>
                 <TR><TD>0.001 &#215; 16&#179; = 4.096</TD></TR>
               </TABLE>
             </TD>
             <TD FIXEDSIZE="TRUE" WIDTH="190" HEIGHT="60">
               <TABLE BORDER="0" CELLBORDER="0" CELLSPACING="0" CELLPADDING="6">
                 <TR><TD SIDES="B"><B>Word 2</B></TD></TR>
                 <TR><TD>0.001 &#215; 16&#178; = 0.256</TD></TR>
               </TABLE>
             </TD>
             <TD FIXEDSIZE="TRUE" WIDTH="190" HEIGHT="60">
               <TABLE BORDER="0" CELLBORDER="0" CELLSPACING="0" CELLPADDING="6">
                 <TR><TD SIDES="B"><B>Word 1</B></TD></TR>
                 <TR><TD>0.001 &#215; 16&#185; = 0.016</TD></TR>
               </TABLE>
             </TD>
             <TD FIXEDSIZE="TRUE" WIDTH="190" HEIGHT="60">
               <TABLE BORDER="0" CELLBORDER="0" CELLSPACING="0" CELLPADDING="6">
                 <TR><TD SIDES="B"><B>Word 0</B></TD></TR>
                 <TR><TD>0.001 &#215; 16&#8304; = 0.001</TD></TR>
               </TABLE>
             </TD>
           </TR>

           <!-- Caption row spanning all 4 cells -->
           <TR>
             <TD COLSPAN="4" BORDER="0">
               <FONT POINT-SIZE="11"><I>4-word register &#8212; 4 bits per word, resolution 0.001 (full scale = 65.535)</I></FONT>
             </TD>
           </TR>

         </TABLE>
       >];
   }

In this case, the value 5.001 would be stored as:

.. graphviz::

   digraph value_5_001 {
       node [shape=plaintext];
       reg [label=<
         <TABLE BORDER="0" CELLBORDER="1" CELLSPACING="0" CELLPADDING="0">

           <!-- The 4 word cells -->
           <TR>
             <TD FIXEDSIZE="TRUE" WIDTH="190" HEIGHT="60">
               <TABLE BORDER="0" CELLBORDER="0" CELLSPACING="0" CELLPADDING="6">
                 <TR><TD SIDES="B"><B>Word 3</B></TD></TR>
                 <TR><TD><B>1</B></TD></TR>
               </TABLE>
             </TD>
             <TD FIXEDSIZE="TRUE" WIDTH="190" HEIGHT="60">
               <TABLE BORDER="0" CELLBORDER="0" CELLSPACING="0" CELLPADDING="6">
                 <TR><TD SIDES="B"><B>Word 2</B></TD></TR>
                 <TR><TD><B>3</B></TD></TR>
               </TABLE>
             </TD>
             <TD FIXEDSIZE="TRUE" WIDTH="190" HEIGHT="60">
               <TABLE BORDER="0" CELLBORDER="0" CELLSPACING="0" CELLPADDING="6">
                 <TR><TD SIDES="B"><B>Word 1</B></TD></TR>
                 <TR><TD><B>8</B></TD></TR>
               </TABLE>
             </TD>
             <TD FIXEDSIZE="TRUE" WIDTH="190" HEIGHT="60">
               <TABLE BORDER="0" CELLBORDER="0" CELLSPACING="0" CELLPADDING="6">
                 <TR><TD SIDES="B"><B>Word 0</B></TD></TR>
                 <TR><TD><B>9</B></TD></TR>
               </TABLE>
             </TD>
           </TR>

           <!-- Caption row spanning all 4 cells -->
           <TR>
             <TD COLSPAN="4" BORDER="0">
               <FONT POINT-SIZE="11"><I>5.001 stored as [1, 3, 8, 9] &#8212; (1&#215;4.096 + 3&#215;0.256 + 8&#215;0.016 + 9&#215;0.001 = 5.001)</I></FONT>
             </TD>
           </TR>

         </TABLE>
       >];
   }


________________________________________________________

Requirements
============

In addition to the value being converted into words, three user-specified inputs must be passed into the model:

1. Resolution (i.e. significance of the least significant bit)
2. Bit-size of each word (e.g. 8-bit, 32-bit)
3. Number of words available to represent the double

________________________________________________________

Model Specifications
====================

Model Structure
~~~~~~~~~~~~~~~

This model is usable as:

- Free functions, useful for one-off conversions.
- An instantiated class, :cpp:class:`ConvertDoubleToUintWords`. Useful for when a conversion is needed as a simulation
  scheduled job.

.. doxygenfile:: convert_double_to_words.hh

Mathematical Formulation
------------------------

See the :ref:`Introduction <introduction>` for a graphical representation of the conversion logic.

________________________________________________________

User's Guide
============

.. important::

    Regardless of the API chosen, data is returned as an array of words with word 0 corresponding to the
    *least-significant word*.

Free Function API
~~~~~~~~~~~~~~~~~

For one-off conversions, one of the free functions may be used. If possible, you should use the version which accepts
an ``std::vector``.

The following demonstrates how to convert the value 5.001 to a 4-word register with 4 bits per word and a resolution of
0.001 bits per word.

.. code-block:: cpp

    const double value = 5.001;
    const unsigned int num_words = 4U;
    const unsigned int bits_per_word = 4U;
    const double resolution = 0.001;

    /* Vector version */
    std::vector<uint32_t> words(num_words);
    convert_double_to_uint_words(value, resolution, words, bits_per_word);
    // words stores [9 8 3 1]

    /* C-style array version */
    uint32_t words[num_words];
    convert_double_to_uint_words(value, resolution, words, std::size(words), bits_per_word);
    // words stores [9 8 3 1]


Class-Based API
~~~~~~~~~~~~~~~

For repeated conversions, it is recommended to create a long-lived instance of the :cpp:class:`ConvertDoubleToUintWords`
class and repeatedly use its :cpp:func:`~ConvertDoubleToUintWords::update` method. The resulting words are stored in its
:cpp:member:`~ConvertDoubleToUintWords::words` member.

The same example shown for the free function APIs is repeated below with the class API.

.. code-block:: cpp

    double value = 5.001;
    const unsigned int num_words = 4U;
    const unsigned int bits_per_word = 4U;
    const double resolution = 0.001;

    ConvertDoubleToUintWords converter(value, resolution, num_words, bits_per_word);

    // converter.words stores [9 8 3 1].
    converter.update();

    // The value has changed. Now converter.words stores [0 0 0 0].
    value = 0.0;
    converter.update();

Extension
~~~~~~~~~

N/A

________________________________________________________

Verification
============

Code Coverage
~~~~~~~~~~~~~

.. code-block:: text

    ------------------------------------------------------------------------------
                               GCC Code Coverage Report
    Directory: .
    ------------------------------------------------------------------------------
    File                                       Lines     Exec  Cover   Missing
    ------------------------------------------------------------------------------
    src/convert_double_to_uint_words.cc
                                                  70       70   100%
    ------------------------------------------------------------------------------
    TOTAL                                         70       70   100%
    -----------------------------------------------------------------------------

See detailed coverage information `here <https://coveralls.io/github/nasa/cml?branch=main>`_.

Exceptions
~~~~~~~~~~

N/A

Unit-Test Cases
~~~~~~~~~~~~~~~

This model uses GoogleTest to test its functionality. Since the model has three different APIs, each one is tested with
the same conditions to ensure that each interface produces the same output.

ConvertDoubleToUintWords.InvalidConfiguration
---------------------------------------------

This test ensures that invalid configuration options or invalid conversion values are caught and handled properly. The
following cases are checked:

.. list-table::
   :widths: 50 50 20
   :header-rows: 1

   * - Case
     - Expected Resolution
     - Result
   * - ``nullptr`` passed to :cpp:func:`convert_double_to_uint_words`
     - Error message emitted
     - Pass
   * - A word count of 0 is specified
     - Error message emitted and output array of words is of size 0
     - Pass
   * - A bit size of 64 is requested, which is larger than the maximum allowable 32
     - Error message emitted
     - Pass
   * - The value to convert is too large to be representable given the specified number of words, bits, and resolution
     - Error message emitted and all bits set to 1
     - Pass
   * - The value to convert is too small to be representable given the specified bit resolution
     - Error message emitted and all bits set to 0
     - Pass
   * - The value to convert has a specified resolution that is smaller than the distance to the next value that can be represented as a double
     - Error message emitted
     - Pass

ConvertDoubleToUintWords.NoRoundoff
-----------------------------------

A variety of cases are run where no round-off correction is required: i.e., either the value is convertible to bits with
no remainder or the remainder is less than half the bit resolution, in which case the remainder is lost due to rounding
down.

For the following cases, all three APIs are used for each case and the expected value is checked against the output from
each. Expected values are described in detail in the test source file, so are repeated here with no explanation.

.. list-table::
   :widths: 20 20 20 50 20
   :header-rows: 1

   * - Value
     - Resolution
     - Bits Per Word
     - Expected Words (Least to Most Significant)
     - Result
   * - 0.001
     - 0.001
     - 4
     - :math:`\begin{pmatrix} 1 & 0 & 0 & 0 \end{pmatrix}`
     - Pass
   * - 5.002
     - 0.001
     - 4
     - :math:`\begin{pmatrix} 10 & 8 & 3 & 1 \end{pmatrix}`
     - Pass
   * - 5.001
     - 0.001
     - 4
     - :math:`\begin{pmatrix} 9 & 8 & 3 & 1 \end{pmatrix}`
     - Pass
   * - 123456.0
     - 1.0
     - 16
     - :math:`\begin{pmatrix} 57920 & 1 & 0 & 0 \end{pmatrix}`
     - Pass
   * - 123457.0
     - 1.0
     - 16
     - :math:`\begin{pmatrix} 57921 & 1 & 0 & 0 \end{pmatrix}`
     - Pass
   * - 123458.0
     - 1.0
     - 16
     - :math:`\begin{pmatrix} 57922 & 1 & 0 & 0 \end{pmatrix}`
     - Pass
   * - 900987654321.181
     - 0.001
     - 16
     - :math:`\begin{pmatrix} 13341 & 35093 & 13169 & 3 \end{pmatrix}`
     - Pass

Per-case decomposition
^^^^^^^^^^^^^^^^^^^^^^

.. dropdown:: Case 1 — 0.001, resolution 0.001

   .. list-table::
      :header-rows: 1
      :widths: 8 40 10 22

      * - Word
        - Significance
        - Value
        - Remainder
      * - 3
        - :math:`0.001 \times (2^4)^3 = 4.096`
        - 0
        - 0.001
      * - 2
        - :math:`0.001 \times (2^4)^2 = 0.256`
        - 0
        - 0.001
      * - 1
        - :math:`0.001 \times (2^4)^1 = 0.016`
        - 0
        - 0.001
      * - 0
        - :math:`0.001 \times (2^4)^0 = 0.001`
        - 1
        - 0.000

   .. math::

      0 \times 4.096 + 0 \times 0.256 + 0 \times 0.016 + 1 \times 0.001 = 0.001

.. dropdown:: Case 2 — 5.002, resolution 0.001

   .. list-table::
      :header-rows: 1
      :widths: 8 40 10 22

      * - Word
        - Significance
        - Value
        - Remainder
      * - 3
        - :math:`0.001 \times (2^4)^3 = 4.096`
        - 1
        - 0.906
      * - 2
        - :math:`0.001 \times (2^4)^2 = 0.256`
        - 3
        - 0.138
      * - 1
        - :math:`0.001 \times (2^4)^1 = 0.016`
        - 8
        - 0.010
      * - 0
        - :math:`0.001 \times (2^4)^0 = 0.001`
        - 10
        - 0.000

   .. math::

      1 \times 4.096 + 3 \times 0.256 + 8 \times 0.016 + 10 \times 0.001 = 5.002

.. dropdown:: Case 3 — 5.001, resolution 0.001

   .. list-table::
      :header-rows: 1
      :widths: 8 40 10 22

      * - Word
        - Significance
        - Value
        - Remainder
      * - 3
        - :math:`0.001 \times (2^4)^3 = 4.096`
        - 1
        - 0.905
      * - 2
        - :math:`0.001 \times (2^4)^2 = 0.256`
        - 3
        - 0.137
      * - 1
        - :math:`0.001 \times (2^4)^1 = 0.016`
        - 8
        - 0.009
      * - 0
        - :math:`0.001 \times (2^4)^0 = 0.001`
        - 9
        - 0.000

   .. math::

      1 \times 4.096 + 3 \times 0.256 + 8 \times 0.016 + 9 \times 0.001 = 5.001

.. dropdown:: Case 4 — 123456.0, resolution 1.0

   .. list-table::
      :header-rows: 1
      :widths: 8 44 12 22

      * - Word
        - Significance
        - Value
        - Remainder
      * - 3
        - :math:`1.0 \times (2^{16})^3 = 281474976710656.0`
        - 0
        - 123456.0
      * - 2
        - :math:`1.0 \times (2^{16})^2 = 4294967296.0`
        - 0
        - 123456.0
      * - 1
        - :math:`1.0 \times (2^{16})^1 = 65536.0`
        - 1
        - 57920.0
      * - 0
        - :math:`1.0 \times (2^{16})^0 = 1.0`
        - 57920
        - 0.0

   .. math::

      0 \times 281474976710656.0 + 0 \times 4294967296.0 + 1 \times 65536.0 + 57920 \times 1.0 = 123456.0

.. dropdown:: Case 5 — 123457.0, resolution 1.0

   .. list-table::
      :header-rows: 1
      :widths: 8 44 12 22

      * - Word
        - Significance
        - Value
        - Remainder
      * - 3
        - :math:`1.0 \times (2^{16})^3 = 281474976710656.0`
        - 0
        - 123457.0
      * - 2
        - :math:`1.0 \times (2^{16})^2 = 4294967296.0`
        - 0
        - 123457.0
      * - 1
        - :math:`1.0 \times (2^{16})^1 = 65536.0`
        - 1
        - 57921.0
      * - 0
        - :math:`1.0 \times (2^{16})^0 = 1.0`
        - 57921
        - 0.0

   .. math::

      0 \times 281474976710656.0 + 0 \times 4294967296.0 + 1 \times 65536.0 + 57921 \times 1.0 = 123457.0

.. dropdown:: Case 6 — 123458.0, resolution 1.0

   .. list-table::
      :header-rows: 1
      :widths: 8 44 12 22

      * - Word
        - Significance
        - Value
        - Remainder
      * - 3
        - :math:`1.0 \times (2^{16})^3 = 281474976710656.0`
        - 0
        - 123458.0
      * - 2
        - :math:`1.0 \times (2^{16})^2 = 4294967296.0`
        - 0
        - 123458.0
      * - 1
        - :math:`1.0 \times (2^{16})^1 = 65536.0`
        - 1
        - 57922.0
      * - 0
        - :math:`1.0 \times (2^{16})^0 = 1.0`
        - 57922
        - 0.0

   .. math::

      0 \times 281474976710656.0 + 0 \times 4294967296.0 + 1 \times 65536.0 + 57922 \times 1.0 = 123458.0

.. dropdown:: Case 7 — 900987654321.181, resolution 0.001

   .. list-table::
      :header-rows: 1
      :widths: 8 44 12 30

      * - Word
        - Significance
        - Value
        - Remainder
      * - 3
        - :math:`0.001 \times (2^{16})^3 = 281474976710.656`
        - 3
        - 56562724189.21301
      * - 2
        - :math:`0.001 \times (2^{16})^2 = 4294967.296`
        - 13169
        - 2299868.18901062
      * - 1
        - :math:`0.001 \times (2^{16})^1 = 65.536`
        - 35093
        - 13.34101061988622
      * - 0
        - :math:`0.001 \times (2^{16})^0 = 0.001`
        - 13341
        - 1.061988621842147e-05

   .. math::

      3 \times 281474976710.656 + 13169 \times 4294967.296 + 35093 \times 65.536 + 13341 \times 0.001 = 900987654321.18103

   .. note::

      The final remainder (:math:`1.06 \times 10^{-5}`) is less than half the
      resolution, so no round-off correction is applied.

ConvertDoubleToUintWords.RoundoffCorrection
-------------------------------------------

A variety of cases are run where round-off correction is required: i.e., the value is not exactly convertible to bits
given the resolution and the remainder is greater than half the bit resolution. In that case, the least significant word
will be incremented by 1 (round-up). This has the potential to spill over into the next most significant word if the
round-up results in the least significant word surpassing its maximum value. Cases were designed to exercise the no
spillover, single spillover, and multi-word spillover scenarios.

For the following cases, all three APIs are used for each case and the expected value is checked against the output from
each. Expected values are described in detail in the following subsection.

.. list-table::
   :widths: 20 20 20 50 20
   :header-rows: 1

   * - Value
     - Resolution
     - Bits Per Word
     - Expected Words (Least to Most Significant)
     - Result
   * - 0.003153
     - 0.0001
     - 4
     - :math:`\begin{pmatrix} 0 & 2 & 0 & 0 \end{pmatrix}`
     - Pass
   * - 12287.8
     - 1.0
     - 4
     - :math:`\begin{pmatrix} 0 & 0 & 0 & 3 \end{pmatrix}`
     - Pass
   * - 900987654321.182
     - 0.0001
     - 16
     - :math:`\begin{pmatrix} 13342 & 35093 & 13169 & 3 \end{pmatrix}`
     - Pass

Per-case decomposition
^^^^^^^^^^^^^^^^^^^^^^

.. dropdown:: Case 1 — 0.003153, resolution 0.0001

   .. list-table::
      :header-rows: 1
      :widths: 8 44 12 22

      * - Word
        - Significance
        - Value
        - Remainder
      * - 3
        - :math:`0.0001 \times (2^4)^3 = 0.4096`
        - 0
        - 0.003153
      * - 2
        - :math:`0.0001 \times (2^4)^2 = 0.0256`
        - 0
        - 0.003153
      * - 1
        - :math:`0.0001 \times (2^4)^1 = 0.0016`
        - 1
        - 0.001553
      * - 0
        - :math:`0.0001 \times (2^4)^0 = 0.0001`
        - 15
        - 0.000053

   .. note::

      The final remainder (0.000053) is greater than half the resolution
      (0.00005), so word 0 is incremented from 15 to 16. This overflows the
      4-bit range, spilling over to 0 and incrementing word 1 from 1 to 2.
      Final words: ``[2, 0, 0, 0]`` (MSW-first).

   .. math::

      0 \times 0.4096 + 0 \times 0.0256 + 2 \times 0.0016 + 0 \times 0.0001 = 0.0032

.. dropdown:: Case 2 — 12287.8, resolution 1.0

   .. list-table::
      :header-rows: 1
      :widths: 8 40 12 22

      * - Word
        - Significance
        - Value
        - Remainder
      * - 3
        - :math:`1.0 \times (2^4)^3 = 4096`
        - 2
        - 4095.8
      * - 2
        - :math:`1.0 \times (2^4)^2 = 256`
        - 15
        - 255.8
      * - 1
        - :math:`1.0 \times (2^4)^1 = 16`
        - 15
        - 15.8
      * - 0
        - :math:`1.0 \times (2^4)^0 = 1`
        - 15
        - 0.8

   .. note::

      The final remainder (0.8) is greater than half the resolution (0.5), so
      word 0 is incremented. This starts a chain of spillovers: word 0
      (15 → 16 → 0), word 1 (15 → 16 → 0), and word 2 (15 → 16 → 0) each
      overflow in turn, finally incrementing word 3 from 2 to 3.
      Final words: ``[3, 0, 0, 0]`` (MSW-first).

   .. math::

      3 \times 4096 + 0 \times 256 + 0 \times 16 + 0 \times 1 = 12288

.. dropdown:: Case 3 — 900987654321.182, resolution 0.001

   .. list-table::
      :header-rows: 1
      :widths: 8 44 12 30

      * - Word
        - Significance
        - Value
        - Remainder
      * - 3
        - :math:`0.001 \times (2^{16})^3 = 281474976710.656`
        - 3
        - 56562724189.21399
      * - 2
        - :math:`0.001 \times (2^{16})^2 = 4294967.296`
        - 13169
        - 2299868.1899871826
      * - 1
        - :math:`0.001 \times (2^{16})^1 = 65.536`
        - 35093
        - 13.34198718238622
      * - 0
        - :math:`0.001 \times (2^{16})^0 = 0.001`
        - 13341
        - 0.0009871823862184215

   .. note::

      The final remainder (:math:`\approx 0.000987`) is greater than half the
      resolution (0.0005), so word 0 is incremented from 13341 to 13342. No
      overflow occurs, so no further spillover is needed.
      Final words: ``[3, 13169, 35093, 13342]`` (MSW-first).

   .. math::

      3 \times 281474976710.656 + 13169 \times 4294967.296 + 35093 \times 65.536 + 13342 \times 0.001 = 900987654321.18213
