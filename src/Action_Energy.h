#ifndef INC_ACTION_ENERGY_H
#define INC_ACTION_ENERGY_H
#include "Action.h"
#include "Energy.h"
#include "CharMask.h"
#include "Timer.h"
#include "ExclusionArray.h"
#include "EwaldOptions.h"
#include "Energy/Ecalc_Nonbond.h"
class PotentialFunction;
/// Calculate energy 
class Action_Energy: public Action {
  public:
    Action_Energy();
    ~Action_Energy();
    DispatchObject* Alloc() const { return (DispatchObject*)new Action_Energy(); }
    void Help() const;
    void Help(ArgList&) const;
  private:
    Action::RetType Init(ArgList&, ActionInit&, int);
    Action::RetType Setup(ActionSetup&);
    Action::RetType DoAction(int, ActionFrame&);
    void Print();

    /// Corresponds to data sets.
    enum Etype { BOND = 0, ANGLE, DIHEDRAL, V14, Q14, VDW, ELEC, KE, TOTAL};
    /// Add energy data set of specified type.
    int AddSet(Etype, DataSetList&, DataFile*, std::string const&);
    /// For debugging the direct sum convergence
    double Dbg_Direct(Frame const&,int);
    /// Corresponds to calculations.
    enum CalcType { C_BND = 0, C_ANG, C_DIH, C_N14, C_NBD, C_LJ,
                    C_COULOMB, C_DIRECT, C_EWALD, C_PME, C_KEAUTO, C_KEVEL, C_KEVV };
    /// Corresponds to type of electrostatics.
    enum ElecType { NO_ELE = 0, SIMPLE, DIRECTSUM, EWALD, PME };
    /// Corresponds to type of KE calc.
    enum KEType { KE_NONE = 0, KE_AUTO, KE_VEL, KE_VV };
    /// Type for iterator over calculations
    typedef std::vector<CalcType>::const_iterator calc_it;

    ElecType elecType_;            ///< Type of electrostatics calc.
    KEType KEtype_;                ///< Type of KE calc.
    std::vector<DataSet*> Energy_; ///< Hold output data sets (length Etype+1)
    std::vector<CalcType> Ecalcs_; ///< Hold which calcs to perform
    Topology* currentParm_;        ///< Hold current topology
    ExclusionArray Excluded_;      ///< Hold exclusion list for current topology/mask
    CharMask Mask1_;               ///< Char mask for all but NB calc
    AtomMask Imask_;               ///< Int mask for NB calc
    Energy_Amber ENE_;             ///< Energy calc class.
    std::string setname_;          ///< Output DataSet name
    int npoints_;                  ///< # cells in each direction (DIRECT)
    int debug_;
    Cpptraj::Energy::Ecalc_Nonbond NB_; ///< Nonbond energy calc class.
    Cpptraj::Energy::Ecalc_Nonbond::CalcType nbCalcType_; ///< Nonbond calculation type
    EwaldOptions ewaldOpts_;       ///< Ewald options

    PotentialFunction* potential_; ///< TODO currently just for openmm, use for everything
    bool use_openmm_;              ///< If true use openmm for calculating total E

    double dt_;                    ///< Time step for estimating kinetic energy (leapfrog)
    bool need_lj_params_;          ///< True if LJ parameters needed.
    bool needs_exclList_;          ///< True if Excluded_ needs to be set up.
    bool bondsToH_;                ///< True if we want to calculate energy of bonds with H
    Timer time_total_;
    Timer time_bond_;
    Timer time_angle_;
    Timer time_tors_;
    Timer time_14_;
    Timer time_NB_;
    Timer time_ke_;
};
#endif
