#ifndef RENDER_VIEW_HPP
#define RENDER_VIEW_HPP

// Qt includes
#include <QWidget>
#include <QPainter>
#include <QList>
#include <QMap>

// C++ includes

// math expression parsing header
#include "muParser.h"

// Local includes
#include "pde_pathtracer.hpp"

class RenderView : public QWidget
{
   Q_OBJECT
public:
   explicit RenderView( QRect viewportArea
                      , QString transformationX
                      , QString transformationY
                      , QStringList paramNames
                      , QWidget *parent = 0 );
   ~RenderView();
   void updateObjects( QList<PointValues> pointPathList, int maxPathLength );

private:
   QRect viewRect;
   QRect viewRectAlwaysVisible;

   double pointX;
   double pointY;
   double t;

//   QStringList           paramsName;
//   QMap<QString, double> paramsVal;
//   QMap<QString, int>    paramsIndex;
   QVector<double> paramVals;
   QVector<mu::Parser *> coordinateParsers;

   QVector<QLineF> segments;
   QVector<QColor> colors;
   int maxSegments = 0;

   void updateViewRect( QSize newViewRectSize );
   void updateColors();
   void ParserError( mu::Parser::exception_type &e );

signals:

public slots:

   // QWidget interface
protected:
   void paintEvent( QPaintEvent *event ) override;
   void resizeEvent( QResizeEvent *event ) override;
};

#endif // RENDER_VIEW_HPP
