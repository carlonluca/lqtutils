/**
 * MIT License
 *
 * Copyright (c) 2020 Luca Carlon
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 **/

#include <QtTest>
#include <QString>
#include <QImage>
#include <QPainter>
#include <QQmlEngine>
#include <QDebug>
#include <QMetaType>
#include <QDataStream>
#include <QMutableSetIterator>

#include "../lqtutils_prop.h"
#include "../lqtutils_string.h"
#include "../lqtutils_settings.h"
#include "../lqtutils_enum.h"
#include "../lqtutils_autoexec.h"
#include "../lqtutils_threading.h"
#include "../lqtutils_math.h"
#include "../lqtutils_time.h"
#include "../lqtutils_ui.h"
#include "../lqtutils_bqueue.h"
#include "../lqtutils_net.h"
#include "../lqtutils_data.h"
#include "../lqtutils_logging.h"
#include "../lqtutils_system.h"
#ifdef LQT_FONT_AWESOME_ENABLED
#include "../lqtutils_fa.h"
#endif

struct LQTSerializeTest
{
    QString s;
    QImage img;
    int i;
};
Q_DECLARE_METATYPE(LQTSerializeTest)

bool operator==(const LQTSerializeTest& t1, const LQTSerializeTest& t2)
{ return t1.s == t2.s && t1.i == t2.i && t1.img == t2.img; }

QDebug& operator<<(QDebug& debug, const LQTSerializeTest& v)
{ return debug.nospace() << "{" << v.s << ", " << v.i << "}"; }

QDataStream& operator<<(QDataStream& out, const LQTSerializeTest& v)
{ return out << v.s << v.i << v.img; }

QDataStream& operator>>(QDataStream& in, LQTSerializeTest& v)
{
    in >> v.s;
    in >> v.i;
    in >> v.img;
    return in;
}

inline uint qHash(const LQTSerializeTest& key, uint seed)
{ return qHash(key.s, seed) ^ key.i; }

class LQtUtilsObject : public QObject
{
    Q_OBJECT
    L_RW_PROP(QString, rwTest, setRwTest, QString())
    L_RW_PROP_AS(QString, rwTestAuto, QString())
    L_RW_PROP_AS(QString, rwTestAuto2, QString("rwTestAuto2"))
    L_RW_PROP_AS(QString, rwTestAuto3)
    L_RO_PROP_AS(QString, roTestAuto, QString())
    L_RO_PROP_AS(QString, roTestAuto2, QString("roTestAuto2"))
    L_RO_PROP_AS(QString, roTestAuto3)
    L_RO_PROP(QString, test, setTest)
    L_RO_PROP_REF_AS(QString, refRoTestAuto)
    L_RO_PROP_REF_AS(QString, refRoTestAuto2, QString("refRoTestAuto2"))
    L_RW_PROP_CS(QString, csProp, QSL("HELLO"))
    L_RW_PROP_REF_CS(QString, csPropRef, QSL("HELLO"))
    L_RO_PROP_REF_CS(QString, csRoPropRef, QSL("csRoPropRef"))
public:
    LQtUtilsObject(QObject* parent = nullptr) : QObject(parent) {}

public slots:
    // Custom setter.
    void set_csProp(const QString& prop) {
        m_csProp = prop;
        m_test = QStringLiteral("csProp");
    }

    void set_csPropRef(const QString& prop) {
        m_csPropRef = prop;
        m_test = QSL("csPropRef");
    }

    void set_csRoProp(const QString& prop) {
        m_csRoPropRef = prop;
        m_test = QSL("csRoPropRef");
    }
};

L_BEGIN_GADGET(LQtUtilsGadget)
L_RO_GPROP_AS(int, someInteger, 5)
L_RO_GPROP_AS(int, someInteger2, 0)
L_RW_GPROP_AS(int, someRwInteger, 6)
L_RW_GPROP_AS(int, someRwInteger2, 0)
L_RO_GPROP_CS(QString, csRoProp, QSL("csRoProp"))
L_RW_GPROP_CS(QString, csRwProp, QSL("csRwProp"))
public:
    void set_csRoProp(const QString&) { set_someInteger(5); }
    void set_csRwProp(const QString&) { set_someInteger(6); }
L_END_GADGET

L_BEGIN_CLASS(LPropTest)
L_RW_PROP_REF(QStringList, myListRef, setMyListRef, QStringList() << "hello")
L_RO_PROP(QStringList, myList, setMyList, QStringList() << "hello")
L_END_CLASS

L_DECLARE_SETTINGS(LSettingsTest, new QSettings("settings.ini", QSettings::IniFormat))
L_DEFINE_VALUE(QString, string1, QString("string1"))
L_DEFINE_VALUE(QSize, size, QSize(100, 100))
L_DEFINE_VALUE(double, temperature, -1)
L_DEFINE_VALUE(QByteArray, image, QByteArray())
L_DEFINE_VALUE(LQTSerializeTest, customVariant, QVariant::fromValue(LQTSerializeTest()))
L_END_CLASS

L_DECLARE_SETTINGS(LSettingsTestSec1, new QSettings("settings.ini", QSettings::IniFormat), "SECTION_1")
L_DEFINE_VALUE(QString, string2, QString("string2"))
L_DEFINE_VALUE(QString, string3, QString("string3"))
L_DEFINE_VALUE(QString, string4, QString("string4"))
L_END_CLASS

L_DECLARE_ENUM(MyEnum,
               Value1 = 1,
               Value2,
               Value3)

class LQtUtilsTest : public QObject
{
    Q_OBJECT
public:
    LQtUtilsTest();
    ~LQtUtilsTest();

private slots:
    void test_case1();
    void test_case2();
    void test_case3();
    void test_case4();
    void test_case5();
    void test_case6();
    void test_case7();
    void test_case8();
    void test_case9();
    void test_case10();
    void test_case11();
    void test_case12();
    void test_case13();
    void test_case14();
    void test_case15();
    void test_case16();
    void test_case17();
    void test_case18();
    void test_case19();
    void test_case20();
    void test_case21();
    void test_case22();
    void test_case23();
    void test_case24();
    void test_case25();
    void test_case25_2();
    void test_case26();
    void test_case27();
    void test_case28();
    void test_case29();
    void test_case30();
    void test_case31();
    void test_case32();
#ifdef LQT_FONT_AWESOME_ENABLED
    void test_case33();
#endif
};

LQtUtilsTest::LQtUtilsTest()
{
    qRegisterMetaType<LQTSerializeTest>();
}

LQtUtilsTest::~LQtUtilsTest() {}

void LQtUtilsTest::test_case1()
{
    const QString s = QSL("HELLOOOOO!");
    LQtUtilsObject test;
    test.setTest(s);
    connect(&test, &LQtUtilsObject::testChanged, this, [] (QString) {
        qDebug() << "Valued changed";
    });
    QVERIFY(test.test() == s);
    test.setTest("HELLO");
    QVERIFY(test.test() != s);
    test.set_rwTestAuto("HELLO AUTOSET");
    QVERIFY(test.rwTestAuto() == "HELLO AUTOSET");
    QVERIFY(test.rwTestAuto2() == "rwTestAuto2");
    QVERIFY(test.rwTestAuto3().isNull());
    test.set_roTestAuto("roTestAuto");
    QVERIFY(test.roTestAuto2() == "roTestAuto2");
    QVERIFY(test.roTestAuto3().isNull());
    QVERIFY(test.refRoTestAuto2() == "refRoTestAuto2");

    test.set_csProp(QSL("csProp"));
    QVERIFY(test.test() == QSL("csProp"));
    QVERIFY(test.csProp() == QSL("csProp"));

    test.set_csPropRef(QSL("csPropRef"));
    QVERIFY(test.test() == QSL("csPropRef"));
    QVERIFY(test.csPropRef() == QSL("csPropRef"));
    test.csPropRef().replace(QSL("cs"), QSL("helloCs"));
    QVERIFY(test.csPropRef() == QSL("helloCsPropRef"));

    LQtUtilsGadget gadget;
    QVERIFY(gadget.someInteger() == 5);
    QVERIFY(gadget.someRwInteger() == 6);
    gadget.set_someInteger2(10);
    gadget.set_someRwInteger2(11);
    QVERIFY(gadget.someInteger2() == 10);
    QVERIFY(gadget.someRwInteger2() == 11);
    gadget.set_csRoProp("");
    QVERIFY(gadget.someInteger() == 5);
    gadget.set_csRwProp("");
    QVERIFY(gadget.someInteger() == 6);
}

void LQtUtilsTest::test_case2()
{
    QImage image(100, 100, QImage::Format_ARGB32);
    QByteArray data;
    QBuffer buffer(&data);
    buffer.open(QIODevice::WriteOnly);
    QPainter p(&image);
    p.setBrush(Qt::red);
    p.drawRect(image.rect());
    image.save(&buffer, "jpg");
    QVERIFY(data.size() != 0);

    {
        QSettings settings("settings.ini", QSettings::IniFormat);
        settings.clear();
    }

    {
        LSettingsTest& settings = LSettingsTest::notifier();
        settings.set_string1("some string");
        settings.set_size(QSize(250, 200));
        settings.set_temperature(36.7);
        settings.set_image(data);

        LSettingsTestSec1 sec1;
        sec1.set_string2(QSL("value_in_section"));
        QCOMPARE(sec1.string3(true), QSL("string3"));
        sec1.set_string3(QSL("value_in_section3"));
        QCOMPARE(sec1.string3(true), QSL("value_in_section3"));
        QCOMPARE(sec1.string4(true), QSL("string4"));
    }

    {
        QSettings settings("settings.ini", QSettings::IniFormat);
        QCOMPARE(settings.value(QSL("string1")), QSL("some string"));
        QCOMPARE(settings.value(QSL("size")).toSize(), QSize(250, 200));
        QCOMPARE(settings.value(QSL("temperature")).toDouble(), 36.7);
        QCOMPARE(settings.value(QSL("image")).toByteArray(), data);
        QCOMPARE(settings.value(QSL("SECTION_1/string2")), "value_in_section");
        QCOMPARE(settings.value(QSL("SECTION_1/string3")), "value_in_section3");
        QCOMPARE(settings.value(QSL("SECTION_1/string4")), "string4");
    }

    bool signalEmitted = false;
    QEventLoop loop;
    QTimer::singleShot(0, this, [] {
        LSettingsTest::notifier().set_size(QSize(1280, 720));
    });
    connect(&LSettingsTest::notifier(), &LSettingsTest::sizeChanged, this, [&signalEmitted] {
        QCOMPARE(LSettingsTest::notifier().size(), QSize(1280, 720));
        signalEmitted = true;
    });
    connect(&LSettingsTest::notifier(), &LSettingsTest::sizeChanged,
            &loop, &QEventLoop::quit);
    loop.exec();

    QCOMPARE(LSettingsTest::notifier().size(), QSize(1280, 720));
    QVERIFY(signalEmitted);
}

void LQtUtilsTest::test_case3()
{
    QMutex mutex;
    int count = 0;

    QEventLoop loop;

    QThreadPool::globalInstance()->setMaxThreadCount(500);
    for (int i = 0; i < 1000; i++) {
        QThreadPool::globalInstance()->start(QRunnable::create([&mutex, &count, &loop] {
            LSettingsTest settings;
            settings.set_string1(QSL("runnable1"));
            settings.set_temperature(35.5);

            QMutexLocker locker(&mutex);
            count++;
            if (count >= 2000)
                loop.quit();
        }));
        QThreadPool::globalInstance()->start(QRunnable::create([&mutex, &count, &loop] {
            LSettingsTest settings;
            settings.set_string1(QSL("runnable2"));
            settings.set_temperature(37.5);

            QMutexLocker locker(&mutex);
            count++;
            if (count >= 2000)
                loop.quit();
        }));
    }

    loop.exec();

    {
        QSettings settings("settings.ini", QSettings::IniFormat);
        QVERIFY(settings.value(QSL("string1")) == QSL("runnable1") ||
                settings.value(QSL("string1")) == QSL("runnable2"));
        QVERIFY(settings.value(QSL("temperature")).toDouble() == 35.5 ||
                settings.value(QSL("temperature")).toDouble() == 37.5);
    }
}

void LQtUtilsTest::test_case4()
{
    QBENCHMARK {
        for (int i = 0; i < 1000; i++) {
            LSettingsTestSec1 sec1;
            sec1.set_string2(QSL("value"));
        }
    }
}

void LQtUtilsTest::test_case5()
{
    QBENCHMARK {
        for (int i = 0; i < 1000; i++) {
            QSettings settings("settings.ini", QSettings::IniFormat);
            settings.setValue(QSL("string2"), QSL("value"));
        }
    }
}

void LQtUtilsTest::test_case6()
{
    QBENCHMARK {
        for (int i = 0; i < 1000; i++) {
            LSettingsTestSec1 sec1;
            QCOMPARE(sec1.string2(), QSL("value"));
        }
    }
}

void LQtUtilsTest::test_case7()
{
    QBENCHMARK {
        for (int i = 0; i < 1000; i++) {
            QSettings settings("settings.ini", QSettings::IniFormat);
            QCOMPARE(settings.value(QSL("string2")), QSL("value"));
        }
    }
}

void LQtUtilsTest::test_case8()
{
    MyEnum::qmlRegisterMyEnum("com.luke", 1, 0);
    QCOMPARE(MyEnum::Value1, 1);
    QCOMPARE(MyEnum::Value2, 2);
    QCOMPARE(QMetaEnum::fromType<MyEnum::Value>().valueToKey(MyEnum::Value3), QSL("Value3"));
}

void LQtUtilsTest::test_case9()
{
    LPropTest test;
    test.myList().append("Luca");

    LPropTest testRef;
    testRef.myListRef().append("Luca");

    QCOMPARE(test.myList().size(), 1);
    QCOMPARE(test.myList().at(0), "hello");
    QCOMPARE(testRef.myListRef().size(), 2);
    QCOMPARE(testRef.myListRef().at(0), "hello");
    QCOMPARE(testRef.myListRef().at(1), "Luca");
}

void LQtUtilsTest::test_case10()
{
    int i = 9;
    {
        lqt::AutoExec autoexec([&] {
            i++;
        });
        QCOMPARE(i, 9);
    }
    QCOMPARE(i, 10);
}

void LQtUtilsTest::test_case11()
{
    int i = 9;
    QThread* currentThread = QThread::currentThread();
    INVOKE_AWAIT_ASYNC(this, [&i, currentThread] {
        i++;
        QCOMPARE(QThread::currentThread(), currentThread);
        QCOMPARE(i, 10);
    });
    QCOMPARE(i, 10);

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
}

void LQtUtilsTest::test_case12()
{
    int i = 9;

    {
        lqt::SharedAutoExec lock2;
        {
            lqt::SharedAutoExec lock;
            {
                lock = lqt::SharedAutoExec([&i] {
                    i++;
                });
                QCOMPARE(i, 9);
            }
            QCOMPARE(i, 9);
            lock2 = lock;
        }
        QCOMPARE(i, 9);
    }
    QCOMPARE(i, 10);
}

void LQtUtilsTest::test_case13()
{
    QVERIFY(lqt::is_in(0, -1, 1));
    QVERIFY(lqt::is_in(19.0, -10.0, 19.1));
    QVERIFY(lqt::is_in(QSL("f"), QSL("a"), QSL("s")));
    QVERIFY(lqt::is_in(QDateTime::currentDateTime(),
                       QDateTime::currentDateTime().addDays(-4),
                       QDateTime::currentDateTime().addSecs(1)));
    QVERIFY(!lqt::is_in(QDateTime::currentDateTime(),
                        QDateTime::currentDateTime().addDays(4),
                        QDateTime::currentDateTime().addSecs(1)));
    QVERIFY(lqt::nearest_in_range(1.1, 2., 4.) == 2);
    QVERIFY(lqt::nearest_in_range(5, 1, 2) == 2);
    QVERIFY(lqt::nearest_in_range(3, 1, 3) == 3);
    QVERIFY(lqt::nearest_in_range(2, 1, 3) == 2);
}

void LQtUtilsTest::test_case14()
{
    QDateTime now = QDateTime::currentDateTime();
    QVERIFY(lqt::today().date() == now.date());
    QVERIFY(lqt::today().time() == QTime(0, 0, 0, 0));
    QVERIFY(lqt::tomorrow().date() != now.date());
    QVERIFY(lqt::tomorrow().time() == QTime(0, 0, 0, 0));
    QVERIFY(lqt::today().date().day() == lqt::tomorrow().date().day() - 1);
    // May fail a couple of times a year.
    QVERIFY(lqt::today().msecsTo(lqt::tomorrow()) == 1000*60*60*24);
}

void LQtUtilsTest::test_case15()
{
    QScopedPointer<lqt::FrameRateMonitor> mon(new lqt::FrameRateMonitor);
}

void LQtUtilsTest::test_case16()
{
    QRectF r(10, 20, 123.45, 678.90);
    QString s = lqt::string_from_rect(r);
    QVERIFY(s == QSL("10,20,123.45,678.9"));
    QRectF r2 = lqt::string_to_rect(s);
    QVERIFY(qFuzzyCompare(r.x(), r2.x()));
    QVERIFY(qFuzzyCompare(r.y(), r2.y()));
    QVERIFY(qFuzzyCompare(static_cast<float>(r.width()), static_cast<float>(r2.width())));
    QVERIFY(qFuzzyCompare(static_cast<float>(r.height()), static_cast<float>(r2.height())));
}

struct LQTTestProducer : public QThread
{
    LQTTestProducer(lqt::BlockingQueue<int>* queue) : QThread(), m_queue(queue) {}
    void run() override {
        static int i = 0;
        while (!isInterruptionRequested()) {
            QThread::msleep(10);
            m_queue->enqueue(i++);
            QVERIFY(m_queue->size() <= 10);
        }
    }
    void requestDispose() {
        requestInterruption();
        m_queue->requestDispose();
    }

private:
    lqt::BlockingQueue<int>* m_queue;
};

struct LQTTestConsumer : public QThread
{
    LQTTestConsumer(lqt::BlockingQueue<int>* queue) : QThread(), m_queue(queue) {}
    void run() override {
        static int i = 0;
        while (!isInterruptionRequested()) {
            QThread::msleep(15);
            std::optional<int> ret = m_queue->dequeue();
            if (!ret)
                return;
            QVERIFY(*ret == i++);
            QVERIFY(lqt::is_in(m_queue->size(), 0, 10));
        }
    }
    void requestDispose() {
        requestInterruption();
        m_queue->requestDispose();
    }

private:
    lqt::BlockingQueue<int>* m_queue;
};

void LQtUtilsTest::test_case17()
{
    lqt::BlockingQueue<int> queue(10, QSL("display_name"));
    LQTTestConsumer consumer(&queue);
    LQTTestProducer producer(&queue);
    consumer.start();
    producer.start();

    QEventLoop loop;
    QTimer::singleShot(30*1000, this, [&] { loop.quit(); });
    loop.exec();

    producer.requestDispose();
    producer.wait();
    consumer.requestDispose();
    consumer.wait();
}

void LQtUtilsTest::test_case18()
{
    lqt::BlockingQueue<int> queue(2);
    queue.enqueue(0);
    queue.enqueue(1);
    QVERIFY(queue.size() == 2);
    QVERIFY(!queue.enqueue(2, 5));
    QVERIFY(queue.size() == 2);

    queue.lockQueue([] (QList<int>* queue) {
        QVERIFY((*queue)[0] == 0);
        QVERIFY((*queue)[1] == 1);
    });

    QVERIFY(queue.enqueueDropFirst(2, 0));
    QVERIFY(queue.size() == 2);

    queue.lockQueue([] (QList<int>* queue) {
        QVERIFY((*queue)[0] == 1);
        QVERIFY((*queue)[1] == 2);
    });

    queue.dequeue();
    queue.dequeue();
    QVERIFY(queue.dequeue(0) == std::nullopt);

    for (int i = 0; i < 1E5; i++)
        queue.dequeue(0);
    QVERIFY(queue.isEmpty());
}

void LQtUtilsTest::test_case19()
{
    QVERIFY(qFuzzyCompare(lqt::string_to_float(QSL("1.6"), 1.6), 1.6f));
    QVERIFY(qFuzzyCompare(lqt::string_to_float(QSL("abc"), 1.6), 1.6f));
    QVERIFY(lqt::string_to_int(QSL("5"), 5) == 5);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverflow"
    QVERIFY(lqt::string_to_int(QString("%1").arg(std::numeric_limits<qint64>::max() + 100), 1) == 1);
    QVERIFY(lqt::string_to_int64(QString("%1").arg(std::numeric_limits<qint64>::max() + 100),
                            1) == std::numeric_limits<qint64>::max() + 100);
#pragma GCC diagnostic pop
}

void LQtUtilsTest::test_case20()
{
    QThread t;
    t.start();
    QEventLoop loop;
    QVERIFY(&t != QThread::currentThread());
    lqt::run_in_thread(&t, [&loop, &t] {
        QVERIFY(&t == QThread::currentThread());
        loop.quit();
    });
    loop.exec();
    t.quit();
    t.wait();
}

void LQtUtilsTest::test_case21()
{
    lqt::CacheValue<bool> boolCache;
    QVERIFY(boolCache.value("value1", [] () -> bool { return true; }) == true);
    QVERIFY(boolCache.value("value1", [] () -> bool { return false; }) == true);
    QVERIFY(boolCache.value("value2", [] () -> bool { return false; }) == false);
    QVERIFY(boolCache.value("value2", [] () -> bool { return true; }) == false);
    QVERIFY(boolCache.isSet("value1"));
    QVERIFY(boolCache.isSet("value2"));
    QVERIFY(!boolCache.isSet("value3"));
    boolCache.reset("value1");
    boolCache.reset("value2");
    QVERIFY(!boolCache.isSet("value1"));
    QVERIFY(!boolCache.isSet("value2"));
    boolCache.setValue("value1", false);
    QVERIFY(!boolCache.value("value1", [] { return false; }) == true);

    lqt::CacheValue<MyEnum::Value> enumCache;
    QVERIFY(enumCache.value("value1", [] () -> MyEnum::Value { return MyEnum::Value1; }) == MyEnum::Value1);
    QVERIFY(enumCache.value("value1", [] () -> MyEnum::Value { return MyEnum::Value2; }) == MyEnum::Value1);
    QVERIFY(enumCache.value("value2", [] () -> MyEnum::Value { return MyEnum::Value1; }) == MyEnum::Value1);
    QVERIFY(enumCache.value("value2", [] () -> MyEnum::Value { return MyEnum::Value2; }) == MyEnum::Value1);
    QVERIFY(enumCache.value("value3", [] () -> MyEnum::Value { return MyEnum::Value3; }) == MyEnum::Value3);
}

void LQtUtilsTest::test_case22()
{
    QThread* t = new QThread;
    t->start();

    QVERIFY(t != QThread::currentThread());

    lqt::run_in_thread_sync(t, [t] {
        QVERIFY(t == QThread::currentThread());
    });

    QObject* o = new QObject;
    o->moveToThread(t);
    lqt::run_in_thread_sync(o, [t] {
        QVERIFY(t == QThread::currentThread());
        return;
    });

    int testValue = 1;
    QVERIFY(lqt::run_in_thread_sync<bool>(t, [&testValue] () -> bool {
        QThread::sleep(3);
        testValue = 5;
        return true;
    }));
    QVERIFY(testValue == 5);

    QVERIFY(lqt::run_in_thread_sync<bool>(o, [] () -> bool {
        return true;
    }));

    o->deleteLater();
    t->quit();
    t->wait();
    t->deleteLater();
}

void LQtUtilsTest::test_case23()
{
    QString tmpFilePath = QSL("/tmp/lqtutils_test_tmp.png");
    lqt::AutoExec exec([&tmpFilePath] {
        QFile::remove(tmpFilePath);
    });

    QScopedPointer<lqt::Downloader> downloader(new lqt::Downloader(
                QSL("https://raw.githubusercontent.com/carlonluca/"
                    "isogeometric-analysis/master/2.3/lagrange.svg.png"),
                tmpFilePath));
    QVERIFY(downloader->state() == LQTDownloaderState::S_IDLE);

    downloader->download();
    QVERIFY(downloader->state() == LQTDownloaderState::S_DOWNLOADING);

    QEventLoop loop;
    connect(downloader.data(), &lqt::Downloader::stateChanged, this, [&loop, &downloader] {
        if (downloader->state() == LQTDownloaderState::S_DONE)
            loop.quit();
    });
    loop.exec();

    QVERIFY(downloader->state() == LQTDownloaderState::S_DONE);
}

void LQtUtilsTest::test_case24()
{
    QString tmpFilePath = QSL("/tmp/lqtutils_test_tmp.png");
    lqt::AutoExec exec([&tmpFilePath] {
        QFile::remove(tmpFilePath);
    });

    QScopedPointer<lqt::Downloader> downloader(new lqt::Downloader(
                QSL("https://raw.githubusercontent.com/carlonluca/"
                    "isogeometric-analysis/master/4.5/iga_knot_insertion_plate_hole.svg.png"),
                tmpFilePath));
    QVERIFY(downloader->state() == LQTDownloaderState::S_IDLE);

    downloader->download();

    QEventLoop loop;
    connect(downloader.data(), &lqt::Downloader::downloadProgress, this, [&loop, &downloader] {
        QVERIFY(downloader->state() == LQTDownloaderState::S_DOWNLOADING);
        downloader->abort();
        loop.quit();
    });
    loop.exec();
    QVERIFY(downloader->state() == LQTDownloaderState::S_ABORTED);
}

void LQtUtilsTest::test_case25()
{
    QScopedPointer<lqt::Downloader> downloader(new lqt::Downloader(
                QSL("https://raw.githubusercontent.com/carlonluca/"
                    "isogeometric-analysis/master/4.5/iga_knot_insertion_plate_hole.svg.png"),
                QSL("/tmp/dirthatdoesnotexist/tmp.png")));
    QVERIFY(downloader->state() == LQTDownloaderState::S_IDLE);

    downloader->download();

    QEventLoop loop;
    connect(downloader.data(), &lqt::Downloader::stateChanged, this, [&loop, &downloader] {
        if (downloader->state() == LQTDownloaderState::S_IO_FAILURE) {
            downloader->abort();
            loop.quit();
        }
    });
    loop.exec();
    QVERIFY(downloader->state() == LQTDownloaderState::S_IO_FAILURE);
}

void LQtUtilsTest::test_case25_2()
{
    QByteArray expected;

    {
        QString tmpFilePath = QSL("/tmp/lqtutils_test_tmp.png");
        lqt::AutoExec exec([&tmpFilePath] {
            QFile::remove(tmpFilePath);
        });

        QScopedPointer<lqt::Downloader> downloader(new lqt::Downloader(
            QSL("https://raw.githubusercontent.com/carlonluca/"
                "isogeometric-analysis/master/2.3/lagrange.svg.png"),
            tmpFilePath));
        QVERIFY(downloader->state() == LQTDownloaderState::S_IDLE);

        downloader->download();
        QVERIFY(downloader->state() == LQTDownloaderState::S_DOWNLOADING);

        QEventLoop loop;
        connect(downloader.data(), &lqt::Downloader::stateChanged, this, [&loop, &downloader] {
            if (downloader->state() == LQTDownloaderState::S_DONE)
                loop.quit();
        });
        loop.exec();

        QVERIFY(downloader->state() == LQTDownloaderState::S_DONE);

        QFile f(tmpFilePath);
        QVERIFY(f.open(QIODevice::ReadOnly));
        expected = f.readAll();
    }

    QByteArray received;
    QScopedPointer<lqt::Downloader> downloader(new lqt::Downloader(
        QSL("https://raw.githubusercontent.com/carlonluca/"
            "isogeometric-analysis/master/2.3/lagrange.svg.png"),
        &received));
    QVERIFY(downloader->state() == LQTDownloaderState::S_IDLE);

    downloader->download();

    QEventLoop loop;
    connect(downloader.data(), &lqt::Downloader::stateChanged, this, [&loop, &downloader] {
        if (downloader->state() == LQTDownloaderState::S_DONE) {
            downloader->abort();
            loop.quit();
        }
    });
    loop.exec();
    QVERIFY(downloader->state() == LQTDownloaderState::S_DONE);
    QVERIFY(received == expected);
}

void LQtUtilsTest::test_case26()
{
    QString tmpFilePath = QSL("/tmp/lqtutils_test_tmp.xz");
    lqt::AutoExec exec([&tmpFilePath] {
        QFile::remove(tmpFilePath);
    });

    QScopedPointer<lqt::Downloader> downloader(new lqt::Downloader(
                QSL("https://raw.githubusercontent.com/carlonluca/"
                "isogeometric-analysis/master/4.5/iga_knot_insertion_plate_hole.svg.png"),
                tmpFilePath));
    QVERIFY(downloader->state() == LQTDownloaderState::S_IDLE);

    downloader->download();
    connect(downloader.data(), &lqt::Downloader::downloadProgress, this, [&downloader] (qint64 progress, qint64 total) {
        qDebug() << "Downloading:" << progress << "/" << total << downloader->state();
    });

    QEventLoop loop;
    connect(downloader.data(), &lqt::Downloader::stateChanged, this, [&loop, &downloader] {
        QVERIFY(downloader->state() == LQTDownloaderState::S_DOWNLOADING ||
                downloader->state() == LQTDownloaderState::S_DONE);
        if (downloader->state() == LQTDownloaderState::S_DONE)
            loop.quit();
    });
    loop.exec();
    QVERIFY(downloader->state() == LQTDownloaderState::S_DONE);
    QVERIFY(lqt::hash(tmpFilePath) == QByteArray::fromHex(QSL("b471254ec7c69949125834e107b45dd5").toUtf8()));
}

void LQtUtilsTest::test_case27()
{
    QString tmpFilePath("/tmp/lqtutils_test_tmp");
    lqt::AutoExec([&tmpFilePath] {
        QFile::remove(tmpFilePath);
    });

    lqt::random_file(tmpFilePath, 1024);
    QVERIFY(QFile(tmpFilePath).size() == 1024);
    QByteArray md51 = lqt::hash(tmpFilePath);

    lqt::random_file(tmpFilePath, 2048);
    QVERIFY(QFile(tmpFilePath).size() == 2048);
    QByteArray md52 = lqt::hash(tmpFilePath);

    lqt::random_file(tmpFilePath, 4000);
    QVERIFY(QFile(tmpFilePath).size() == 4000);
    QByteArray md53 = lqt::hash(tmpFilePath);

    QVERIFY(md51 != md52);
    QVERIFY(md51 != md53);
    QVERIFY(md52 != md53);

    qDebug() << "MD5:" << md51.toHex() << md52.toHex() << md53.toHex();
    QVERIFY(QFile(tmpFilePath).remove());
}

void LQtUtilsTest::test_case28()
{
    QVERIFY(lqt::approx_equal(.01f, .01f, .0f));
    QVERIFY(!lqt::approx_equal(.01f, .02f, .0f));
    QVERIFY(lqt::approx_equal(1.f/2, 2.f/4, .0001f));
    QVERIFY(lqt::approx_equal(0.3, 0.1 + 0.2, 0.0005));
}

void LQtUtilsTest::test_case29()
{
    QImage img(100, 100, QImage::Format_ARGB32);
    img.fill(Qt::red);
    LQTSerializeTest t {
        QSL("test"), img, 10
    };

    LSettingsTest().set_customVariant(t);

    LQTSerializeTest t2 = LSettingsTest().customVariant();
    QVERIFY(t.i == t2.i);
    QVERIFY(t.img == t2.img);
    QVERIFY(t.s == t2.s);

    LSettingsTest settings;
    QBENCHMARK {
        for (int i = 0; i < 1E5; i++)
            QVERIFY(settings.size() == QSize(1280, 720));
    }
}

void LQtUtilsTest::test_case30()
{
    LQTSerializeTest t1;
    t1.i = 1;
    t1.s = "Hello";

    LQTSerializeTest t2;
    t2.i = 2;
    t2.s = "Luca";

    QList<LQTSerializeTest> list2 = QList<LQTSerializeTest>() << t1 << t2;
#if QT_VERSION_MAJOR > 5
    QString res = QDebug::toString(lqt::ListOutput<LQTSerializeTest> { list2 });
    QVERIFY(res == "( {Hello, 1}, {Luca, 2} )");
#endif

    // Cannot compare this to a string as the output is not constant.
    QSet<LQTSerializeTest> set = QSet<LQTSerializeTest> {
            t1, t2
    };
    qDebug() << "Set:" << lqt::SetOutput<LQTSerializeTest> { set };

    // Cannot compare this to a string as the output is not constant.
    QHash<QString, LQTSerializeTest> hash = QHash<QString, LQTSerializeTest> {
        { "1", t1 },
        { "2", t2 }
    };
    qDebug() << "Hash:" << lqt::HashOutput<QString, LQTSerializeTest> { hash };
}

void LQtUtilsTest::test_case31()
{
    QSet<int> set = { 1, 2, 3, 4, 5 };
    QMutableSetIterator<int> it(set);
    while (it.hasNext()) {
        const int* any = lqt::get_any(set);
        if (!any) {
            QVERIFY(set.isEmpty());
            break;
        }
        QVERIFY(set.contains(*any));
        set.remove(*any);
    }
}

void LQtUtilsTest::test_case32()
{
    std::optional<lqt::MemData> mem = lqt::read_mem_data();
    QVERIFY(mem.has_value());
    QVERIFY(mem->totalMemBytes > 0);
    QVERIFY(mem->freeMemBytes > 0);
    QVERIFY(mem->totalMemBytes > mem->freeMemBytes);
}

#ifdef LQT_FONT_AWESOME_ENABLED
void LQtUtilsTest::test_case33()
{
    Q_INIT_RESOURCE(lqtutils_fa);
    QVERIFY(lqt::embed_font_awesome(nullptr));
}
#endif

QTEST_GUILESS_MAIN(LQtUtilsTest)

#include "tst_lqtutils.moc"
