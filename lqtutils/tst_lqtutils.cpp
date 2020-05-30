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

#include "../lqtutils_prop.h"
#include "../lqtutils_string.h"
#include "../lsettings.h"

class LQtUtilsObject : public QObject
{
	Q_OBJECT
    L_RO_PROP(QString, test, setTest, QString())
public:
	LQtUtilsObject(QObject* parent = nullptr) :
		QObject(parent) {}
};

L_DECLARE_SETTINGS(LSettingsTest, new QSettings("settings.ini", QSettings::IniFormat))
L_DEFINE_VALUE(QString, string1, QString("string1"), toString)
L_DEFINE_VALUE(QSize, size, QSize(100, 100), toSize)
L_DEFINE_VALUE(double, temperature, -1, toDouble)
L_DEFINE_VALUE(QByteArray, image, QByteArray(), toByteArray)
L_END_CLASS

class LQtUtilsTest : public QObject
{
	Q_OBJECT
public:
	LQtUtilsTest();
	~LQtUtilsTest();

private slots:
	void test_case1();
    void test_case2();
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
        LSettingsTest& settings = LSettingsTest::instance();
        settings.set_string1("some string");
        settings.set_size(QSize(250, 200));
        settings.set_temperature(36.7);
        settings.set_image(data);
    }

    {
        QSettings settings("settings.ini", QSettings::IniFormat);
        QCOMPARE(settings.value(QSL("string1")), QSL("some string"));
        QCOMPARE(settings.value(QSL("size")).toSize(), QSize(250, 200));
        QCOMPARE(settings.value(QSL("temperature")).toDouble(), 36.7);
        QCOMPARE(settings.value(QSL("image")).toByteArray(), data);
    }

    bool signalEmitted = false;
    QEventLoop loop;
    QTimer::singleShot(0, this, [] {
        LSettingsTest::instance().set_size(QSize(1280, 720));
    });
    connect(&LSettingsTest::instance(), &LSettingsTest::sizeChanged, this, [&signalEmitted] {
        QCOMPARE(LSettingsTest::instance().size(), QSize(1280, 720));
        signalEmitted = true;
    });
    connect(&LSettingsTest::instance(), &LSettingsTest::sizeChanged,
            &loop, &QEventLoop::quit);
    loop.exec();

    QCOMPARE(LSettingsTest::instance().size(), QSize(1280, 720));
    QVERIFY(signalEmitted);
}

QTEST_MAIN(LQtUtilsTest)

#include "tst_lqtutils.moc"
