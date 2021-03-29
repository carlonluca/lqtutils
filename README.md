# LQtUtils
This is a module containing a few tools that I typically use in Qt apps.
## lqtutils_prop.h
Contains a few useful macros to synthetize Qt props. For instance:
```c++
class Fraction : public QObject
{
    Q_OBJECT
    L_RW_PROP(double, numerator, setNumerator, 5)
    L_RW_PROP(double, denominator, setDenominator, 9)
public:
    Fraction(QObject* parent = nullptr) : QObject(parent) {}
};
```
instead of:
```c++
class Fraction : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double numerator READ numerator WRITE setNumerator NOTIFY numeratorChanged)
    Q_PROPERTY(double denominator READ denominator WRITE setDenominator NOTIFY denominatorChanged)
public:
    Fraction(QObject* parent = nullptr) :
        QObject(parent),
        m_numerator(5),
        m_denominator(9)
        {}

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
```c++
L_BEGIN_CLASS(Fraction)
L_RW_PROP(double, numerator, setNumerator, 5)
L_RW_PROP(double, denominator, setDenominator, 9)
L_END_CLASS
```
The L_RW_PROP and L_RO_PROP macros are overloaded, and can therefore be cassed with three or four params. The last param is used if you want to init the prop to some specific value automatically.

### References

If you need to be able to modify the property itself from C++ instead of resetting it, you can use the *_REF alternatives of L_RW_PROP and L_RO_PROP. In that case, getter methods return a reference to the type in C++.

### Signals With or Without Parameters

By default, signals are generated with the value passed in the argument. If you prefer signals without params, you can define LQTUTILS_OMIT_ARG_FROM_SIGNAL **before** including the header.

## lqtutils_settings.h
Contains a few tools that can be used to speed up writing simple settings to a file. Settings will still use QSettings and are therefore fully compatible. The macros are simply shortcuts to synthetise code. I only used this for creating ini files, but should work for other formats. An example:
```c++
L_DECLARE_SETTINGS(LSettingsTest, new QSettings("settings.ini", QSettings::IniFormat))
L_DEFINE_VALUE(QString, string1, QString("string1"), toString)
L_DEFINE_VALUE(QSize, size, QSize(100, 100), toSize)
L_DEFINE_VALUE(double, temperature, -1, toDouble)
L_DEFINE_VALUE(QByteArray, image, QByteArray(), toByteArray)
L_END_CLASS

L_DECLARE_SETTINGS(LSettingsTestSec1, new QSettings("settings.ini", QSettings::IniFormat), "SECTION_1")
L_DEFINE_VALUE(QString, string2, QString("string2"), toString)
L_END_CLASS
```
This will provide an interface to a "strong type" settings file containing a string, a QSize value, a double, a jpg image and another string, in a specific section of the ini file. Each class is reentrant like QSettings and can be instantiated in multiple threads.
Each class also provides a unique notifier: the notifier can be used to receive notifications when any thread in the code changes the settings, and can also be used in bindings in QML code. For an example, refer to LQtUtilsQuick:
```c++
Window {
    visible: true
    x: settings.appX
    y: settings.appY
    width: settings.appWidth
    height: settings.appHeight
    title: qsTr("Hello World")

    Connections {
        target: settings
        onAppWidthChanged:
            console.log("App width saved:", settings.appWidth)
        onAppHeightChanged:
            console.log("App width saved:", settings.appHeight)
    }

    Binding { target: settings; property: "appWidth"; value: width }
    Binding { target: settings; property: "appHeight"; value: height }
    Binding { target: settings; property: "appX"; value: x }
    Binding { target: settings; property: "appY"; value: y }
}
```
## lqtutils_enum.h
Contains a macro to define a enum and register it with the meta-object system. This enum can then be exposed to the QML. To create the enum simply do:
```c++
L_DECLARE_ENUM(MyEnum,
               Value1 = 1,
               Value2,
               Value3)
```
This enum is exposed using a namespace without subclassing QObject. Register with the QML engine with:
```c++
qmlRegisterUncreatableMetaObject(MyEnum::staticMetaObject, "con.luke", 1, 0, "MyEnum", "Access to enums & flags only");
```

## lqtutils_fsm.h
A QState subclass that includes a state name and prints it to stdout when each state is entered.

## lqtutils_threading.h
```LQTRecursiveMutex``` is a simple QMutex subclass defaulting to recursive mode.

```INVOKE_AWAIT_ASYNC``` is a wrapper around QMetaObject::invokeMethod that allows to execute a slot or lambda in the thread of an obj, synchronously awaiting for the result. E.g.:
```c++
QThread* t = new QThread;
t->start();
QObject* obj = new QObject;
obj->moveToThread(t);
INVOKE_AWAIT_ASYNC(obj, [&i, currentThread, t] {
    i++;
    QCOMPARE(t, QThread::currentThread());
    QVERIFY(QThread::currentThread() != currentThread);
    QCOMPARE(i, 11);
});
QCOMPARE(i, 11);
```

## lqtutils_autoexec.h
A class that can be used to execute a lambda whenever the current scope ends, e.g.:
```c++
int i = 9;
{
    LQTAutoExec autoexec([&] {
        i++;
    });
    QCOMPARE(i, 9);
}
QCOMPARE(i, 10);
```

```LQTSharedAutoExec```: a class that can create copiable autoexec objects. Useful to implement locks with a function being executed when the lock is released.