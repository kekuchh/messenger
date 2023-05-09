#include "mainwidget.h"
#include "./ui_mainwidget.h"

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
{
    conn = new Connection("127.0.0.1", 13);

    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    ui->passwordLine->setEchoMode(QLineEdit::Password);

    connect(conn, SIGNAL(readyRead()), this, SLOT(receiveMessage()));
}

MainWidget::~MainWidget()
{
    delete conn;
    delete ui;
}

void MainWidget::receiveMessage()
{
    try {
        ui->label_5->setText("1111111");
        boost::property_tree::ptree root = conn->receiveFromServer();
        ui->label_5->setText("2222222");
        std::string responseName = root.get<std::string>("responseName");
        ui->label_5->setText("333333");

        ui->label_4->setText((responseName + std::to_string(c)).c_str());
        c++;
        if (responseName == "authorization") {
            if (root.get<std::string>("status") == "success") {
                username = ui->loginLine->text().toStdString();
                std::unordered_map<std::string, std::string> data;
                data["requestName"] = "getDialogs";
                data["username"] = username;
                conn->sendToServer(data);

                data.clear();
                data["requestName"] = "getNewDialogs";
                data["username"] = username;
                conn->sendToServer(data);

                ui->stackedWidget->setCurrentIndex(1);
            } else {
                ui->label_3->setText("Неверный логин/пароль");
            }
        } else if (responseName == "registration") {
            if (root.get<std::string>("status") == "success") {
                ui->stackedWidget->setCurrentIndex(0);
                ui->label_3->setText("Аккаунт успешно создан");
            } else {
                ui->label_8->setText("Логин занят");
            }
        } else if (responseName == "dialogs") {
            auto dialogsNode = root.get_child("dialogs");
            for(auto it = dialogsNode.begin(); it != dialogsNode.end(); ++it) {
                std::string user = it->second.get<std::string>("username");
                dialogs[user] = it->second.get<int>("id");
                ui->dialogsList->addItem(user.c_str());
            }
        } else if (responseName == "addNewMessage") {
            ui->label_5->setText("555555");
            if (openDialogs.contains(root.get<int>("dialog_id"))) {
                openDialogs[root.get<int>("dialog_id")]->addMessage(root.get<std::string>("author") + ": " + root.get<std::string>("text"));
                ui->label_5->setText("66666");
            }
            emit conn->readyRead();
        } else if (responseName == "NewDialogs") {
            auto dialogsNode = root.get_child("dialogs");
            for(auto it = dialogsNode.begin(); it != dialogsNode.end(); ++it) {
                std::string user = it->second.data();
                ui->newDialogsList->addItem(user.c_str());
            }
        } else if (responseName == "createdDialog") {
            int id = root.get<int>("dialog_id");
            QListWidgetItem* item = ui->newDialogsList->currentItem();
            Dialog* dialog = new Dialog(this, id, username, conn);

            ui->tabWidget->insertTab(ui->tabWidget->count(), dialog, item->text());
            openDialogs[id] = dialog;
            ui->newDialogsList->takeItem(ui->newDialogsList->row(item));
            ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-1);
        }
        else if (responseName == "success") {
            //
        }
    } catch (std::exception& e) {
        ui->label_4->setText(e.what());
    }
}

void MainWidget::loadMessages()
{

}

void MainWidget::on_showPassword_stateChanged(int arg1)
{
    if(arg1 == 2) {
        ui->passwordLine->setEchoMode(QLineEdit::Normal);
    }
    else {
        ui->passwordLine->setEchoMode(QLineEdit::Password);
    }
}


void MainWidget::on_enterButton_clicked()
{
    std::unordered_map<std::string, std::string> data;
    data["requestName"] = "authorization";
    data["login"] = ui->loginLine->text().toStdString();
    data["password"] = ui->passwordLine->text().toStdString();

    conn->sendToServer(data);
}

void MainWidget::on_dialogButton_clicked()
{
    QListWidgetItem* item = ui->dialogsList->currentItem();
    if (!item) {
        return;
    }

    int id = dialogs[item->text().toStdString()];
    Dialog* dialog = new Dialog(this, id, username, conn);

    ui->tabWidget->insertTab(ui->tabWidget->count(), dialog, item->text());
    openDialogs[id] = dialog;
    ui->dialogsList->takeItem(ui->dialogsList->row(item));
    ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-1);
}


void MainWidget::on_registrationButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}


void MainWidget::on_createAccountButton_clicked()
{
    std::unordered_map<std::string, std::string> data;
    data["requestName"] = "registration";
    data["login"] = ui->loginLineReg->text().toStdString();
    data["password"] = ui->passwordLineReg->text().toStdString();

    conn->sendToServer(data);
}


void MainWidget::on_homeButton_clicked()
{
    ui->loginLineReg->clear();
    ui->passwordLineReg->clear();
    ui->stackedWidget->setCurrentIndex(0);
}


void MainWidget::on_newDialogButton_clicked()
{
    QListWidgetItem* item = ui->newDialogsList->currentItem();
    if (!item) {
        return;
    }

    std::unordered_map<std::string, std::string> data;
    data["requestName"] = "createDialog";
    data["username"] = username;
    data["buddy"] = item->text().toStdString();

    conn->sendToServer(data);
}


void MainWidget::on_tabWidget_tabCloseRequested(int index)
{
    if(index != 0) {
        for (auto& [id, d] : openDialogs) {
            if (ui->tabWidget->indexOf(d) == index) {
                openDialogs.erase(id);
                std::string name = ui->tabWidget->tabText(index).toStdString();
                dialogs[name] = id;
                ui->dialogsList->addItem(name.c_str());
                break;
            }
        }
        ui->tabWidget->removeTab(index);
    }
}

