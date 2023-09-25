#pragma once

#include "common.h"
#include "EnhancedSlider.h"
#include "ImageEditor.h"
#include "ui_BatchItImage.h"
#include "ui_DialogEditPresetDesc.h"
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
    void changeEvent(QEvent* event);
    void closeEvent(QCloseEvent* event) override;
private:
    //Ui::Dialog_MessageWindow* m_ui;
};


class DialogEditPresetDesc : public QDialog
{
    Q_OBJECT
public:
    DialogEditPresetDesc(QString title, QString message, std::vector<Preset>* preset_list, uint current_selected_preset, QWidget* parent = nullptr);
    ~DialogEditPresetDesc();
    Ui::Dialog_EditPresetDesc ui;
signals:
    void ButtonClicked(const QDialogButtonBox::StandardButton&);
private slots:
    void ButtonBoxClicked(QAbstractButton* button);
    void PresetIndexChanged(int index);
protected:
    void closeEvent(QCloseEvent* event) override;
private:
    std::vector<Preset>* preset_list;
    uint current_selected_preset;
    void UpdateComboBox();
};


class BatchItImage : public QMainWindow
{
    Q_OBJECT

public:
    BatchItImage(QWidget *parent = nullptr);
    ~BatchItImage();

    void LoadInUiData();
    void SavePreset(bool save_all = false);
    void LoadPreset(Preset);
    void LoadPresets();
    static void AddPresetsToComboBox(std::vector<Preset>* preset_list, QComboBox* preset_cb[], uint count = 1);
    bool SavePresetDialog();
    void LoadFileIntoTree(int, int = -1);
    void BuildFileMetadataList(const QStringList file_list);
    std::string BytesToFileSizeString(std::uintmax_t);
    std::string CreateNewFileName(std::string file_name_changes, std::string metadata_inserts[]);
    int GetCurrentFileTreeRow();
    int IsFileInList(std::string path, std::vector<FileMetadata> list);
    int IsFileInList(std::string path, std::vector<FileMetadata> list, const size_t search_range[2]);
    std::vector<FileMetadata>::iterator IsFileInListIterator(std::string path, std::vector<FileMetadata> list);
    std::string ReplaceAll(std::string str, const std::string& from, const std::string& to, bool no_really_all = false);
    
public slots:
    void Test();
    void ChangePreset(int index);
    void ChangePresetDescription();
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
    void UpdateComboBoxToolTip();
    void EnableSpecificFormatOptions(bool loading_preset = false);
    void HandleFileMetadata(FileMetadata* file_metadata);
    //void HandleFileMetadata(const std::vector<struct FileMetadata> &file_metadata);
    void DeleteConfirmationPopup(bool clear_all = false);

signals:
    void valueChanged(int value);
    void currentIndexChanged(int index);

private:
    Ui::BatchItImageClass ui;
    QString preset_settings_file;
    MessageWindow* m_window;
    bool m_window_shown = false;

    // Enums of named objects, widgets or methods
    const struct FileColumn {
        enum { FILE_SELECTED, FILE_NAME, IMAGE_DIMENSIONS, FILE_SIZE, DATE_CREATED, DATE_MODIFIED, COUNT };
        enum { FILE_LOAD_ORDER, FILE_PATH, IMAGE_SIZES, FILE_SIZES, DATE_FILE_CREATED, DATE_FILE_MODIFIED, FILE_COLUMN_COUNT };
    };
    const struct SortOrder { enum { ASCENDING1, DESCENDING1, ASCENDING2, DESCENDING2 }; };
    const struct ActionMenu { enum { action_add, action_delete, action_clear, action_select, action_view, action_preview, COUNT }; };
    const struct SaveOption { enum { OVERWRITE, RENAME_ORG, NEW_NAME }; };
    const struct FilePathOptions {
        enum {
            groupBox_FileRename, radioButton_Overwrite, radioButton_RenameOriginal, radioButton_NewFileName, label_Add,
            groupBox_SaveDir, radioButton_RelativePath, radioButton_AbsolutePath, pushButton_AddBackOneDir, pushButton_FindAbsolutePath, COUNT
        };
    };
    const struct FormatJpegOptions {
        enum {
            label_FormatFlags, label_Quality, checkBox_Optimize, checkBox_Progressive,
            label_Compression, label_ExtraSetting1, label_ExtraSetting2, COUNT
        };
    };
    const struct FormatJp2Options { enum { label_Compression, COUNT }; };
    const struct FormatPngOptions { enum { label_FormatFlags, checkBox_Optimize, label_Compression, COUNT }; };
    const struct FormatWebpOptions { enum { label_Quality, COUNT }; };
    const struct FormatAvifOptions { enum { label_Quality, label_Compression, label_ExtraSetting2, COUNT }; };
    const struct FormatPbmOptions { enum { checkBox_Optimize, COUNT }; };
    const struct FormatPamOptions { enum { label_FormatFlags, COUNT }; };
    const struct FormatTiffOptions { enum { label_FormatFlags, label_Quality, label_ExtraSetting1, label_ExtraSetting2, COUNT }; };
    const struct FormatExrOptions { enum { label_FormatFlags, checkBox_Optimize, checkBox_Progressive, label_Compression, COUNT }; };
    const struct FormatHdrOptions { enum { label_FormatFlags, COUNT }; };
    const struct OtherOptions { enum { tab_1, tab_2, tab_3, checkBox_SearchSubDirs, pushButton_EditAndSave, COUNT }; };
    const struct DialogMessages { enum { action_delete, action_clear, SavePresetDialog, HandleFileMetadata, CheckAbsolutePath, COUNT }; }; // TODO
    
    UIData dialog_messages[DialogMessages::COUNT]; // TODO
    
    // Tree Data
    UIData file_tree_headers[FileColumn::COUNT];
    UIData file_tree_menu_items[ActionMenu::COUNT];
    std::string file_tree_other_text[FileColumn::COUNT];

    // Tab, Label, Check Box, and Button Data
    UIData file_path_options[FilePathOptions::COUNT];
    UIData format_jpeg_options[FormatJpegOptions::COUNT];
    UIData format_jp2_options[FormatJp2Options::COUNT];
    UIData format_png_options[FormatPngOptions::COUNT];
    UIData format_webp_options[FormatPngOptions::COUNT];
    UIData format_avif_options[FormatAvifOptions::COUNT];
    UIData format_pbm_options[FormatPbmOptions::COUNT];
    UIData format_pam_options[FormatPamOptions::COUNT];
    UIData format_tiff_options[FormatTiffOptions::COUNT];
    UIData format_exr_options[FormatExrOptions::COUNT];
    UIData format_hdr_options[FormatHdrOptions::COUNT];
    UIData other_options[OtherOptions::COUNT];

    // Combo Box Data
    UIData width_selections[6];
    UIData height_selections[6];
    UIData resampling_selections[3];
    UIData file_name_creation[4];
    UIData image_formats[22];
    UIData format_jpeg_subsamplings[5];
    UIData format_png_compression[5];
    UIData format_pam_tupletype[6];
    UIData format_tiff_compression[24];
    UIData format_tiff_resolution_unit[3];
    UIData format_exr_compression[10];
    UIData format_hdr_compression[2];

    QString supported_image_extensions_dialog_str = ""; // Built from image_formats    

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

    //bool search_subdirs = true;
    std::vector<struct Preset> preset_list;
    int current_selected_preset = 0;
    bool unsaved_preset_settings = false; // TODO
    int current_file_column_sorted = -1;
    int current_file_sort_order = -1;
    float current_load_number = 0.0f;
    float load_interval  = 0.0f;

    std::vector<struct FileMetadata> current_file_metadata_list;
    std::vector<struct FileMetadata> deleted_file_metadata_list;
    int last_load_count = 0;

    void SetupFileTree();
    void AddUiObjectData(QObject* object, UIData* ui_data);
    void PopulateComboBox(QComboBox*, UIData*, int);
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
