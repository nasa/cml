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
  double CX;     /* (--) X-body aero force coefficient. */
  double CY;     /* (--) Y-body aero force coefficient. */
  double CZ;     /* (--) Z-body aero force coefficient. */
  double CA;     /* (--) Axial force coefficient. */
  double CN;     /* (--) Normal force coefficient. */
  double CD;     /* (--) Drag force coefficient. */
  double CS;     /* (--) Side force coefficient. */
  double CL;     /* (--) Drag force coefficient. */
  double CN_sym; /* (--) Symmetric normal force coefficient. */
  double CL_sym; /* (--) Symmetric lift force coefficient. */
  double Cm_sym; /* (--) Symmetric aero pitch coefficient. */
  double dCl_dp; /* (--) Aero roll damp wrt body roll rate. */
  double dCm_dq; /* (--) Aero pitch damp wrt body pitch rate. */
  double dCn_dr; /* (--) Aero yaw damp wrt body yaw rate. */

  AeroCoefficientsCoreBase()
    :
    CX(0.0), CY(0.0), CZ(0.0),
    CA(0.0), CN(0.0), CD(0.0),
    CS(0.0), CL(0.0),
    CN_sym(0.0), CL_sym(0.0), Cm_sym(0.0),
    dCl_dp(0.0), dCm_dq(0.0), dCn_dr(0.0)
  { };
};

/*******************************************************************************
AeroCoefficientsCore
Purpose:(These are the core-values, shared between the Table-lookup and the API.)
*******************************************************************************/
class AeroCoefficientsCore : public AeroCoefficientsCoreBase
{
public:
  double Cl_cg;  /* (--) Aero roll coefficient about CG. */
  double Cm_cg;  /* (--) Aero pitch coefficient about CG. */
  double Cn_cg;  /* (--) Aero yaw coefficient about CG. */
  double Cl_mrc; /* (--) Aero roll coefficient about MRC. */
  double Cm_mrc; /* (--) Aero pitch coefficient about MRC. */
  double Cn_mrc; /* (--) Aero yaw coefficient about MRC. */

  AeroCoefficientsCore()
    :
    AeroCoefficientsCoreBase(),
    Cl_cg(0.0),  Cm_cg(0.0),  Cn_cg(0.0),
    Cl_mrc(0.0), Cm_mrc(0.0), Cn_mrc(0.0)
  {};
};


/*****************************************************************************
AeroCoefficientsOffDiagExtension
Purpose:(Additional values. Together with the core.)
*****************************************************************************/
class AeroCoefficientsOffDiagExtension
{
public:
  double dCl_dq; /* (--) Aero roll damp wrt body pitch rate. */
  double dCl_dr; /* (--) Aero roll damp wrt body yaw rate. */
  double dCm_dp; /* (--) Aero pitch damp wrt body roll rate. */
  double dCm_dr; /* (--) Aero pitch damp wrt body yaw rate. */
  double dCn_dp; /* (--) Aero yaw damp wrt body roll rate. */
  double dCn_dq; /* (--) Aero yaw damp wrt body pitch rate. */

  AeroCoefficientsOffDiagExtension()
    :
    dCl_dq(0.0), dCl_dr(0.0), dCm_dp(0.0),
    dCm_dr(0.0), dCn_dp(0.0), dCn_dq(0.0)
  {};
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
  AeroCoefficientsTable()
    :
    AeroCoefficientsCore(),
    AeroCoefficientsOffDiagExtension()
    {};
};

/*******************************************************************************
AeroCoefficientsDisp
Purpose: (These are the core-values for the dispersions.)
*******************************************************************************/
class AeroCoefficientsDisp : public AeroCoefficientsCoreBase,
                             public AeroCoefficientsOffDiagExtension
{
public:
  double Cl; /* (--) Common roll coeff used for dispersions;
                     this is applied directly to Cl_mrc, and indirectly
                     to Cl_cg when it is computed from the dispersed value
                     of Cl_mrc.*/
  double Cm; /* (--) Common pitch coeff used for dispersions
                     this is applied directly to Cm_mrc, and indirectly
                     to Cm_cg when it is computed from the dispersed value
                     of Cm_mrc.*/
  double Cn; /* (--) Common yaw coeff used for dispersions
                     this is applied directly to Cn_mrc, and indirectly
                     to Cn_cg when it is computed from the dispersed value
                     of Cn_mrc.*/

  AeroCoefficientsDisp()
    :
    AeroCoefficientsCoreBase(),
    AeroCoefficientsOffDiagExtension(),
    Cl(0.0),  Cm(0.0),  Cn(0.0)
  {};
};
#endif
