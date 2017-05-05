#include "plot_window.hpp"
#include <QApplication>

int main(
   int argc
 , char *argv[]
){
   QApplication app( argc, argv );
   PlotWindow w;

   QCoreApplication::setOrganizationName( "UEC" );
   QCoreApplication::setApplicationName( "PDE PathTracer" );

   w.show();

   return app.exec();
}
