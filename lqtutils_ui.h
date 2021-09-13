/**
 * MIT License
 *
 * Copyright (c) 2021 Luca Carlon
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

#include <QObject>
#include <QQuickWindow>
#include <QDateTime>

#include "lqtutils_prop.h"

class LQTFrameRateMonitor : public QObject
{
    Q_OBJECT
    L_RO_PROP_AS(int, fps, 0)
public:
    LQTFrameRateMonitor(QQuickWindow* w = nullptr, QObject* parent = nullptr) : QObject(parent) {
        if (!w)
            return;
        connect(w, &QQuickWindow::frameSwapped,
                this, &LQTFrameRateMonitor::onFrameSwapped);
    }

private slots:
    void onFrameSwapped() {
        mTimestamps.append(QDateTime::currentDateTime());
        refresh();
    }

private:
    void refresh() {
        QDateTime now = QDateTime::currentDateTime();
        for (int i = mTimestamps.size() - 1; i >= 0; i--)
            if (mTimestamps[i].msecsTo(now) > 1000)
                mTimestamps.removeAt(i);
        set_fps(mTimestamps.size());
    }

private:
    QList<QDateTime> mTimestamps;
};
