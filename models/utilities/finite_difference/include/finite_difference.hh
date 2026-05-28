/*******************************************************************************
Purpose:
   (Estimate derivatives using finite differences.)

Assumptions and Limitations:
   ((These classes compute derivatives using second-order backwards difference
     methods.  These methods require several function evaluations in order to
     generate a derivative.  When the object cannot yet compute a derivative,
     its compute method returns false and sets the derivative argument to 0.0.
     Once it has enough data, the compute method returns true and sets the
     derivative argument to the computed derivative.)
    (The compute methods of these objects should be called as scheduled jobs,
     not as derivative jobs.)
    (There is no protection for calling compute twice with the same time
     input.))

Library Dependency:
   ((../src/finite_difference.cc))

Programmers:
   (((Daniel Matz) (NASA) (July 2017) (Initial version)))
*******************************************************************************/

#ifndef FINITE_DIFFERENCE_HH
#define FINITE_DIFFERENCE_HH

// Tell SWIG to treat an argument with signature "double &derivative" as an
// output.  That is, when called from Python, the compute function will take two
// arguments (value and time), and will return two values (the bool and
// derivative).
#ifdef SWIG
%apply double *OUTPUT {double &derivative};
#endif

/****************************************************************************
ScalarFirstDerivative
Purpose: Estimate the first derivative of a value using backwards difference
****************************************************************************/
class ScalarFirstDerivative {

    public:

        ScalarFirstDerivative();

        bool compute(const double value,
                     const double time,
                     double &derivative);

    private:

        int pass_count; /* (--) Number of times compute() has been called */
        double value_im1; /* (--) Value at i minus 1 */
        double time_im1; /* (--) Time at i minus 1 */
        double value_im2; /* (--) Value at i minus 2 */
        double time_im2; /* (--) Time at i minus 2 */
};

/****************************************************************************
Vector3FirstDerivative
Purpose: Estimate the first derivative of a vector using backwards difference
****************************************************************************/
class Vector3FirstDerivative {

    public:

        bool compute(const double (&value)[3],
                     const double time,
                     double (&derivative)[3]);

    private:

        ScalarFirstDerivative scalar_first_derivative[3]; /* (--) 
            container for 3 instances of ScalarFirstDerivative */
};

/****************************************************************************
Matrix3x3FirstDerivative
Purpose: Estimate the first derivative of a matrix using backwards difference
****************************************************************************/
class Matrix3x3FirstDerivative {

    public:

        bool compute(const double (&value)[3][3],
                     const double time,
                     double (&derivative)[3][3]);

    private:

        ScalarFirstDerivative scalar_first_derivative[3][3]; /* (--)
            container for 9 instances of ScalarFirstDerivative */
};

/****************************************************************************
ScalarSecondDerivative
Purpose: Estimate the second derivative of a value using backwards difference
****************************************************************************/
class ScalarSecondDerivative {

    public:

        ScalarSecondDerivative();

        bool compute(const double value,
                     const double time,
                     double &derivative);

    private:

        int pass_count; /* (--) Number of times compute() has been called */
        double value_im1; /* (--) Value at i minus 1 */
        double time_im1; /* (--) Time at i minus 1 */
        double value_im2; /* (--) Value at i minus 2 */
        double time_im2; /* (--) Time at i minus 2 */
        double value_im3; /* (--) Value at i minus 3 */
        double time_im3; /* (--) Time at i minus 3 */
};

/*****************************************************************************
Vector3SecondDerivative
Purpose: Estimate the second derivative of a vector using backwards difference
*****************************************************************************/
class Vector3SecondDerivative {

    public:

        bool compute(const double (&value)[3],
                     const double time,
                     double (&derivative)[3]);

    private:

        ScalarSecondDerivative scalar_second_derivative[3]; /* (--)
            Container for 3 instances of ScalarSecondDerivative */
};

#ifdef SWIG
%clear double &derivative;
#endif

#endif
