/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Labs Calendar module of the Qt Toolkit.
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

#include "qquickcalendar_p.h"

QT_BEGIN_NAMESPACE

/*!
    \qmltype Calendar
    \inherits QObject
//! \instantiates QQuickCalendar
    \inqmlmodule Qt.labs.calendar
    \brief A calendar namespace.

    The Calendar singleton provides miscellaneous calendar related
    utilities.

    The Qt Labs Calendar module uses 0-based month numbers to be consistent
    with the JavaScript Date type, that is used by the QML language. This
    means that \c Date::getMonth() can be assigned to MonthGrid::month and
    WeekNumberColumn::month as is. When dealing with month numbers
    directly, it is highly recommended to use the following enumeration values
    to avoid confusion.

    \value Calendar.January January (0)
    \value Calendar.February February (1)
    \value Calendar.March March (2)
    \value Calendar.April April (3)
    \value Calendar.May May (4)
    \value Calendar.June June (5)
    \value Calendar.July July (6)
    \value Calendar.August August (7)
    \value Calendar.September September (8)
    \value Calendar.October October (9)
    \value Calendar.November November (10)
    \value Calendar.December December (11)

    \sa MonthGrid, DayOfWeekRow, WeekNumberColumn
*/

QQuickCalendar::QQuickCalendar(QObject *parent) : QObject(parent)
{
}

QT_END_NAMESPACE
