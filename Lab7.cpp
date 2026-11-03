#include "Lab7.h"

#include <QAction>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QStringList>
#include <QTableWidget>
#include <QTableWidgetItem>

Lab7::Lab7(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    connect(ui.btnGuardar, &QPushButton::clicked,
        this, &Lab7::guardarContacto);
    connect(ui.btnGuardarCambios, &QPushButton::clicked,
        this, &Lab7::guardarCambios);
    connect(ui.btnEliminar, &QPushButton::clicked,
        this, &Lab7::eliminarSeleccionado);

    connect(ui.actionRegistrar, &QAction::triggered,
        this, &Lab7::mostrarRegistrar);
    connect(ui.actionHistorial, &QAction::triggered,
        this, &Lab7::mostrarHistorial);
    connect(ui.actionEditar, &QAction::triggered,
        this, &Lab7::mostrarEditar);
    connect(ui.actionEliminar, &QAction::triggered,
        this, &Lab7::mostrarEliminar);

    connect(ui.tblContactos, &QTableWidget::itemSelectionChanged,
        this, &Lab7::actualizarCamposEdicion);

    ui.tblContactos->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.tblContactos->setSelectionMode(QAbstractItemView::SingleSelection);
    ui.tblContactos->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui.tblContactos->setSortingEnabled(false);
    ui.tblContactos->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui.tblContactos->verticalHeader()->setVisible(false);

    aplicarModo(Modo::Registrar);
}

Lab7::~Lab7()
{}

void Lab7::guardarContacto()
{
    QString nombre = ui.txtNombre->text().trimmed();
    QString telefono = ui.txtTelefono->text().trimmed();
    QString correo = ui.txtCorreo->text().trimmed();

    if (nombre.isEmpty() || telefono.isEmpty() || correo.isEmpty()) {
        QMessageBox::warning(this, "Datos incompletos",
            "Completá los tres campos.");
        return;
    }

    for (const QString& dato : QStringList{nombre, telefono, correo}) {
        if (dato.contains(';') || dato.contains('\n') ||
            dato.contains('\r')) {
            QMessageBox::warning(this, "Dato no permitido",
                "No usés punto y coma ni saltos de línea.");
            return;
        }
    }

    if (!archivoContactos.guardar(nombre, telefono, correo)) {
        QMessageBox::critical(this, "No se pudo guardar",
            "Revisá permisos y espacio disponible.\nArchivo: "
            + archivoContactos.ruta());
        return;
    }

    ui.txtNombre->clear();
    ui.txtTelefono->clear();
    ui.txtCorreo->clear();
    cargarTabla();
    QMessageBox::information(this, "Guardado completo",
        "El contacto se guardó correctamente.\nArchivo: "
        + archivoContactos.ruta());
    ui.txtNombre->setFocus();
}

void Lab7::mostrarRegistrar()
{
    aplicarModo(Modo::Registrar);
}

void Lab7::mostrarHistorial()
{
    aplicarModo(Modo::Historial);
    cargarTabla();
    ui.tblContactos->clearSelection();
    limpiarEdicion();
}

void Lab7::mostrarEditar()
{
    aplicarModo(Modo::Editar);
    cargarTabla();
    ui.tblContactos->clearSelection();
    limpiarEdicion();
}

void Lab7::mostrarEliminar()
{
    aplicarModo(Modo::Eliminar);
    cargarTabla();
    ui.tblContactos->clearSelection();
    limpiarEdicion();
}

void Lab7::guardarCambios()
{
    const int fila = filaSeleccionada();
    if (fila < 0) {
        QMessageBox::information(this, "Sin selección",
            "Seleccioná un contacto de la tabla para editarlo.");
        return;
    }

    const Contacto contacto{
        ui.txtEditNombre->text().trimmed(),
        ui.txtEditTelefono->text().trimmed(),
        ui.txtEditCorreo->text().trimmed()
    };

    QString error;
    if (!validarContacto(contacto, error)) {
        QMessageBox::warning(this, "Datos no válidos", error);
        return;
    }

    if (!archivoContactos.actualizar(fila, contacto)) {
        QMessageBox::critical(this, "No se pudo actualizar",
            "Revisá permisos y espacio disponible.\nArchivo: "
            + archivoContactos.ruta());
        return;
    }

    cargarTabla();
    ui.tblContactos->selectRow(fila);
    QMessageBox::information(this, "Cambios guardados",
        "El contacto se actualizó correctamente.");
}

void Lab7::eliminarSeleccionado()
{
    const int fila = filaSeleccionada();
    if (fila < 0) {
        QMessageBox::information(this, "Sin selección",
            "Seleccioná un contacto de la tabla para eliminarlo.");
        return;
    }

    QTableWidgetItem* itemNombre = ui.tblContactos->item(fila, 0);
    const QString nombre = itemNombre ? itemNombre->text() : QString();

    const QMessageBox::StandardButton respuesta = QMessageBox::question(
        this, "Eliminar contacto",
        "¿Eliminar el contacto \"" + nombre + "\"?\n"
        "Esta acción no se puede deshacer.",
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No);

    if (respuesta != QMessageBox::Yes)
        return;

    if (!archivoContactos.eliminar(fila)) {
        QMessageBox::critical(this, "No se pudo eliminar",
            "Revisá permisos y espacio disponible.\nArchivo: "
            + archivoContactos.ruta());
        return;
    }

    cargarTabla();
    ui.tblContactos->clearSelection();
    limpiarEdicion();
    QMessageBox::information(this, "Contacto eliminado",
        "El contacto se eliminó correctamente.");
}

void Lab7::actualizarCamposEdicion()
{
    if (modoActual != Modo::Editar)
        return;

    const int fila = filaSeleccionada();
    if (fila < 0) {
        limpiarEdicion();
        return;
    }

    QTableWidgetItem* nombre = ui.tblContactos->item(fila, 0);
    QTableWidgetItem* telefono = ui.tblContactos->item(fila, 1);
    QTableWidgetItem* correo = ui.tblContactos->item(fila, 2);

    ui.txtEditNombre->setText(nombre ? nombre->text() : QString());
    ui.txtEditTelefono->setText(telefono ? telefono->text() : QString());
    ui.txtEditCorreo->setText(correo ? correo->text() : QString());
}

void Lab7::aplicarModo(Modo modo)
{
    modoActual = modo;

    if (modo == Modo::Registrar) {
        ui.stackedWidget->setCurrentWidget(ui.pageRegistrar);
        ui.txtNombre->setFocus();
        return;
    }

    ui.stackedWidget->setCurrentWidget(ui.pageContactos);

    const bool editando = (modo == Modo::Editar);
    const bool eliminando = (modo == Modo::Eliminar);

    ui.grpEdicion->setVisible(editando);
    ui.btnEliminar->setVisible(eliminando);

    if (editando) {
        ui.lblAyuda->setText(
            "Seleccioná un contacto de la tabla, modificá sus datos "
            "y presioná Guardar cambios.");
    }
    else if (eliminando) {
        ui.lblAyuda->setText(
            "Seleccioná un contacto de la tabla y presioná "
            "Eliminar seleccionado.");
    }
    else {
        ui.lblAyuda->setText(
            "Historial de contactos. La tabla es solo de lectura.");
    }
}

void Lab7::cargarTabla()
{
    QString error;
    const QVector<Contacto> contactos = archivoContactos.leer(&error);

    ui.tblContactos->clearContents();
    ui.tblContactos->setRowCount(contactos.size());

    for (int fila = 0; fila < contactos.size(); ++fila) {
        const Contacto& contacto = contactos.at(fila);

        QTableWidgetItem* itemNombre = new QTableWidgetItem(contacto.nombre);
        QTableWidgetItem* itemTelefono = new QTableWidgetItem(contacto.telefono);
        QTableWidgetItem* itemCorreo = new QTableWidgetItem(contacto.correo);

        itemNombre->setFlags(itemNombre->flags() & ~Qt::ItemIsEditable);
        itemTelefono->setFlags(itemTelefono->flags() & ~Qt::ItemIsEditable);
        itemCorreo->setFlags(itemCorreo->flags() & ~Qt::ItemIsEditable);

        ui.tblContactos->setItem(fila, 0, itemNombre);
        ui.tblContactos->setItem(fila, 1, itemTelefono);
        ui.tblContactos->setItem(fila, 2, itemCorreo);
    }

    if (!error.isEmpty()) {
        QMessageBox::warning(this, "No se pudo leer el archivo", error);
    }
}

void Lab7::limpiarEdicion()
{
    ui.txtEditNombre->clear();
    ui.txtEditTelefono->clear();
    ui.txtEditCorreo->clear();
}

int Lab7::filaSeleccionada() const
{
    if (!ui.tblContactos->selectionModel())
        return -1;

    const QModelIndexList filas =
        ui.tblContactos->selectionModel()->selectedRows();
    if (filas.isEmpty())
        return -1;

    return filas.first().row();
}

bool Lab7::validarContacto(const Contacto& contacto, QString& error) const
{
    if (contacto.nombre.isEmpty() || contacto.telefono.isEmpty() ||
        contacto.correo.isEmpty()) {
        error = "Completá los tres campos.";
        return false;
    }

    for (const QString& dato :
         QStringList{contacto.nombre, contacto.telefono, contacto.correo}) {
        if (dato.contains(';') || dato.contains('\n') ||
            dato.contains('\r')) {
            error = "No usés punto y coma ni saltos de línea.";
            return false;
        }
    }

    return true;
}
