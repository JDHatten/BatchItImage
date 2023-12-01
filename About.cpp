#include "About.h"

DialogAbout::DialogAbout(QString dialog_title, QString app_title, QString app_version, QString app_creator, QString app_description, std::string readme, QWidget* parent)
{
    ui.setupUi(this);
    setWindowTitle(dialog_title);
    ui.label_AppTitle->setText(app_title);
    ui.label_AppVersion->setText(app_version);
    ui.label_AppCreator->setText(app_creator);
    ui.label_AppDescription->setText(app_description);

    checkForUpdate();

    Q_ASSERT(connect(ui.pushButton_OK, &QPushButton::clicked, this, 
        [this] {
            accept();
        }));
    Q_ASSERT(connect(ui.pushButton_ReadMe, &QPushButton::clicked, this, 
        [=] {
            ShellExecute(0, 0, std::wstring(readme.begin(), readme.end()).c_str(), 0, 0, SW_SHOW);
        }));
    Q_ASSERT(connect(ui.pushButton_GitHub, &QPushButton::clicked, this, 
        [this] {
            ShellExecute(0, 0, L"https://github.com/JDHatten/BatchItImage", 0, 0, SW_SHOW);
        }));
}

size_t DialogAbout::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

DialogAbout::~DialogAbout()
{}

QString DialogAbout::getDateLastUpdated()
{

    return "";
}

bool DialogAbout::checkForUpdate()
{
    CURL* curl;
    CURLcode curl_res;
    std::string read_buffer;
    int major_version = 1;
    int minor_version = 0;
    int build_version = 0;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://github.com/JDHatten/BatchItImage/releases/latest");
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);

        // Note: Must get this data or it will auto send it to console.
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &read_buffer);

        curl_res = curl_easy_perform(curl);

        // Check for errors
        if (curl_res != CURLE_OK) {
            qDebug() << "curl_easy_perform() failed:" << curl_easy_strerror(curl_res);
        }

        if (curl_res == CURLE_OK) {
            char* url;
            curl_res = curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &url);

            if (curl_res == CURLE_OK && url)
                qDebug() << "CURLINFO_EFFECTIVE_URL:" << url;
        }

        curl_easy_cleanup(curl);

        //qDebug() << read_buffer;
    }

    return false;
}
