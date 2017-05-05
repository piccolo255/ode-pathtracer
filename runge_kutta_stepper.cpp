#include "runge_kutta_stepper.hpp"

RungeKuttaStepper::RungeKuttaStepper(
){
   derivationMode  = DerivationMode::None;
   calculationMode = CalculationMode::None;

   varParser   = NULL;
   paramParser = NULL;
   vars        = NULL;
   params      = NULL;
}

RungeKuttaStepper::~RungeKuttaStepper(
){
   if( vars != NULL )
      delete[] vars;
   if( params != NULL )
      delete[] params;
   if( varParser != NULL )
      delete[] varParser;
   if( paramParser != NULL )
      delete[] paramParser;
}

void RungeKuttaStepper::SetConditions(
   DerivationVector ddt_rules
 , EquationVector param_rules
 , PointValues val_init
 , double timeSlice
){
   // initialize parser
   try {
      varCount   = ddt_rules.size();
      paramCount = param_rules.size();

      // delete old settings, if present
      if( vars != NULL )
         delete[] vars;
      if( params != NULL )
         delete[] params;
      if( varParser != NULL )
         delete[] varParser;
      if( paramParser != NULL )
         delete[] paramParser;

      // allocate memory
      vars   = new double[varCount];
      params = new double[paramCount];
      varParser   = new mu::Parser[varCount];
      paramParser = new mu::Parser[paramCount];

      // set parsers
      for( int i = 0; i < varCount; i++ ){
         varParser[i].DefineVar( "t", &t );
         for( int j = 0; j < varCount; j++ )
            varParser[i].DefineVar( ddt_rules[j].first.toStdString(), &vars[j] );
         for( int j = 0; j < paramCount; j++ )
            varParser[i].DefineVar( param_rules[j].first.toStdString(), &params[j] );
         varParser[i].SetExpr( ddt_rules[i].second.toStdString() );
      }
      for( int i = 0; i < paramCount; i++ ){
         paramParser[i].DefineVar( "t", &t );
         for( int j = 0; j < varCount; j++ )
            paramParser[i].DefineVar( ddt_rules[j].first.toStdString(), &vars[j] );
         for( int j = 0; j < paramCount; j++ )
            paramParser[i].DefineVar( param_rules[j].first.toStdString(), &params[j] );
         paramParser[i].SetExpr( param_rules[i].second.toStdString() );
      }
   } catch( mu::Parser::exception_type &e ){
      ParserError( e );
   }

   derivationMode  = DerivationMode::Rule;
   calculationMode = CalculationMode::Step;

   init = val_init;
   h    = timeSlice;

   // calculate initial parameter values
   init.Param.resize( paramCount );
   t = init.T;
   for( int i = 0; i < varCount; i++ )
      vars[i] = init.Val[i];
   for( int i = 0; i < paramCount; i++ )
      init.Param[i] = params[i] = paramParser[i].Eval();
}

PointValues RungeKuttaStepper::CalculateStep(
){
   static PointValues pv = init;

   PointValues step_i1;
   step_i1 = Step( pv );
   pv = step_i1;

   return pv;
}

PointValues RungeKuttaStepper::Step(
   PointValues val_i
){
   switch( derivationMode ){
   case DerivationMode::Function:
//      // k1
//      ValueVector k1 = derive( val_i );
//
//      // k2
//      temp.first = val_i.first + h/2.0;
//      for( int i = 0; i < count; i++ )
//         temp.second[i] = val_i.second[i] + h/2.0 * k1[i];
//      ValueVector k2 = derive( temp );
//
//      // k3
//      temp.first = val_i.first + h/2.0;
//      for( int i = 0; i < count; i++ )
//         temp.second[i] = val_i.second[i] + h/2.0 * k2[i];
//      ValueVector k3 = derive( temp );
//
//      // k4
//      temp.first = val_i.first + h;
//      for( int i = 0; i < count; i++ )
//         temp.second[i] = val_i.second[i] + h * k3[i];
//      ValueVector k4 = derive( temp );
//
//      // final result
//      ValueVector res( val_i.second );
//      for( int i = 0; i < count; i++ )
//         res[i] += h/6.0 * ( k1[i] + 2.0*k2[i] + 2.0*k3[i] + k4[i] );
//
//      return res;
      break;
   case DerivationMode::Rule:
      try {
         QVector<double> k1( varCount );
         QVector<double> k2( varCount );
         QVector<double> k3( varCount );
         QVector<double> k4( varCount );

         // k1
         t = val_i.T;
         for( int i = 0; i < varCount; i++ )
            vars[i] = val_i.Val[i];
         for( int i = 0; i < paramCount; i++ )
            params[i] = val_i.Param[i];
         for( int i = 0; i < varCount; i++ )
            k1[i] = varParser[i].Eval();

         // k2
         t = val_i.T + h/2.0;
         for( int i = 0; i < varCount; i++ )
            vars[i] = val_i.Val[i] + h/2.0 * k1[i];
         for( int i = 0; i < paramCount; i++ )
            params[i] = paramParser[i].Eval();
         for( int i = 0; i < varCount; i++ )
            k2[i] = varParser[i].Eval();

         // k3
         t = val_i.T + h/2.0;
         for( int i = 0; i < varCount; i++ )
            vars[i] = val_i.Val[i] + h/2.0 * k2[i];
         for( int i = 0; i < paramCount; i++ )
            params[i] = paramParser[i].Eval();
         for( int i = 0; i < varCount; i++ )
            k3[i] = varParser[i].Eval();

         // k4
         t = val_i.T + h;
         for( int i = 0; i < varCount; i++ )
            vars[i] = val_i.Val[i] + h * k3[i];
         for( int i = 0; i < paramCount; i++ )
            params[i] = paramParser[i].Eval();
         for( int i = 0; i < varCount; i++ )
            k4[i] = varParser[i].Eval();

         // final result
         QVector<double> newVal( val_i.Val );
         for( int i = 0; i < varCount; i++ )
            newVal[i] += h/6.0 * ( k1[i] + 2.0*k2[i] + 2.0*k3[i] + k4[i] );

         // parameters
         t = val_i.T + h;
         QVector<double> newParam( val_i.Param.size() );
         for( int i = 0; i < varCount; i++ )
            vars[i] = newVal[i];
         for( int i = 0; i < paramCount; i++ )
            newParam[i] = paramParser[i].Eval();

         PointValues val_ip1;
         val_ip1.T     = val_i.T + h;
         val_ip1.Val   = newVal;
         val_ip1.Param = newParam;

         return val_ip1;
      } catch( mu::Parser::exception_type &e ){
         ParserError( e );
      }
      break;
   case DerivationMode::None:
      break;
   }

   std::cerr << "Error in RungeKutta: derivations undefined." << std::endl;
   exit( EXIT_FAILURE );
}

void RungeKuttaStepper::ParserError(
   mu::ParserBase::exception_type &e
){
   std::cerr << std::endl << "Parsing error:" << std::endl;
   std::cerr << "------" << std::endl;
   std::cerr << "Message:  " << e.GetMsg()   << std::endl;
   std::cerr << "Formula:  " << e.GetExpr()  << std::endl;
   std::cerr << "Token:    " << e.GetToken() << std::endl;
   std::cerr << "Position: " << e.GetPos()   << std::endl;
   std::cerr << "Errcode:  " << e.GetCode()  << std::endl;
   exit( EXIT_FAILURE );
}
