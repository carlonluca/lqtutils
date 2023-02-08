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

#ifndef LQTUTILS_LOGGING_H
#define LQTUTILS_LOGGING_H

#include <QDebug>

namespace lqt {

template<typename T>
struct ListOutput
{
    QList<T>& out;
};

template<typename T>
struct SetOutput
{
    QSet<T>& out;
};

template<typename T1, typename T2>
struct HashOutput
{
    QHash<T1, T2>& out;
};


template<typename T>
inline QDebug operator<<(QDebug debug, const ListOutput<T>& c)
{
    QDebugStateSaver saver(debug);
    if (c.out.isEmpty())
        return debug << "( )";
    QListIterator<T> it(c.out);
    debug.noquote().nospace() << "( ";
    while (it.hasNext()) {
        debug.noquote() << it.next();
        if (it.hasNext())
            debug.noquote() << ", ";
    }
    debug.noquote().nospace() << " )";
    return debug;
}

template<typename T>
inline QDebug operator<<(QDebug debug, const SetOutput<T>& c)
{
    QDebugStateSaver saver(debug);
    if (c.out.isEmpty())
        return debug << "{ }";
    QSetIterator<T> it(c.out);
    debug.noquote().nospace() << "{ ";
    while (it.hasNext()) {
        debug.noquote() << it.next();
        if (it.hasNext())
            debug.noquote() << ", ";
    }
    debug.noquote().nospace() << " }";
    return debug;
}

template<typename T1, typename T2>
inline QDebug operator<<(QDebug debug, const HashOutput<T1, T2>& c)
{
    QDebugStateSaver saver(debug);
    if (c.out.isEmpty())
        return debug << "{ }";
    QHashIterator<T1, T2> it(c.out);
    debug.noquote().nospace() << "{ ";
    while (it.hasNext()) {
        const auto& n = it.next();
        debug.noquote().nospace() << "{ " << n.key() << ", " << n.value() << " }";
        if (it.hasNext())
            debug.noquote() << ", ";
    }
    debug.noquote().nospace() << " }";
    return debug;
}

}

#endif
