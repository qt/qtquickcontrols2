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

#include <QtQml/qqmlextensionplugin.h>

#include "qquickdayofweekrow_p.h"
#include "qquickmonthgrid_p.h"
#include "qquickweeknumbercolumn_p.h"
#include "qquickcalendarmodel_p.h"
#include "qquickcalendar_p.h"

QT_BEGIN_NAMESPACE

class QtLabsCalendarPlugin: public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    QtLabsCalendarPlugin(QObject *parent = nullptr);
    void registerTypes(const char *uri) override;
};

QtLabsCalendarPlugin::QtLabsCalendarPlugin(QObject *parent) : QQmlExtensionPlugin(parent)
{
}

static QObject *calendarSingleton(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);
    return new QQuickCalendar;
}

void QtLabsCalendarPlugin::registerTypes(const char *uri)
{
    qmlRegisterType<QQuickDayOfWeekRow>(uri, 1, 0, "AbstractDayOfWeekRow");
    qmlRegisterType<QQuickMonthGrid>(uri, 1, 0, "AbstractMonthGrid");
    qmlRegisterType<QQuickWeekNumberColumn>(uri, 1, 0, "AbstractWeekNumberColumn");
    qmlRegisterType<QQuickCalendarModel>(uri, 1, 0, "CalendarModel");
    qmlRegisterSingletonType<QQuickCalendar>(uri, 1, 0, "Calendar", calendarSingleton);
}

QT_END_NAMESPACE

#include "qtlabscalendarplugin.moc"
