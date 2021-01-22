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

#include "qquickplatformicon_p.h"

QT_BEGIN_NAMESPACE

QUrl QQuickPlatformIcon::source() const
{
    return m_source;
}

void QQuickPlatformIcon::setSource(const QUrl& source)
{
    m_source = source;
}

QString QQuickPlatformIcon::name() const
{
    return m_name;
}

void QQuickPlatformIcon::setName(const QString& name)
{
    m_name = name;
}

bool QQuickPlatformIcon::isMask() const
{
    return m_mask;
}

void QQuickPlatformIcon::setMask(bool mask)
{
    m_mask = mask;
}

bool QQuickPlatformIcon::operator==(const QQuickPlatformIcon &other) const
{
    return m_source == other.m_source && m_name == other.m_name && m_mask == other.m_mask;
}

bool QQuickPlatformIcon::operator!=(const QQuickPlatformIcon &other) const
{
    return !(*this == other);
}

QT_END_NAMESPACE
