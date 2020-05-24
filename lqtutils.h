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

#ifndef LQTUTILS_H
#define LQTUTILS_H

// A read-write prop both in C++ and in QML.
#define L_RW_PROP(type, name, setter)                                      \
    public:                                                                \
        type name() const { return m_##name ; }                            \
    public Q_SLOTS:                                                        \
        void setter(type name) {                                           \
            if (m_##name == name) return;                                  \
            m_##name = name;                                               \
            emit name##Changed(name);                                      \
        }                                                                  \
    Q_SIGNALS:                                                             \
        void name##Changed(type name);                                     \
    private:                                                               \
        Q_PROPERTY(type name READ name WRITE setter NOTIFY name##Changed); \
        type m_##name;

// A read-write prop from C++, but read-only from QML.
#define L_RO_PROP(type, name, setter)                         \
    public:                                                   \
        type name() const { return m_##name ; }               \
    public Q_SLOTS:                                           \
        void setter(type name) {                              \
            if (m_##name == name) return;                     \
            m_##name = name;                                  \
            emit name##Changed(name);                         \
        }                                                     \
    Q_SIGNALS:                                                \
        void name##Changed(type name);                        \
    private:                                                  \
        Q_PROPERTY(type name READ name NOTIFY name##Changed); \
        type m_##name;

#if defined Q_OS_BLACKBERRY || defined Q_OS_ANDROID || defined Q_OS_IOS || defined Q_OS_WP
#define L_OS_MOBILE
#else
#define L_OS_DESKTOP
#endif

#ifndef QSL
#define QSL QStringLiteral
#endif // QSL

#endif // LQTUTILS_H
