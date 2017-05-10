#include "plot_window.hpp"
#include "ui_plot_window.h"

PlotWindow::PlotWindow(
   QWidget *parent
) :
   QMainWindow( parent ),
   ui( new Ui::PlotWindow )
{
   ui->setupUi( this );

//   // load input file
//   inputData( filename );

//   qDebug() << "Initial values: t = " << initialValues.T << ", " << initialValues.Val;

//   pointPath.clear();

//   // create render widget
//   views.push_back( new RenderView( plotViewport, plotTransformX, plotTransformY, paramNames ) );
//   //views.push_back( new RenderView( plotViewport, plotTransformX, plotTransformY, paramNames ) );

   // set layout
   mainLayout = new QGridLayout( ui->centralWidget );
//   mainLayout->setColumnStretch( 0, 1 );
//   mainLayout->setColumnStretch( 1, 1 );
//   mainLayout->addWidget( views[0], 0, 0 );
//   mainLayout->addWidget( views[1], 0, 1 );
//   ui->centralWidget->setLayout( mainLayout );

   // create actions
   QAction *runAction = new QAction( tr("&Run"), this );
   runAction->setShortcut( QKeySequence( Qt::Key_Space ) );
   runAction->setStatusTip( tr("Run or pause simulation.") );
   runAction->setCheckable( true );
   connect( runAction, &QAction::toggled, this, &PlotWindow::toggleSimulationRun );

   QAction *exitAction = new QAction( tr("&Exit"), this );
   exitAction->setShortcut( QKeySequence( Qt::Key_Escape ) );
   exitAction->setStatusTip( tr("Exit program.") );
   connect( exitAction, &QAction::triggered, this, &PlotWindow::exitProgram );

   // create dock for labels
   labelDock = new QDockWidget( tr("Labels"), this );
//   dockWidget = new LabelDockWidget( paramNames, labelDock );
//   connect( this, &PlotWindow::updateParamLabels, dockWidget, &LabelDockWidget::updateParamLabels );
//   labelDock->setWidget( dockWidget );
   labelDock->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
   addDockWidget( Qt::RightDockWidgetArea, labelDock );

//   // add predefined labels
//   dockWidget->addParamLabel( "t", false );
//   for( auto name : labelNamesConfig ){
//      dockWidget->addParamLabel( name );
//   }

   // make toolbar
   ui->mainToolBar->addAction( runAction );
   ui->mainToolBar->addSeparator();
   ui->mainToolBar->addAction( labelDock->toggleViewAction() );
   ui->mainToolBar->addSeparator();
   ui->mainToolBar->addAction( exitAction );

   // decorate window
   setWindowTitle( tr("ODE PathTracer") );

//   // prepare stepper
//   stepper = new RungeKuttaStepper;
//   stepper->SetConditions( varRules, paramRules, initialValues, dt );

   // start simulation
   qRegisterMetaType<PointValues>();
//   simulation = new SimulationLoop( plotMaxFPS, plotSkip, stepper );
//   connect( simulation, &SimulationLoop::updateView, this, &PlotWindow::updateView );
//   simulation->suspend();
//   simulation->start();

   openProblem( "input.ini" );
}

PlotWindow::~PlotWindow(
){
   simulation->stop();
   while( simulation->isRunning() ){
      ui->statusBar->showMessage( tr("Waiting for threads to stop...") );
      QThread::yieldCurrentThread();
//      Sleep(100);
   };

//   delete dockWidget;
   delete simulation;
   delete stepper;
   for( auto v : views ){
      delete v;
   }

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
   labelNamesConfig = readEntry<QStringList>( inputFile, SECTION_PLOT, "label_parameters", QStringList() );

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
   const QString filename
){
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
   for( auto name : labelNamesConfig ){
      dockWidget->addParamLabel( name );
   }

   // update window title
   setWindowTitle( filename + tr(" ODE PathTracer") );

   // prepare stepper
   stepper = new RungeKuttaStepper;
   stepper->SetConditions( varRules, paramRules, initialValues, dt );

   // start simulation
   simulation = new SimulationLoop( plotMaxFPS, plotSkip, stepper );
   connect( simulation, &SimulationLoop::updateView, this, &PlotWindow::updateView );
   simulation->suspend();
   simulation->start();
}

void PlotWindow::closeProblem(
){

}
