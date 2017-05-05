#ifndef SIMULATION_LOOP_HPP
#define SIMULATION_LOOP_HPP

// Qt headers
#include <QObject>
#include <QThread>
#include <QtDebug>
#include <QElapsedTimer>

// Local headers
#include "pde_pathtracer.hpp"
#include "runge_kutta_stepper.hpp"

class SimulationLoop : public QThread
{
   Q_OBJECT

public:
   explicit SimulationLoop( int maxFPS
                          , int skipSteps
                          , RungeKuttaStepper *rk
                          , QObject *parent = 0 );
   void run() Q_DECL_OVERRIDE;
   void suspend();
   void resume();
   void stop();

signals:
   void updateView( PointValues newPoint );

private:
   int minUpdateInterval;
   int skip;
   bool stateSuspend;
   bool stateExit;
   RungeKuttaStepper *stepper;

   void checkState();
};

#endif // SIMULATION_LOOP_HPP
