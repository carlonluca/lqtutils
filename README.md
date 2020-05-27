# LQtUtils
This is a module containing a few tools that I typically use in Qt apps.
## lqtutils_prop.h
Contains a few useful macros to synthetize Qt props. For instance:
```
class Fraction : public QObject
{
    Q_OBJECT
    L_RW_PROP(double, numerator, setNumerator)
    L_RW_PROP(double, denominator, setDenominator)
public:
    Fraction(QObject* parent = nullptr) : QObject(parent) {}
};
```
instead of:
```
class Fraction : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double numerator READ numerator WRITE setNumerator NOTIFY numeratorChanged)
    Q_PROPERTY(double denominator READ denominator WRITE setDenominator NOTIFY denominatorChanged)
public:
    Fraction(QObject* parent = nullptr) : QObject(parent) {}

    double numerator() const {
        return m_numerator;
    }

    double denominator() const {
        return m_denominator;
    }

public slots:
    void setNumerator(double numerator) {
        if (m_numerator == numerator)
            return;
        m_numerator = numerator;
        emit numeratorChanged(numerator);
    }

    void setDenominator(double denominator) {
        if (m_denominator == denominator)
            return;
        m_denominator = denominator;
        emit denominatorChanged(denominator);
    }

signals:
    void numeratorChanged(double numerator);
    void denominatorChanged(double denominator);

private:
    double m_numerator;
    double m_denominator;
};
```
When the QObject subclass is not supposed to have a particular behavior, the two macros L_BEGIN_CLASS and L_END_CLASS can speed up the declaration even more:
```
L_BEGIN_CLASS(Fraction)
L_RW_PROP(double, numerator, setNumerator)
L_RW_PROP(double, denominator, setDenominator)
L_END_CLASS
```