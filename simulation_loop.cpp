#include "simulation_loop.hpp"

SimulationLoop::SimulationLoop(
   int maxFPS
 , int skipSteps
 , RungeKuttaStepper *stepperMethod
 , QObject */*parent*/ // unused
){
   minUpdateInterval = 1000 / maxFPS;
   skip = skipSteps;
   stepper = stepperMethod;

   stateSuspend = false;
   stateExit = false;
}

void SimulationLoop::run(
){
   PointValues pv;

   QElapsedTimer updateTimer;
   updateTimer.start();

   while( true ){
      while( stateSuspend && !stateExit ){
         yieldCurrentThread();
      }
      if( stateExit )
         return;

      for( int i = 0; i < skip; i++ ){
         stepper->CalculateStep();
         while( stateSuspend && !stateExit ){
            yieldCurrentThread();
         }
         if( stateExit )
            return;
      }
      if( stateExit )
         return;

      pv = stepper->CalculateStep();
      while( stateSuspend && !stateExit ){
         yieldCurrentThread();
      }
      if( stateExit )
         return;

      // limit update rate
      int yields = 0;
      while( ( !updateTimer.hasExpired( minUpdateInterval ) || stateSuspend ) && !stateExit ){
         yields++;
         //yieldCurrentThread();
         long sleepDuration = minUpdateInterval-updateTimer.elapsed();
         sleepDuration = sleepDuration > 0 ? sleepDuration : 1;
         msleep( sleepDuration );
      }
      if( stateExit )
         return;

      // run update
      //qDebug() << "timer: " << updateTimer.elapsed() << " of " << minUpdateInterval << ", yield # = " << yields;
      emit updateView( pv );
      updateTimer.start();
   }
}

void SimulationLoop::suspend(
){
   stateSuspend = true;
}

void SimulationLoop::resume(
){
   stateSuspend = false;
}

void SimulationLoop::stop(
){
   stateExit = true;
}

void SimulationLoop::checkState(
){
   while( stateSuspend ){
      if( stateExit ) exit();
      yieldCurrentThread();
   }
}
