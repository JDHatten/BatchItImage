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
#include <thread>
#include <windows.h>
#include <QtWidgets>
#include <QObject>
#include <QDialog>
#include <QString>

class DialogAbout : public QDialog
{
    Q_OBJECT

public:
    DialogAbout(QString dialog_title, QString app_title, QString app_version, QString app_creator, QString app_description,
        QString updating_message, QString update_available, QString update_not_available, std::string readme,
        int build_version, QWidget* parent = nullptr);
    ~DialogAbout();

signals:
    void updateFound(bool);

private slots:
    void showDownloadButton(bool show);

private:
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
    bool isUpdateAvailable(int build_version);
    Ui::Dialog_About ui;
    QString update_available;
    QString update_not_available;

protected:
    //void closeEvent(QCloseEvent* event) override;
};

#endif // ABOUT_H