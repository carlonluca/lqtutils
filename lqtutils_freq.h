/**
 * MIT License
 *
 * Copyright (c) 2022 Luca Carlon
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

#ifndef LQTUTILS_FREQ_H
#define LQTUTILS_FREQ_H

#include <QObject>
#include <QDateTime>
#include <QList>
#include <QMutex>
#include <QtGlobal>
#include <QTimer>
#include <QMutableListIterator>

#include "lqtutils_prop.h"

QT_FORWARD_DECLARE_CLASS(QTimer);

namespace lqt {

#define AUTO_REFRESH_INTERVAL 1000

class FreqMeter : public QObject
{
    Q_OBJECT
    L_RO_PROP_AS(int, freq, 0)
public:
    explicit FreqMeter(QObject* parent = nullptr) :
        QObject(parent) {
        m_refreshTimer = new QTimer(this);
        connect(m_refreshTimer, &QTimer::timeout,
                this, &FreqMeter::refresh);
        m_refreshTimer->setInterval(AUTO_REFRESH_INTERVAL);
        m_refreshTimer->setSingleShot(true);
        m_refreshTimer->start();
    }

public slots:
    void registerSample() {
        QMutexLocker locker(&m_mutex);
        m_timestamps.append(QDateTime::currentDateTime());
        QTimer::singleShot(0, this, &FreqMeter::refresh);
    }

    void refresh() {
        QMutexLocker locker(&m_mutex);
        QDateTime now = QDateTime::currentDateTime();
        QMutableListIterator<QDateTime> it(m_timestamps);
        while (it.hasNext()) {
            if (it.next().msecsTo(now) > 1000)
                it.remove();
            else
                break;
        }
        set_freq(m_timestamps.size());
        m_refreshTimer->stop();
        m_refreshTimer->start();
    }

private:
    QMutex m_mutex;
    QList<QDateTime> m_timestamps;
    QTimer* m_refreshTimer;
};

#undef AUTO_REFRESH_INTERVAL

} // namespace

#endif // LQTUTILS_FREQ_H
