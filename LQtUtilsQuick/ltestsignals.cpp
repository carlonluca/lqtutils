#include <QDebug>

#include "lquicksettings.h"
#include "ltestsignals.h"

LTestSignals::LTestSignals(QObject *parent) : QObject(parent)
{
    connect(&LQuickSettings::instance(), &LQuickSettings::appWidthChanged, this, [] (int width) {
        qDebug() << "C++ app width changed:" << width;
    });
}
