#ifndef LSETTINGS_H
#define LSETTINGS_H

#include <QObject>
#include <QSettings>

#define L_DEFINE_VALUE(type, name, def, f)                                 \
    public:                                                                \
        type name() const { return m_settings.value(#name, def).f(); }     \
    public Q_SLOTS:                                                        \
        void setter(type value) {                                          \
            if (name() == value) return;                                   \
            m_settings.setValue(#name, value);                             \
            emit name##Changed(value);                                     \
        }                                                                  \
    Q_SIGNALS:                                                             \
        void name##Changed(type name);                                     \
    private:                                                               \
        Q_PROPERTY(type name READ name WRITE setter NOTIFY name##Changed)

#endif
