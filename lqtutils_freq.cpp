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


#include <QTimer>
#include <QMutableListIterator>

#include "lqtutils_freq.h"

LQTFreqMeter::LQTFreqMeter(QObject* parent) :
    QObject(parent)
{
    m_refreshTimer = new QTimer(this);
    connect(m_refreshTimer, &QTimer::timeout,
            this, &LQTFreqMeter::refresh);
    m_refreshTimer->setInterval(1000);
    m_refreshTimer->setSingleShot(true);
    m_refreshTimer->start();
}

void LQTFreqMeter::registerSample()
{
    m_timestamps.append(QDateTime::currentDateTime());
    refresh();
}

void LQTFreqMeter::refresh()
{
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
