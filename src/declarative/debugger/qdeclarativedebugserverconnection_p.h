/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the QtQuick1 module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVEDEBUGSERVERCONNECTION_H
#define QDECLARATIVEDEBUGSERVERCONNECTION_H

#include <private/qtquick1global_p.h>
#include <QtCore/qobject.h>

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QDeclarativeDebugServer;
class Q_QUICK1_EXPORT QDeclarativeDebugServerConnection
{
public:
    QDeclarativeDebugServerConnection() {}
    virtual ~QDeclarativeDebugServerConnection() {}

    virtual void setServer(QDeclarativeDebugServer *server) = 0;
    virtual void setPort(int port, bool bock) = 0;
    virtual bool isConnected() const = 0;
    virtual void send(const QList<QByteArray> &messages) = 0;
    virtual void disconnect() = 0;
    virtual bool waitForMessage() = 0;
};

#define QDeclarativeDebugServerConnection_iid "org.qt-project.Qt.QDeclarativeDebugServerConnection"

Q_DECLARE_INTERFACE(QDeclarativeDebugServerConnection, QDeclarativeDebugServerConnection_iid)

QT_END_NAMESPACE

QT_END_HEADER

#endif // QDECLARATIVEDEBUGSERVERCONNECTION_H
