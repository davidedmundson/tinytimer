#include <KWayland/Server/display.h>
#include <KWayland/Server/datadevicemanager_interface.h>
#include <KWayland/Server/output_interface.h>
#include <KWayland/Server/compositor_interface.h>
#include <KWayland/Server/shell_interface.h>
#include <KWayland/Server/seat_interface.h>

#include <QCoreApplication>
#include <QTest>
#include <QProcess>

#include <QDBusConnection>
#include <QDBusMessage>

#define PROCESS_NAME "dolphin"

class DaveTest : public QObject
{
    Q_OBJECT
public:
    DaveTest();
private slots:
    void testQProcess();
    void testKInit();
private:
    void initServer();
    KWayland::Server::ShellInterface *m_shell = nullptr;
    KWayland::Server::Display *m_display = nullptr;
};

DaveTest::DaveTest()
{
}

void DaveTest::testQProcess()
{
    initServer();
    QEventLoop e;
    connect(m_shell, &KWayland::Server::ShellInterface::surfaceCreated,
            &e, &QEventLoop::quit);

    QProcess::startDetached(PROCESS_NAME, {"--platform", "wayland"});
    QBENCHMARK {
        e.exec();
    }
    //will crash dolphin which is fine
    delete m_display;
    QTest::qSleep(1000);
}

void DaveTest::testKInit()
{
    initServer();
    QEventLoop e;
    connect(m_shell, &KWayland::Server::ShellInterface::surfaceCreated,
            &e, &QEventLoop::quit);

    auto msg = QDBusMessage::createMethodCall("org.kde.klauncher5", "/KLauncher", "org.kde.KLauncher", "kdeinit_exec");
    msg << PROCESS_NAME;
    msg << QStringList({"--platform", "wayland"});
    msg << QStringList();
    msg << QString();
    QDBusConnection::sessionBus().call(msg, QDBus::NoBlock);
    QBENCHMARK {
        e.exec();
    }
    //will crash dolphin which is fine
    delete m_display;
    QTest::qSleep(1000);
}

void DaveTest::initServer()
{
    qDebug() << "init server";
    using namespace KWayland::Server;
    m_display = new Display;
    m_display->setSocketName(QStringLiteral("wayland-0"));
    m_display->start();
    DataDeviceManagerInterface *ddm = m_display->createDataDeviceManager();
    ddm->create();
    CompositorInterface *compositor = m_display->createCompositor(m_display);
    compositor->create();
    m_shell = m_display->createShell(m_display);
    m_shell->create();
    m_display->createShm();
    OutputInterface *output = m_display->createOutput(m_display);
    output->setPhysicalSize(QSize(269, 202));
    const QSize windowSize(1024, 768);
    output->addMode(windowSize);
    output->create();
    SeatInterface *seat = m_display->createSeat();
    seat->setHasKeyboard(true);
    seat->setHasPointer(true);
    seat->setName(QStringLiteral("testSeat0"));
    seat->create();
}

QTEST_GUILESS_MAIN(DaveTest);

#include "main_wayland.moc"
