#include <QTest>
#include "VirtualMachineGUI.hpp"

#include "test_config.hpp"

class GUITests : public QObject {
  Q_OBJECT

private slots:

    void testLoad();
    void testWidgets();
    void testInitialState();
    void testFile02();
    void testFile03();
  
private:
    VirtualMachineGUI gui;
};

void GUITests::testLoad(){
    std::string vmfile = "test00.asm";
    std::string input = TEST_FILE_DIR + "/vm/" + vmfile;
    QString filename = QString::fromStdString(input);
    gui.load(filename);
    QCOMPARE(gui.getStatusLineEdit()->text(), QString("Ok"));

    vmfile = "test000.asm"; //invalid call
    input = TEST_FILE_DIR + "/vm/" + vmfile;
    filename = QString::fromStdString(input);
    gui.load(filename);
    QCOMPARE(gui.getStatusLineEdit()->text(), QString("Error: Unable to open file"));
}

void GUITests::testWidgets() {
    std::string vmfile = "test00.asm";
    std::string input = TEST_FILE_DIR + "/vm/" + vmfile;
    QString filename = QString::fromStdString(input);
    gui.load(filename);
    QCOMPARE(gui.getStatusLineEdit()->text(), QString("Ok"));
    QVERIFY(gui.findChild< QPlainTextEdit* >("text") != nullptr);
    QVERIFY(gui.findChild< QTableView* >("memory") != nullptr);
    QVERIFY(gui.findChild< QTableView* >("registers") != nullptr);
    QVERIFY(gui.findChild< QLineEdit* >("status") != nullptr);
    QVERIFY(gui.findChild< QPushButton* >("step") != nullptr);
}

void GUITests::testInitialState() {
    std::string vmfile = "test00.asm";
    std::string input = TEST_FILE_DIR + "/vm/" + vmfile;
    QString filename = QString::fromStdString(input);
    gui.load(filename);
    for (int i = 0; i < 35; i++) {
        QStandardItem* data = gui.getRegisterModel()->item(i, 2);
        QCOMPARE(data->text(), QString("0x00000000"));
    }
    for (int i = 3; i < 35; i++) {
        QStandardItem* data = gui.getRegisterModel()->item(i, 0);
        std::string regName = "$" + std::to_string(i - 3);
        QString name = QString::fromStdString(regName);
        QCOMPARE(data->text(), name);
    }
    for (int i = 0; i < 1024; i++) {
        QStandardItem* data = gui.getMemoryModel()->item(i, 1);
        QCOMPARE(data->text(), QString("0x00"));
    }
}
void GUITests::testFile02()
{   
    std::string vmfile = "test02.asm";
    std::string input = TEST_FILE_DIR + "/vm/" + vmfile;
    QString filename = QString::fromStdString(input);
    gui.load(filename);
    QPushButton* step = gui.getStepButton();
    // Step 1
    QTest::mouseClick(step, Qt::LeftButton);
    QStandardItem* pc = gui.getRegisterModel()->item(0, 2);
    QStandardItem* t0 = gui.getRegisterModel()->item(11, 2);
    QCOMPARE(pc->text(), QString("0x00000001"));
    QCOMPARE(t0->text(), QString("0x00000004"));
    // Step 2
    QTest::mouseClick(step, Qt::LeftButton);
    pc = gui.getRegisterModel()->item(0, 2);
    QStandardItem* t1 = gui.getRegisterModel()->item(12, 2);
    QCOMPARE(pc->text(), QString("0x00000002"));
    QCOMPARE(t1->text(), QString("0x00000007"));

    // Step 3
    QTest::mouseClick(step, Qt::LeftButton);
    pc = gui.getRegisterModel()->item(0, 2);
    QStandardItem* mem = gui.getMemoryModel()->item(0, 1);
    QCOMPARE(pc->text(), QString("0x00000003"));
    QCOMPARE(mem->text(), QString("0x07"));

    // Step 4
    QTest::mouseClick(step, Qt::LeftButton);
    pc = gui.getRegisterModel()->item(0, 2);
    mem = gui.getMemoryModel()->item(4, 1);
    QCOMPARE(pc->text(), QString("0x00000004"));
    QCOMPARE(mem->text(), QString("0x07"));

    // Step 5
    QTest::mouseClick(step, Qt::LeftButton);
    pc = gui.getRegisterModel()->item(0, 2);
    mem = gui.getMemoryModel()->item(8, 1);
    QCOMPARE(pc->text(), QString("0x00000005"));
    QCOMPARE(mem->text(), QString("0x07"));

    // Step 6
    QTest::mouseClick(step, Qt::LeftButton);
    pc = gui.getRegisterModel()->item(0, 2);
    mem = gui.getMemoryModel()->item(12, 1);
    QCOMPARE(pc->text(), QString("0x00000006"));
    QCOMPARE(mem->text(), QString("0x07"));

    // Step 7
    QTest::mouseClick(step, Qt::LeftButton);
    pc = gui.getRegisterModel()->item(0, 2);
    mem = gui.getMemoryModel()->item(16, 1);
    QCOMPARE(pc->text(), QString("0x00000007"));
    QCOMPARE(mem->text(), QString("0x07"));

    QTest::mouseClick(step, Qt::LeftButton);
    QCOMPARE(gui.getStatusLineEdit()->text(), QString("Error: Program counter out of bounds"));
}

void GUITests::testFile03()
{
    std::string vmfile = "test03.asm";
    std::string input = TEST_FILE_DIR + "/vm/" + vmfile;
    QString filename = QString::fromStdString(input);
    gui.load(filename);
    QPushButton* step = gui.getStepButton();
    // Step 1
    QTest::mouseClick(step, Qt::LeftButton);
    QStandardItem* pc = gui.getRegisterModel()->item(0, 2);
    QStandardItem* t0 = gui.getRegisterModel()->item(11, 2);
    QCOMPARE(pc->text(), QString("0x00000001"));
    QCOMPARE(t0->text(), QString("0x00000064"));
    std::cout << "Step 1" << std::endl;
    // Step 2
    QTest::mouseClick(step, Qt::LeftButton);
    pc = gui.getRegisterModel()->item(0, 2);
    QStandardItem* t1 = gui.getRegisterModel()->item(12, 2);
    QCOMPARE(pc->text(), QString("0x00000002"));
    QCOMPARE(t1->text(), QString("0x00000000"));

    // Step 3
    QTest::mouseClick(step, Qt::LeftButton);
    pc = gui.getRegisterModel()->item(0, 2);
    QStandardItem* t2 = gui.getRegisterModel()->item(13, 2);
    QCOMPARE(pc->text(), QString("0x00000003"));
    QCOMPARE(t2->text(), QString("0x00000001"));

    // Step 4
    QTest::mouseClick(step, Qt::LeftButton);
    pc = gui.getRegisterModel()->item(0, 2);
    QStandardItem* t3 = gui.getRegisterModel()->item(14, 2);
    QCOMPARE(pc->text(), QString("0x00000004"));
    QCOMPARE(t3->text(), QString("0x00000002"));

    // Step 5
    QTest::mouseClick(step, Qt::LeftButton);
    pc = gui.getRegisterModel()->item(0, 2);
    QStandardItem* t4 = gui.getRegisterModel()->item(15, 2);
    QCOMPARE(pc->text(), QString("0x00000005"));
    QCOMPARE(t4->text(), QString("0x00000004"));

    // Overstep
    QTest::mouseClick(step, Qt::LeftButton);
    QCOMPARE(gui.getStatusLineEdit()->text(), QString("Error: Program counter out of bounds"));
}
QTEST_MAIN(GUITests)
#include "GUITests.moc"

