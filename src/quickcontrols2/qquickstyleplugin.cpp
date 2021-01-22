/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Quick Controls 2 module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#include "qquickstyleplugin_p.h"
#include "qquickstyle.h"
#include "qquickstyle_p.h"
#include "qquickstyleselector_p.h"

QT_BEGIN_NAMESPACE

class QQuickStylePluginPrivate
{
public:
    mutable QScopedPointer<QQuickStyleSelector> selector;
};

QQuickStylePlugin::QQuickStylePlugin(QObject *parent)
    : QQmlExtensionPlugin(parent), d_ptr(new QQuickStylePluginPrivate)
{
}

QQuickStylePlugin::~QQuickStylePlugin()
{
}

QString QQuickStylePlugin::name() const
{
    return QString();
}

void QQuickStylePlugin::initializeTheme(QQuickTheme *theme)
{
    Q_UNUSED(theme);
}

QUrl QQuickStylePlugin::resolvedUrl(const QString &fileName) const
{
    Q_D(const QQuickStylePlugin);
    if (!d->selector) {
        d->selector.reset(new QQuickStyleSelector);
        const QString style = QQuickStyle::name();
        if (!style.isEmpty())
            d->selector->addSelector(style);

        const QString fallback = QQuickStylePrivate::fallbackStyle();
        if (!fallback.isEmpty() && fallback != style)
            d->selector->addSelector(fallback);

        const QString theme = name();
        if (!theme.isEmpty() && theme != style)
            d->selector->addSelector(theme);

        d->selector->setPaths(QQuickStylePrivate::stylePaths(true));
    }
    return d->selector->select(fileName);
}

QT_END_NAMESPACE
