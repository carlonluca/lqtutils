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

class LTestSettings : public QObject
{
    Q_OBJECT
public:
    static LTestSettings& instance() {
        static LTestSettings _instance;
        return _instance;
    }

    L_DEFINE_VALUE(QString, string1, QString("string1"), toString)

protected:
    LTestSettings(QObject* parent = nullptr) : QObject(parent) {}

protected:
    QSettings m_settings;
};

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

}

QTEST_APPLESS_MAIN(LQtUtilsTest)

#include "tst_lqtutils.moc"
