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

#define GET_MACRO(_1, _2, _3, _4, NAME,...) NAME
#define L_RW_PROP(...) GET_MACRO(__VA_ARGS__, L_RW_PROP4, L_RW_PROP3, L_RW_PROP2)(__VA_ARGS__)
#define L_RO_PROP(...) GET_MACRO(__VA_ARGS__, L_RO_PROP4, L_RO_PROP3, L_RO_PROP2)(__VA_ARGS__)

// A read-write prop both in C++ and in QML.
#define L_RW_PROP4(type, name, setter, def)                                \
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
        type m_##name = def;

#define L_RW_PROP3(type, name, setter)                                     \
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
#define L_RO_PROP4(type, name, setter, def)                   \
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
        type m_##name = def;

#define L_RO_PROP3(type, name, setter, def)                   \
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
        type m_##name = def;

#if defined Q_OS_BLACKBERRY || defined Q_OS_ANDROID || defined Q_OS_IOS || defined Q_OS_WP
#define L_OS_MOBILE
#else
#define L_OS_DESKTOP
#endif

#ifndef QSL
#define QSL QStringLiteral
#endif // QSL

#ifdef __GNUC__
#define LC_LIKELY(x) \
   __builtin_expect((x), 1)
#define LC_UNLIKELY(x) \
   __builtin_expect((x), 0)
#else
#define LC_LIKELY(x) (x)
#define LC_UNLIKELY(x) (x)
#endif // __GNUC__

#endif // LQTUTILS_H
