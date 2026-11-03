#include "ArchivoContactos.h"

#include <QDir>
#include <QFile>
#include <QSaveFile>
#include <QStringConverter>
#include <QStringList>
#include <QTextStream>

QString ArchivoContactos::ruta() const
{
    return QDir::homePath() + "/Lab7Datos/contactos.txt";
}

bool ArchivoContactos::guardar(const QString& nombre,
                              const QString& telefono,
                              const QString& correo) const
{
    QString carpeta = QDir::homePath() + "/Lab7Datos";
    if (!QDir().mkpath(carpeta))
        return false;

    QFile archivo(ruta());
    if (!archivo.open(QIODevice::WriteOnly | QIODevice::Append))
        return false;

    QString linea = nombre + ";" + telefono + ";" + correo + "\n";
    QByteArray datos = linea.toUtf8();
    bool completo = archivo.write(datos) == datos.size();
    bool vaciado = archivo.flush();
    archivo.close();
    return completo && vaciado;
}

QVector<Contacto> ArchivoContactos::leer(QString* error) const
{
    QVector<Contacto> contactos;

    QFile archivo(ruta());
    if (!archivo.exists())
        return contactos;

    if (!archivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (error)
            *error = "No se pudo abrir el archivo: " + ruta();
        return contactos;
    }

    QTextStream entrada(&archivo);
    entrada.setEncoding(QStringConverter::Utf8);

    while (!entrada.atEnd()) {
        QString linea = entrada.readLine();
        if (linea.startsWith(QChar(0xFEFF)))
            linea.remove(0, 1);
        if (linea.endsWith('\r'))
            linea.chop(1);
        if (linea.isEmpty())
            continue;

        const QStringList partes = linea.split(';');
        if (partes.size() != 3) {
            if (error)
                *error = "Se encontró una línea con formato inválido en " + ruta();
            continue;
        }

        contactos.append({partes.at(0), partes.at(1), partes.at(2)});
    }

    archivo.close();
    return contactos;
}

bool ArchivoContactos::actualizar(int indice, const Contacto& contacto) const
{
    QString error;
    QVector<Contacto> contactos = leer(&error);
    if (!error.isEmpty())
        return false;

    if (indice < 0 || indice >= contactos.size())
        return false;

    contactos[indice] = contacto;
    return reescribir(contactos);
}

bool ArchivoContactos::eliminar(int indice) const
{
    QString error;
    QVector<Contacto> contactos = leer(&error);
    if (!error.isEmpty())
        return false;

    if (indice < 0 || indice >= contactos.size())
        return false;

    contactos.removeAt(indice);
    return reescribir(contactos);
}

bool ArchivoContactos::reescribir(const QVector<Contacto>& contactos) const
{
    QString carpeta = QDir::homePath() + "/Lab7Datos";
    if (!QDir().mkpath(carpeta))
        return false;

    QSaveFile archivo(ruta());
    if (!archivo.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;

    for (const Contacto& contacto : contactos) {
        const QString linea = contacto.nombre + ";" + contacto.telefono + ";"
            + contacto.correo + "\n";
        const QByteArray datos = linea.toUtf8();
        if (archivo.write(datos) != datos.size()) {
            archivo.cancelWriting();
            return false;
        }
    }

    return archivo.commit();
}
