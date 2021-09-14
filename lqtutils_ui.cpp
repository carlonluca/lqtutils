#include <QTimer>
#include <QQuickWindow>

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
    for (int i = m_timestamps.size() - 1; i >= 0; i--)
        if (m_timestamps[i].msecsTo(now) > 1000)
            m_timestamps.removeAt(i);
    set_fps(m_timestamps.size());
    m_refreshTimer->stop();
    m_refreshTimer->start();
}
