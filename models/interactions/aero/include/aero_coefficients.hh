/*******************************************************************************
PURPOSE:
  (Define function prototype for aero_disperse)

PROGRAMMERS:
  (((Ryan Whitley) (NASA) (April 2006) (ARES) (Initial implementation))
   ((Jeremy Rea) (NASA) (July 2006) (ARES) (RDLaa08984))
   ((Jeremy Rea) (NASA) (July 2006) (ARES) (RDLaa09002))
   ((Jeremy Rea) (NASA) (Jan 2007) (CEV) (RDLaa09504: Implement CEV aerodata subroutine V0.2))
   ((Jeremy Rea) (NASA) (April 2007) (CEV) (RDLaa09782: Implement CEV aerodata subroutine V0.25.0))
   ((Jeremy Rea) (NASA) (Feb 2008) (CEV) (RDLaa10272: Implement CEV aerodata subroutine V0.40.2))
   ((Jeremy Rea) (NASA) (Mar 2008) (CEV) (RDLaa10365: Implement CEV aerodata subroutine V0.41))
   ((Sara Blatz) (NASA) (Mar 2009) (CEV) (Implement CEV aerodata subroutine V0.52.2))
   ((Sara Blatz) (NASA) (June 2009) (CEV) (Implement CEV aerodata subroutine V0.53.1))
   ((Sara McNamara) (NASA) (Oct 2010) (CEV) (Implement CEV aerodata subroutine V0.56))
   ((Brian Bihari) (ESCG) (May 2012) (MPCV) (Implement CEV aerodata subroutine V0.70))
   ((Gary Turner) (OSR) (January 2016) (Antares) (Reorganizing to eliminate redundancy and streamline the system))
   ((Brent Caughron) (OSR) (Dec 2020) (Antares) (Code Review and IV&V)))
*******************************************************************************/

#ifndef CML_AERO_COEFFICIENTS_HH
#define CML_AERO_COEFFICIENTS_HH

/*******************************************************************************
AeroCoefficientsCoreBase
Purpose: (These are the core-values, shared between the Table-lookup, the API,
          and the dispersions thereof.)
*******************************************************************************/
class AeroCoefficientsCoreBase
{
public:
  double CX{0.0};     /* (--) X-body aero force coefficient. */
  double CY{0.0};     /* (--) Y-body aero force coefficient. */
  double CZ{0.0};     /* (--) Z-body aero force coefficient. */
  double CA{0.0};     /* (--) Axial force coefficient. */
  double CN{0.0};     /* (--) Normal force coefficient. */
  double CD{0.0};     /* (--) Drag force coefficient. */
  double CS{0.0};     /* (--) Side force coefficient. */
  double CL{0.0};     /* (--) Drag force coefficient. */
  double CN_sym{0.0}; /* (--) Symmetric normal force coefficient. */
  double CL_sym{0.0}; /* (--) Symmetric lift force coefficient. */
  double Cm_sym{0.0}; /* (--) Symmetric aero pitch coefficient. */
  double dCl_dp{0.0}; /* (--) Aero roll damp wrt body roll rate. */
  double dCm_dq{0.0}; /* (--) Aero pitch damp wrt body pitch rate. */
  double dCn_dr{0.0}; /* (--) Aero yaw damp wrt body yaw rate. */

  AeroCoefficientsCoreBase() = default;
};

/*******************************************************************************
AeroCoefficientsCore
Purpose:(These are the core-values, shared between the Table-lookup and the API.)
*******************************************************************************/
class AeroCoefficientsCore : public AeroCoefficientsCoreBase
{
public:
  double Cl_cg{0.0};  /* (--) Aero roll coefficient about CG. */
  double Cm_cg{0.0};  /* (--) Aero pitch coefficient about CG. */
  double Cn_cg{0.0};  /* (--) Aero yaw coefficient about CG. */
  double Cl_mrc{0.0}; /* (--) Aero roll coefficient about MRC. */
  double Cm_mrc{0.0}; /* (--) Aero pitch coefficient about MRC. */
  double Cn_mrc{0.0}; /* (--) Aero yaw coefficient about MRC. */

  AeroCoefficientsCore() = default;
};


/*****************************************************************************
AeroCoefficientsOffDiagExtension
Purpose:(Additional values. Together with the core.)
*****************************************************************************/
class AeroCoefficientsOffDiagExtension
{
public:
  double dCl_dq{0.0}; /* (--) Aero roll damp wrt body pitch rate. */
  double dCl_dr{0.0}; /* (--) Aero roll damp wrt body yaw rate. */
  double dCm_dp{0.0}; /* (--) Aero pitch damp wrt body roll rate. */
  double dCm_dr{0.0}; /* (--) Aero pitch damp wrt body yaw rate. */
  double dCn_dp{0.0}; /* (--) Aero yaw damp wrt body roll rate. */
  double dCn_dq{0.0}; /* (--) Aero yaw damp wrt body pitch rate. */

  AeroCoefficientsOffDiagExtension() = default;
};

/*******************************************************************************
AeroCoefficientsTable
Purpose:(This is the full set of aero coefficients , with the intent of being
         common between the table-lookup capability and the API model.
         Comprises the core-set with the diagonal extension.)
Clarification:
        (Could have inherited from AeroCoefficientsCore only and added
         the elements in the diagonal extension as struct elements here;
         this is the only place those diagonal extension values get used.
         Architectural decision made here to put the diagonal extension in
         its own struct and inherit it here because it is unclear whether
         those diagonal elements will be needed by other systems in the future.)
*******************************************************************************/
class AeroCoefficientsTable : public AeroCoefficientsCore,
                              public AeroCoefficientsOffDiagExtension
{
public:
  AeroCoefficientsTable() = default;
};

/*******************************************************************************
AeroCoefficientsDisp
Purpose: (These are the core-values for the dispersions.)
*******************************************************************************/
class AeroCoefficientsDisp : public AeroCoefficientsCoreBase,
                             public AeroCoefficientsOffDiagExtension
{
public:
  double Cl{0.0}; /* (--) Common roll coeff used for dispersions;
                          this is applied directly to Cl_mrc, and indirectly
                          to Cl_cg when it is computed from the dispersed value
                          of Cl_mrc.*/
  double Cm{0.0}; /* (--) Common pitch coeff used for dispersions
                          this is applied directly to Cm_mrc, and indirectly
                          to Cm_cg when it is computed from the dispersed value
                          of Cm_mrc.*/
  double Cn{0.0}; /* (--) Common yaw coeff used for dispersions
                          this is applied directly to Cn_mrc, and indirectly
                          to Cn_cg when it is computed from the dispersed value
                          of Cn_mrc.*/

  AeroCoefficientsDisp() = default;
};
#endif