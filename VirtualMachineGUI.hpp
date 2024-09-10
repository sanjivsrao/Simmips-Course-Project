#ifndef VIRTUALMACHINEGUI_H
#define VIRTUALMACHINEGUI_H

#include "VirtualMachine.hpp"
#include "Parser.hpp"

#include <QApplication>
#include <QWidget>
#include <QTableView>
#include <QTableWidget>
#include <QDesktopWidget>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QDebug>
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QTextBlock>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QHeaderView>

#include <iomanip>
#include <sstream>
#include <fstream>
#include <thread>

class VirtualMachineGUI : public QWidget {
    Q_OBJECT
public:
    VirtualMachineGUI(QWidget* parent = nullptr);
    ~VirtualMachineGUI();

    // Define getter functions for testing
    QPlainTextEdit* getTextEdit() const { return text; }
    QTableView* getRegistersTableView() const { return registers; }
    QTableView* getMemoryTableView() const { return memory; }
    QLineEdit* getStatusLineEdit() const { return status; }
    QPushButton* getStepButton() const { return step; }
    QStandardItemModel* getRegisterModel() const { return registerModel; }
    QStandardItemModel* getMemoryModel() const { return memoryModel; }

    void load(const QString& filename);

private:
    QPlainTextEdit* text;
    QTableView* registers;
    QTableView* memory;
    QLineEdit* status;
    QPushButton* step;
    QPushButton* run;
    QPushButton* break_;
    QTextCursor cursor;

    QStandardItemModel* registerModel;
    QStandardItemModel* memoryModel;
    simmips::VirtualMachine vm;
    std::atomic<bool> running;
    std::thread executionThread;
    int firstHighlightedLine;
    int highlightIndex;

signals:
    void updateTableSignal();

private slots:
    void stepClicked();
    void runClicked();
    void executionLoop();
    void breakClicked();
    void updateTableSlot();

private:
    std::string hex_convert_reg(uint32_t value);
    std::string hex_convert_addr(uint8_t value);
    void highlightLine(int line_num, bool first_call);
    void updateReg(simmips::RegisterKind regKind);
    void updateTables();
};

#endif
