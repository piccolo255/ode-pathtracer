#include "label_dock_widget.hpp"

LabelDockWidget::LabelDockWidget(
   QStringList paramNames
 , QWidget *parent
) : QWidget(parent){
   this->paramNames = paramNames;

   // create layout
   labelLayout = new QGridLayout( this );
   labelLayout->setColumnStretch( 0, 0 );
   labelLayout->setColumnStretch( 1, 1 );
   labelLayout->setColumnStretch( 2, 0 );

   // combo box for adding new labels
   QComboBox   *addLabelBox = new QComboBox();
   QStringList paramNamesSorted( paramNames );
   paramNamesSorted.sort();
   addLabelBox->addItem( "" );
   addLabelBox->addItems( paramNamesSorted );
   addLabelBox->hide(); // TODO: remove when add-label functionality is implemented

   // spacer to push labels to the top
   QSpacerItem *spacer   = new QSpacerItem( 0, 0, QSizePolicy::Fixed , QSizePolicy::MinimumExpanding );

   // add items
   labelLayout->addWidget( addLabelBox, 0, 1 );
   labelLayout->addItem( spacer,   1, 1 );

   // finalize
   setLayout( labelLayout );
}

LabelDockWidget::~LabelDockWidget(
){
   // stub
}

void LabelDockWidget::addParamLabel(
   QString name
 , bool removable
){
   if( labelParamIndex.contains( name ) ){
      std::cerr << "Can't set label: duplicate parameter '" << name.toStdString() << "'" << std::endl;
      return;
   }

   // set param index
   if( name == "t" ){
      // time parameter is not in the param vector,
      // so it requires special handling
      labelParamIndex[name] = -1;
   } else {
      int index = paramNames.indexOf( QRegExp(name) );
      if( index < 0 ){
         // item not found
         std::cerr << "Can't set label: unknown parameter '" << name.toStdString() << "'" << std::endl;
         return;
      }
      labelParamIndex[name] = index;
   }

   // need to insert a new row above combo box
   int rows = labelLayout->rowCount();

   // move combo box and spacer one down
   QWidget *cb = labelLayout->itemAtPosition( rows-2, 1 )->widget();
   QSpacerItem *spacer = labelLayout->itemAtPosition( rows-1, 1 )->spacerItem();
   labelLayout->removeItem( spacer );
   labelLayout->removeWidget( cb );
   labelLayout->addWidget( cb, rows-1, 1 );
   labelLayout->addItem( spacer, rows, 1 );

   // add new label - title
   QLabel *titleLabel = new QLabel( name, labelLayout->parentWidget() );
   labelLayout->addWidget( titleLabel, rows-2, 0 );

   // add new label - value
   QLabel *valueLabel = new QLabel( "---", labelLayout->parentWidget() );
   valueLabel->setAlignment( Qt::AlignRight | Qt::AlignCenter );
   valueLabel->setTextInteractionFlags( Qt::TextInteractionFlag::TextSelectableByMouse | Qt::TextInteractionFlag::TextSelectableByKeyboard );
   labelLayout->addWidget( valueLabel, rows-2, 1 );

   // add new label - remove box
   if( removable ){
      QLabel *removeLabel = new QLabel( "<font color=red>x</font>", labelLayout->parentWidget() );

      labelLayout->addWidget( removeLabel, rows-2, 2 );
   }

   labelNames.push_back( name );
   valueLabels[name] = valueLabel;
}

void LabelDockWidget::updateParamLabels(
   PointValues values
){
   QString val;
   for( auto name : labelNames ){
      int pi = labelParamIndex[name];
      if( pi < 0 ){
         // time
         val = QString::number( values.T );
      } else {
         val = QString::number( values.Param[pi] );
      }
      valueLabels[name]->setText( val );
   }
}

void LabelDockWidget::resizeEvent(
   QResizeEvent *event
){
   QWidget::resizeEvent( event );
   if( minimumWidth() < labelLayout->minimumSize().width() ){
      this->setMinimumWidth( labelLayout->minimumSize().width() );
   }
   // qDebug() << event->oldSize() << " -> " << event->size() << " /// layout " << labelLayout->sizeHint();
}
