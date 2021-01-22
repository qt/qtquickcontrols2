/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Quick Templates 2 module of the Qt Toolkit.
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

#include "qquickdeferredexecute_p_p.h"

#include <QtCore/qhash.h>
#include <QtQml/qqmlengine.h>
#include <QtQml/private/qqmldata_p.h>
#include <QtQml/private/qqmlcomponent_p.h>
#include <QtQml/private/qqmlobjectcreator_p.h>

QT_BEGIN_NAMESPACE

namespace QtQuickPrivate {

typedef QHash<uint, QQmlComponentPrivate::DeferredState *> DeferredStates;

static inline uint qHash(QObject *object, const QString &propertyName)
{
    return ::qHash(object) + ::qHash(propertyName);
}

Q_GLOBAL_STATIC(DeferredStates, deferredStates)

static void cancelDeferred(QQmlData *ddata, int propertyIndex)
{
    auto dit = ddata->deferredData.rbegin();
    while (dit != ddata->deferredData.rend()) {
        (*dit)->bindings.remove(propertyIndex);
        ++dit;
    }
}

static bool beginDeferred(QQmlEnginePrivate *enginePriv, const QQmlProperty &property, QQmlComponentPrivate::DeferredState *deferredState)
{
    QObject *object = property.object();
    QQmlData *ddata = QQmlData::get(object);
    Q_ASSERT(!ddata->deferredData.isEmpty());

    int propertyIndex = property.index();
    int wasInProgress = enginePriv->inProgressCreations;

    for (auto dit = ddata->deferredData.rbegin(); dit != ddata->deferredData.rend(); ++dit) {
        QQmlData::DeferredData *deferData = *dit;

        auto bindings = deferData->bindings;
        auto range = bindings.equal_range(propertyIndex);
        if (range.first == bindings.end())
            continue;

        QQmlComponentPrivate::ConstructionState *state = new QQmlComponentPrivate::ConstructionState;
        state->completePending = true;

        QQmlContextData *creationContext = nullptr;
        state->creator.reset(new QQmlObjectCreator(deferData->context->parent, deferData->compilationUnit, creationContext));

        enginePriv->inProgressCreations++;

        typedef QMultiHash<int, const QV4::CompiledData::Binding *> QV4PropertyBindingHash;
        auto it = std::reverse_iterator<QV4PropertyBindingHash::iterator>(range.second);
        auto last = std::reverse_iterator<QV4PropertyBindingHash::iterator>(range.first);
#if Q_QML_PRIVATE_API_VERSION < 7
        while (it != last) {
            if (!state->creator->populateDeferredBinding(property, deferData, *it))
                state->errors << state->creator->errors;
            ++it;
        }
#else
        state->creator->beginPopulateDeferred(deferData->context);
        while (it != last) {
            state->creator->populateDeferredBinding(property, deferData->deferredIdx, *it);
            ++it;
        }
        state->creator->finalizePopulateDeferred();
        state->errors << state->creator->errors;
#endif

        deferredState->constructionStates += state;

        // Cleanup any remaining deferred bindings for this property, also in inner contexts,
        // to avoid executing them later and overriding the property that was just populated.
        cancelDeferred(ddata, propertyIndex);
        break;
    }

    return enginePriv->inProgressCreations > wasInProgress;
}

void beginDeferred(QObject *object, const QString &property)
{
    QQmlData *data = QQmlData::get(object);
    if (data && !data->deferredData.isEmpty() && !data->wasDeleted(object)) {
        QQmlEnginePrivate *ep = QQmlEnginePrivate::get(data->context->engine);

        QQmlComponentPrivate::DeferredState *state = new QQmlComponentPrivate::DeferredState;
        if (beginDeferred(ep, QQmlProperty(object, property), state))
            deferredStates()->insert(qHash(object, property), state);
        else
            delete state;

        // Release deferred data for those compilation units that no longer have deferred bindings
        data->releaseDeferredData();
    }
}

void cancelDeferred(QObject *object, const QString &property)
{
    QQmlData *data = QQmlData::get(object);
    if (data)
        cancelDeferred(data, QQmlProperty(object, property).index());
}

void completeDeferred(QObject *object, const QString &property)
{
    QQmlData *data = QQmlData::get(object);
    QQmlComponentPrivate::DeferredState *state = deferredStates()->take(qHash(object, property));
    if (data && state && !data->wasDeleted(object)) {
        QQmlEnginePrivate *ep = QQmlEnginePrivate::get(data->context->engine);
        QQmlComponentPrivate::completeDeferred(ep, state);
    }
    delete state;
}

} // QtQuickPrivate

QT_END_NAMESPACE
