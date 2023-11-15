#pragma once
#ifndef   BATCHITIMAGE_H
#define   BATCHITIMAGE_H

#include "common.h"
#include "Preset.h"
#include "EnhancedProgressBar.h"
#include "EnhancedSlider.h"
#include "ImageEditor.h"
#include "ImageSaver.h"
#include "ui_BatchItImage.h"
#include "ui_DialogEditPresetDesc.h"
#include "ui_DialogMessage.h"


// Enums
namespace UI {
    namespace MenuBar { // TODO
        const struct File {
            enum {
                menu_File, action_AddImages, menu_RecentImageFiles, action_load_all_files, action_clear_all_files, action_OpenLogDirectory, action_Close, COUNT
            };
        };
        const struct Edit {
            enum {
                menu_Edit, action_AddNewPreset, action_RemovePreset, action_SavePresets, action_ChangePresetDesc, action_ShowFormatFilter, COUNT
            };
        };
        const struct Help {
            enum { menu_Help, action_About, action_AboutQt, action_Help, COUNT };
        };

    };
    namespace EditOption {
        const struct Resize { enum { groupBox_Resize, checkBox_KeepAspectRatio, COUNT }; };
        const struct Background { enum { groupBox_Background, pushButton_ColorDialog, label_ColorPreview, COUNT }; };
        const struct Blur {
            enum {
                groupBox_Blur, checkBox_BlurNormalize, label_BlurX1, label_BlurY1, label_BlurX2, label_BlurY2, label_BlurD, COUNT
            };
        };
        const struct Rotation { enum { groupBox_Rotation, checkBox_IncreaseBounds, checkBox_FlipImage, COUNT }; };
        const struct Watermark {
            enum {
                groupBox_Watermark, pushButton_Watermark, label_WatermarkLocation, label_WatermarkTransparency,
                label_WatermarkOffset, COUNT
            };
        };
    };
    namespace FileOption {
        const struct FilePath {
            enum {
                groupBox_FileRename, radioButton_Overwrite, radioButton_RenameOriginal, radioButton_NewFileName, label_Add,
                groupBox_SaveDir, radioButton_RelativePath, radioButton_AbsolutePath, pushButton_AddBackOneDir,
                pushButton_FindAbsolutePath, COUNT
            };
        };
        const struct FormatJpeg {
            enum {
                label_FormatFlags, label_Quality, checkBox_Optimize, checkBox_Progressive, label_Compression, label_ExtraSetting1,
                label_ExtraSetting2, COUNT
            };
        };
        const struct FormatJp2 { enum { label_Compression, COUNT }; };
        const struct FormatPng { enum { label_FormatFlags, checkBox_Optimize, label_Compression, COUNT }; };
        const struct FormatWebp { enum { label_Quality, COUNT }; };
        const struct FormatAvif { enum { label_Quality, label_Compression, label_ExtraSetting2, COUNT }; };
        const struct FormatPbm { enum { checkBox_Optimize, COUNT }; };
        const struct FormatPam { enum { label_FormatFlags, COUNT }; };
        const struct FormatTiff { enum { label_FormatFlags, label_Quality, label_ExtraSetting1, label_ExtraSetting2, COUNT }; };
        const struct FormatExr { enum { label_FormatFlags, checkBox_Optimize, checkBox_Progressive, label_Compression, COUNT }; };
        const struct FormatHdr { enum { label_FormatFlags, COUNT }; };
    }; 
    const struct StatusBar {
        enum { SavePreset, clear_all_files, COUNT };
    };
    const struct Other {
        enum {
            tab_1, tab_2, tab_3, checkBox_SearchSubDirs, label_EditSave, pushButton_EditSaveAll, pushButton_EditSaveSelected, COUNT
        };
    };
};
namespace FileTree {
    namespace ActionMenu {
        const struct MainMenu {
            enum {
                action_add, action_delete, action_clear, action_select, action_unselect, action_select_all, action_select_none,
                action_view, action_preview, COUNT
            };
        };
        namespace SubMenu {
            const struct FilterImageFormats {
                enum { // Note: action_submenu_filter = line, takes same spot in "Actions List"
                    action_submenu_undock, action_submenu_filter, action_filter_jpeg, action_filter_jp2, action_filter_png,
                    action_filter_webp, action_filter_bmp, action_filter_avif, action_filter_pbm, action_filter_sr, action_filter_tiff,
                    action_filter_exr, action_filter_hdr, COUNT
                };
            };
        };
    };
    const struct Column {
        enum { FILE_SELECTED, FILE_NAME, IMAGE_DIMENSIONS, FILE_SIZE, DATE_CREATED, DATE_MODIFIED, COUNT };
        enum { FILE_LOAD_ORDER, FILE_PATH, IMAGE_SIZES, FILE_SIZES, DATE_FILE_CREATED, DATE_FILE_MODIFIED, FILE_COLUMN_COUNT };
    };
    const struct SortOrder {
        enum { ASCENDING1, DESCENDING1, ASCENDING2, DESCENDING2 };
    };
};
namespace Dialog {
    const struct Messages {
        enum {
            delete_dialog, delete_dialog_clear, CreateNewPreset, ChangePresetDescription, save_preset_dialog, save_preset_dialog_closing,
            remove_preset_dialog, remove_preset_dialog_halted, non_image_file_dialog, check_wm_path_dialog, check_path_dialog,
            log_created_dialog, log_created_dialog_updated, log_created_dialog_error, COUNT
        };
    };
    const struct Buttons { // TODO
        enum {
            COUNT
        };
    };
    const struct FileSearch {
        enum {
            load_image_files_dialog, load_image_files_dialog_dir, get_image_file_path_dialog, GetSaveDirectoryPath, all_images_dialog_str, all_files_dialog_str,
            log_file_new_save_path, log_file_new_save_path_extensions, COUNT
        };
    };
};
namespace LogFile{
    const struct Line {
        enum {
            ThickDivider, ThinDivider, Title, SessionStart, SessionEnd, Batch, SummarySuccesses, SummaryErrors, SummaryTime,
            UnsavedSettings, ImageNumber, SaveSuccess, SaveCanceled, EditError, SaveError, COUNT
        };
    };
};


class DialogMessage : public QDialog 
{
    Q_OBJECT
public:
    typedef uint CustomButtons;
    const struct CustomButton {
        enum : CustomButtons {
            NoCustomButton = 0, SaveContinue = 1, Continue = 2, ResetCancel = 4,
            SaveClose = 8, Close = 16, Delete = 32, OpenLog = 64, SaveLogAs = 128
        };
    };
    DialogMessage(QString title, QString message,
        const QFlags<QDialogButtonBox::StandardButton> buttons = QDialogButtonBox::StandardButton::NoButton,
        const DialogMessage::CustomButtons custom_buttons = CustomButton::NoCustomButton,
        QWidget* parent = nullptr, bool bold_message_text = false);
    ~DialogMessage();
    Ui::Dialog_Message ui;
signals:
    void buttonClicked(const QDialogButtonBox::StandardButton&);
    void buttonRoleClicked(const QDialogButtonBox::ButtonRole&);
public slots:
    void buttonBoxClicked(QAbstractButton* button);
protected:
    void changeEvent(QEvent* event);
    void closeEvent(QCloseEvent* event) override;
private:
    QFont message_font = QFont("Segoe UI", 18);
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
    /// Save a single preset to the settings file.
    /// </summary>
    /// <param name="checked">--The index of a preset in preset_list.</param>
    void SavePresetToSettingsFile(int index);
    /// <summary>
    /// Write recently loaded image file paths to the settings file.
    /// </summary>
    void SaveRecentFiles();
    /// <summary>
    /// Read recently loaded image file paths from the settings file.
    /// </summary>
    void LoadRecentFiles();
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
    /// [Static] Find and replace all matched text in a string.
    /// </summary>
    /// <param name="str">--The text to search.</param>
    /// <param name="from">--Find this text.</param>
    /// <param name="to">--Replace with this text</param>
    /// <param name="no_really_all">--If true will include cases where "to" is a substring of "from".</param>
    /// <returns>An edited string.</returns>
    static const std::string ReplaceAll(std::string str, const std::string& from, const std::string& to, bool no_really_all = false);
    /// <summary>
    /// [Static] Find and replace all matched text in a QString.
    /// </summary>
    /// <param name="str">--The text to search.</param>
    /// <param name="from">--Find this text.</param>
    /// <param name="to">--Replace with this text.</param>
    /// <param name="case_sensitivity">--Qt::CaseSensitive or Qt::CaseInsensitive.</param>
    /// <param name="no_really_all">--If true will include cases where "to" is a substring of "from".</param>
    /// <returns>An edited QString.</returns>
    const QString ReplaceAll(QString str, const QString& from, const QString& to,
        const Qt::CaseSensitivity case_sensitivity = Qt::CaseSensitive, bool no_really_all = false);
    
public slots:
    void Test();
    /// <summary>
    /// Change the currently selected preset.
    /// </summary>
    /// <param name="index">--The index of the preset.</param>
    void ChangePreset(int index);
    /// <summary>
    /// Save current (or all) preset(s) to the preset list and to the user settings file.
    /// </summary>
    /// <param name="save_all">--Save all presets to settings file.</param>
    void SavePreset(bool save_all = false);
    /// <summary>
    /// Dialog asking user to save or discard any changes made to current preset before proceeding.
    /// </summary>
    /// <param name="include_cancel_buttons">--Toggle to include "Cancel" buttons.</param>
    /// <param name="closing">--Use closing app text/buttons. [Default: false]</param>
    /// <returns>True if a "Cancel" (or X) button was clicked.</returns>
    bool SavePresetDialog(bool include_cancel_buttons = false, bool closing = false);
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
    /// Dialog confirming the removal of current selected preset.
    /// </summary>
    void RemoveCurrentPreset();
    /// <summary>
    /// Start editing and saving images in file tree (in another thread).
    /// </summary>
    /// <param name="selected_only">--Only edit and save selected files in file tree.</param>
    void EditAndSave(bool selected_only = false);
    /// <summary>
    /// Cancel and stop all image edit processes currently running. Images that have already finished editing and sent
    /// off to be saved will still be saved and shown in logs as successful while others shown as canceled and not saved.
    /// </summary>
    void CancelAllImageEditing();
    /// <summary>
    /// Show open file dialog allowing user to load one or more image files.
    /// </summary>
    /// <param name="from_directory">--Select an entire directory instead of individual files.</param>
    void LoadImageFiles(bool from_directory = false);
    /// <summary>
    /// Show open file dialog allowing user to add an image file.
    /// </summary>
    /// <param name="default_image_path">--Default image path to both start search from and return if canceled.</param>
    QString GetImageFile(QString default_image_path = "");
    /// <summary>
    /// Add a single image file to file tree.
    /// </summary>
    /// <param name="file">--A single file path.</param>
    void AddNewFile(QString file);
    /// <summary>
    /// Add many image files to file tree. Will first check for any directories and add any files found to the list before
    /// sending it too BuildFileMetadataList(). 
    /// </summary>
    /// <param name="file_list">--List of file and/or directory paths.</param>
    void AddNewFiles(QStringList file_list);
    /// <summary>
    /// Slot event called when file tree check box changed.
    /// </summary>
    /// <param name="index">--Index of selected file in tree.</param>
    /// <param name="toggle">--Check box toggle.</param>
    void FileSelectionChange(uint index, bool toggle);
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
    /// Opens a directory dialog to obtain an existing directory path.
    /// </summary>
    /// <returns>A QString directory path.</returns>
    QString GetSaveDirectoryPath();

private slots:
    /// <summary>
    /// Update the sender (or specific) combo box status and tool tip when signal sent.
    /// </summary>
    void UpdateComboBoxTextTips(QComboBox* combo_box = nullptr);
    /// <summary>
    /// Update the sender (or specific) line edit status and tool tip when signal sent.
    /// </summary>
    void UpdateLineEditTextTips(QLineEdit* line_edit = nullptr);
    /// <summary>
    /// Enable or disable all objects in a group.
    /// </summary>
    /// <param name="option_group">--List of objects.</param>
    /// <param name="enabled">--Toggle</param>
    void EnableOptionGroup(QObjectList option_group, bool enabled);
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
    /// Set the background color of label_ColorPreview to represent the chosen background color option.
    /// </summary>
    void SetColorPreviewStyleSheet();
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
    /// Select (check) specific file in file tree.
    /// </summary>
    /// <param name="index">--Index of file in list.</param>
    void FileSelectionToggle(int index);
    /// <summary>
    /// Select (check) "All" or "None" files in file tree.
    /// </summary>
    /// <param name="toggle">--All/true or None/false.</param>
    void FileSelectionToggleAll(bool toggle);
    /// <summary>
    /// Select (check) files in file tree using a filter based on image file formats.
    /// </summary>
    /// <param name="actions">--List of all submenu actions.</param>
    void FileSelectionFilter(QList<QAction*> actions);
    /// <summary>
    /// After files loaded resize all columns to fit content in file tree and save recent files loaded.
    /// </summary>
    void FileLoadingFinished();
    /// <summary>
    /// After files sorted resize all columns to fit content in file tree.
    /// </summary>
    void FileSortingFinished();
    /// <summary>
    /// Confirmation popup asking user how to handle the deleting of files in file tree.
    /// </summary>
    /// <param name="clear_all">--If true will clear file list entirely, else deletes are selective.</param>
    void DeleteConfirmationPopup(bool clear_all = false);
    /// <summary>
    /// Add/insert special data from a combo box representing specific metadata to a line edit.
    /// </summary>
    void AddTextToFileName();
    /// <summary>
    /// Check if path from ui lineEdit_WatermarkPath exists and if it doesn't, revert to last known existing path.
    /// </summary>
    void CheckWatermarkPath();
    /// <summary>
    /// Makes sure ui lineEdit_RelativePath uses platform specific slashes, converting all in current text. 
    /// </summary>
    void CheckRelativePath();
    /// <summary>
    /// Check if path from ui lineEdit_AbsolutePath exists and if it doesn't, replace text with last existing path or a default path.
    /// </summary>
    void CheckAbsolutePath();

signals:
    void valueChanged(int value);
    void currentIndexChanged(int index);
    void progressMade(float multiplier = 1.0f);

private:
    Ui::BatchItImageClass ui;
    QString preset_settings_file;
    bool non_image_file_dialog_shown = false;

    // Flags for preset options that were changed in the ui.
    struct OptionTrackerFlags {
    private:
        ulong i = 1;
        std::vector<uint> max_numbers;
        uint iPlus(uint next) {
            return i = i + next;
        }
        uint reset() {
            max_numbers.push_back(i + i);
            return i = 1;
        }
        std::string maxTrackerNumbers() {
            max_numbers.push_back(i + i);
            std::string str = "";
            for (auto& t : max_numbers) {
                str += std::to_string(t) + ", ";
            }
            return str;
        }

    public:
        const uint NoChange = 0;

        // Image Edit Options
        const uint comboBox_WidthMod = i;
        const uint spinBox_WidthNumber = iPlus(i);
        const uint comboBox_HeightMod = iPlus(i);
        const uint spinBox_HeightNumber = iPlus(i);
        const uint comboBox_Resample = iPlus(i);
        const uint checkBox_KeepAspectRatio = iPlus(i);

        const uint comboBox_BorderType = iPlus(i);
        const uint pushButton_ColorDialog = iPlus(i);

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
        const uint spinBox_WatermarkOffsetX = iPlus(i);
        const uint spinBox_WatermarkOffsetY = iPlus(i);

        // Image Save Options
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

        std::string printAllTrackerFlags() {
            return
                "\n  NoChange:                      " + std::to_string(NoChange) +
                "\n  comboBox_WidthMod:             " + std::to_string(comboBox_WidthMod) +
                "\n  spinBox_WidthNumber:           " + std::to_string(spinBox_WidthNumber) +
                "\n  comboBox_HeightMod:            " + std::to_string(comboBox_HeightMod) +
                "\n  spinBox_HeightNumber:          " + std::to_string(spinBox_HeightNumber) +
                "\n  comboBox_Resample:             " + std::to_string(comboBox_Resample) +
                "\n  checkBox_KeepAspectRatio:      " + std::to_string(checkBox_KeepAspectRatio) +

                "\n  comboBox_BorderType:           " + std::to_string(comboBox_BorderType) +
                "\n  pushButton_ColorDialog:        " + std::to_string(pushButton_ColorDialog) +

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
                "\n  spinBox_WatermarkOffsetX:      " + std::to_string(spinBox_WatermarkOffsetX) +
                "\n  spinBox_WatermarkOffsetY:      " + std::to_string(spinBox_WatermarkOffsetY) +

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

                "\n  uint:   " + std::to_string(std::numeric_limits<unsigned int>::max()) +
                "\n  ulong:  " + std::to_string(std::numeric_limits<unsigned long>::max()) +
                "\n  size_t: " + std::to_string(std::numeric_limits<size_t>::max()) +
                "\n  Maxes:  " + maxTrackerNumbers() +
                "\n";
        }
    } Option;
    ulong edit_options_change_tracker = 0;
    ulong save_options_change_tracker = 0;

    // UI Data to be added to an object/widget
    struct UIData {
        int data; // A Value, Default, Enum, Index, etc.
        QString name; // A ui object's title, name, or other short length text.
        QString desc; // Descriptive text used in Tooltips, Statusbars, Dialog Messages, etc.
    };

    // Menu Bar
    std::array<QString, UI::MenuBar::File::COUNT>* menu_bar_file = new std::array<QString, UI::MenuBar::File::COUNT>;
    std::array<QString, UI::MenuBar::Edit::COUNT>* menu_bar_edit = new std::array<QString, UI::MenuBar::Edit::COUNT>;
    std::array<QString, UI::MenuBar::Help::COUNT>* menu_bar_help = new std::array<QString, UI::MenuBar::Help::COUNT>;

    // Tree UIData (Arrays placed on the heap will be deleted after use.)
    std::array<UIData, FileTree::Column::COUNT>* file_tree_headers = new std::array<UIData, FileTree::Column::COUNT>;
    std::array<UIData, FileTree::ActionMenu::MainMenu::COUNT>* file_tree_menu_items
        = new std::array<UIData, FileTree::ActionMenu::MainMenu::COUNT>;
    std::array<UIData, FileTree::ActionMenu::SubMenu::FilterImageFormats::COUNT>* file_tree_sub_menu_formats
        = new std::array<UIData, FileTree::ActionMenu::SubMenu::FilterImageFormats::COUNT>;
    std::array<QString, FileTree::Column::COUNT> file_tree_other_text;
    QString select_text;
    QString unselect_text;

    // Tab, Label, Check Box, and Button UIData
    std::array<UIData, UI::FileOption::FilePath::COUNT>* file_path_options = new std::array<UIData, UI::FileOption::FilePath::COUNT>;
    std::array<UIData, UI::EditOption::Resize::COUNT>* resize_options = new std::array<UIData, UI::EditOption::Resize::COUNT>;
    std::array<UIData, UI::EditOption::Background::COUNT> background_options;// = new std::array<UIData, UI::EditOption::Background::COUNT>; // TODO: desc
    std::array<UIData, UI::EditOption::Blur::COUNT>* blur_options = new std::array<UIData, UI::EditOption::Blur::COUNT>; // TODO: desc
    std::array<UIData, UI::EditOption::Rotation::COUNT>* rotation_options = new std::array<UIData, UI::EditOption::Rotation::COUNT>; // TODO: desc
    std::array<UIData, UI::EditOption::Watermark::COUNT>* watermark_options = new std::array<UIData, UI::EditOption::Watermark::COUNT>;
    std::array<UIData, UI::FileOption::FormatJpeg::COUNT> format_jpeg_options;
    std::array<UIData, UI::FileOption::FormatJp2::COUNT> format_jp2_options;
    std::array<UIData, UI::FileOption::FormatPng::COUNT> format_png_options;
    std::array<UIData, UI::FileOption::FormatPng::COUNT> format_webp_options;
    std::array<UIData, UI::FileOption::FormatAvif::COUNT> format_avif_options;
    std::array<UIData, UI::FileOption::FormatPbm::COUNT> format_pbm_options;
    std::array<UIData, UI::FileOption::FormatPam::COUNT> format_pam_options;
    std::array<UIData, UI::FileOption::FormatTiff::COUNT> format_tiff_options;
    std::array<UIData, UI::FileOption::FormatExr::COUNT> format_exr_options;
    std::array<UIData, UI::FileOption::FormatHdr::COUNT> format_hdr_options;
    std::array<UIData, UI::StatusBar::COUNT> status_bar_messages;
    std::array<UIData, UI::Other::COUNT>* other_options = new std::array<UIData, UI::Other::COUNT>;

    // Combo Box UIData
    std::array<UIData, 6>* width_selections = new std::array<UIData, 6>;
    std::array<UIData, 6>* height_selections = new std::array<UIData, 6>;
    std::array<UIData, 3>* resampling_selections = new std::array<UIData, 3>; // TODO: add more Resampling Filters
    std::array<UIData, 7>* border_types = new std::array<UIData, 7>;
    std::array<UIData, 7>* blur_filters = new std::array<UIData, 7>;
    std::array<UIData, 9>* watermark_locations = new std::array<UIData, 9>;
    std::array<UIData, ImageSaver::MetadataFlags::COUNT>* file_name_creation = new std::array<UIData, ImageSaver::MetadataFlags::COUNT>;
    std::array<UIData, ImageSaver::ImageExtension::COUNT>* image_formats = new std::array<UIData, ImageSaver::ImageExtension::COUNT>;
    std::array<UIData, 5> format_jpeg_subsamplings;
    std::array<UIData, 5> format_png_compression;
    std::array<UIData, 6> format_pam_tupletype;
    std::array<UIData, 24> format_tiff_compression;
    std::array<UIData, 3> format_tiff_resolution_unit;
    std::array<UIData, 10> format_exr_compression;
    std::array<UIData, 2> format_hdr_compression;

    // Other UIData
    std::array<UIData, Dialog::Messages::COUNT> dialog_messages;
    std::array<UIData, 6> blur_depth_selections;
    std::array <QString, Dialog::FileSearch::COUNT> file_dialog_titles;
    std::array <std::string, LogFile::Line::COUNT> log_text; // TODO
    std::array <QString, ImageSaver::ImageExtension::COUNT> extension_list;
    //QString supported_image_extensions_dialog_str = ""; // Built from extension_list
    QStringList file_extension_filters; // Built from extension_list
    QColor background_color = QColor(0, 0, 0, 255);

    // Fonts
    const QFont* font_serif = new QFont("Times", 10, QFont::Bold);
    const QFont* font_default = new QFont("Segoe UI", 9);
    const QFont* font_default_light = new QFont("Segoe UI", 9, QFont::Thin);
    const QFont* font_default_bold = new QFont("Segoe UI", 9, QFont::Bold);
    const QFont* font_mono = new QFont("New Courier", 9);
    const QFont* font_mono_bold = new QFont("New Courier", 9, QFont::Bold);

    // Menu Bar Items
    QAction* action_load_all_files = new QAction(this);
    QAction* action_clear_all_files = new QAction(this);
    QAction* action_line_recent_top = new QAction(this);
    QAction* action_line_recent_bottom = new QAction(this);

    // Right Click Menu Items
    QAction* action_add;
    QAction* action_delete;
    QAction* action_clear;
    QAction* action_select;
    QAction* action_select_all;
    QAction* action_select_none;
    QAction* action_submenu_filter;
    QAction* action_view;
    QAction* action_preview;
    QMenu* submenu_format_filter;

    // File Tree Scrolling
    QScroller* scroller_file_tree;
    int mouse_button_down = Qt::MouseButton::NoButton;
    bool is_horizontal_scrolling = false;

    // File Paths
    const std::filesystem::path default_path = std::filesystem::current_path();
    const QString qdefault_path = QString::fromStdString(default_path.string());
    QString last_user_loaded_path = qdefault_path;
    const std::filesystem::path log_directory_path = default_path / "logs"; // TODO: User Setting
    std::string last_existing_load_path;
    std::string last_existing_save_path;
    QString last_existing_wm_path = "";
    std::string last_selected_format;

    // Preset and File Lists
    std::vector<Preset> preset_list;
    std::vector<FileMetadata> current_file_metadata_list;
    std::vector<FileMetadata> deleted_file_metadata_list;
    QStringList recent_file_paths_loaded;
    uint current_selected_preset = 0;
    int current_file_column_sorted = -1;
    int current_file_sort_order = -1;
    int last_load_count = 0;
    qsizetype recent_file_paths_loaded_max = 10; // TODO: User Setting

    // Logging
    std::vector<std::string> log_lines;
    std::chrono::time_point<std::chrono::system_clock> session_start_time;
    std::chrono::time_point<std::chrono::system_clock> image_edit_start_time;
    uint log_batch_number = 0;
    uint log_batch_summary = 0;
    uint log_end_line = 0;
    uint successful_image_edits = 0;
    uint successful_image_saves = 0;
    uint image_save_errors = 0;
    qsizetype max_log_files = 20; // TODO: User Setting

    std::vector<ImageEditor*> ie_pointer_list;

    const std::function<void()> function_FileLoadingFinished = std::bind(&BatchItImage::FileLoadingFinished, this);
    const std::function<void()> function_FileSortingFinished = std::bind(&BatchItImage::FileSortingFinished, this);
    const std::function<void()> function_PrintBatchImageLog = std::bind(&BatchItImage::PrintBatchImageLog, this);

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
    /// <typeparam name="ui_data_size"></typeparam>
    /// <param name="ui_data">--Reference to an array of QStrings.</param>
    /// <param name="objects">--A list of pointers to objects/widgets.</param>
    template<std::size_t ui_data_size>void AddUiDataTo(const std::array<QString, ui_data_size>& ui_data, const std::vector<QObject*>& objects);
    /// <summary>
    /// Add display text, tooltip descriptions, and other data to various types of ui objects/widgets.
    /// </summary>
    /// <param name="object">--Pointer to an object/widget.</param>
    /// <param name="ui_data">--Reference to a QString.</param>
    void AddUiDataTo(QObject* object, const QString& ui_data);
    /// <summary>
    /// Add display text, tooltip descriptions, and other data to various types of ui objects/widgets.
    /// </summary>
    /// <typeparam name="ui_data_size"></typeparam>
    /// <param name="ui_data">--Reference to an array of UIData.</param>
    /// <param name="objects">--A list of pointers to objects/widgets.</param>
    template<std::size_t ui_data_size>void AddUiDataTo(const std::array<UIData, ui_data_size>& ui_data, const std::vector<QWidget*>& objects);
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
    /// <param name="ui_data">--UIData array to enter into a combo box.</param>
    /// <param name="string_data">--Array of QStrings to replace integer data from ui_data with.</param>
    template<std::size_t ui_data_size, std::size_t string_data_size = 0>void PopulateComboBox(
        QComboBox* cb, const std::array<UIData, ui_data_size>& ui_data, const uint default_index = 0,
        const std::array <QString, string_data_size>& string_data = {});
    /// <summary>
    /// Add items/data to various combo boxes which include titles, tooltip descriptions, and other data.
    /// </summary>
    /// <param name="cb">--Pointer to a QComboBox.</param>
    /// <param name="ui_data">--UIData array to enter into a combo box.</param>
    /// <param name="string_data">--Array of std::strings to replace integer data from ui_data with.</param>
    template<std::size_t ui_data_size, std::size_t string_data_size>void PopulateComboBox(
        QComboBox* cb, const std::array<UIData, ui_data_size>& ui_data, const uint default_index,
        const std::array <std::string, string_data_size>& string_data);
    /// <summary>
    /// Connect all initial Ui object events (signals and slots).
    /// </summary>
    void UiConnections();
    /// <summary>
    /// Update an option tracker if an option changed.
    /// </summary>
    /// <param name="tracker">--A tracker holding all the options changed.</param>
    /// <param name="tracked_option">--The tracking code of changed option.</param>
    /// <param name="preset_value">--The saved preset option value.</param>
    /// <param name="changed_option_value">--The unsaved changed option value.</param>
    /// <returns>The tracker</returns>
    ulong TrackOptionChanges(ulong tracker, uint tracked_option, int preset_value, int changed_option_value);
    /// <summary>
    /// Update an option tracker if an option changed.
    /// </summary>
    /// <param name="tracker">--A tracker holding all the options changed.</param>
    /// <param name="tracked_option">--The tracking code of changed option.</param>
    /// <param name="preset_string">--The saved preset option string.</param>
    /// <param name="changed_option_string">--The unsaved changed option string.</param>
    /// <returns>The tracker</returns>
    ulong TrackOptionChanges(ulong tracker, uint tracked_option, std::string preset_string, std::string changed_option_string);
    /// <summary>
    /// Remove a group of options from an option tracker.
    /// </summary>
    /// <param name="tracker">--A tracker holding all the options changed.</param>
    /// <param name="tracked_options">--Vector of tracked option codes.</param>
    /// <returns>The tracker</returns>
    ulong RemoveOptionsChanged(ulong tracker, std::vector<uint> tracked_options);
    /// <summary>
    /// Remove all options from all option trackers.
    /// </summary>
    void RemoveOptionsChanged();
    /// <summary>
    /// Build a "right click" context menu for the file tree.
    /// </summary>
    void SetupFileTreeContextMenu();
    /// <summary>
    /// Build and add recently loading image files to the top menu recent files submenu.
    /// </summary>
    void BuildRecentFilesMenu();
    /// <summary>
    /// Create a header for the log file and add current settings used to log.
    /// </summary>
    void StartBatchImageLog();
    /// <summary>
    /// Write log to file.
    /// </summary>
    void PrintBatchImageLog();
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
    /// <param name="matching_extension_list">--All files returned must match extensions in this list.</param>
    /// <returns>QStringList of file paths.</returns>
    template<typename DirectoryIter, std::size_t array_size>QStringList IterateDirectory(DirectoryIter iterator, std::array<QString, array_size>& matching_extension_list);
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
    /// Create any missing directories in a path.
    /// </summary>
    /// <param name="file_path">--A full file system path.</param>
    /// <returns>"True" if an error occurs.</returns>
    bool CreateDirectories(std::filesystem::path file_path);
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
    /// Check for any unsaved preset options and ask to save, or not, before closing.
    /// </summary>
    /// <param name="event"></param>
    void closeEvent(QCloseEvent* event) override;
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

    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
};

#endif // BATCHITIMAGE_H