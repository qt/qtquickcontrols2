/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Labs Platform module of the Qt Toolkit.
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

#include "qquickplatformmenuseparator_p.h"

QT_BEGIN_NAMESPACE

/*!
    \qmltype MenuSeparator
    \inherits MenuItem
//!     \instantiates QQuickPlatformMenuSeparator
    \inqmlmodule Qt.labs.platform
    \since 5.8
    \brief A native menu separator.

    The MenuSeparator type is provided for convenience. It is a MenuItem
    that has the \l {MenuItem::}{separator} property set to \c true by default.

    \image qtlabsplatform-menubar.png

    \labs

    \sa Menu, MenuItem
*/

QQuickPlatformMenuSeparator::QQuickPlatformMenuSeparator(QObject *parent)
    : QQuickPlatformMenuItem(parent)
{
    setSeparator(true);
}

QT_END_NAMESPACE
