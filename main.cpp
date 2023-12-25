#include <QtWidgets/QApplication>
#include <QtCore/QFile>
#include "GUI.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QFile file("./css/stylesheet.qss");
    file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());
    app.setStyleSheet(styleSheet);

    GUI window;
    window.setWindowTitle("Sémantická podobnost");
    window.resize(600, 500);
    window.show();

    return app.exec();
}
