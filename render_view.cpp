#include "render_view.hpp"

RenderView::RenderView(
   QRect   viewportArea
 , QString transformationX
 , QString transformationY
 , QStringList paramNames
 , QWidget * /*parent*/ // unused
){
   viewRectAlwaysVisible = viewportArea;
   //updateViewRect( this->size() );

   // initialize parser
   try {
      // clear
      paramVals.clear();
      for( auto parser : coordinateParsers ){
         delete parser;
      }
      coordinateParsers.clear();

      // create parsers
      coordinateParsers.push_back( new mu::Parser );
      coordinateParsers.push_back( new mu::Parser );

      // register parameters
//      qDebug() << "Registering parameters...";
      paramVals.resize( paramNames.size() );
//      qDebug() << "--- param count = " << paramVals.size() << " or " << paramNames.size();
      for( auto parser : coordinateParsers ){
//         qDebug() << "--- parser";
         parser->DefineVar( "t", &t );
         for( int i = 0; i < paramVals.size(); i++  ){
            parser->DefineVar( paramNames[i].toStdString(), &(paramVals[i]) );
//            qDebug() << "------ registered param " << paramNames[i];
         }
      }
//      qDebug() << "Registering parameters done.";

      // set coordinate transformation
      coordinateParsers[0]->SetExpr( transformationX.toStdString() );
      coordinateParsers[1]->SetExpr( transformationY.toStdString() );
   } catch( mu::Parser::exception_type &e ){
      ParserError( e );
   }
}

RenderView::~RenderView(
){
   for( auto parser : coordinateParsers ){
      delete parser;
   }
   coordinateParsers.clear();
}

void RenderView::updateObjects(
   QList<PointValues> pointPathList
 , int maxPathLength
){
   double x, y;
   bool firstPoint = true;
   QPointF p1, p2;
   segments.clear();
   segments.reserve( pointPathList.size() );

   for( auto &point : pointPathList ){
      t = point.T;
      for( int i = 0; i < paramVals.size(); i++  ){
         paramVals[i] = point.Param[i];
      }
      try {
         x = coordinateParsers[0]->Eval();
         y = coordinateParsers[1]->Eval();
         //qDebug() << "Point ( " << x << ", " << y << " )";
      } catch( mu::Parser::exception_type &e ){
         ParserError( e );
      }

      if( firstPoint ){
         p1 = QPoint( x, y );
         firstPoint = false;
      } else {
         p2 = p1;
         p1 = QPointF( x, y );
         segments.append( QLineF( p1, p2 ) );
      }
   }

   if( maxPathLength != maxSegments ){
      maxSegments = maxPathLength;
      updateColors();
   }
}

void RenderView::updateViewRect( QSize newViewRectSize ){
   int defW = viewRectAlwaysVisible.width();
   int defH = viewRectAlwaysVisible.height();
   int newW = newViewRectSize.width();
   int newH = newViewRectSize.height();

   int x, y, w, h; // calculated values

   // determine which orientation needs to be letterboxed
   float ratio;

   if( defW*newH < defH*newW ){
      // fit-to-height, letterbox left and right
      ratio = (defH / (float)newH);
   } else {
      // fit-to-width, letterbox up and down
      ratio = (defW / (float)newW);
   }

   w = newW * ratio;
   h = newH * ratio;

   x = viewRectAlwaysVisible.x() + (defW-w)/2;
   y = viewRectAlwaysVisible.y() + (defH-h)/2;

//   qDebug() << "Canvas size: " << newW << " x " << newH
//            << " \tNew viewport: (" << x << ", " << y << ") " << w << " x " << h;

   viewRect.setRect( x, y, w, h );
}

void RenderView::updateColors(
){
   float h, s, v;

   colors.resize( maxSegments );

   for( int i = 0; i < colors.size(); i++ ){
      h = 0;
      s = 0 + 1.0/maxSegments*i;
      v = 0 + 1.0/maxSegments*i;
      colors[i].setHsvF( h, s, v );
   }
}

void RenderView::ParserError(
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

void RenderView::paintEvent(
   QPaintEvent * event
){
   QPainter painter(this);

   painter.setRenderHint( QPainter::Antialiasing );
   painter.fillRect( event->rect(), QBrush(Qt::white) );

   painter.setWindow( viewRect );

//   qDebug() << "Window: " << geometry().width() << "x" << geometry().height();
//   qDebug() << "Geometry: ( " << startX << ", " << startY << " ) " << width << "x" << height;
//   qDebug() << "Viewport: " << painter.viewport();
//   qDebug() << "World: " << painter.window();

   // draw particle path
   // draw lines backwards, from old to new
   for( int i = segments.size()-1; i >= 0; i-- ){
      painter.setPen( QPen( QBrush( colors[i] ), 0 ) );
      painter.drawLine( segments[i] );
   }

   // draw axes
   painter.setPen( QPen( QBrush( Qt::blue ), 0 ) );
   painter.drawLine( viewRect.x(), 0, viewRect.x()+viewRect.width(), 0 );
   painter.drawLine( 0, viewRect.y(), 0,  viewRect.y()+viewRect.height() );

//   // draw viewport
//   painter.drawRect( viewRectAlwaysVisible );
}

void RenderView::resizeEvent( QResizeEvent *event ){
   updateViewRect( event->size() );
}
