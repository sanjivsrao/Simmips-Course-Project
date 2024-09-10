#include "VirtualMachineGUI.hpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QString file_arg;
    if (argc != 2) {
        qDebug() << "Error:0: Usage: simmips-ide <filename>; Error due to incorrect # of args";
        return EXIT_FAILURE;
    }
    else {
        std::string filename = argv[1];
        file_arg = QString::fromStdString(filename);
    }
    VirtualMachineGUI gui;
    gui.load(file_arg);
    gui.show();

    return app.exec();
}
