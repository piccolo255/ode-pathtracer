#include "plot_window.hpp"
#include "ui_plot_window.h"

PlotWindow::PlotWindow(
   QWidget *parent
) :
   QMainWindow( parent ),
   ui( new Ui::PlotWindow )
{
   ui->setupUi( this );

   // create layout
   mainLayout = new QGridLayout( ui->centralWidget );

   // create actions
   openProblemAction = new QAction( tr("&Open"), this );
   openProblemAction->setShortcut( QKeySequence( Qt::Key_O ) );
   openProblemAction->setStatusTip( tr("Open problem.") );
   connect( openProblemAction, &QAction::triggered, this, &PlotWindow::openProblem );

   closeProblemAction = new QAction( tr("&Close"), this );
   closeProblemAction->setShortcut( QKeySequence( Qt::Key_C ) );
   closeProblemAction->setStatusTip( tr("Close problem.") );
   closeProblemAction->setEnabled( false );
   connect( closeProblemAction, &QAction::triggered, this, &PlotWindow::closeProblem );

   runAction = new QAction( tr("&Run"), this );
   runAction->setShortcut( QKeySequence( Qt::Key_Space ) );
   runAction->setStatusTip( tr("Run or pause simulation.") );
   runAction->setCheckable( true );
   runAction->setEnabled( false );
   connect( runAction, &QAction::toggled, this, &PlotWindow::toggleSimulationRun );

   exitAction = new QAction( tr("&Exit"), this );
   exitAction->setShortcut( QKeySequence( Qt::Key_Escape ) );
   exitAction->setStatusTip( tr("Exit program.") );
   connect( exitAction, &QAction::triggered, this, &PlotWindow::exitProgram );

   // create dock for labels
   labelDock = new QDockWidget( tr("Labels"), this );
   labelDock->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
   addDockWidget( Qt::RightDockWidgetArea, labelDock );


   // make toolbar
   ui->mainToolBar->addAction( openProblemAction );
   ui->mainToolBar->addAction( closeProblemAction );
   ui->mainToolBar->addSeparator();
   ui->mainToolBar->addAction( runAction );
   ui->mainToolBar->addSeparator();
   ui->mainToolBar->addAction( labelDock->toggleViewAction() );
   ui->mainToolBar->addSeparator();
   ui->mainToolBar->addAction( exitAction );

   // decorate window
   setWindowTitle( tr("ODE PathTracer") );

   // register custom types so they can be used in slots/signals
   qRegisterMetaType<PointValues>();
}

PlotWindow::~PlotWindow(
){
   closeProblem();

   delete ui;
}

void PlotWindow::updateView(
   PointValues newPoint
){
//   qDebug() << "Adding point @ t = " << newPoint.T << " *** " << newPoint.Param;

   pointPath.push_front( newPoint );
   if( pointPath.size() > plotMaxPathSegments )
      pointPath.removeLast();

   for( auto v : views ){
      v->updateObjects( pointPath, plotMaxPathSegments );
   }

   for( auto v : views ){
      v->repaint();
   }

   updateParamLabels( newPoint );
}

void PlotWindow::ThrowError(
   QString msg
){
   ERROUT << "ERROR: " << msg;
   exit( EXIT_FAILURE );
}

void PlotWindow::inputData(
   const QString filename
){
   QSettings *inputFile;
   inputFile = new QSettings( filename, QSettings::IniFormat );

   // parameter and variable names
   paramNames = readEntry<QStringList>( inputFile, SECTION_NAMES, "parameter_names", QStringList() );
   varNames   = readEntry<QStringList>( inputFile, SECTION_NAMES, "variable_names",  QStringList() );

   // label names
   labelNames = readEntry<QStringList>( inputFile, SECTION_PLOT, "label_parameters", QStringList() );

   // parameter equations
   paramRules.resize( paramNames.size() );
   for( int i = 0; i < paramRules.size(); i++ ){
      paramRules[i].first = paramNames[i];
      paramRules[i].second = readEntry<QString>( inputFile, SECTION_PARAM_EQ, paramNames[i], "0" );
   }

   // variable derivation equations and initial values
   varRules.resize( varNames.size() );
   for( int i = 0; i < varRules.size(); i++ ){
      varRules[i].first  = varNames[i];
      varRules[i].second = readEntry<QString>( inputFile, SECTION_VAR_DERIV, varNames[i], "0" );

      double tempdbl = readEntry<double>( inputFile, SECTION_VAR_INIT, varNames[i], 0.0 );
      initialValues.Val.push_back( tempdbl );
   }

   // time
   initialValues.T = readEntry<double>( inputFile, SECTION_TIME, "t_init", 0.0 );
   dt              = readEntry<double>( inputFile, SECTION_TIME, "dt",     0.1 );

   // plot
   int x1 = readEntry<int>( inputFile, SECTION_PLOT, "x1", -10 );
   int y1 = readEntry<int>( inputFile, SECTION_PLOT, "y1", -10 );
   int x2 = readEntry<int>( inputFile, SECTION_PLOT, "x2",  10 );
   int y2 = readEntry<int>( inputFile, SECTION_PLOT, "y2",  10 );
   plotViewport.setCoords( x1, y1, x2, y2 );
   plotTransformX  = readEntry<QString>( inputFile, SECTION_PLOT, "x_transform", "x" );
   plotTransformY  = readEntry<QString>( inputFile, SECTION_PLOT, "y_transform", "y" );
   plotMaxFPS          = readEntry<int>( inputFile, SECTION_PLOT, "max_fps", 60 );
   plotSkip            = readEntry<int>( inputFile, SECTION_PLOT, "frame_skip", 0 );
   plotMaxPathSegments = readEntry<int>( inputFile, SECTION_PLOT, "max_segments", 100 );
}

QStringList PlotWindow::tokenizeString(
   QString &str
){
   QRegExp delimiters("(\\ |\\,|\\t)");
   QStringList result = str.split( delimiters, QString::SkipEmptyParts );

   return result;
}

void PlotWindow::toggleSimulationRun(
   bool toggled
){
   //qDebug() << "Run clicked";
   if( simulation == NULL ){
      return;
   }
   if( toggled ){
      //qDebug() << "::ON";
      simulation->resume();
   } else {
      //qDebug() << "::OFF";
      simulation->suspend();
   }
}

void PlotWindow::exitProgram(
   bool /* checked */ // unused
){
   QMessageBox confirmBox;
   confirmBox.setText( "Do you really want to exit?" );
   confirmBox.setStandardButtons( QMessageBox::Ok | QMessageBox::Cancel );
   confirmBox.setDefaultButton( QMessageBox::Cancel );
   int confirmExit = confirmBox.exec();
   if( confirmExit != QMessageBox::Ok ){
      // cancel
      return;
   } else {
      // exit
      this->close();
   }
}

void PlotWindow::openProblem(
   bool /* checked */ // unused
){
   loadProblem( "input.ini" );

   closeProblemAction->setEnabled( true );
   runAction->setChecked( false );
   runAction->setEnabled( true );
}

void PlotWindow::loadProblem(
   const QString filename
){
   // close problem if one is already opened
   closeProblem();

   // load input file
   inputData( filename );

   // create render surface(s)
   views.push_back( new RenderView( plotViewport, plotTransformX, plotTransformY, paramNames ) );

   // add render surfaces to main window
   mainLayout->addWidget( views[0], 0, 0 );

   // set dock widget for labels
   dockWidget = new LabelDockWidget( paramNames, labelDock );
   connect( this, &PlotWindow::updateParamLabels, dockWidget, &LabelDockWidget::updateParamLabels );
   labelDock->setWidget( dockWidget );

   // add predefined labels to label dock
   dockWidget->addParamLabel( "t", false );
   for( auto name : labelNames ){
      dockWidget->addParamLabel( name );
   }

   // update window title
   setWindowTitle( filename + tr(" - ODE PathTracer") );

   // prepare stepper
   stepper = new RungeKuttaStepper;
   stepper->SetConditions( varRules, paramRules, initialValues, dt );

   // start simulation
   simulation = new SimulationLoop( plotMaxFPS, plotSkip, stepper );
   connect( simulation, &SimulationLoop::updateView, this, &PlotWindow::updateView );
   simulation->suspend();
   simulation->start();

   ui->statusBar->showMessage( tr("Problem opened.") );
}

void PlotWindow::closeProblem(
   bool /* checked */ // unused
){
   // update gui elements
   closeProblemAction->setEnabled( false );
   runAction->setChecked( false );
   runAction->setEnabled( false );

   // end simulation
   if( simulation != NULL ){
      simulation->stop();
      while( simulation->isRunning() ){
         ui->statusBar->showMessage( tr("Waiting for threads to stop...") );
         QThread::yieldCurrentThread();\
      }

      delete simulation;
      simulation = NULL;
   }
   if( stepper != NULL ){
      delete stepper;
      stepper = NULL;
   }

   for( auto v : views ){
      delete v;
   }
   views.clear();

   if( dockWidget != NULL ){
      delete dockWidget;
      dockWidget = NULL;
   }

   // update window title
   setWindowTitle( tr("ODE PathTracer") );

   ui->statusBar->showMessage( tr("Problem closed.") );
}
