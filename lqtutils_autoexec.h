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

#ifndef LQTUTILS_AUTOEXEC_H
#define LQTUTILS_AUTOEXEC_H

#include <QSharedPointer>

#include <functional>

namespace lqt {

class AutoExec
{
public:
    AutoExec(std::function<void()> func) { reset(func); }
    AutoExec() { reset(nullptr); }
    ~AutoExec() { if (m_func) m_func(); }
    void reset(std::function<void()> func) { m_func = func; }
    void reset() { reset(nullptr); }
private:
    std::function<void()> m_func;
};

class SharedAutoExec
{
public:
    SharedAutoExec(std::function<void()> func) :
        m_exec(new AutoExec(func)) {}
    SharedAutoExec() {}

protected:
    QSharedPointer<AutoExec> m_exec;
};

} // namespace

#endif // LQTUTILS_AUTOEXEC_H
