/*******************************************************************************
Purpose:
   (Estimate derivatives using finite differences.)

Programmers:
   (((Daniel Matz) (NASA) (July 2017) (Initial version))
    ((Tony Varesic) (OSR) (Apr 2021) (added divide protection)))
*******************************************************************************/

#include "../include/finite_difference.hh"
#include "cml/models/utilities/math_utils/include/math_utils.hh" // for divide_protected()

/******************************************************************************
ScalarFirstDerivative Constructor
******************************************************************************/
ScalarFirstDerivative::ScalarFirstDerivative()
    :
    pass_count(0),
    value_im1(0.0),
    time_im1(0.0),
    value_im2(0.0),
    time_im2(0.0)
{
}

/******************************************************************************
ScalarFirstDerivative::compute
Purpose:Estimate first derivative of a value using backwards difference
******************************************************************************/
bool ScalarFirstDerivative::compute(
    const double value,
    const double time,
    double &derivative)
{
    bool is_good;
    if (pass_count < 2) {
        pass_count++;
        is_good = false;
        derivative = 0.0;
    } else {
        is_good = true;
        const double dt1 = time - time_im1;
        const double dt2 = time - time_im2;
        derivative =   MathUtils::divide_protected(dt1 + dt2 , dt1 * dt2, 0.0, true) * value
                     + MathUtils::divide_protected(dt2 , dt1 * (dt1 - dt2), 0.0, true) * value_im1
                     - MathUtils::divide_protected(dt1 , dt2 * (dt1 - dt2), 0.0, true) * value_im2;

    }
    value_im2 = value_im1;
    time_im2 = time_im1;
    value_im1 = value;
    time_im1 = time;
    return is_good;
}

/******************************************************************************
Vector3FirstDerivative::compute
Purpose:Estimate first derivative of a vector using backwards difference
******************************************************************************/
bool Vector3FirstDerivative::compute(
    const double (&value)[3],
    const double time,
    double (&derivative)[3])
{
    bool is_good = true;
    for (int i = 0; i < 3; i++) {
        bool status = scalar_first_derivative[i].compute(value[i], time, derivative[i]);
        is_good = is_good and status;
    }
    return is_good;
}

/******************************************************************************
Matrix3x3FirstDerivative::compute
Purpose:Estimate first derivative of a matrix using backwards difference
******************************************************************************/
bool Matrix3x3FirstDerivative::compute(
    const double (&value)[3][3],
    const double time,
    double (&derivative)[3][3])
{
    bool is_good = true;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            bool status = scalar_first_derivative[i][j].compute(value[i][j], time, derivative[i][j]);
            is_good = is_good and status;
        }
    }
    return is_good;
}

/******************************************************************************
ScalarSecondDerivative Constructor
******************************************************************************/
ScalarSecondDerivative::ScalarSecondDerivative()
    :
    pass_count(0),
    value_im1(0.0),
    time_im1(0.0),
    value_im2(0.0),
    time_im2(0.0),
    value_im3(0.0),
    time_im3(0.0)
{
}

/******************************************************************************
ScalarSecondDerivative::compute
Purpose:Estimate second derivative of a value using backwards difference
******************************************************************************/
bool ScalarSecondDerivative::compute(
    const double value,
    const double time,
    double &derivative)
{
    bool is_good;
    if (pass_count < 3) {
        pass_count++;
        is_good = false;
        derivative = 0.0;
    } else {
        is_good = true;
        const double dt1 = time - time_im1;
        const double dt2 = time - time_im2;
        const double dt3 = time - time_im3;
        derivative =   2 * MathUtils::divide_protected(dt2 + dt3 , dt1 * dt2 * (dt3 - dt1), 0.0, true) * value
                     - 2 * MathUtils::divide_protected(dt1 + dt2 , dt2 * dt3 * (dt3 - dt1), 0.0, true) * value
                     + 2 * MathUtils::divide_protected(dt2 + dt3 , dt1 * (dt1 - dt2) * (dt3 - dt1), 0.0, true) * value_im1
                     + 2 * MathUtils::divide_protected(dt1 + dt3 , dt2 * (dt1 - dt2) * (dt2 - dt3), 0.0, true) * value_im2
                     + 2 * MathUtils::divide_protected(dt1 + dt2 , dt3 * (dt2 - dt3) * (dt3 - dt1), 0.0, true) * value_im3;
    }
    value_im3 = value_im2;
    time_im3 = time_im2;
    value_im2 = value_im1;
    time_im2 = time_im1;
    value_im1 = value;
    time_im1 = time;
    return is_good;
}

/******************************************************************************
Vector3SecondDerivative::compute
Purpose:Estimate second derivative of a vector using backwards difference
******************************************************************************/
bool Vector3SecondDerivative::compute(
    const double (&value)[3],
    const double time,
    double (&derivative)[3])
{
    bool is_good = true;
    for (int i = 0; i < 3; i++) {
        bool status = scalar_second_derivative[i].compute(value[i], time, derivative[i]);
        is_good = is_good and status;
    }

    return is_good;
}

