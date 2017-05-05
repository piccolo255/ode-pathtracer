#ifndef PLOT_WINDOW_HPP
#define PLOT_WINDOW_HPP

// Qt headers
#include <QMainWindow>
#include <QList>
#include <QLayout>
#include <QSettings>
#include <QtDebug>
#include <QtWidgets>

// C headers
#include <cstdlib>

// C++ headers
#include <iostream>
#include <iomanip>

// Boost headers
//#include <boost/property_tree/ptree.hpp>
//#include <boost/property_tree/ini_parser.hpp>
//#include <boost/tokenizer.hpp>

// Local headers
#include "pde_pathtracer.hpp"
#include "runge_kutta_stepper.hpp"
#include "render_view.hpp"
#include "simulation_loop.hpp"

// OUT and IN can be redefined as a filestream
// to enable direct file input/output
// In MinGW already defined in windef.h
#ifdef OUT
#undef OUT
#endif // OUT
#define OUT qDebug()
#ifdef IN
#undef IN
#endif
#define IN  std::cin
#define ERROUT qDebug()
#define LF  std::endl
#define ABS std::abs

// Ini file sections
#define SECTION_NAMES     "names"
#define SECTION_PARAM_EQ  "parameter equations"
#define SECTION_VAR_DERIV "variable derivations"
#define SECTION_VAR_INIT  "variable initial"
#define SECTION_TIME      "time"
#define SECTION_PLOT      "plot"

namespace Ui {
class PlotWindow;
}

class PlotWindow : public QMainWindow
{
   Q_OBJECT

public:
   explicit PlotWindow( QWidget *parent = 0 );
   ~PlotWindow();

public slots:
   void updateView( PointValues newPoint );

private:
   Ui::PlotWindow *ui;
   RungeKuttaStepper *stepper;
   SimulationLoop *simulation;

   QList<PointValues> pointPath;
   QList<RenderView *> views;
   QList<QLabel> labels;

   // Names
   QStringList paramNames;
   QStringList varNames;

   // Equations
   EquationVector   paramRules;
   DerivationVector varRules;

   // Initial values
   PointValues initialValues;

   // Time parameters
   double dt;

   // Labels
   QStringList labelNames;
   QVector<int> labelParamIndex;
   QVector<int> labelDockRow;

   // Plot parameters
   QString plotTransformX;
   QString plotTransformY;
   QRect   plotViewport;
   int plotMaxFPS;
   int plotSkip;
   int plotMaxPathSegments;

   void ThrowError( QString msg );
   void inputData( const QString filename );
   QStringList tokenizeString( QString &str );
   void toggleSimulationRun( bool toggled );
   void exitProgram( bool checked = false );
   void addParamLabel( QString name, bool removable = true );
   void removeParamLabel( QString name );
   void updateParamLabels( PointValues values );

   template <class T> T readEntry(
      //boost::property_tree::ptree pt
      QSettings *inputFile
    , QString section
    , QString name
    , T defaultValue
   ){
      T value;

      // show warning if key is missing
      if( !(inputFile->contains( section+"/"+name )) ){
         qDebug() << "WARNING: Key \"" << name << "\" in section [" << section << "] not found.\n"
                  << "         Assuming " << name << " = " << defaultValue << "\n";
      }

      // get value
      QVariant var = inputFile->value( section+"/"+name, defaultValue );
      value = var.value<T>();

      //qDebug() << "read value = " << value;

      return value;
   }
};

#endif // PLOT_WINDOW_HPP
