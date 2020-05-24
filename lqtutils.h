#ifndef LQTUTILS_H
#define LQTUTILS_H

// A read-write prop both in C++ and in QML.
#define L_RW_PROP(type, name, setter)                                      \
    public:                                                                \
        type name() const { return m_##name ; }                            \
    public Q_SLOTS:                                                        \
        void setter(type name) {                                           \
            if (m_##name == name) return;                                  \
            m_##name = name;                                               \
            emit name##Changed(name);                                      \
        }                                                                  \
    Q_SIGNALS:                                                             \
        void name##Changed(type name);                                     \
    private:                                                               \
        Q_PROPERTY(type name READ name WRITE setter NOTIFY name##Changed); \
        type m_##name;

// A read-write prop from C++, but read-only from QML.
#define L_RO_PROP(type, name, setter)                         \
    public:                                                   \
        type name() const { return m_##name ; }               \
    public Q_SLOTS:                                           \
        void setter(type name) {                              \
            if (m_##name == name) return;                     \
            m_##name = name;                                  \
            emit name##Changed(name);                         \
        }                                                     \
    Q_SIGNALS:                                                \
        void name##Changed(type name);                        \
    private:                                                  \
        Q_PROPERTY(type name READ name NOTIFY name##Changed); \
        type m_##name;

#if defined Q_OS_BLACKBERRY || defined Q_OS_ANDROID || defined Q_OS_IOS || defined Q_OS_WP
#define L_OS_MOBILE
#else
#define L_OS_DESKTOP
#endif

#ifndef QSL
#define QSL QStringLiteral
#endif // QSL

#endif // LQTUTILS_H
