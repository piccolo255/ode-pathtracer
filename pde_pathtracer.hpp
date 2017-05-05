#ifndef PDE_PATH_TRACER_HPP
#define PDE_PATH_TRACER_HPP

// Qt headers
#include <QtDebug>
#include <QVector>
#include <QPair>

typedef struct {
   double T;
   QVector<double> Val;
   QVector<double> Param;
} PointValues;
Q_DECLARE_METATYPE( PointValues )

typedef QPair<QString, QString> Equation; // parameter name and equation
typedef QVector<Equation> EquationVector;

typedef QPair<QString, QString> Derivation; // variable name and d/dt
typedef QVector<Derivation> DerivationVector;

#endif // PDE_PATH_TRACER_HPP
