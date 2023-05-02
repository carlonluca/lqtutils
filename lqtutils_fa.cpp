/**
 * MIT License
 *
 * Copyright (c) 2023 Luca Carlon
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

#include <QQmlEngine>
#include <QQmlContext>
#include <QFontDatabase>
#include <QQmlComponent>

#include "lqtutils_fa.h"
#include "lqtutils_string.h"

namespace lqt
{

bool embed_font_awesome(QQmlContext* ctx)
{
    if (QFontDatabase::addApplicationFont(QSL(":/lqtutils/fonts/Font Awesome 6 Brands-Regular-400.otf")) == -1) {
        qWarning() << "Could not load font awesome brands regular";
        return false;
    }

    if (QFontDatabase::addApplicationFont(QSL(":/lqtutils/fonts/Font Awesome 6 Free-Regular-400.otf")) == -1) {
        qWarning() << "Could not load font awesome free regular";
        return false;
    }

    if (QFontDatabase::addApplicationFont(QSL(":/lqtutils/fonts/Font Awesome 6 Free-Solid-900.otf")) == -1) {
        qWarning() << "Could not load font awesome free solid";
        return false;
    }

    QQmlEngine* engine = ctx->engine();
    if (!engine) {
        qWarning() << "Cannot find qml engine";
        return false;
    }

    if (ctx) {
        ctx->setContextProperty("fontAwesomeBrandsRegular",
                                QFontDatabase::font("Font Awesome 6 Brands", "Regular", 9));
        ctx->setContextProperty("fontAwesomeFreeRegular",
                                QFontDatabase::font("Font Awesome 6 Free", "Regular", 9));
        ctx->setContextProperty("fontAwesomeFreeSolid",
                                QFontDatabase::font("Font Awesome 6 Free", "Solid", 9));
    }

    return true;
}

}
