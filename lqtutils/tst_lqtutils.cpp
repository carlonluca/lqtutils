#include <QtTest>

#include "../lqtutils.h"

class LQtUtilsObject : public QObject
{
	Q_OBJECT
	L_RO_PROP(QString, test, setTest)
public:
	LQtUtilsObject(QObject* parent = nullptr) :
		QObject(parent) {}
};

class LQtUtilsTest : public QObject
{
	Q_OBJECT
public:
	LQtUtilsTest();
	~LQtUtilsTest();

private slots:
	void test_case1();
};

LQtUtilsTest::LQtUtilsTest()
{}

LQtUtilsTest::~LQtUtilsTest()
{}

void LQtUtilsTest::test_case1()
{
	const QString s = QSL("HELLOOOOO!");
	LQtUtilsObject test;
	test.setTest(s);
	QVERIFY(test.test() == s);
	test.setTest("HELLO");
	QVERIFY(test.test() != s);
}

QTEST_APPLESS_MAIN(LQtUtilsTest)

#include "tst_lqtutils.moc"
