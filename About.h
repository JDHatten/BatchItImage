#pragma once
#pragma comment(lib, "wldap32.lib")
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "Ws2_32.lib")
//#define CURL_STATICLIB

#ifndef   ABOUT_H
#define   ABOUT_H

#include "ui_DialogAbout.h"
#include <curl/curl.h>
#include <shellapi.h>
#include <windows.h>
#include <QtWidgets>
#include <QObject>
#include <QDialog>
#include <QString>

class DialogAbout : public QDialog
{
    Q_OBJECT

public:
    DialogAbout(QString dialog_title, QString app_title, QString app_version, QString app_creator, QString app_desciption, std::string readme, QWidget* parent = nullptr);
    ~DialogAbout();
    Ui::Dialog_About ui;

signals:
    //void buttonClicked(const QDialogButtonBox::StandardButton&);

private slots:
    //void buttonBoxClicked(QAbstractButton* button);

private:
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
    QString getDateLastUpdated();
    bool checkForUpdate();

protected:
    //void closeEvent(QCloseEvent* event) override;
};

#endif // ABOUT_H