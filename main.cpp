#include <QGuiApplication>
#include <QQmlContext>
#include <QFileSystemWatcher>
#include <iostream>
#include "enhancedqmlapplicationengine.h"
#include <QFileInfo>
#include <QDir>
#include <QDirIterator>


int main(int argc, char *argv[])
{
#if defined(Q_OS_WIN)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);
    QFileSystemWatcher watcher;
    if(argc < 2)
    {
        std::cout << "Missing qml filepath." << std::endl;
        return -1;
    }

//    QString mainQmlPath = QFileInfo("./main.qml").dir().relativeFilePath(QString(argv[1]));
    QString mainQmlPath = "file:///" + QUrl(QString(argv[1])).toString();
    QString dirName = QUrl(QString(argv[1])).toString();
    watcher.addPath(dirName);
    QDirIterator iter(dirName, QDirIterator::Subdirectories);
    while(iter.hasNext())
        watcher.addPath(iter.next());
    for(auto dir : watcher.directories())
    {
        std::cout << "Watching '" << dir.toStdString() << "' for changes." << std::endl;
    }

    EnhancedQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("$QmlEngine", &engine);
    engine.load(QUrl("qrc:/main.qml"));
    if (engine.rootObjects().isEmpty())
    {
        std::cerr << "main.qml not found. Please make sure it's in the same directory as the executable." << std::endl;
        return -1;
    }

    QObject* loader = engine.rootObjects().at(0)->findChild<QObject*>("loader");
    QMetaObject::invokeMethod(loader, "reload", Q_ARG(QVariant, QVariant::fromValue(mainQmlPath)));
    engine.connect(&watcher, &QFileSystemWatcher::directoryChanged, [&watcher, &loader, &mainQmlPath](const QString& path) {
        QFileInfo info(path);
        if(info.exists())
            watcher.addPath(path);
        else
            watcher.removePath(path);
        QMetaObject::invokeMethod(loader, "reload", Q_ARG(QVariant, QVariant::fromValue(mainQmlPath)));
        std::cout << "Reloading.." << std::endl;
    });

    engine.connect(&watcher, &QFileSystemWatcher::fileChanged, [&loader, &mainQmlPath](const QString&) {
        QMetaObject::invokeMethod(loader, "reload", Q_ARG(QVariant, QVariant::fromValue(mainQmlPath)));
        std::cout << "Reloading.." << std::endl;
    });

    return app.exec();
}
