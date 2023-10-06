#pragma once
#ifndef   BATCHITIMAGE_H
#define   BATCHITIMAGE_H

#include "common.h"
#include "EnhancedProgressBar.h"
#include "EnhancedSlider.h"
#include "ImageEditor.h"
#include "ImageSaver.h"
#include "ui_BatchItImage.h"
#include "ui_DialogEditPresetDesc.h"
#include "ui_MessageWindow.h"


class MessageWindow : public QDialog 
{
    Q_OBJECT
public:
    MessageWindow(QString title, QString message, QFlags<QDialogButtonBox::StandardButton> buttons, QWidget* parent = nullptr);
    ~MessageWindow();
    Ui::Dialog_MessageWindow ui;
signals:
    void buttonClicked(const QDialogButtonBox::StandardButton&);
public slots:
    void buttonBoxClicked(QAbstractButton* button);
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
    DialogEditPresetDesc(QString title, QString message, std::vector<Preset>* preset_list, uint preset_index, QWidget* parent = nullptr);
    ~DialogEditPresetDesc();
    Ui::Dialog_EditPresetDesc ui;
signals:
    void buttonClicked(const QDialogButtonBox::StandardButton&);
    void presetIndexSelected(int);
private slots:
    void buttonBoxClicked(QAbstractButton* button);
    void presetIndexChanged(int index);
protected:
    void closeEvent(QCloseEvent* event) override;
private:
    std::vector<Preset>* preset_list;
    uint current_selected_preset;
    void updateComboBox();
};


/// <summary>
/// A QObject that will take a file path and build a struct with metadata.
/// Intended to be ran in another thread as a "Worker".
/// Slot -> getFileMetadata() -> Signal -> fileMetadataReady(FileMetadata*)
/// </summary>
/// <param name="file_path">--A file path string.</param>
/// <param name="load_order">--This file metadata should be added now on initialization.</param>
/// <param name="parent">--Parent QObject</param>
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
    void getFileMetadata();
signals:
    void fileMetadataReady(FileMetadata* file_metadata);
};


class BatchItImage : public QMainWindow
{
    Q_OBJECT

public:
    BatchItImage(QWidget *parent = nullptr);
    ~BatchItImage();

    /// <summary>
    /// Save current (or all) preset to the preset list and to the user settings file.
    /// </summary>
    /// <param name="save_all">--Save all presets to settings file.</param>
    void SavePreset(bool save_all = false);
    /// <summary>
    /// Save a single preset to the settings file.
    /// </summary>
    /// <param name="checked">--The index of a preset in preset_list.</param>
    void SavePresetToSettingsFile(int index);
    /// <summary>
    /// Load a preset's data into various UI elements.
    /// </summary>
    /// <param name="preset">--A preset with all image edits to be made.</param>
    void LoadPreset(Preset);
    /// <summary>
    /// Load presets from settings file or get defaults if none found.
    /// </summary>
    void LoadPresets();
    /// <summary>
    /// Dialog asking user to save or discard any changes made to current preset before editing images.
    /// </summary>
    /// <returns>True if editing of images aborted by user.</returns>
    bool SavePresetDialog();
    /// <summary>
    /// [Static] Add preset descriptions to one or more combo boxes.
    /// </summary>
    /// <param name="preset_list">--Pointer to the preset list.</param>
    /// <param name="preset_cb">--Vector of combo box pointers.</param>
    static void AddPresetsToComboBox(std::vector<Preset>* preset_list, std::vector<QComboBox*> preset_cb);
    /// <summary>
    /// Returns the index of the currently selected preset.
    /// </summary>
    /// <returns></returns>
    uint CurrentSelectedPreset();
    /// <summary>
    /// Insert a file into the tree widget from the list of file metadata.
    /// </summary>
    /// <param name="file_index">--An index from "current_file_metadata_list"</param>
    /// <param name="sorted_column">--The column index that was last sorted. Default value is -1/none.</param>
    void LoadFileIntoTree(int, int = -1);
    /// <summary>
    /// Get the top level index of the current row highlighted in the file tree.
    /// </summary>
    /// <returns>Only the index of the top level row is returned.</returns>
    int GetCurrentFileTreeRow();
    /// <summary>
    /// Check if file path is already in a file list.
    /// </summary>
    /// <param name="path">--File path</param>
    /// <param name="list">--List of file metadata</param>
    /// <returns>Index pointing to where the path was found in list.</returns>
    int IsFileInList(std::string path, std::vector<FileMetadata> list);
    /// <summary>
    /// Check if file path is already in a file list.
    /// </summary>
    /// <param name="path">--File path</param>
    /// <param name="list">--List of file metadata</param>
    /// <param name="search_range">--A range to search, {start, end}.</param>
    /// <returns>Index pointing to where the path was found in list.</returns>
    int IsFileInList(std::string path, std::vector<FileMetadata> list, const size_t search_range[2]);
    /// <summary>
    /// Check if file path is already in a file list.
    /// </summary>
    /// <param name="path">--File path</param>
    /// <param name="list">--List of file metadata</param>
    /// <returns>Iterator where the path was found in list.</returns>
    std::vector<FileMetadata>::iterator IsFileInListIterator(std::string path, std::vector<FileMetadata> list);
    /// <summary>
    /// [Static] Find and replace all matched text.
    /// </summary>
    /// <param name="str">--The text to search.</param>
    /// <param name="from">--Find this text.</param>
    /// <param name="to">--Replace with this text</param>
    /// <param name="no_really_all">--If true will include cases where "to" is a substring of "from". Default is false.</param>
    /// <returns>An edited string.</returns>
    static const std::string ReplaceAll(std::string str, const std::string& from, const std::string& to, bool no_really_all = false);
    
public slots:
    void Test();
    /// <summary>
    /// Change the currently selected preset.
    /// </summary>
    /// <param name="index">--The index of the preset.</param>
    void ChangePreset(int index);
    /// <summary>
    /// Open dialog allowing user to change any preset description.
    /// </summary>
    /// <param name="selected_preset_index">--Index of preset to be edited.</param>
    /// <param name="title">--Title of dialog window.</param>
    /// <param name="message">--Label message of dialog window.</param>
    void ChangePresetDescription(int selected_preset_index, QString title, QString message);
    /// <summary>
    /// Create a new preset and open a dialog to name it.
    /// </summary>
    void CreateNewPreset();
    /// <summary>
    /// Start editing and saving images in file tree (in another thread).
    /// </summary>
    void EditAndSave();
    /// <summary>
    /// Show open file dialog allowing user to load one or more image files.
    /// </summary>
    void LoadImageFiles();
    /// <summary>
    /// Check for any directories and add any files found to the list before sending it too BuildFileMetadataList(). 
    /// </summary>
    /// <param name="file_list">--List of file and/or directory paths.</param>
    void AddNewFiles(QStringList file_list);
    /// <summary>
    /// Slot event called when file tree check box changed.
    /// </summary>
    /// <param name="checked">--Check box toggle.</param>
    void FileSelectionChange(bool checked);
    /// <summary>
    /// Sort a column in the file tree widget by sorting the list of file metadata then reinserting it back into the file tree.
    /// </summary>
    /// <param name="index">--The selected column index.</param>
    void SortFileTreeByColumn(int index);
    /// <summary>
    /// Remove one or more file rows from file tree.
    /// </summary>
    /// <param name="button_clicked">--Based on a clicked button's role delete one, multiple, or all rows.</param>
    void RemoveFileFromTree(const QDialogButtonBox::StandardButton& role);
    /// <summary>
    /// Add/insert special data from a combo box representing specific metadata to a line edit.
    /// </summary>
    void AddTextToFileName(); 
    /// <summary>
    /// Makes sure ui lineEdit_RelativePath uses platform specific slashes, converting all in current text. 
    /// </summary>
    void CheckRelativePath();
    /// <summary>
    /// Opens a directory dialog to obtain an existing directory path.
    /// </summary>
    /// <returns>A QString directory path.</returns>
    QString GetSaveDirectoryPath();

private slots:
    /// <summary>
    /// [Slot Only] Update the sender combo box status and tool tip when signal sent.
    /// </summary>
    void UpdateComboBoxTextTips();
    /// <summary>
    /// Enable specific ui options depending on which blur filter option is currently selected.
    /// </summary>
    /// <param name="loading_preset">--If loading presets do not insert any default values. Default is false.</param>
    void EnableSpecificBlurOptions(bool loading_preset = false);
    /// <summary>
    /// Enable format specific ui options depending on which format change option is currently selected.
    /// Each format makes use of a different set of options.
    /// </summary>
    /// <param name="loading_preset">--If loading presets do not insert any default values. Default is false.</param>
    void EnableSpecificFormatOptions(bool loading_preset = false);
    /// <summary>
    /// Build a list of files with it's metadata each on another thread. Results will be sent to HandleFileMetadata().
    /// </summary>
    /// <param name="file_list">--List of file paths.</param>
    void BuildFileMetadataList(const QStringList file_list);
    /// <summary>
    /// A callback function that handles a file's metadata after it is made by adding it to the current list and then file tree.
    /// </summary>
    /// <param name="file_metadata">--A pointer to the metadata.</param>
    void HandleFileMetadata(FileMetadata* file_metadata);
    /// <summary>
    /// Save edited image file using the current ui options.
    /// </summary>
    /// <param name="image_index">--Image index in current list of images to be edited.</param>
    /// <param name="image_editor">--Pointer to the ImageEditor used to edit image.</param>
    void SaveImageFile(int image_index, ImageEditor* image_editor);
    /// <summary>
    /// Update log with all the image edits made and file saving details.
    /// </summary>
    /// <param name="image_editor">--Pointer to the ImageEditor used to edit image.</param>
    /// <param name="saved_image">--Pointer the the ImageSaver used to save image.</param>
    void UpdateLog(ImageEditor* edited_image, ImageSaver* saved_image);
    /// <summary>
    /// Resize all columns to fit content in file tree.
    /// </summary>
    void ResizeFileTreeColumns();
    /// <summary>
    /// Confirmation popup asking user how to handle the deleting of files in file tree.
    /// </summary>
    /// <param name="clear_all">--If true will clear file list entirely, else deletes are selective.</param>
    void DeleteConfirmationPopup(bool clear_all = false);
    /// <summary>
    /// Check if path from ui lineEdit_AbsolutePath exists and if it doesn't, replace text with last existing path or a default path.
    /// </summary>
    void CheckAbsolutePath();

signals:
    void valueChanged(int value);
    void currentIndexChanged(int index);
    void progressMade(float multiplier = 1.0f);

private:
    //QMainWindow* main_object;
    Ui::BatchItImageClass ui;
    QString preset_settings_file;
    MessageWindow* m_window;
    bool m_window_shown = false;

    // Tracks preset options changed in the ui.
    struct OptionTracker {
        ulong i = 1;
        const uint NoChange = 0;

        // Image Edit
        const uint comboBox_WidthMod = i;
        const uint spinBox_WidthNumber = iPlus(i);
        const uint comboBox_HeightMod = iPlus(i);
        const uint spinBox_HeightNumber = iPlus(i);
        const uint comboBox_Resample = iPlus(i);
        const uint checkBox_KeepAspectRatio = iPlus(i);

        const uint comboBox_BorderType = iPlus(i);
        const uint pushButton_ColorPicker = iPlus(i);

        const uint comboBox_BlurFilter = iPlus(i);
        const uint checkBox_BlurNormalize = iPlus(i);
        const uint verticalSlider_BlurX1 = iPlus(i);
        const uint verticalSlider_BlurY1 = iPlus(i);
        const uint verticalSlider_BlurX2 = iPlus(i);
        const uint verticalSlider_BlurY2 = iPlus(i);
        const uint verticalSlider_BlurD = iPlus(i);

        const uint dial_Rotation = iPlus(i);
        const uint checkBox_IncreaseBounds = iPlus(i);
        const uint checkBox_FlipImage = iPlus(i);

        const uint groupBox_Watermark = iPlus(i);
        const uint lineEdit_WatermarkPath = iPlus(i);
        const uint comboBox_WatermarkLocation = iPlus(i);
        const uint spinBox_WatermarkTransparency = iPlus(i);
        const uint spinBox_WatermarkOffset = iPlus(i);

        // Image Save
        const uint radioButton_Overwrite = reset();
        const uint radioButton_RenameOriginal = iPlus(i);
        const uint radioButton_NewFileName = iPlus(i);
        const uint lineEdit_FileName = iPlus(i);

        const uint radioButton_RelativePath = iPlus(i);
        const uint lineEdit_RelativePath = iPlus(i);
        const uint radioButton_AbsolutePath = iPlus(i);
        const uint lineEdit_AbsolutePath = iPlus(i);

        const uint groupBox_ChangeFormat = iPlus(i);
        const uint comboBox_ImageFormat = iPlus(i);
        const uint comboBox_FormatFlags = iPlus(i);
        const uint horizontalSlider_Quality = iPlus(i);
        const uint checkBox_Optimize = iPlus(i);
        const uint checkBox_Progressive = iPlus(i);
        const uint spinBox_Compression = iPlus(i);
        const uint spinBox_ExtraSetting1 = iPlus(i);
        const uint spinBox_ExtraSetting2 = iPlus(i);

        uint iPlus(uint next) {
            return i = i + next;
        }
        uint reset() {
            return i = 1;
        }
        std::string printAllTrackers() const {
            return
                "\n  NoChange:                      " + std::to_string(NoChange) +
                "\n  comboBox_WidthMod:             " + std::to_string(comboBox_WidthMod) +
                "\n  spinBox_WidthNumber:           " + std::to_string(spinBox_WidthNumber) +
                "\n  comboBox_HeightMod:            " + std::to_string(comboBox_HeightMod) +
                "\n  spinBox_HeightNumber:          " + std::to_string(spinBox_HeightNumber) +
                "\n  comboBox_Resample:             " + std::to_string(comboBox_Resample) +
                "\n  checkBox_KeepAspectRatio:      " + std::to_string(checkBox_KeepAspectRatio) +

                "\n  comboBox_BorderType:           " + std::to_string(comboBox_BorderType) +
                "\n  pushButton_ColorPicker:        " + std::to_string(pushButton_ColorPicker) +

                "\n  comboBox_BlurFilter:           " + std::to_string(comboBox_BlurFilter) +
                "\n  checkBox_BlurNormalize:        " + std::to_string(checkBox_BlurNormalize) +
                "\n  verticalSlider_BlurX1:         " + std::to_string(verticalSlider_BlurX1) +
                "\n  verticalSlider_BlurY1:         " + std::to_string(verticalSlider_BlurY1) +
                "\n  verticalSlider_BlurX2:         " + std::to_string(verticalSlider_BlurX2) +
                "\n  verticalSlider_BlurY2:         " + std::to_string(verticalSlider_BlurY2) +
                "\n  verticalSlider_BlurD:          " + std::to_string(verticalSlider_BlurD) +

                "\n  dial_Rotation:                 " + std::to_string(dial_Rotation) +
                "\n  checkBox_IncreaseBounds:       " + std::to_string(checkBox_IncreaseBounds) +
                "\n  checkBox_FlipImage:            " + std::to_string(checkBox_FlipImage) +

                "\n  groupBox_Watermark:            " + std::to_string(groupBox_Watermark) +
                "\n  lineEdit_WatermarkPath:        " + std::to_string(lineEdit_WatermarkPath) +
                "\n  comboBox_WatermarkLocation:    " + std::to_string(comboBox_WatermarkLocation) +
                "\n  spinBox_WatermarkTransparency: " + std::to_string(spinBox_WatermarkTransparency) +
                "\n  spinBox_WatermarkOffset:       " + std::to_string(spinBox_WatermarkOffset) +

                "\n  radioButton_Overwrite:         " + std::to_string(radioButton_Overwrite) +
                "\n  radioButton_RenameOriginal:    " + std::to_string(radioButton_RenameOriginal) +
                "\n  radioButton_NewFileName:       " + std::to_string(radioButton_NewFileName) +
                "\n  lineEdit_FileName:             " + std::to_string(lineEdit_FileName) +

                "\n  radioButton_RelativePath:      " + std::to_string(radioButton_RelativePath) +
                "\n  lineEdit_RelativePath:         " + std::to_string(lineEdit_RelativePath) +
                "\n  radioButton_AbsolutePath:      " + std::to_string(radioButton_AbsolutePath) +
                "\n  lineEdit_AbsolutePath:         " + std::to_string(lineEdit_AbsolutePath) +

                "\n  groupBox_ChangeFormat:         " + std::to_string(groupBox_ChangeFormat) +
                "\n  comboBox_ImageFormat:          " + std::to_string(comboBox_ImageFormat) +
                "\n  comboBox_FormatFlags:          " + std::to_string(comboBox_FormatFlags) +
                "\n  horizontalSlider_Quality:      " + std::to_string(horizontalSlider_Quality) +
                "\n  checkBox_Optimize:             " + std::to_string(checkBox_Optimize) +
                "\n  checkBox_Progressive:          " + std::to_string(checkBox_Progressive) +
                "\n  spinBox_Compression:           " + std::to_string(spinBox_Compression) +
                "\n  spinBox_ExtraSetting1:         " + std::to_string(spinBox_ExtraSetting1) +
                "\n  spinBox_ExtraSetting2:         " + std::to_string(spinBox_ExtraSetting2) +
                "\n";
        }
    } Option;
    ulong edit_options_changed = 0;
    ulong save_options_changed = 0;

    // Enums of named objects, widgets or methods
    const struct FileColumn {
        enum { FILE_SELECTED, FILE_NAME, IMAGE_DIMENSIONS, FILE_SIZE, DATE_CREATED, DATE_MODIFIED, COUNT };
        enum { FILE_LOAD_ORDER, FILE_PATH, IMAGE_SIZES, FILE_SIZES, DATE_FILE_CREATED, DATE_FILE_MODIFIED, FILE_COLUMN_COUNT };
    };
    const struct SortOrder { enum { ASCENDING1, DESCENDING1, ASCENDING2, DESCENDING2 }; };
    const struct ActionMenu { enum { action_add, action_delete, action_clear, action_select, action_view, action_preview, COUNT }; };
    const struct ResizeOptions { enum { groupBox_Resize, checkBox_KeepAspectRatio, COUNT }; };
    const struct BackgroundOptions { enum { groupBox_Background, pushButton_ColorPicker, label_ColorPreview, COUNT }; };
    const struct BlurOptions {
        enum { groupBox_Blur, checkBox_BlurNormalize, label_BlurX1, label_BlurY1, label_BlurX2, label_BlurY2, label_BlurD, COUNT };
    };
    const struct RotationOptions { enum { groupBox_Rotation, checkBox_IncreaseBounds, checkBox_FlipImage, COUNT }; };
    const struct FilePathOptions {
        enum { groupBox_FileRename, radioButton_Overwrite, radioButton_RenameOriginal, radioButton_NewFileName,
            label_Add, groupBox_SaveDir, radioButton_RelativePath, radioButton_AbsolutePath, pushButton_AddBackOneDir, 
            pushButton_FindAbsolutePath, COUNT };
    };
    const struct FormatJpegOptions {
        enum { label_FormatFlags, label_Quality, checkBox_Optimize, checkBox_Progressive,
            label_Compression, label_ExtraSetting1, label_ExtraSetting2, COUNT };
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
    const struct DialogMessages { enum { action_delete, action_clear, SavePresetDialog, HandleFileMetadata, CheckAbsolutePath, COUNT }; };
        
    // Tree UIData (Arrays placed on the heap will be deleted after use.)
    std::array<UIData, FileColumn::COUNT>* file_tree_headers = new std::array<UIData, FileColumn::COUNT>;
    std::array<UIData, ActionMenu::COUNT>* file_tree_menu_items = new std::array<UIData, ActionMenu::COUNT>;
    std::array<std::string, FileColumn::COUNT> file_tree_other_text;

    // Tab, Label, Check Box, and Button UIData
    std::array<UIData, FilePathOptions::COUNT>* file_path_options = new std::array<UIData, FilePathOptions::COUNT>;
    std::array<UIData, ResizeOptions::COUNT>* resize_options = new std::array<UIData, ResizeOptions::COUNT>;
    std::array<UIData, BackgroundOptions::COUNT>* background_options = new std::array<UIData, BackgroundOptions::COUNT>; // TODO: desc
    std::array<UIData, BlurOptions::COUNT>* blur_options = new std::array<UIData, BlurOptions::COUNT>; // TODO: desc
    std::array<UIData, RotationOptions::COUNT>* rotation_options = new std::array<UIData, RotationOptions::COUNT>; // TODO: desc

    std::array<UIData, FormatJpegOptions::COUNT> format_jpeg_options;
    std::array<UIData, FormatJp2Options::COUNT> format_jp2_options;
    std::array<UIData, FormatPngOptions::COUNT> format_png_options;
    std::array<UIData, FormatPngOptions::COUNT> format_webp_options;
    std::array<UIData, FormatAvifOptions::COUNT> format_avif_options;
    std::array<UIData, FormatPbmOptions::COUNT> format_pbm_options;
    std::array<UIData, FormatPamOptions::COUNT> format_pam_options;
    std::array<UIData, FormatTiffOptions::COUNT> format_tiff_options;
    std::array<UIData, FormatExrOptions::COUNT> format_exr_options;
    std::array<UIData, FormatHdrOptions::COUNT> format_hdr_options;
    std::array<UIData, OtherOptions::COUNT>* other_options = new std::array<UIData, OtherOptions::COUNT>;

    // Combo Box UIData
    std::array<UIData, 6>* width_selections = new std::array<UIData, 6>;
    std::array<UIData, 6>* height_selections = new std::array<UIData, 6>;
    std::array<UIData, 3>* resampling_selections = new std::array<UIData, 3>; // TODO: add more Resampling Filters
    std::array<UIData, 7>* border_types = new std::array<UIData, 7>;
    std::array<UIData, 7>* blur_filters = new std::array<UIData, 7>;
    std::array<UIData, 4>* file_name_creation = new std::array<UIData, 4>;
    //std::array<UIData, 23>* image_formats = new std::array<UIData, 23>;
    std::array<UIData, 23> image_formats;
    std::array<UIData, 5> format_jpeg_subsamplings;
    std::array<UIData, 5> format_png_compression;
    std::array<UIData, 6> format_pam_tupletype;
    std::array<UIData, 24> format_tiff_compression;
    std::array<UIData, 3> format_tiff_resolution_unit;
    std::array<UIData, 10> format_exr_compression;
    std::array<UIData, 2> format_hdr_compression;

    // Other UIData
    std::array<UIData, DialogMessages::COUNT> dialog_messages; // TODO
    std::array<UIData, 6> blur_depth_selections;

    QString supported_image_extensions_dialog_str = ""; // Built from image_formats    

    const QFont* font_serif = new QFont("Times", 10, QFont::Bold);
    const QFont* font_default = new QFont("Segoe UI", 9);
    const QFont* font_default_light = new QFont("Segoe UI", 9, QFont::Thin);
    const QFont* font_default_bold = new QFont("Segoe UI", 9, QFont::Bold);
    const QFont* font_mono = new QFont("New Courier", 9);
    const QFont* font_mono_bold = new QFont("New Courier", 9, QFont::Bold);

    // Right Click Menu Items
    QAction* action_add;
    QAction* action_delete;
    QAction* action_clear;
    QAction* action_select;
    QAction* action_view;
    QAction* action_preview;

    // File Paths
    const std::filesystem::path default_path = std::filesystem::current_path();
    const QString qdefault_path = QString::fromStdString(default_path.string());
    std::string last_existing_load_path;
    std::string last_existing_save_path;
    std::string last_selected_format;

    // Presets
    std::vector<struct Preset> preset_list;
    std::vector<struct FileMetadata> current_file_metadata_list;
    std::vector<struct FileMetadata> deleted_file_metadata_list;
    uint current_selected_preset = 0;
    bool unsaved_preset_settings = false; // TODO

    int current_file_column_sorted = -1;
    int current_file_sort_order = -1;
    float current_load_number = 0.0f;
    float load_interval  = 0.0f;
    int last_load_count = 0;

    const std::function<void()> function_ResizeFileTreeColumns = std::bind(&BatchItImage::ResizeFileTreeColumns, this);

    /// <summary>
    /// Load in all text and other ui data.
    /// </summary>
    void LoadInUiData();
    /// <summary>
    /// Setup file tree with initial settings and header titles.
    /// </summary>
    void SetupFileTree();
    /// <summary>
    /// Add display text, tooltip descriptions, and other data to various types of ui objects/widgets.
    /// </summary>
    /// <param name="ui_data">--Reference to an array of UIData.</param>
    /// <param name="objects">--A list of pointers to objects/widgets.</param>
    template<std::size_t array_size>void AddUiDataTo(const std::array<UIData, array_size>& ui_data, const std::vector<QWidget*>& objects);
    /// <summary>
    /// Add display text, tooltip descriptions, and other data to various types of ui objects/widgets.
    /// </summary>
    /// <param name="object">--Pointer to an object/widget.</param>
    /// <param name="ui_data">--Reference to a UIData.</param>
    void AddUiDataTo(QObject* object, const UIData& ui_data);
    /// <summary>
    /// Add items/data to various combo boxes which include titles, tooltip descriptions, and other data.
    /// </summary>
    /// <param name="cb">--Pointer to a QComboBox.</param>
    /// <param name="ui_data">--Data array to enter into a combo box.</param>
    template<std::size_t array_size>void PopulateComboBox(QComboBox* cb, const std::array<UIData, array_size>& ui_data);
    /// <summary>
    /// Update the a combo box status and tool tip.
    /// </summary>
    /// <param name="">--.</param>
    void UpdateComboBoxTextTips(QComboBox* cb); // unused
    /// <summary>
    /// Connect all initial Ui object events (signals and slots).
    /// </summary>
    void UiConnections();
    /// <summary>
    /// Update the track changes to options.
    /// </summary>
    /// <param name="preset_value">--The saved preset option value.</param>
    /// <param name="changed_option_value">--The unsaved changed option value.</param>
    /// <param name="tracked_option">--The tracking code of changed option.</param>
    bool UpdateOptionsChanged(int preset_value, int changed_option_value, uint tracked_option);
    /// <summary>
    /// Remove group of options from edit_options_changed.
    /// </summary>
    /// <param name="tracked_options">--Vector of tracked options.</param>
    void RemoveOptionsChanged(std::vector<uint> tracked_options);
    /// <summary>
    /// Build a "right click" context menu for the file tree.
    /// </summary>
    void SetupFileTreeContextMenu();
    /// <summary>
    /// Toggle usability of certain action context menu items.
    /// </summary>
    /// <param name="enable">--True enabled, false disabled</param>
    void ToggleFileTreeContextMenuItems(bool enable);
    /// <summary>
    /// Iterate over a directory and append every supported image file found to a list.
    /// </summary>
    /// <typeparam name="DirectoryIter"></typeparam>
    /// <param name="iterator">--"directory_iterator" or "recursive_directory_iterator"</param>
    /// <returns>QStringList of file paths.</returns>
    template<typename DirectoryIter>QStringList IterateDirectory(DirectoryIter iterator);
    /// <summary>
    /// Delete all widget objects in a widget tree's row or every row if "row_index" is not set.
    /// </summary>
    /// <param name="tree">--The tree widget</param>
    /// <param name="row_index">--The row to delete. Default all rows.</param>
    void DeleteTreeItemObjects(QTreeWidget* tree, int row_index = -1);
    /// <summary>
    /// Convert bytes into a formatted file size string -> KB -> MB -> GB -> TB.
    /// </summary>
    /// <param name="bytes">--File size in bytes.</param>
    /// <returns>A Formatted String - Example: " 1.37 MB "</returns>
    std::string BytesToFileSizeString(std::uintmax_t);
    /// <summary>
    /// Get the last existing directory path or a default path if none.
    /// </summary>
    /// <returns>A QString directory path.</returns>
    QString GetLastExistingSavePath();

protected:
    /// <summary>
    /// Overridden protected function that watches an objects events.
    /// </summary>
    /// <param name="watched">--Widget</param>
    /// <param name="event"></param>
    /// <returns>If true the event has been intercepted, no pass-through.</returns>
    bool eventFilter(QObject* watched, QEvent* event) override;
    /// <summary>
    /// Overridden protected function that intercepts file urls dropped.
    /// </summary>
    /// <param name="event"></param>
    void dropEvent(QDropEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    /// <summary>
    /// Overridden protected function that can intercept keyboard key presses.
    /// </summary>
    /// <param name="event"></param>
    void keyPressEvent(QKeyEvent* event) override;
};

#endif // BATCHITIMAGE_H