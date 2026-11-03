#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Lab7.h"
#include "ArchivoContactos.h"

class Lab7 : public QMainWindow
{
    Q_OBJECT

public:
    Lab7(QWidget *parent = nullptr);
    ~Lab7();

private slots:
    void guardarContacto();
    void mostrarRegistrar();
    void mostrarHistorial();
    void mostrarEditar();
    void mostrarEliminar();
    void guardarCambios();
    void eliminarSeleccionado();
    void actualizarCamposEdicion();

private:
    enum class Modo
    {
        Registrar,
        Historial,
        Editar,
        Eliminar
    };

    Ui::Lab7Class ui;
    ArchivoContactos archivoContactos;
    Modo modoActual = Modo::Registrar;

    void aplicarModo(Modo modo);
    void cargarTabla();
    void limpiarEdicion();
    int filaSeleccionada() const;
    bool validarContacto(const Contacto& contacto, QString& error) const;
};
