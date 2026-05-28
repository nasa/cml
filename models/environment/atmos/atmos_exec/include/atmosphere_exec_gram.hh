/******************************** TRICK HEADER **********************************
PURPOSE:
   (Interface for the GRAM atmosphere.)

LIBRARY DEPENDENCY:
    (
     (../src/atmosphere_exec_gram.cc)
    )

PROGRAMMERS:
   (
    ((Gary Turner) (OSR) (May 2016) (ANTARES)
       (New architecture to isolate the atmospheres from the master executive))
   )
********************************************************************************/

#ifndef ATMOS_EXEC_GRAM_HH
#define ATMOS_EXEC_GRAM_HH

#include "cml/models/dynamics/state_descriptors/extended_planetary_derived_state/include/extended_planetary_derived_state.hh"

#include "atmosphere_exec_atmos_base.hh"

/* START Stubbed classes / structures for CML usage */
struct GramOut
{
  double atm_pressure;                /* (N/m2)  Atmospheric pressure */
  double atm_density;                 /* (kg/m3) Atmospheric density */
  double atm_temperature;             /* (K)     Atmospheric temperature */
  double atm_speed_of_sound;          /* (m/s)   Atmospheric speed of sound */
  double atm_wind_vmag;               /* (m/s)   Atmospheric wind magnitude */
  double atm_wind_angle_blowing_to;   /* (rad)
      Atmospheric wind angle blowing to direction */
  double atm_wind_angle_blowing_from; /* (rad)
      Atmospheric wind angle blowing from direction */
  double atm_wind_velocity_tc[3];     /* (m/s)
      Atmospheric wind velocity in topocentric (north-east-down) system */

  double atm_wind_velocity_td[3];     /* (m/s)
      Atmospheric wind velocity in topodetic (north-east-down) system */
  double drh_out; /* (--)  Atmospheric density: Total perturbation factor
          [rho = rho_mean*(1+drh)] (%/100) */
  double sdh_out; /* (--)  Atmospheric density: Standard deviation about GRAM
          nominal (%/100) */
  double dhp_out; /* (--)  Atmospheric density: GRAM perturbed deviation from
          1976 Standard Atmosphere (%/100) */
  double d3sh76;  /* (--)  Atmospheric density: GRAM 3-sigma high deviation
          about 1976 Standard */
  double d3sl76;  /* (--)  Atmospheric density: GRAM 3-sigma low deviation
          about 1976 Standard */
  double dmn76;   /* (--)  Atmospheric density: GRAM nominal deviation from
          1976 Standard Atmosphere */
  double rp1l;    /* (--)  Previous value of GRAM large-scale atmospheric
          pressure perturbation (%/100) */
  double rd1l;    /* (--)  Previous value of GRAM large-scale atmospheric
          density perturbation (%/100) */
  double rt1l;    /* (--)  Previous value of GRAM large-scale atmospheric
          temperature perturbation (%/100) */
  double ru1l;    /* (m/s) Previous value of GRAM large-scale E-W wind
          perturbation */
  double rv1l;    /* (m/s) Previous value of GRAM large-scale N-S wind
          perturbation */
  double rp1s;    /* (--)  Previous value of GRAM small-scale atmospheric
          pressure perturbation (%/100) */
  double rd1s;    /* (--)  Previous value of GRAM small-scale atmospheric
          density perturbation (%/100) */
  double rt1s;    /* (--)  Previous value of GRAM small-scale atmospheric
          temperature perturbation (%/100) */
  double ru1s;    /* (m/s) Previous value of GRAM small-scale E-W wind
          perturbation */
  double rv1s;    /* (m/s) Previous value of GRAM small-scale N-S wind
          perturbation */
  double rw1;     /* (m/s) Previous value of GRAM vertical wind perturbation */
};

class GramIn
{
 public:
  bool   gram_active; /* (--)  GRAM model activation flag */
  bool   inflt_gram_init_enable;
                      /* (--)  Enable GRAM initialization in flight */
  double latitude;    /* (rad) latitude of desired GRAM t-lookup */
  double longitude;   /* (rad) longitude of desired GRAM t-lookup */
  double alt;         /* (m)   geocentric altitude of desired GRAM t-lookup */
  double alt_offset;  /* (m)   MSL - geodetic altitude offset */

  GramIn()
  :
    gram_active(true),
    inflt_gram_init_enable(true),
    latitude(0.0),
    longitude(0.0),
    alt(0.0),
    alt_offset(0.0)
  {}
};

// Structures defined to overlay the GRAM Fortran common blocks
/*
..................................................................
  JW Note: for the sizes, see the common blocks that were added
           to the GRAM file Cfiles_E10_C.f90. If new variables
           are added/removed in subsequent versions, these
	       numbers will need to be updated.
..................................................................
*/
class GramCache
{
 public:
  static const unsigned int   iotemp_int_size   =  7, iotemp_dbl_size = 49,
     timeo_int_size    =   6, timeo_dbl_size    = 18, vert_int_size   =  1,
     vert_dbl_size     =  35, rcset1_int_size   =  2, rcset1_dbl_size = 25,
     scalecom_size     = 245, iucom_size        = 12, wincom_size     = 15,
     datacom_size      =  20, speccom_size      = 18, jaccon_size     =  9,
     comper_size       =  31, srfdat_size       = 18, concom_size     =  9,
     pertinit_int_size =   2, pertinit_dbl_size =  6;

  int    iotemp_int   [iotemp_int_size  ];
  double iotemp_dbl   [iotemp_dbl_size  ];
  int    timeo_int    [timeo_int_size   ];
  double timeo_dbl    [timeo_dbl_size   ];
  int    vert_int     [vert_int_size    ];
  double vert_dbl     [vert_dbl_size    ];
  int    rcset1_int   [rcset1_int_size  ];
  double rcset1_dbl   [rcset1_dbl_size  ];
  double scalecom     [scalecom_size    ];
  int    iucom        [iucom_size       ];
  double wincom       [wincom_size      ];
  double datacom      [datacom_size     ];
  double speccom      [speccom_size     ];
  double jaccon       [jaccon_size      ];
  double comper       [comper_size      ];
  double srfdat       [srfdat_size      ];
  double concom       [concom_size      ];
  int    pertinit_int [pertinit_int_size];
  double pertinit_dbl [pertinit_dbl_size];

// FIXME: These arrays were for an attempt to solve the multi-body problem. It
//     didn't work, so they have been removed to prevent excessive data copying;
//     however, I have left them commented here and in gram_E10_interface.cc and
//     Cfiles_E10_C.f90 in case it is helpful to whoever comes back to this
//     problem. --Daniel Ghan, May 2020
//  double doub1[500];
//  int    int0[32];
//  double doub2[385];
//  int    int3[19];
//  double doub3[1352];
//  float  float4[825630];
//  double doub4[19];
//  double doub5[6330];
//  int    int6[304];
//  double doub6[400];
};

class GramInterface
{
 public:

  GramOut out;   /* (--) output */
  GramIn  input; /* (--) input */
  GramCache cache; /* (--) Copies of the GRAM Fortran common blocks to avoid
                              multiple vehicles interfering with each other */

  /* ------------------- */
  /* Function Prototypes */
  /* ------------------- */
  GramInterface();
  virtual ~GramInterface(){};

  virtual void initialize(   // Return: -- Void
   double TSIM,      // IN: s      Simulation time
   double HGRAM,     // IN: m      Height above WGS84 ellipsoid
   double THGRAM,    // IN: rad    Aircraft longitude
   double PHGRAM,    // IN: rad    Aircraft latitude
   int    GR_YEAR,   // IN: --     Base date
   int    GR_MONTH,  // IN: --     Base date
   int    GR_DAY,    // IN: --     Base date
   int    GR_HOUR,   // IN: --     Base time
   int    GR_MIN,    // IN: --     Base time
   double GR_SEC);   // IN: --     Base time

  virtual void update(       // Return: -- Void
   double TSIM,      // IN: s      Simulation time
   double H_GC,      // IN: m      Geocentric height above WGS84 ellipsoid
   double ALONG,     // IN: rad    Vehicle longitude
   double GCLAT);    // IN: rad    Vehicle latitude
};

/* END   Stubbed classes / structures for CML usage */

class AtmosphereExec_Gram : public AtmosphereExec_AtmosWindsBase
{
  protected: // External references
    GramInterface & gram_i;/* (--) GRAM 2010 Trick interface structure */
    const GramInterface * master_gram;/* (--)
             A GRAM 2010 for copying from at activation */

    const double & dyn_time; /* (--) Reference to JEOD's dynamic time*/

  public:
    bool activate_gram_from_master; /* (--)
             Copy data from external GRAM model at model activation.*/

    AtmosphereExec_Gram(GramInterface & gram_E10_in,
                        const GramInterface * master_gram_10_in,
                        ExtendedPlanetaryDerivedState & planet_state_in,
                        const double & dyn_time_in,
                        AtmosExecOutput & master_output);
    virtual ~AtmosphereExec_Gram(){};

    virtual void activate() override;
    virtual void deactivate() override;
    void set_time( int year, int month, int day, int hour, int min, double sec);
    virtual bool initialize_atmos() override {return initialize();};
    virtual bool initialize_winds() override {return initialize();};

    virtual void update_atmos() override;
    virtual void update_winds() override;

  protected:
    bool initialized; /* (--) Internal flag to indicate that initialize has run.*/
    int     year_init;   /* (--)  Atmos initial year  input */
    int     month_init;  /* (--)  Atmos initial month input */
    int     day_init;    /* (--)  Atmos initial day   input */
    int     hour_init;   /* (--)  Atmos initial hour  input */
    int     min_init;    /* (--)  Atmos initial min   input */
    double  sec_init;    /* (s)   Atmos initial sec   input */

    double last_update_time; /* (s) time at which GRAM update was last called.*/

    bool initialize();

  private:
    AtmosphereExec_Gram (const AtmosphereExec_Gram&);
    AtmosphereExec_Gram & operator = (const AtmosphereExec_Gram&);
};
#endif
