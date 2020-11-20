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

#include "../lqtutils_prop.h"
#include "../lqtutils_string.h"
#include "../lqtutils_settings.h"
#include "../lqtutils_enum.h"
#include "../lqtutils_autoexec.h"
#include "../lqtutils_threading.h"

class LQtUtilsObject : public QObject
{
    Q_OBJECT
    L_RO_PROP(QString, test, setTest, QString())
public:
     LQtUtilsObject(QObject* parent = nullptr) :
     QObject(parent) {}
};

L_BEGIN_CLASS(LPropTest)
L_RW_PROP_REF(QStringList, myListRef, setMyListRef, QStringList() << "hello")
L_RO_PROP(QStringList, myList, setMyList, QStringList() << "hello")
L_END_CLASS

L_DECLARE_SETTINGS(LSettingsTest, new QSettings("settings.ini", QSettings::IniFormat))
L_DEFINE_VALUE(QString, string1, QString("string1"), toString)
L_DEFINE_VALUE(QSize, size, QSize(100, 100), toSize)
L_DEFINE_VALUE(double, temperature, -1, toDouble)
L_DEFINE_VALUE(QByteArray, image, QByteArray(), toByteArray)
L_END_CLASS

L_DECLARE_SETTINGS(LSettingsTestSec1, new QSettings("settings.ini", QSettings::IniFormat), "SECTION_1")
L_DEFINE_VALUE(QString, string2, QString("string2"), toString)
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
};

LQtUtilsTest::LQtUtilsTest() {}
LQtUtilsTest::~LQtUtilsTest() {}

void LQtUtilsTest::test_case1()
{
    const QString s = QSL("HELLOOOOO!");
    LQtUtilsObject test;
    test.setTest(s);
    QVERIFY(test.test() == s);
    test.setTest("HELLO");
    QVERIFY(test.test() != s);
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
        LSettingsTest& settings = LSettingsTest::notifier();
        settings.set_string1("some string");
        settings.set_size(QSize(250, 200));
        settings.set_temperature(36.7);
        settings.set_image(data);

        LSettingsTestSec1 sec1;
        sec1.set_string2(QSL("value_in_section"));
    }

    {
        QSettings settings("settings.ini", QSettings::IniFormat);
        QCOMPARE(settings.value(QSL("string1")), QSL("some string"));
        QCOMPARE(settings.value(QSL("size")).toSize(), QSize(250, 200));
        QCOMPARE(settings.value(QSL("temperature")).toDouble(), 36.7);
        QCOMPARE(settings.value(QSL("image")).toByteArray(), data);
        QCOMPARE(settings.value(QSL("SECTION_1/string2")), "value_in_section");
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
    qmlRegisterUncreatableMetaObject(MyEnum::staticMetaObject, "con.luke", 1, 0, "MyEnum", "Access to enums & flags only");
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
        LQTAutoExec autoexec([&] {
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
        LQTSharedAutoExec lock2;
        {
            LQTSharedAutoExec lock;
            {
                lock = LQTSharedAutoExec([&i] {
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

QTEST_MAIN(LQtUtilsTest)

#include "tst_lqtutils.moc"
