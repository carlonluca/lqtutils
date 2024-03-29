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

#ifndef LQTUTILS_MISC_H
#define LQTUTILS_MISC_H

#include <QtGlobal>

#if defined Q_OS_BLACKBERRY || defined Q_OS_ANDROID || defined Q_OS_IOS || defined Q_OS_WP
#define L_OS_MOBILE
#else
#define L_OS_DESKTOP
#endif

inline bool operator!(const QString& str)
{
    return str.isEmpty();
}

namespace lqt {

///
/// \brief coalesce is a C++ implementation of the coalesce operator.
/// \param lhs value to be evaluated: if evaluates to true, then it is
/// returned, otherwise rhs is returned.
/// \param rhs is the value to be returned if lhs does not evaluate
/// to true.
/// \return lhs if true, rhs otherwise.
///
template<typename T>
T coalesce(T lhs, T rhs)
{
    return !lhs ? rhs : lhs;
}

}

#endif // LQTUTILS_MISC_H
