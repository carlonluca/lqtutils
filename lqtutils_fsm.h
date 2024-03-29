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

#ifndef LQTUTILS_FSM_H
#define LQTUTILS_FSM_H

#include <QState>
#include <QDebug>
#include <QString>

#include "lqtutils_prop.h"

namespace lqt {

class LoggedState : public QState
{
    Q_OBJECT
    L_RW_PROP(QString, stateName, setStateName)
public:
    LoggedState(QState::ChildMode childMode, QString name = QString(), QState* parent = nullptr) :
        QState(childMode, parent) { m_stateName = name; init(); }
    LoggedState(QString name = QString(), QState* parent = nullptr) :
        QState(parent) { m_stateName = name; init(); }

protected:
    virtual void init() {
        connect(this, &QState::entered, this, [this] {
            qDebug() << "Entered state:" << stateName();
        });
    }
};

} // namespace

#endif // LQTUTILS_FSM_H
