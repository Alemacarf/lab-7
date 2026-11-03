# Lab 7 — Agenda de contactos (Qt Widgets)

Práctica de **Programación 1**: aplicación de escritorio para registrar contactos en un archivo de texto, consultarlos en una tabla, editarlos y eliminarlos.

Repositorio: <https://github.com/Alemacarf/lab-7.git>

## Requisitos

- Visual Studio 2026 (proyecto generado con la herramienta de Qt para Visual Studio).
- Qt 6.12.0 para MSVC 2022 64 bits (`C:/Qt/6.12.0/msvc2022_64`).
- Extensión **Qt Visual Studio Tools**.

El proyecto ya incluye `QT_NO_DEPRECATED_WARNINGS` en las definiciones del preprocesador para las configuraciones Debug y Release, y compila los archivos como UTF-8.

## Cómo abrir y ejecutar

1. Abrir `Lab 7.sln` con Visual Studio.
2. Seleccionar la configuración `Debug` y la plataforma `x64`.
3. Compilar con **Compilar → Recompilar solución**.
4. Ejecutar con **Ctrl + F5**.

## Funcionalidad

El menú **Navegación** tiene cuatro opciones:

- **Registrar**: formulario con Nombre, Teléfono y Correo. Al guardar, agrega una línea al archivo, limpia los campos y muestra una confirmación.
- **Historial**: lee el archivo completo y muestra los contactos en una tabla de solo lectura. Al volver a entrar no se duplican las filas.
- **Editar**: permite seleccionar una fila, modificar sus tres datos y guardar los cambios. Solo se actualiza el registro seleccionado.
- **Eliminar**: permite seleccionar una fila, pide confirmación y elimina únicamente ese registro. Si se cancela, los datos se conservan.

La tabla usa `SelectRows` y `SingleSelection`, no permite edición directa ni ordenamiento, para que la fila seleccionada siempre corresponda al mismo registro del archivo.

## Archivo de datos

La clase `ArchivoContactos` concentra el acceso al archivo. La ruta es:

```text
C:/Users/<usuario>/Lab7Datos/contactos.txt
```

En esta máquina se usó:

```text
C:/Users/gollo/Lab7Datos/contactos.txt
```

Cada línea tiene el formato:

```text
nombre;telefono;correo
```

Se usa `QSaveFile` para reescribir el archivo al editar o eliminar, de modo que el reemplazo solo se completa si la escritura termina correctamente. Para agregar contactos se conserva el modo `Append`, sin borrar los registros anteriores.

En el repositorio se incluye `contactos-ejemplo.txt` con tres contactos de prueba. Se puede copiar a la ruta anterior para ver el historial con datos de ejemplo.

## Comprobación de editar y eliminar

La aplicación se probó con contactos ficticios:

1. Se crearon **Ana Mora**, **Luis Rojas** y **María Solís**. El historial mostró las tres filas y volver a abrirlo no las duplicó.
2. En **Editar** se seleccionó a Luis Rojas y se cambió su teléfono de `7000-2222` a `7111-9999`. El cambio apareció en la tabla y quedó guardado en el `.txt` sin agregar ni borrar filas.
3. En **Eliminar** se seleccionó a María Solís y primero se canceló la confirmación: el archivo conservó los tres contactos. Después se confirmó la eliminación: quedaron dos contactos.
4. Se cerró y volvió a abrir la aplicación. El historial conservó los dos contactos y el teléfono editado de Luis Rojas.
5. También se comprobó que guardar con un campo vacío no agrega ninguna línea y que intentar editar sin seleccionar una fila muestra una indicación sin modificar el archivo.

## Capturas

- `capturas/guardado.png`: guardado con mensaje de confirmación.
- `capturas/historial.png`: historial final con los dos contactos después de editar y eliminar.

## Estructura del proyecto

```text
Lab 7.sln
Lab 7.vcxproj
Lab7.h / Lab7.cpp / Lab7.ui   Ventana principal y formulario
ArchivoContactos.h / .cpp     Lectura y escritura del archivo
main.cpp
contactos-ejemplo.txt
capturas/
```
