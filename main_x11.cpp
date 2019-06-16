#include <KToolInvocation>
#include <KWindowSystem>

#include <QCoreApplication>
#include <QTest>
#include <QProcess>


class DaveTest : public QObject
{
    Q_OBJECT
public:
    DaveTest();
private slots:
    void testQProcess();
    void testKRun();
private:
    void initServer();
    void waitForWindow();
};

DaveTest::DaveTest()
{
}

void DaveTest::testQProcess()
{
    initServer();
    QBENCHMARK {
        QProcess::startDetached("dolphin");
        waitForWindow();
    };
    QProcess::execute("killall -9 dolphin");
    QTest::qSleep(1000);
}

void DaveTest::testKRun()
{
    initServer();
    QBENCHMARK {
        int pid;
        KToolInvocation::startServiceByDesktopName("dolphin", QString(), 0, 0, &pid, QByteArray(), true);
        waitForWindow();
    }
    QProcess::execute("killall -9 dolphin");
    QTest::qSleep(1000);
}



void DaveTest::waitForWindow()
{
    QEventLoop e;
    connect(KWindowSystem::self(), &KWindowSystem::windowAdded, &e, &QEventLoop::quit);
    e.exec();
}

void DaveTest::initServer()
{
}

QTEST_GUILESS_MAIN(DaveTest);

#include "main_x11.moc"
