#ifndef RUNGE_KUTTA_STEPPER_HPP
#define RUNGE_KUTTA_STEPPER_HPP

// C headers
#include <cstdlib>

// C++ headers
#include <vector>
#include <utility>
#include <iostream>

// math expression parsing header
#include "muParser.h"

// Local headers
#include "ode_pathtracer.hpp"

enum class DerivationMode{
   None
 , Function
 , Rule
};

enum class CalculationMode{
   None
 , All
 , Step
};

class RungeKuttaStepper
{
public:
   RungeKuttaStepper( void );
   ~RungeKuttaStepper( void );

   void SetConditions( DerivationVector ddt_rules
                     , EquationVector param_rules
                     , PointValues val_init
                     , double timeSlice );

   PointValues CalculateStep();
   PointValues Step( PointValues val_i );

private:
//   QVector<double> (*derive)( PointValues );

   int varCount;
   int paramCount;
   double t;
   double *vars;
   double *params;
   mu::Parser *varParser;
   mu::Parser *paramParser;

   DerivationMode derivationMode;
   CalculationMode calculationMode;

   double t_init, t_end;
   PointValues init;
   int n;
   double h;

   void ParserError( mu::Parser::exception_type &e );
};

#endif // RUNGE_KUTTA_STEPPER_HPP
