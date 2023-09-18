#pragma once

#include "common.h"
#include "ImageEditor.h"
#include "ui_BatchItImage.h"
#include "ui_MessageWindow.h"


class FileMetadataWorker : public QObject
{
    Q_OBJECT
public:
    FileMetadataWorker(std::string file, int load_order = 0, QObject* parent = nullptr);
    ~FileMetadataWorker();
private:
    std::string file_path;
    std::vector<struct FileMetadata>* current_file_metadata_list;
    FileMetadata* file_metadata = new FileMetadata{ 0, "", 0, 0, 0, 0, 0, false };
    int load_order = 0;
private slots:
    void GetFileMetadata();
signals:
    //void FileMetadataReady(const std::vector<struct FileMetadata> &file_list);
    void FileMetadataReady(FileMetadata* file_metadata);
};


class MessageWindow : public QDialog 
{
    Q_OBJECT
public:
    MessageWindow(QString title, QString message, QFlags<QDialogButtonBox::StandardButton> buttons, QWidget* parent = nullptr);
    ~MessageWindow();
    Ui::Dialog_MessageWindow ui;
signals:
    void ButtonClicked(const QDialogButtonBox::StandardButton&);
public slots:
    void ButtonBoxClicked(QAbstractButton* button);
protected:
    void changeEvent(QEvent* e);
private:
    //Ui::Dialog_MessageWindow* m_ui;
};


class BatchItImage : public QMainWindow
{
    Q_OBJECT

public:
    BatchItImage(QWidget *parent = nullptr);
    ~BatchItImage();

    void SavePreset(bool save_all = false);
    void LoadPreset(Preset);
    void LoadPresets();
    void LoadFileIntoTree(int);
    void BuildFileMetadataList(const QStringList file_list);
    std::string BytesToFileSizeString(std::uintmax_t);
    std::string CreateNewFileName(std::string file_name_changes, std::string change_data[]);
    int GetCurrentFileTreeRow();
    int IsFileInList(std::string path, std::vector<FileMetadata> list);
    int IsFileInList(std::string path, std::vector<FileMetadata> list, const size_t search_range[2]);
    std::vector<FileMetadata>::iterator IsFileInListIterator(std::string path, std::vector<FileMetadata> list);
    std::string ReplaceAll(std::string str, const std::string& from, const std::string& to, bool no_really_all = false);
    
public slots:
    void Test();
    void ChangePreset(int index);
    void LoadImageFiles();
    void AddNewFiles(QStringList file_list);
    void FileSelectionChange(bool checked);
    void EditAndSave();
    void AddTextToFileName(); 
    void CheckRelativePath();
    void CheckAbsolutePath();
    QString GetSaveDirectoryPath();
    void SortFileTreeByColumn(int index);
    void RemoveFileFromTree(const QDialogButtonBox::StandardButton& role);

private slots:
    void EnableSpecificFormatOptions(bool loading_preset = false);
    void HandleFileMetadata(FileMetadata* file_metadata);
    //void HandleFileMetadata(const std::vector<struct FileMetadata> &file_metadata);
    void DeleteConfirmationPopup(bool clear_all = false);

signals:
    void valueChanged(int value);
    void currentIndexChanged(int index);

private:
    Ui::BatchItImageClass ui;
    //Ui::Dialog_MessageWindow* ui_m;
    QString preset_settings_file;
    MessageWindow* m_window;
    bool m_window_shown = false;

    struct FormatJpegOptions {
        const enum { label_FormatFlags, label_Quality, checkBox_Optimize, checkBox_Progressive, label_Compression, label_ExtraSetting1, label_ExtraSetting2, COUNT };
    };
    struct FormatJp2Options { const enum { label_Compression, COUNT }; };
    struct FormatPngOptions { const enum { label_FormatFlags, checkBox_Optimize, label_Compression, COUNT }; };
    struct FormatExrOptions { const enum { label_FormatFlags, checkBox_Optimize, checkBox_Progressive, label_Compression, COUNT }; };

    // Label and Check Box Data
    UIData format_jpeg_options[FormatJpegOptions::COUNT];
    UIData format_jp2_options[FormatJp2Options::COUNT];
    UIData format_png_options[FormatPngOptions::COUNT];
    UIData format_exr_options[FormatExrOptions::COUNT];

    // Combo Box Data
    UIData width_selections[6];
    UIData height_selections[6];
    UIData resampling_selections[3];
    UIData file_name_creation[4];
    UIData image_formats[22];
    UIData format_jpeg_subsamplings[5];
    UIData format_png_compression[5];
    UIData format_pam_tupletype[6];
    UIData format_exr_compression[10];
    UIData format_hdr_compression[2];

    QString supported_image_extensions_dialog_str = ""; // Built from image_formats    

    const enum SaveOptionSelections { OVERWRITE, RENAME_ORG, NEW_NAME };
    const enum FileColumns { FILE_SELECTED, FILE_NAME, IMAGE_DIMENSIONS, FILE_SIZE, DATE_CREATED, DATE_MODIFIED, FILE_COLUMN_COUNT };
    const enum FileColumnsAlt { FILE_LOAD_ORDER, FILE_PATH, IMAGE_SIZES, FILE_SIZES, DATE_FILE_CREATED, DATE_FILE_MODIFIED, FILE_COLUMN_NUMBER };
    const enum SortOrders { ASCENDING1, DESCENDING1, ASCENDING2, DESCENDING2 };
    const std::string ADD_FILE_NAME = "<FILE_NAME>";
    const std::string ADD_COUNTER = "<COUNTER>";
    const std::string ADD_WIDTH = "<WIDTH>";
    const std::string ADD_HEIGHT = "<HEIGHT>";

    const QFont* font_serif = new QFont("Times", 10, QFont::Bold);
    const QFont* font_default = new QFont("Segoe UI", 9);
    const QFont* font_default_light = new QFont("Segoe UI", 9, QFont::Thin);
    const QFont* font_default_bold = new QFont("Segoe UI", 9, QFont::Bold);
    const QFont* font_mono = new QFont("New Courier", 9);
    const QFont* font_mono_bold = new QFont("New Courier", 9, QFont::Bold);

    QAction* action_add;
    QAction* action_delete;
    QAction* action_clear;
    QAction* action_select;
    QAction* action_view;
    QAction* action_preview;

    const std::filesystem::path default_path = std::filesystem::current_path();
    const QString qdefault_path = QString::fromStdString(default_path.string());
    QString GetLastExistingSavePath();

    std::string last_existing_load_path;
    std::string last_existing_save_path;
    std::string last_selected_format;

    bool search_subdirs = true;
    std::vector<struct Preset> preset_list;
    int current_selected_preset = 0;
    int current_file_column_sorted = -1;
    int current_file_sort_order = -1;
    float current_load_number = 0.0f;
    float load_interval  = 0.0f;

    std::vector<struct FileMetadata> current_file_metadata_list;
    std::vector<struct FileMetadata> deleted_file_metadata_list;
    int last_load_count = 0;

    void PopulateComboBoxes(QComboBox*, UIData*, int);
    void SetupFileTreeContextMenu();

    template<typename DirectoryIter>
    QStringList IterateDirectory(DirectoryIter iterator);

    void ToggleFileTreeContextMenuItems(bool enable);
    void ResizeFileTreeColumns();
    const std::function<void()> function_ResizeFileTreeColumns = std::bind(&BatchItImage::ResizeFileTreeColumns, this);
    void DeleteTreeItemObjects(QTreeWidget* tree, int row_index = -1);
    
    void SaveImageFile(cv::Mat* image, uint image_edits_made, int image_index, ImageEditor* ie);

    void UpdateProgressBar(std::function<void()> func, float multiplier = 1.0f);
    void UpdateProgressBar(float multiplier);
    void UpdateProgressBar();
    void InitiateProgressBar(int max_ticks, float multiplier);
    void InitiateProgressBar(int max_ticks);

    void DebugListPrint(int list);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
    void actionEvent(QActionEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
};
