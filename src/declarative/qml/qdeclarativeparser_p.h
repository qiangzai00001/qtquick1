/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtQuick1 module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVEPARSER_P_H
#define QDECLARATIVEPARSER_P_H

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

#include "qdeclarative.h"

#include <QtCore/qbytearray.h>
#include <QtCore/qlist.h>
#include <QtCore/qurl.h>
#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>

#include <private/qobject_p.h>
#include <private/qdeclarativerefcount_p.h>
#include <private/qdeclarativeglobal_p.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QDeclarativePropertyCache;
namespace QDeclarativeJS { namespace AST { class Node; } }

/*
    XXX

    These types are created (and owned) by the QDeclarativeXmlParser and consumed by the 
    QDeclarativeCompiler.  During the compilation phase the compiler will update some of
    the fields for both its own use and for the use of the upcoming QDeclarativeDom API.

    The types are part of the generic sounding "QDeclarativeParser" namespace for legacy 
    reasons (there used to be more in this namespace) and will be cleaned up and
    migrated into a more appropriate location shortly.
*/
namespace QDeclarativeParser
{
    struct Location 
    {
        Location() : line(-1), column(-1) {}
        int line;
        int column;
    };

    struct LocationRange
    {
        LocationRange() : offset(0), length(0) {}
        quint32 offset;
        quint32 length;
    };

    struct LocationSpan
    {
        Location start;
        Location end;
        LocationRange range;

        bool operator<(LocationSpan &o) const {
            return (start.line < o.start.line) ||
                   (start.line == o.start.line && start.column < o.start.column);
        }
    };

    class Property;
    class Object : public QDeclarativeRefCount
    {
    public:
        Object();
        virtual ~Object(); 

        // Type of the object.  The integer is an index into the 
        // QDeclarativeCompiledData::types array, or -1 if the object is a property
        // group.
        int type;
        // The url of this object if it is an external type.  Used by the DOM
        QUrl url;

        // version information if type is defined in library or C++
        int majorVersion;
        int minorVersion;

        // The fully-qualified name of this type
        QByteArray typeName;
        // The class name
        QByteArray className;
        // The id assigned to the object (if any).  Set by the QDeclarativeCompiler
        QString id;
        // The id index assigned to the object (if any).  Set by the QDeclarativeCompiler
        int idIndex;
        // Custom parsed data
        QByteArray custom;
        // Bit mask of the properties assigned bindings
        QByteArray bindingBitmask; 
        void setBindingBit(int);
        // Returns the metaobject for this type, or 0 if not available.  
        // Internally selectd between the metatype and extObject variables
        const QMetaObject *metaObject() const;

        // The compile time metaobject for this type
        const QMetaObject *metatype;
        // The synthesized metaobject, if QML added signals or properties to
        // this type.  Otherwise null
        QAbstractDynamicMetaObject extObject;
        QByteArray metadata; // Generated by compiler
        QByteArray synthdata; // Generated by compiler
        QDeclarativePropertyCache *synthCache; // Generated by compiler

        Property *getDefaultProperty();
        Property *getProperty(const QByteArray &name, bool create=true);

        Property *defaultProperty;
        QHash<QByteArray, Property *> properties;

        // Output of the compilation phase (these properties continue to exist
        // in either the defaultProperty or properties members too)
        void addValueProperty(Property *);
        void addSignalProperty(Property *);
        void addAttachedProperty(Property *);
        void addGroupedProperty(Property *);
        void addValueTypeProperty(Property *);
        void addScriptStringProperty(Property *, int = 0);
        QList<Property *> valueProperties;
        QList<Property *> signalProperties;
        QList<Property *> attachedProperties;
        QList<Property *> groupedProperties;
        QList<Property *> valueTypeProperties;
        QList<QPair<Property *, int> > scriptStringProperties;

        // Script blocks that were nested under this object
        struct ScriptBlock {
            enum Pragma { 
                None   = 0x00000000,
                Shared = 0x00000001
            };
            Q_DECLARE_FLAGS(Pragmas, Pragma)

            QString code;
            QString file;
            Pragmas pragmas;
        };

        // The bytes to cast instances by to get to the QDeclarativeParserStatus 
        // interface.  -1 indicates the type doesn't support this interface.
        // Set by the QDeclarativeCompiler.
        int parserStatusCast;

        LocationSpan location;

        struct DynamicProperty {
            DynamicProperty();
            DynamicProperty(const DynamicProperty &);

            enum Type { Variant, Int, Bool, Real, String, Url, Color, Time, Date, DateTime, Alias, Custom, CustomList };

            bool isDefaultProperty;
            Type type;
            QByteArray customType;
            QByteArray name;
            QDeclarativeParser::Property *defaultValue;
            LocationSpan location;
        };
        struct DynamicSignal {
            DynamicSignal();
            DynamicSignal(const DynamicSignal &);

            QByteArray name;
            QList<QByteArray> parameterTypes;
            QList<QByteArray> parameterNames;
        };
        struct DynamicSlot {
            DynamicSlot();
            DynamicSlot(const DynamicSlot &);

            QByteArray name;
            QString body;
            QList<QByteArray> parameterNames;
            LocationSpan location;
        };

        // The list of dynamic properties
        QList<DynamicProperty> dynamicProperties;
        // The list of dynamic signals
        QList<DynamicSignal> dynamicSignals;
        // The list of dynamic slots
        QList<DynamicSlot> dynamicSlots;
    };

    class Q_QUICK1_EXPORT Variant
    {
    public:
        enum Type {
            Invalid,
            Boolean,
            Number,
            String,
            Script
        };

        Variant();
        Variant(const Variant &);
        Variant(bool);
        Variant(double, const QString &asWritten=QString());
        Variant(const QString &);
        Variant(const QString &, QDeclarativeJS::AST::Node *);
        Variant &operator=(const Variant &);

        Type type() const;

        bool isBoolean() const { return type() == Boolean; }
        bool isNumber() const { return type() == Number; }
        bool isString() const { return type() == String; }
        bool isScript() const { return type() == Script; }
        bool isStringList() const;

        bool asBoolean() const;
        QString asString() const;
        double asNumber() const;
        QString asScript() const;
        QDeclarativeJS::AST::Node *asAST() const;
        QStringList asStringList() const;

    private:
        Type t;
        union {
            bool b;
            double d;
            QDeclarativeJS::AST::Node *n;
        };
        QString s;
    };

    class Value : public QDeclarativeRefCount
    {
    public:
        Value();
        virtual ~Value();

        enum Type {
            // The type of this value assignment is not yet known
            Unknown,
            // This is used as a literal property assignment
            Literal,
            // This is used as a property binding assignment
            PropertyBinding,
            // This is used as a QDeclarativePropertyValueSource assignment
            ValueSource,
            // This is used as a QDeclarativePropertyValueInterceptor assignment
            ValueInterceptor,
            // This is used as a property QObject assignment
            CreatedObject,
            // This is used as a signal object assignment
            SignalObject,
            // This is used as a signal expression assignment
            SignalExpression,
            // This is used as an id assignment only
            Id
        };
        Type type;

        // ### Temporary (for id only)
        QString primitive() const { return value.isString() ? value.asString() : value.asScript(); }

        // Primitive value
        Variant value;
        // Object value
        Object *object;

        LocationSpan location;
    };

    class Property : public QDeclarativeRefCount
    {
    public:
        Property();
        Property(const QByteArray &n);
        virtual ~Property();

        // The Object to which this property is attached
        Object *parent;

        Object *getValue(const LocationSpan &);
        void addValue(Value *v);
        void addOnValue(Value *v);

        // The QVariant::Type of the property, or 0 (QVariant::Invalid) if 
        // unknown.
        int type;
        // The metaobject index of this property, or -1 if unknown.
        int index;

        // Returns true if this is an empty property - both value and values
        // are unset.
        bool isEmpty() const;
        // The list of values assigned to this property.  Content in values
        // and value are mutually exclusive
        QList<Value *> values;
        // The list of values assigned to this property using the "on" syntax
        QList<Value *> onValues;
        // The accessed property.  This is used to represent dot properties.
        // Content in value and values are mutually exclusive.
        Object *value;
        // The property name
        QByteArray name;
        // True if this property was accessed as the default property.  
        bool isDefault;
        // True if the setting of this property will be deferred.  Set by the
        // QDeclarativeCompiler
        bool isDeferred;
        // True if this property is a value-type pseudo-property
        bool isValueTypeSubProperty;
        // True if this property is a property alias.  Set by the 
        // QDeclarativeCompiler
        bool isAlias;

        LocationSpan location;
        LocationRange listValueRange;
        QList<int> listCommaPositions;
    };
}

Q_DECLARE_OPERATORS_FOR_FLAGS(QDeclarativeParser::Object::ScriptBlock::Pragmas);

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QDeclarativeParser::Variant)

QT_END_HEADER

#endif // QDECLARATIVEPARSER_P_H
