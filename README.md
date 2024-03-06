# Manual de Usuario: LChat

LChat es una aplicación de chat que permite la comunicación entre usuarios mediante el uso de sockets y memoria compartida. Este proyecto consta de dos componentes principales: `LChatServer`, el servidor que gestiona las conexiones y mensajes entre los clientes, y `LChatClient`, el cliente que se utiliza para conectarse al servidor y comunicarse con otros usuarios.

![CleanShot 2024-03-06 at 16 34 49](https://github.com/zsantiago8712/LatinChat/assets/59674428/b0c6593e-e100-460d-aa84-0ff2b6da6de0)


## Requisitos Previos

Antes de comenzar, asegúrate de tener instalado `xmake` en tu sistema. `xmake` es un sistema de construcción de proyectos multiplataforma. Si aún no lo has instalado, puedes encontrar las instrucciones de instalación en [https://xmake.io](https://xmake.io).

## Instalación de Xmake

### Windows

1. Descarga y ejecuta el instalador desde [https://xmake.io/#/guide/installation](https://xmake.io/#/guide/installation).
2. Sigue las instrucciones del instalador.

### macOS

Abre una terminal y ejecuta el siguiente comando:

```bash
brew install xmake
```

## Construcción del Proyecto

1. Abre una terminal o línea de comandos.
2. Navega hasta el directorio raíz del proyecto de LChat.
3. Ejecuta el siguiente comando para construir el proyecto:

```bash
xmake
```

Este comando construirá tanto el servidor (`LChatServer`) como el cliente (`LChatClient`).

## Ejecución del Servidor

### Windows

Para iniciar el servidor en Windows, abre una línea de comandos en el directorio del proyecto y ejecuta:

```cmd
xmake run LChatServer
```

### macOS

Para iniciar el servidor en macOS, abre una terminal y ejecuta:

```bash
xmake run LChatServer
```

## Ejecución del Cliente

### Windows

Para iniciar un cliente en Windows, abre una nueva línea de comandos en el directorio del proyecto y ejecuta:

```cmd
xmake run LChatClient
```

### macOS

Para iniciar un cliente en macOS, abre una nueva terminal y ejecuta:

```bash
xmake run LChatClient
```

## Uso del Chat

Una vez que el servidor está en ejecución y te has conectado con el cliente, sigue estas instrucciones para la interacción:

- **Para escribir un mensaje:** Antes de poder escribir un mensaje, debes activar el input presionando `w` o `W`. Esto te permitirá comenzar a escribir tu mensaje.
- **Para enviar el mensaje:** Después de escribir tu mensaje, presiona `Enter` para enviarlo.
- **Para cerrar el cliente o servidor:** Simplemente presiona `Escape` en cualquier momento para cerrar la aplicación.

## Cierre

Para cerrar el servidor o un cliente, no es necesario cerrar la ventana de la terminal; simplemente presiona `q` para terminar la ejecución de manera segura.
```

Este formato está diseñado para ser incluido directamente en tu archivo README.md. Puedes ajustarlo según las necesidades específicas de tu proyecto, incluyendo detalles adicionales o modificando cualquier sección para que coincida mejor con la funcionalidad y el uso de tu aplicación LChat.
