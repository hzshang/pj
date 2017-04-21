#include "logindialog.h"
#include "ui_logindialog.h"
#include "sock.h"
#include "md5.h"
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>

extern sock mysock;

loginDialog::loginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::loginDialog)
{
    ui->setupUi(this);
    this->setFixedSize( this->width(),this->height());
    setUserInfo();
    QObject::connect(ui->userNameComboBox,SIGNAL(currentTextChanged(QString)),this,SLOT(setPwd()));
}

loginDialog::~loginDialog()
{
    delete ui;
}

void loginDialog::setUserInfo()
{
    logins.clear();
    int size = settings->beginReadArray("logins1");
    for (int i = 0; i < size; ++i) {
         settings->setArrayIndex(i);
         Login login;
         login.userName = settings->value("userName").toString();
         login.password = settings->value("password").toString();
         logins.append(login);
     }
     settings->endArray();
     map.clear();
     for(QList<Login>::iterator it = logins.begin();it != logins.end();it++)
     {
         map.insert(it->userName,it->password);
     }
     int index = 0;
     QMap<QString, QString>::const_iterator i;
     ui->userNameComboBox->setEditable(true);
     for(i = map.constBegin(); i != map.constEnd(); ++i)
     {
         ui->userNameComboBox->insertItem(index,i.key());
         index++;
     }
}

void loginDialog::updateUserInfo()
{
    QString u = ui->userNameComboBox->currentText();
    QString p = ui->loginPwdLineEdit->text();;
    int flag = 0;
    for(QList<Login>::iterator it = logins.begin();it != logins.end();it++)
    {
        if(it->userName == u)
        {
            it->password = p;
            flag = 1;
        }
    }
    if(!flag)
    {
        Login login;
        login.password = p;
        login.userName = u;
        logins.append(login);
    }
    settings->clear();
    settings->beginWriteArray("logins1");
    for (int i = 0; i < logins.size(); ++i) {
        settings->setArrayIndex(i);
        settings->setValue("userName", logins.at(i).userName);
        settings->setValue("password", logins.at(i).password);
    }
    settings->endArray();
}

bool loginDialog::checkPwd()
{
    QString userName = ui->userNameComboBox->currentText();
    QString pwd = ui->loginPwdLineEdit->text();
    QString userPwd = md5(pwd);
    QJsonObject sendData;
    sendData.insert("pid",1);
    sendData.insert("user",userName);
    sendData.insert("pwd",userPwd);
    mysock.send(sendData);

    QJsonDocument recvData = mysock.recv();
    QJsonObject data = recvData.object();
    bool state = data["state"].toBool();
    if(!state)
    {
        QMessageBox::warning(this,"警告","密码不正确！",QMessageBox::Ok);
        ui->loginPwdLineEdit->clear();
        ui->loginPwdLineEdit->setFocus();
        return false;
    }
    return true;
}

void loginDialog::onRegisterDone(QString user)
{
    this->show();
    ui->userNameComboBox->setCurrentText(user);
    ui->loginPwdLineEdit->setFocus();
}

void loginDialog::onResetPwdDone(QString user)
{
    this->show();
    ui->userNameComboBox->setCurrentText(user);
    ui->loginPwdLineEdit->setFocus();
}

void loginDialog::receiveIPSet(QString h, int p)
{
    this->show();
    host = h;
    port = p;
}

/*void loginDialog::readFile()
{
    QFile file(filePath);
    map.clear();
    file.open(QIODevice::ReadOnly);
    QDataStream in(&file);
    QString user;
    QString pwd;

    while ( !stream.eof() ) {
    in>>user>>pwd;
    map.insert(user,pwd);
    }
    file.close();
}

void loginDialog::writeFile()
{
    QMap<QString, QString>::iterator it;
    QString str = ui->loginUsrNameLineEdit->text();
    it = map.find(str);
    if(it != map.end())
        map[str] = ui->loginPwdLineEdit->text();
    QFile file(filePath);
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);
    file.resize(0);
    QMap<QString, QString>::const_iterator i;
    for(i = map.constBegin(); i != map.constEnd(); ++i)
    {
        out<<i.value()<<i.key();
    }
    file.close();
}*/

void loginDialog::receiveRegister()
{
    this->show();
}

void loginDialog::receiveResetPwd()
{
    this->show();
}

void loginDialog::receiveStudentInfo()
{
    this->show();
    setUserInfo();
}

void loginDialog::receiveTeacherInfo()
{
    this->show();
    setUserInfo();
}

void loginDialog::on_registerPushButton_clicked()
{
    if(host == "")
    {
        host = "qq.ssfly.club";
        port = 3307;
        mysock.connect(host,port);
    }
    else
        mysock.connect(host,port);

    this->hide();
    emit showRegister();
}

void loginDialog::on_resetPwdPushButton_clicked()
{
    if(host == "")
    {
        host = "qq.ssfly.club";
        port = 3307;
        mysock.connect(host,port);
    }
    else
        mysock.connect(host,port);

    this->hide();
    emit showResetPwd();
}

void loginDialog::on_loginPushButton_clicked()
{
    if(host == "")
    {
        host = "qq.ssfly.club";
        port = 3307;
        mysock.connect(host,port);
    }
    else
        mysock.connect(host,port);

    if(!checkPwd())
        return;
    this->hide();
    QString userName = ui->userNameComboBox->currentText();
    if(rememberPwd)
    {
        updateUserInfo();
        ui->rememberPwd->setChecked(false);
    }
    ui->userNameComboBox->clear();
    ui->loginPwdLineEdit->clear();
    ui->userNameComboBox->setFocus();
    if(userName.mid(0,1) == "0")
    {
        emit showTeaInfo(userName);
    }
    else
        emit showStuInfo(userName);
}

void loginDialog::on_rememberPwd_clicked()
{
   if(ui->rememberPwd->isChecked())
       rememberPwd = true;
}

void loginDialog::setPwd()
{
    if(ui->userNameComboBox->currentText().isEmpty())
        return;
    QString str = ui->userNameComboBox->currentText();
    QMap<QString, QString>::iterator it;
    it = map.find(str);
    if(it != map.end())
        ui->loginPwdLineEdit->setText(it.value());
}

void loginDialog::on_setButton_clicked()
{
    this->hide();
    emit showIPSet();
}
