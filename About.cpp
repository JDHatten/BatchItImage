#include "About.h"

DialogAbout::DialogAbout(QString dialog_title, QString app_title, QString app_version, QString app_creator, QString app_description,
    QString updating_message, QString update_available, QString update_not_available, std::string readme, QWidget* parent)
{
    ui.setupUi(this);
    ui.commandLinkButton_Download->setVisible(false);
    ui.commandLinkButton_Download->setFlat(true);

    setWindowTitle(dialog_title);
    ui.label_AppTitle->setText(app_title);
    ui.label_AppVersion->setText(app_version);
    ui.label_AppCreator->setText(app_creator);
    ui.label_AppDescription->setText(app_description);
    ui.label_Version->setText(updating_message);

    DialogAbout::update_available = update_available;
    DialogAbout::update_not_available = update_not_available;

    auto worker_thread = std::thread(&DialogAbout::isUpdateAvailable, this);

    Q_ASSERT(connect(ui.pushButton_OK, &QPushButton::clicked, this, [this] { accept(); }));
    Q_ASSERT(connect(ui.pushButton_ReadMe, &QPushButton::clicked, this, 
        [=] {
            ShellExecute(0, 0, std::wstring(readme.begin(), readme.end()).c_str(), 0, 0, SW_SHOW);
        }));
    Q_ASSERT(connect(ui.pushButton_GitHub, &QPushButton::clicked, this, 
        [this] {
            ShellExecute(0, 0, L"https://github.com/JDHatten/BatchItImage", 0, 0, SW_SHOW);
        }));
    Q_ASSERT(connect(this, SIGNAL(updateFound(bool)), this, SLOT(showDownloadButton(bool))));

    worker_thread.detach();
}

size_t DialogAbout::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

DialogAbout::~DialogAbout()
{}

void DialogAbout::showDownloadButton(bool show)
{
    ui.commandLinkButton_Download->setVisible(show);
    if (show) {
        ui.label_Version->setText(update_available);
        Q_ASSERT(connect(ui.commandLinkButton_Download, &QPushButton::clicked, this,
            [this] {
                ShellExecute(0, 0, L"https://github.com/JDHatten/BatchItImage/releases/latest", 0, 0, SW_SHOW);
            }));
    }
    else {
        ui.label_Version->setText(update_not_available);
    }
}

bool DialogAbout::isUpdateAvailable()
{
    CURL* curl;
    CURLcode curl_res;
    std::string read_buffer;
    int major_version = 1;
    int minor_version = 0;
    int build_version = 0;
    bool is_update_available = false;
    
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://github.com/JDHatten/BatchItImage/releases/latest");
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);

        // Note: Must get this data or it will be auto sent to the console.
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &read_buffer);

        curl_res = curl_easy_perform(curl);

        if (curl_res == CURLE_OK) {
            char* url;
            curl_res = curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &url);
            //qDebug() << read_buffer;

            if (curl_res == CURLE_OK && url) {
                qDebug() << "CURLINFO_EFFECTIVE_URL:" << url;
                std::string version = std::filesystem::path(url).filename().string();
                //qDebug() << version;

                // Split app version into 3 numbers
                std::vector<int> version_split;
                version_split.reserve(3);
                for (char& c : version)
                    if (not isdigit(c))
                        c = ' ';
                int value;
                std::stringstream ss(version);
                while (ss >> value)
                    version_split.push_back(value);

                qDebug() << version_split;

                if (not version_split.empty())
                    if (version_split.size() > 2)
                        if (version_split.at(2) > build_version)
                            is_update_available = true;
            }
        }
        else {
            qDebug() << "curl_easy_perform() failed:" << curl_easy_strerror(curl_res);
        }
        curl_easy_cleanup(curl);
    }
    emit updateFound(is_update_available);
    return is_update_available;
}
