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

#include "lqtutils_prop.h"

QT_FORWARD_DECLARE_CLASS(QTimer);

namespace lqt {

class FreqMeter : public QObject
{
    Q_OBJECT
    L_RO_PROP_AS(int, freq, 0)
public:
    explicit FreqMeter(QObject* parent = nullptr);

public slots:
    void registerSample();
    void refresh();

private:
    QMutex m_mutex;
    QList<QDateTime> m_timestamps;
    QTimer* m_refreshTimer;
};

} // namespace

#endif // LQTUTILS_FREQ_H
