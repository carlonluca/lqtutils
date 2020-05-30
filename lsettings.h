#ifndef LSETTINGS_H
#define LSETTINGS_H

#include <QObject>
#include <QSettings>

#define L_DEFINE_VALUE(type, name, def, f)                                    \
    public:                                                                   \
        type name() const { return m_settings->value(#name, def).f(); }       \
    public Q_SLOTS:                                                           \
        void set_##name(type value) {                                         \
            if (name() == value) return;                                      \
            m_settings->setValue(#name, value);                               \
            emit name##Changed(value);                                        \
        }                                                                     \
    Q_SIGNALS:                                                                \
        void name##Changed(type name);                                        \
    private:                                                                  \
        Q_PROPERTY(type name READ name WRITE set_##name NOTIFY name##Changed)

#define L_DECLARE_SETTINGS(classname, qsettings)                 \
    class classname : public QObject                             \
    {                                                            \
    private: \
        Q_OBJECT \
    public:                                                      \
        static classname& instance() {                           \
            static classname _instance;                          \
            return _instance;                                    \
        }                                                        \
    private:                                                     \
        classname(QObject* parent = nullptr) : QObject(parent) { \
            m_settings = qsettings;                              \
        }                                                        \
        ~classname() { delete m_settings; }                      \
    protected:                                                   \
        QSettings* m_settings;

#endif
