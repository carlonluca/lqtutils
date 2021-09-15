#include <QTimer>
#include <QQuickWindow>
#include <QMutableListIterator>

#include "lqtutils_ui.h"

LQTFrameRateMonitor::LQTFrameRateMonitor(QQuickWindow* w, QObject* parent) :
    QObject(parent)
{
    if (!w)
        return;
    connect(w, &QQuickWindow::frameSwapped,
            this, &LQTFrameRateMonitor::onFrameSwapped);
    m_refreshTimer = new QTimer(this);
    connect(m_refreshTimer, &QTimer::timeout,
            this, &LQTFrameRateMonitor::refresh);
    m_refreshTimer->setInterval(1000);
    m_refreshTimer->setSingleShot(true);
    m_refreshTimer->start();
}

void LQTFrameRateMonitor::onFrameSwapped()
{
    m_timestamps.append(QDateTime::currentDateTime());
    refresh();
}

void LQTFrameRateMonitor::refresh()
{
    QDateTime now = QDateTime::currentDateTime();
    QMutableListIterator<QDateTime> it(m_timestamps);
    while (it.hasNext()) {
        if (it.next().msecsTo(now) > 1000)
            it.remove();
        else
            break;
    }
    set_fps(m_timestamps.size());
    m_refreshTimer->stop();
    m_refreshTimer->start();
}
