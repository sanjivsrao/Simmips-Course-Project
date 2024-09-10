#include "VirtualMachineGUI.hpp"


VirtualMachineGUI::VirtualMachineGUI(QWidget* parent) : QWidget(parent) {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    //Initialize highlighted source line and skipped lines
    firstHighlightedLine = 0;
    highlightIndex = 0;

    //Initialize running boolean
    running = false;

    //Initialize asm file display
    text = new QPlainTextEdit(this);
    text->setObjectName("text");

    //Connect table update signal to slot
    connect(this, SIGNAL(updateRegistersSignal()), this, SLOT(updateRegistersSlot()));

    QHBoxLayout* dataLayout = new QHBoxLayout;

    //Initialize registers table
    registers = new QTableView(this);
    registerModel = new QStandardItemModel(35, 3, this);
    registers->setObjectName("registers");
    registers->setSelectionBehavior(QAbstractItemView::SelectRows);
    registers->setModel(registerModel);
    registers->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QStringList registerHeaders;
    registerHeaders << "Number" << "Alias" << "Value (Hex)";
    registerModel->setHorizontalHeaderLabels(registerHeaders);
    registers->verticalHeader()->setVisible(false);
    registers->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);


    // Initialize first 3 registers
    QStandardItem* staticRegs1 = new QStandardItem(QString(""));
    registerModel->setItem(0, 0, staticRegs1);

    QStandardItem* staticRegs2 = new QStandardItem(QString(""));
    registerModel->setItem(1, 0, staticRegs2);

    QStandardItem* staticRegs3 = new QStandardItem(QString(""));
    registerModel->setItem(2, 0, staticRegs3);

    for (int i = 0; i < 32; ++i) {
        QStandardItem* numberItem = new QStandardItem(QString("$%1").arg(i));
        registerModel->setItem(i+3, 0, numberItem);
    }

    //Initialize memory table
    memory = new QTableView(this);
    memoryModel = new QStandardItemModel(100, 2, this);
    memory->setObjectName("memory");
    memory->setSelectionBehavior(QAbstractItemView::SelectRows);
    memory->setModel(memoryModel);
    memory->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QStringList memoryHeaders;
    memoryHeaders << "Address (Hex)" << "Value (Hex)";
    memoryModel->setHorizontalHeaderLabels(memoryHeaders);
    memory->verticalHeader()->setVisible(false);
    memory->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    //Initialize status line
    status = new QLineEdit(this);
    status->setObjectName("status");
    status->setReadOnly(true);

    // Add tables to the layout
    dataLayout->addWidget(text);
    dataLayout->addWidget(registers);
    dataLayout->addWidget(memory);

    //Initialize step
    step = new QPushButton("Step", this);
    step->setObjectName("step");
    connect(step, &QPushButton::clicked, this, &VirtualMachineGUI::stepClicked);

    //Initialize run
    run = new QPushButton("Run", this);
    run->setObjectName("run");
    connect(run, &QPushButton::clicked, this, &VirtualMachineGUI::runClicked);

    //Initialize break_
    break_ = new QPushButton("Break", this);
    break_->setObjectName("break");
    connect(break_, &QPushButton::clicked, this, &VirtualMachineGUI::breakClicked);

    QVBoxLayout* bottomLayout = new QVBoxLayout;
    bottomLayout->addWidget(status);


    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(step);
    buttonLayout->addWidget(run);
    buttonLayout->addWidget(break_);

    mainLayout->addLayout(dataLayout);
    mainLayout->addLayout(bottomLayout);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
    resize(800, 800);
}

VirtualMachineGUI::~VirtualMachineGUI()
{
    if (running) {
        running = false;
        executionThread.join();
    }
}


void VirtualMachineGUI::load(const QString& filename) {
    std::ifstream file(filename.toStdString());
    if (!file.is_open()) {
        status->setText("Error: Unable to open file");
        return;
    }

    simmips::TokenList tokens = simmips::tokenize(file);
    file.close();
    std::pair<simmips::ParseError, simmips::VirtualMachine> output = simmips::parseVM(tokens);

    vm = output.second;
    for (auto it = tokens.begin(); it != tokens.end(); it++) {
        const auto& currentToken = *it;
        if (currentToken.value() == "main:") {
            it++;
            while (it != tokens.end()) {
                const auto& currentToken = *it;
                if (currentToken.type() != simmips::EOL) {
                    firstHighlightedLine = currentToken.line()-1;
                    std::cout << "fHL: " << firstHighlightedLine << std::endl;
                    break;
                }
                it++;
            }
            break;
        }
    }

    file.open(filename.toStdString());
    QString text_asm;
    std::string line;
    while (std::getline(file, line)) {
        text_asm.append(QString::fromStdString(line) + "\n");
    }
    text->setPlainText(text_asm);
    text->setReadOnly(true);
    highlightLine(firstHighlightedLine, true);

    QStandardItem* pcAlias = new QStandardItem(QString::fromStdString(vm.getRegisterAlias(simmips::RegisterKind::RegisterPC)));
    registerModel->setItem(0, 1, pcAlias);
    QStandardItem* hiAlias = new QStandardItem(QString::fromStdString(vm.getRegisterAlias(simmips::RegisterKind::RegisterHi)));
    registerModel->setItem(1, 1, hiAlias);
    QStandardItem* loAlias = new QStandardItem(QString::fromStdString(vm.getRegisterAlias(simmips::RegisterKind::RegisterLo)));
    registerModel->setItem(2, 1, loAlias);
    // Load registers aliases
    for (int i = 0; i < 32; ++i) {
        simmips::RegisterKind regKind = static_cast<simmips::RegisterKind>(i);
        QStandardItem* aliasItem = new QStandardItem(QString::fromStdString(vm.getRegisterAlias(regKind)));
        registerModel->setItem(i+3, 1, aliasItem);
    }
    
    // Load memory addresses
    int rowCount = output.second.getMemorySize();
    memoryModel->setRowCount(rowCount);
    for (int i = 0; i < rowCount; ++i) {
        QStandardItem* addressItem = new QStandardItem(QString::fromStdString(hex_convert_reg(i)));
        memoryModel->setItem(i, 0, addressItem);
    }

    if (output.first) {
        status->setText(QString("%1").arg(output.first.message().c_str()));
    }
    else {
        status->setText("Ok");
        updateTables();
    }
}

std::string VirtualMachineGUI::hex_convert_reg(uint32_t value) {
    std::stringstream ss;
    ss << "0x" << std::setfill('0') << std::setw(8) << std::hex << value;
    return ss.str();
}

std::string VirtualMachineGUI::hex_convert_addr(uint8_t value) {
    std::stringstream ss;
    ss << "0x" << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(value);
    return ss.str();
}

void VirtualMachineGUI::highlightLine(int line_num, bool first_call)
{
    if (first_call) {
        cursor = text->textCursor();
        cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, line_num);
        cursor.select(QTextCursor::LineUnderCursor);
        cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
        cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
        highlightIndex = line_num;
    }
    else {
        if (vm.isError()) return;
        if (vm.getBranch() == 0) {
            highlightIndex++;
        }
        else if (vm.getBranch() == 1) {
            int white_space = 0;
            for (int i = line_num; i < (highlightIndex); i++) {
                cursor.clearSelection();
                cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
                cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, i);
                if (cursor.block().text().isEmpty()) {
                    std::cout << "Logged white space" << std::endl;
                    white_space++;
                }
                cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
                cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
                QString text = cursor.selectedText();
                int lastIndex = text.size() - 1;
                text.replace(lastIndex, 1, "\n");
                std::string text_str = text.toStdString();
                for (size_t i = 0; i < text_str.size(); ++i) {
                    if (text_str[i] == ':' && text_str[i + 1] == '\n') {
                        std::cout << "Logged label" << std::endl;
                        white_space++;
                    }
                }
            }
            highlightIndex = vm.getCurrentSourceLine() + line_num + white_space;
            std::cout << "highlight index post branch: " << highlightIndex << std::endl;
        }
        cursor.clearSelection();
        cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
        cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, highlightIndex);
        while (!cursor.atEnd() && cursor.block().text().isEmpty()) {
            cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, 1);
            highlightIndex++;
        }
        cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
        cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
        QString text = cursor.selectedText();
        int lastIndex = text.size() - 1;
        text.replace(lastIndex, 1, "\n");
        std::string text_str = text.toStdString();
        for (size_t i = 0; i < text_str.size(); ++i) {
            if (text_str[i] == ':' && text_str[i + 1] == '\n') {
                cursor.clearSelection();
                cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
                cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, highlightIndex);
                cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, 1);
                cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
                cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
                highlightIndex++;
                break;
            }
        }

        /*
        cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
        cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
        */
        /*
        if (!cursor.atEnd()) {
            cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
            cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
        }
        // Shifts cursor back up if last line
        else {
            cursor.movePosition(QTextCursor::Up, QTextCursor::MoveAnchor, 1);
            cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
            cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
        }*/
    }

    // Select text for highlighting
    QTextEdit::ExtraSelection selection;
    selection.format.setBackground(Qt::yellow);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = cursor;

    // Apply new highlighting
    QList<QTextEdit::ExtraSelection> extraSelections;
    extraSelections.append(selection);
    text->setExtraSelections(extraSelections);
}




void VirtualMachineGUI::updateReg(simmips::RegisterKind regKind) {
    std::string regValue = hex_convert_reg(vm.getRegister(regKind));

    int row = -1;
    int colValue = 2;

    switch (regKind) {
    case simmips::RegisterPC:
        row = 0;
        break;
    case simmips::RegisterHi:
        row = 1;
        break;
    case simmips::RegisterLo:
        row = 2;
        break;
    default:
        row = static_cast<int>(regKind+3);
        break;
    }
    QStandardItem* regItem = registerModel->item(row, colValue);
    if (regItem) {
        regItem->setText(QString::fromStdString(regValue));
    }
    else {
        regItem = new QStandardItem(QString::fromStdString(regValue));
        registerModel->setItem(row, colValue, regItem);
    }
}

void VirtualMachineGUI::updateTables() {
    // Load named register values
    updateReg(simmips::RegisterKind::RegisterPC);
    updateReg(simmips::RegisterKind::RegisterHi);
    updateReg(simmips::RegisterKind::RegisterLo);

    // Load numbered register values
    for (int i = 0; i < 32; ++i) {
        updateReg(static_cast<simmips::RegisterKind>(i));
    }

    // Load memory values
    int rowCount = vm.getMemorySize();
    for (int i = 0; i < rowCount; ++i) {
        std::string addrValue = hex_convert_addr(vm.getMemoryByte(i));
        QStandardItem* addrItem = memoryModel->item(i, 1);
        if (addrItem) {
            addrItem->setText(QString::fromStdString(addrValue));
        }
        else {
            addrItem = new QStandardItem(QString::fromStdString(addrValue));
            memoryModel->setItem(i, 1, addrItem);
        }
    }
    emit updateTableSignal();
}


void VirtualMachineGUI::stepClicked() {
    if (!vm.isError()) {
        vm.step();
        if (vm.getCurrentSourceLine() < vm.getInstructionSize()) {
            highlightLine(firstHighlightedLine, false);
        }
        updateTables();
        if (vm.isError()) {
            status->setText(QString("%1").arg("Error: Program counter out of bounds"));
            return;
        }
        emit updateTableSignal();
    }
}

void VirtualMachineGUI::runClicked() {
    if (!running.load()) {
        running = true;
        // Disable all widgets except the "Break" button
        step->setEnabled(false);
        run->setEnabled(false);
        break_->setEnabled(true);

        // Start the execution loop in a new thread
        executionThread = std::thread(&VirtualMachineGUI::executionLoop, this);
    }
}

void VirtualMachineGUI::executionLoop() {
    while (running.load()) {
        // Calls step clicked function
        stepClicked();
    }
}

void VirtualMachineGUI::breakClicked() {
    if (running.load()) {
        running = false;
        if (executionThread.joinable()) {
            executionThread.join();
        }
        // Re-enable all widgets
        step->setEnabled(true);
        run->setEnabled(true);
        break_->setEnabled(true);
        emit updateTableSignal();
    }
}

void VirtualMachineGUI::updateTableSlot()
{
    registers->update();
    memory->update();
}

