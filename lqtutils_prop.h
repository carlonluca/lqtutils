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

#ifndef LQTUTILS_PROP_H
#define LQTUTILS_PROP_H

// The EXPAND macro here is only needed for MSVC:
// https://stackoverflow.com/questions/5134523/msvc-doesnt-expand-va-args-correctly
#define EXPAND( x ) x
#define L_PROP_GET_MACRO(_1, _2, _3, _4, NAME,...) NAME
#define L_RW_PROP(...) \
    EXPAND( L_PROP_GET_MACRO(__VA_ARGS__, L_RW_PROP4, L_RW_PROP3, L_RW_PROP2)(__VA_ARGS__) )
#define L_RO_PROP(...) \
    EXPAND( L_PROP_GET_MACRO(__VA_ARGS__, L_RO_PROP4, L_RO_PROP3, L_RO_PROP2)(__VA_ARGS__) )

// A read-write prop both in C++ and in QML.
#define L_RW_PROP_(type, name, setter)                                     \
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
        Q_PROPERTY(type name READ name WRITE setter NOTIFY name##Changed);

#define L_RW_PROP4(type, name, setter, def)                                \
    L_RW_PROP_(type, name, setter)                                         \
    type m_##name = def;

#define L_RW_PROP3(type, name, setter)                                     \
    L_RW_PROP_(type, name, setter)                                         \
    type m_##name;

// A read-write prop from C++, but read-only from QML.
#define L_RO_PROP_(type, name, setter)                        \
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
        Q_PROPERTY(type name READ name NOTIFY name##Changed);

#define L_RO_PROP4(type, name, setter, def)                   \
    L_RO_PROP_(type, name, setter)                            \
    type m_##name = def;

#define L_RO_PROP3(type, name, setter)                        \
    L_RO_PROP_(type, name, setter)                            \
    type m_##name;

// Convenience macros to speed up a QObject subclass.
#define L_BEGIN_CLASS(name)                           \
    class name : public QObject                       \
    {                                                 \
        Q_OBJECT                                      \
    public:                                           \
        Q_INVOKABLE name(QObject* parent = nullptr) : \
            QObject(parent) {}                        \
    private:

#define L_END_CLASS };

#endif // LQTUTILS_PROP_H
