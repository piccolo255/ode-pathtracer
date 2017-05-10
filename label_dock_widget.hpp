#ifndef LABEL_DOCK_WIDGET_HPP
#define LABEL_DOCK_WIDGET_HPP

// Qt headers
#include <QtDebug>
#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QComboBox>
#include <QResizeEvent>

// C++ headers
#include <iostream>

// Local headers
#include "ode_pathtracer.hpp"

class LabelDockWidget : public QWidget
{
   Q_OBJECT
public:
   explicit LabelDockWidget( QStringList paramNames
                           , QWidget *parent = 0 );
   ~LabelDockWidget();

   void addParamLabel( QString name
                     , bool    removable = true );

signals:

public slots:
   void updateParamLabels( PointValues values );

private:
   // Elements
   QGridLayout *labelLayout;

   // Names
   QStringList paramNames;
   QStringList labelNames;

   // Relationships
   QMap<QString, int> labelParamIndex;
   QMap<QString, QLabel*> valueLabels;

protected:
   void resizeEvent( QResizeEvent *event ) override;
};

#endif // LABEL_DOCK_WIDGET_HPP
