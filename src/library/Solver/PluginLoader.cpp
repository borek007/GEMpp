#include "PluginLoader.h"
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QStringList>

PluginLoader::PluginLoader() : QPluginLoader() {}

PluginLoader::~PluginLoader() {}

Solver* PluginLoader::loadSolver(Solver::Solvers solver) {
    QString name = Solver::solverName[solver];
    const QString basePath = QCoreApplication::applicationDirPath();
    #if defined(WIN32) || defined(WIN64)
        QString suffix;
    #if defined(GEMDEBUG)
        suffix = "d";
    #else
        suffix.clear();
    #endif
        QString candidate = QDir(basePath).absoluteFilePath(
            QString("GEM++%1%2.dll").arg(name, suffix));
        setFileName(candidate);
    #elif defined(LINUX)
        const QStringList candidates = {
            QDir(basePath).absoluteFilePath(QString("../lib/libGEM++%1.so").arg(name)),
            QDir(basePath).absoluteFilePath(QString("lib/libGEM++%1.so").arg(name)),
            QDir(basePath).absoluteFilePath(QString("libGEM++%1.so").arg(name))
        };
        for (const QString &candidate : candidates) {
            if (QFileInfo::exists(candidate)) {
                setFileName(candidate);
                break;
            }
        }
        if (fileName().isEmpty() && !candidates.isEmpty()) {
            setFileName(candidates.first());
        }
    #endif

    QObject *plugin = instance();
    if(!plugin)
        Exception(QString("%1 plugin cannot be loaded : %2").arg(name,errorString()));
    SolverFactory *solverFactory = qobject_cast<SolverFactory *>(plugin);
    if(!solverFactory)
        Exception(QString("%1 solver cannot be used.").arg(name));
    return solverFactory->create();
}
