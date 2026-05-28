/*############################################################################
PURPOSE:
  (A variable whose type is determined at runtime.)

PROGRAMMERS:
  (((Andrew Spencer) (OSR) (June 2015) (CR3333) (Initial version))
   ((Daniel Ghan) (OSR) (October 2021) (Antares) (Refactor)))
############################################################################*/
#ifndef CML_FAULT_UNTYPED_VARIABLE_HH
#define CML_FAULT_UNTYPED_VARIABLE_HH

/*******************************************************************************
UntypedVariableBase
Purpose:(Allows access to a variable's value without knowing its type at compile
         time. This is accomplished by doing something like
           UntypedVariableBase* x = new UntypedVariable<T>;
         where T is a typename, perhaps a template argument.
        )
*******************************************************************************/
class UntypedVariableBase {
  public:
    UntypedVariableBase() {}
    virtual ~UntypedVariableBase() {}
    virtual double get_value() = 0;
};


/*******************************************************************************
UntypedVariable
Purpose:(Provides UntypedVariableBase with access to a variable of any type.)
*******************************************************************************/
template<typename T> class UntypedVariable : public UntypedVariableBase {
  public:
    explicit UntypedVariable(T& var) : variable(var) {}
    virtual ~UntypedVariable(){}

    double get_value() override { return static_cast<double>(variable); }

    T& variable; /* (--) A variable of any type. */
};

#endif
