#ifndef LTESTSIGNALS_H
#define LTESTSIGNALS_H

#include <QObject>

Q_NAMESPACE

class LTestSignals : public QObject
{
    Q_OBJECT
public:
    explicit LTestSignals(QObject* parent = nullptr);
};

#endif // LTESTSIGNALS_H
