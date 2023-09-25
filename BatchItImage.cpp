#ifndef    BATCHITIMAGE_H
#define    BATCHITIMAGE_H
#include "BatchItImage.h"
#endif


/*
TODO: 
    Log Actions
    Holding right click scrolls left to right
    Track changes to undo file list deletes or image edit ui changes
    Image File Filter... show/edit only PNG files, files > 1 MB, etc.

*/


MessageWindow::MessageWindow(QString title, QString message, QFlags<QDialogButtonBox::StandardButton> buttons, QWidget* parent)
    : QDialog(parent)//, m_ui(new Ui::Dialog_MessageWindow)
{
    //m_ui->setupUi(this);
    ui.setupUi(this);
    //m_ui = &ui;
    setWindowTitle(title);
    ui.label_Message->setText(message);
    ui.buttonBox->setStandardButtons(buttons);
    connect(ui.buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(ButtonBoxClicked(QAbstractButton*)));
}
MessageWindow::~MessageWindow()
{
    DEBUG("MessageWindow Deconstructed");
    //delete m_ui;
}
void MessageWindow::changeEvent(QEvent* event)
{
    //DEBUG("MessageWindow changeEvent");
    QDialog::changeEvent(event);
}
void MessageWindow::closeEvent(QCloseEvent* event)
{
    if (event->spontaneous()) {
        DEBUG("The X-close button was clicked.");
        emit ButtonClicked(QDialogButtonBox::Close);
    }
    QWidget::closeEvent(event);
}
void MessageWindow::ButtonBoxClicked(QAbstractButton* button)
{
    QDialogButtonBox::StandardButton std_button = ui.buttonBox->standardButton(button);
    DEBUG2("MessageWindow::ButtonBoxClicked: ", std_button);
    emit ButtonClicked(std_button);
    /*
    QDialogButtonBox::Ok	    0x00000400	An "OK" button defined with the AcceptRole.
    QDialogButtonBox::Open	    0x00002000	An "Open" button defined with the AcceptRole.
    QDialogButtonBox::Save  	0x00000800	A "Save" button defined with the AcceptRole.
    QDialogButtonBox::Cancel	0x00400000	A "Cancel" button defined with the RejectRole.
    QDialogButtonBox::Close	    0x00200000	A "Close" button defined with the RejectRole.
    QDialogButtonBox::Discard	0x00800000	A "Discard" or "Don't Save" button, depending on the platform, defined with the DestructiveRole.
    QDialogButtonBox::Apply	    0x02000000	An "Apply" button defined with the ApplyRole.
    QDialogButtonBox::Reset 	0x04000000	A "Reset" button defined with the ResetRole.
    QDialogButtonBox::RestoreDefaults	0x08000000	A "Restore Defaults" button defined with the ResetRole.
    QDialogButtonBox::Help	    0x01000000	A "Help" button defined with the HelpRole.
    QDialogButtonBox::SaveAll	0x00001000	A "Save All" button defined with the AcceptRole.
    QDialogButtonBox::Yes	    0x00004000	A "Yes" button defined with the YesRole.
    QDialogButtonBox::YesToAll	0x00008000	A "Yes to All" button defined with the YesRole.
    QDialogButtonBox::No	    0x00010000	A "No" button defined with the NoRole.
    QDialogButtonBox::NoToAll	0x00020000	A "No to All" button defined with the NoRole.
    QDialogButtonBox::Abort	    0x00040000	An "Abort" button defined with the RejectRole.
    QDialogButtonBox::Retry	    0x00080000	A "Retry" button defined with the AcceptRole.
    QDialogButtonBox::Ignore	0x00100000	An "Ignore" button defined with the AcceptRole.
    QDialogButtonBox::NoButton	0x00000000	An invalid button.
    */
}


DialogEditPresetDesc::DialogEditPresetDesc(QString title, QString message, std::vector<Preset>* preset_list, uint current_selected_preset, QWidget* parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    setWindowTitle(title);
    ui.label_Message->setText(message);
    DialogEditPresetDesc::preset_list = preset_list;
    DialogEditPresetDesc:: current_selected_preset = current_selected_preset;
    UpdateComboBox();
    PresetIndexChanged(current_selected_preset);
    connect(ui.buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(ButtonBoxClicked(QAbstractButton*)));
    connect(ui.comboBox_Preset_4, SIGNAL(currentIndexChanged(int)), this, SLOT(PresetIndexChanged(int)));
}
DialogEditPresetDesc::~DialogEditPresetDesc()
{
    DEBUG("DialogEditPresetDesc Deconstructed");
}
void DialogEditPresetDesc::PresetIndexChanged(int index)
{
    DEBUG2("PresetIndexChanged:", index);
    ui.lineEdit_PresetDesc->setText(preset_list->at(index).description);
    current_selected_preset = index;
}
void DialogEditPresetDesc::UpdateComboBox()
{
    DEBUG("UpdateComboBox");
    BatchItImage::AddPresetsToComboBox(preset_list, &ui.comboBox_Preset_4);
}
void DialogEditPresetDesc::closeEvent(QCloseEvent* event)
{
    if (event->spontaneous()) {
        DEBUG("The X-close button was clicked.");
        emit ButtonClicked(QDialogButtonBox::Close);
    }
    QWidget::closeEvent(event);
}
void DialogEditPresetDesc::ButtonBoxClicked(QAbstractButton* button)
{
    QDialogButtonBox::StandardButton std_button = ui.buttonBox->standardButton(button);
    DEBUG2("DialogEditPresetDesc::ButtonBoxClicked: ", std_button);

    if (QDialogButtonBox::Apply & std_button) {
        QString updated_description = ui.lineEdit_PresetDesc->text();
        preset_list->at(current_selected_preset).description = updated_description;
        DEBUG(updated_description.toStdString());
        UpdateComboBox();
    }
    else if (QDialogButtonBox::Reset & std_button) {
        PresetIndexChanged(current_selected_preset);
    }
    emit ButtonClicked(std_button);
}


/// <summary>
/// A QObject that will take a file path and build a struct with metadata.
/// Intended to be ran in another thread as a "Worker".
/// Slot -> GetFileMetadata() -> Signal -> FileMetadataReady(FileMetadata*)
/// </summary>
/// <param name="file_path">--A file path string.</param>
/// <param name="load_order">--This file metadata should be added now on initialization.</param>
/// <param name="parent">--The parent object that creates this</param>
FileMetadataWorker::FileMetadataWorker(std::string file_path, int load_order, QObject* parent)
{
    FileMetadataWorker::file_path = file_path;
    FileMetadataWorker::load_order = load_order;
    //current_file_metadata_list = new std::vector<struct FileMetadata>;
    //current_file_metadata_list->reserve(1);
}
void FileMetadataWorker::GetFileMetadata()
{
    DEBUG2("GetFileMetadata From: ", file_path);

    // Image File Dimensions
    cv::Mat file_image = cv::imread(file_path); // TODO: Find (is there?) another way to get width/height without reading/opening file. This is slow when adding 100+ files.

    if (not file_image.empty()) {

        // Get File Metadata:
        struct stat t_stat;
        stat(file_path.data(), &t_stat);

        //current_file_metadata_list->push_back(
        //    {
        //        load_order,         // Load Order Index
        //        file_path,          // File Path String
        //        t_stat.st_size,     // File Size in Bytes
        //        file_image.cols,    // Width
        //        file_image.rows,    // Height
        //        t_stat.st_ctime,    // Unix Time Created
        //        t_stat.st_mtime,    // Unix Time Modified
        //        false               // Selected
        //    });
        //emit FileMetadataReady(*current_file_metadata_list);

        file_metadata->load_order = load_order;
        file_metadata->path = file_path;
        file_metadata->size = t_stat.st_size;
        file_metadata->width = file_image.cols;
        file_metadata->height = file_image.rows;
        file_metadata->date_created = t_stat.st_ctime;
        file_metadata->date_modified = t_stat.st_mtime;
        file_metadata->selected = false;
    }
    else {
        file_metadata->path = file_path; // File that could not be read. Bad image / not an image / etc.
    }
    emit FileMetadataReady(file_metadata);
}
FileMetadataWorker::~FileMetadataWorker()
{
    DEBUG("FileMetadataWorker Deconstructed");
}


BatchItImage::BatchItImage(QWidget* parent) : QMainWindow(parent)
{
    DEBUG("Debug Build");

    ui.setupUi(this);
    setAcceptDrops(true);

    preset_settings_file = QApplication::applicationDirPath() + "/settings.ini";
    DEBUG(preset_settings_file.toStdString());

    // Add QT About Menu Item
    QAction* aboutQtAct = new QAction(this);
    aboutQtAct->setText("About QT");
    ui.menuHelp->addAction(aboutQtAct);

    /***************************
        UI Text/Data
    ****************************/

    LoadInUiData();

    ui.tabWidget->setCurrentIndex(std::get<int>(other_options[OtherOptions::tab_1].data));
    ui.tabWidget->setTabText(OtherOptions::tab_1, QString::fromStdString(other_options[OtherOptions::tab_1].name));
    ui.tab_1->setToolTip(QString::fromStdString(other_options[OtherOptions::tab_1].desc));
    ui.tabWidget->setTabText(OtherOptions::tab_2, QString::fromStdString(other_options[OtherOptions::tab_2].name));
    ui.tab_2->setToolTip(QString::fromStdString(other_options[OtherOptions::tab_2].desc));
    ui.tabWidget->setTabText(OtherOptions::tab_3, QString::fromStdString(other_options[OtherOptions::tab_3].name));
    ui.tab_3->setToolTip(QString::fromStdString(other_options[OtherOptions::tab_3].desc));

    AddUiObjectData(ui.groupBox_FileRename, &file_path_options[FilePathOptions::groupBox_FileRename]);
    AddUiObjectData(ui.radioButton_Overwrite, &file_path_options[FilePathOptions::radioButton_Overwrite]);
    AddUiObjectData(ui.radioButton_RenameOriginal, &file_path_options[FilePathOptions::radioButton_RenameOriginal]);
    AddUiObjectData(ui.radioButton_NewFileName, &file_path_options[FilePathOptions::radioButton_NewFileName]);
    AddUiObjectData(ui.label_Add, &file_path_options[FilePathOptions::label_Add]);
    AddUiObjectData(ui.groupBox_SaveDir, &file_path_options[FilePathOptions::groupBox_SaveDir]);
    AddUiObjectData(ui.radioButton_RelativePath, &file_path_options[FilePathOptions::radioButton_RelativePath]);
    AddUiObjectData(ui.radioButton_AbsolutePath, &file_path_options[FilePathOptions::radioButton_AbsolutePath]);
    AddUiObjectData(ui.pushButton_AddBackOneDir, &file_path_options[FilePathOptions::pushButton_AddBackOneDir]);
    AddUiObjectData(ui.pushButton_FindAbsolutePath, &file_path_options[FilePathOptions::pushButton_FindAbsolutePath]);
    AddUiObjectData(ui.checkBox_SearchSubDirs, &other_options[OtherOptions::checkBox_SearchSubDirs]);
    AddUiObjectData(ui.pushButton_EditAndSave, &other_options[OtherOptions::pushButton_EditAndSave]);

    PopulateComboBox(ui.comboBox_WidthMod, width_selections, sizeof(width_selections) / sizeof(UIData));
    PopulateComboBox(ui.comboBox_HeightMod, height_selections, sizeof(height_selections) / sizeof(UIData));
    PopulateComboBox(ui.comboBox_Resample, resampling_selections, sizeof(resampling_selections) / sizeof(UIData));
    PopulateComboBox(ui.comboBox_AddText, file_name_creation, sizeof(file_name_creation) / sizeof(UIData));
    PopulateComboBox(ui.comboBox_ImageFormat, image_formats, sizeof(image_formats) / sizeof(UIData));
    //PopulateComboBox(ui.comboBox_FormatFlags, format_jpeg_subsamplings, sizeof(format_jpeg_subsamplings) / sizeof(UIData));

    /***************************
        Prep UI Widgets, Etc
    ****************************/

    ui.progressBar->setVisible(false);
    preset_list.reserve(10);
    current_file_metadata_list.reserve(30);
    deleted_file_metadata_list.reserve(10);

    ui.dial_Rotation->setInvertedAppearance(true);
    ui.dial_Rotation->setInvertedControls(true);

    // TODO: get settings for search sub dirs, recent images loaded (last 10?), others?
    //ui.checkBox_SearchSubDirs->setChecked(search_subdirs);

    SetupFileTree();
    LoadPresets();

    // All characters allowed in file names or paths, plus <>. 
    // <> will later be replaced if used correctly and taken out if not.
    QRegularExpression file_name_re("(\\w|\\d|\\s|`|~|!|@|#|\\$|%|\\^|&|\\(|\\)|-|_|=|\\+|\\[|\\{|\\]|\\}|;|'|,|<|\\.|>)*"); 
    QRegularExpression file_path_re("(\\w|\\d|\\s|`|~|!|@|#|\\$|%|\\^|&|\\(|\\)|-|_|=|\\+|\\[|\\{|\\]|\\}|;|'|,|\\.|\\\\|/|:)*");
    QValidator* file_name_validator = new QRegularExpressionValidator(file_name_re, this);
    QValidator* file_path_validator = new QRegularExpressionValidator(file_path_re, this);
    ui.lineEdit_FileName->setValidator(file_name_validator);
    ui.lineEdit_RelativePath->setValidator(file_path_validator);
    ui.lineEdit_AbsolutePath->setValidator(file_path_validator);

    // Create image extension string for "file open dialog"
    supported_image_extensions_dialog_str.append("Images: (");
    for (const auto& ext : image_formats) {
        supported_image_extensions_dialog_str.append("*" + std::get<std::string>(ext.data) + " ");
    }
    supported_image_extensions_dialog_str.insert(supported_image_extensions_dialog_str.size() - 1, ")");
    supported_image_extensions_dialog_str.append("\nAll Files (*)");
    DEBUG(supported_image_extensions_dialog_str.toStdString());

    /***************************
        UI Events
    ****************************/

    // Menu Actions
    connect(ui.action_About, SIGNAL(triggered(bool)), this, SLOT(Test())); // TODO
    //connect(ui.action_About, &QAbstractButton::pressed, this, &QApplication::aboutQt);
    connect(aboutQtAct, &QAction::triggered, [this] { QApplication::aboutQt(); });
    connect(ui.action_AddImages, SIGNAL(triggered(bool)), this, SLOT(LoadImageFiles()));
    connect(ui.action_AddImageToCombine, SIGNAL(triggered(bool)), this, SLOT(Test())); // TODO
    connect(ui.action_AddNewPreset, SIGNAL(triggered(bool)), this, SLOT(Test())); // TODO
    connect(ui.action_ChangePresetDesc, SIGNAL(triggered(bool)), this, SLOT(ChangePresetDescription()));
    connect(ui.action_Close, SIGNAL(triggered(bool)), this, SLOT(Test())); // TODO
    connect(ui.action_RemovePreset, SIGNAL(triggered(bool)), this, SLOT(Test())); // TODO
    connect(ui.action_SaveLogAs, SIGNAL(triggered(bool)), this, SLOT(Test())); // TODO
    connect(ui.action_SavePresets, SIGNAL(triggered(bool)), this, SLOT(Test())); // TODO

    // Image File Tree
    connect(ui.treeWidget_FileInfo->header(), SIGNAL(sectionClicked(int)), this, SLOT(SortFileTreeByColumn(int)));
    //connect(ui.checkBox_SearchSubDirs, &QCheckBox::stateChanged, [this] { search_subdirs = ui.checkBox_SearchSubDirs->isChecked(); });
    SetupFileTreeContextMenu();

    // Image Edit Widgets
    connect(ui.comboBox_Preset_1, SIGNAL(currentIndexChanged(int)), this, SLOT(ChangePreset(int)));
    //connect(ui.comboBox_Preset_1, SIGNAL(currentIndexChanged(int)), ui.comboBox_Preset_2, SLOT(ChangePresets(int))); // Done in the ui xml + 9 Others
    connect(ui.comboBox_WidthMod, SIGNAL(currentIndexChanged(int)), this, SLOT(UpdateComboBoxToolTip()));
    connect(ui.comboBox_HeightMod, SIGNAL(currentIndexChanged(int)), this, SLOT(UpdateComboBoxToolTip()));
    connect(ui.comboBox_Resample, SIGNAL(currentIndexChanged(int)), this, SLOT(UpdateComboBoxToolTip()));
    connect(ui.dial_Rotation, SIGNAL(valueChanged(int)), this, SLOT(Test())); // TODO

    // Image Save Widgets
    connect(ui.pushButton_EditAndSave, SIGNAL(clicked(bool)), this, SLOT(EditAndSave()));
    connect(ui.comboBox_AddText, SIGNAL(activated(int)), this, SLOT(AddTextToFileName()));
    connect(ui.comboBox_AddText, SIGNAL(currentIndexChanged(int)), this, SLOT(UpdateComboBoxToolTip()));
    connect(ui.lineEdit_RelativePath, SIGNAL(editingFinished()), this, SLOT(CheckRelativePath()));
    connect(ui.pushButton_AddBackOneDir, &QAbstractButton::pressed,
        [this] {
            ui.lineEdit_RelativePath->setText(ui.lineEdit_RelativePath->text().prepend("../"));
            CheckRelativePath();
        });
    connect(ui.lineEdit_AbsolutePath, SIGNAL(editingFinished()), this, SLOT(CheckAbsolutePath()));
    connect(ui.pushButton_FindAbsolutePath, &QAbstractButton::pressed,
        [this] { ui.lineEdit_AbsolutePath->setText(GetSaveDirectoryPath()); });
    connect(ui.groupBox_ChangeFormat, SIGNAL(toggled(bool)), this, SLOT(EnableSpecificFormatOptions()));
    connect(ui.comboBox_ImageFormat, &QComboBox::currentIndexChanged,
        [this] {
            ui.comboBox_ImageFormat->setToolTip(ui.comboBox_ImageFormat->currentData(Qt::ToolTipRole).toString());
            EnableSpecificFormatOptions();
        });
    connect(ui.comboBox_FormatFlags, &QComboBox::currentIndexChanged,
        [this] {
            ui.comboBox_FormatFlags->setToolTip(ui.comboBox_FormatFlags->currentData(Qt::ToolTipRole).toString());
            if (ui.comboBox_ImageFormat->currentData() == ".exr") {
                if (ui.comboBox_FormatFlags->currentData() == cv::IMWRITE_EXR_COMPRESSION_DWAA or
                    ui.comboBox_FormatFlags->currentData() == cv::IMWRITE_EXR_COMPRESSION_DWAB) {
                    DEBUG("IMWRITE_EXR_COMPRESSION_DWA");
                    ui.label_Compression->setFont(*font_default);
                    ui.spinBox_Compression->setEnabled(true);
                    ui.spinBox_ExtraSetting1->setSingleStep(1);
                }
                else {
                    ui.label_Compression->setFont(*font_default_light);
                    ui.spinBox_Compression->setEnabled(false);
                }
            }
        });

    /***************************
        Testing
    ****************************/

    // TODO: Combine Tree
    ui.treeWidget_Combine->clear();

    QTreeWidgetItem* new_item = new QTreeWidgetItem(ui.treeWidget_Combine);
    QComboBox* comboBox = new QComboBox(this);
    comboBox->addItems(QStringList() << "item1" << "item2");
    ui.treeWidget_Combine->setItemWidget(new_item, 1, comboBox);

    ui.treeWidget_Combine->addTopLevelItem(new_item);

    // TODO: Status Bar
    //QString status_message = "BatchItImage";
    //ui.statusbar->showMessage(status_message, -1);

}

BatchItImage::~BatchItImage() {}


// Just for quick testing
void BatchItImage::Test()
{
    DEBUG("--TEST--");
    QObject* obj = sender();
    DEBUG(obj->objectName().toStdString());
}


/// <summary>
/// Load in all text and other ui data.
/// </summary>
void BatchItImage::LoadInUiData()
{
    // treeWidget_FileInfo
    file_tree_headers[FileColumn::FILE_SELECTED].data = 1; // TODO: 1 = Initial Sort/Bold Text (after files loaded)?
    file_tree_headers[FileColumn::FILE_SELECTED].name = "";
    file_tree_headers[FileColumn::FILE_SELECTED].desc = "The load order of image files.";
    file_tree_headers[FileColumn::FILE_NAME].data = 0;
    file_tree_headers[FileColumn::FILE_NAME].name = "File Name";
    file_tree_headers[FileColumn::FILE_NAME].desc = "The name of an image file, click an arrow to show full path.";
    file_tree_headers[FileColumn::IMAGE_DIMENSIONS].data = 0;
    file_tree_headers[FileColumn::IMAGE_DIMENSIONS].name = "Dimensions";
    file_tree_headers[FileColumn::IMAGE_DIMENSIONS].desc = "Image Dimensions/Size (Width x Height).";
    file_tree_headers[FileColumn::FILE_SIZE].data = 0;
    file_tree_headers[FileColumn::FILE_SIZE].name = "File Size";
    file_tree_headers[FileColumn::FILE_SIZE].desc = "The file size in bytes.";
    file_tree_headers[FileColumn::DATE_CREATED].data = 0;
    file_tree_headers[FileColumn::DATE_CREATED].name = "Date Created";
    file_tree_headers[FileColumn::DATE_CREATED].desc = "The date a file was created.";
    file_tree_headers[FileColumn::DATE_MODIFIED].data = 0;
    file_tree_headers[FileColumn::DATE_MODIFIED].name = "Date Modified";
    file_tree_headers[FileColumn::DATE_MODIFIED].desc = "The date a file was last modified.";

    file_tree_menu_items[ActionMenu::action_add].data = 0; // TODO: 1 = Bold, default double click action?
    file_tree_menu_items[ActionMenu::action_add].name = "Add Images";
    file_tree_menu_items[ActionMenu::action_add].desc = "Add more images to this file viewer.";
    file_tree_menu_items[ActionMenu::action_delete].data = 0;
    file_tree_menu_items[ActionMenu::action_delete].name = "Delete Images";
    file_tree_menu_items[ActionMenu::action_delete].desc = "Delete images from this file viewer (does not delete from system).";
    file_tree_menu_items[ActionMenu::action_clear].data = 0;
    file_tree_menu_items[ActionMenu::action_clear].name = "Clear List";
    file_tree_menu_items[ActionMenu::action_clear].desc = "Clear entire list of files from file viewer.";
    file_tree_menu_items[ActionMenu::action_select].data = 0;
    file_tree_menu_items[ActionMenu::action_select].name = "Select Image";
    file_tree_menu_items[ActionMenu::action_select].desc = "Select or check image file currently highlighted.";
    file_tree_menu_items[ActionMenu::action_view].data = 0;
    file_tree_menu_items[ActionMenu::action_view].name = "View Image";
    file_tree_menu_items[ActionMenu::action_view].desc = "View image file currently highlighted.";
    file_tree_menu_items[ActionMenu::action_preview].data = 0;
    file_tree_menu_items[ActionMenu::action_preview].name = "Preview Modified Image";
    file_tree_menu_items[ActionMenu::action_preview].desc = "Preview a modified version of the image file currently highlighted using the current selected preset.\n" \
        "This modified image will only be a preview the edits will not be saved to file.";

    file_tree_other_text[FileColumn::FILE_LOAD_ORDER] = "Load Order: ";
    file_tree_other_text[FileColumn::FILE_NAME] = "File Path: ";
    file_tree_other_text[FileColumn::IMAGE_DIMENSIONS] = "Image [Width x Height]: ";
    file_tree_other_text[FileColumn::FILE_SIZE] = "File Size: ";
    file_tree_other_text[FileColumn::DATE_CREATED] = "Date File Created: ";
    file_tree_other_text[FileColumn::DATE_MODIFIED] = "Date File Modified: ";

    // comboBox_WidthMod Item Selections
    width_selections[0].data = ImageEditor::NO_CHANGE;
    width_selections[0].name = "No Change";
    width_selections[0].desc = "Image widths may still be modified if 'keep aspect ratio' is checked.";
    width_selections[1].data = ImageEditor::CHANGE_TO;
    width_selections[1].name = "Change Width To:";
    width_selections[1].desc = "All image widths will be modified to a specific number.";
    width_selections[2].data = ImageEditor::MODIFY_BY;
    width_selections[2].name = "Modify Width By:";
    width_selections[2].desc = "This adds to or subtracts from an image's current width. Ex. 1080 + '220' = 1300";
    width_selections[3].data = ImageEditor::MODIFY_BY_PCT;
    width_selections[3].name = "Modify Width By (%):";
    width_selections[3].desc = "This modifies an image's current width by percentage. Ex. 720 x '200%' = 1440";
    width_selections[4].data = ImageEditor::DOWNSCALE;
    width_selections[4].name = "Downscale Width To:";
    width_selections[4].desc = "All images above entered width will be modified to that specific number.\n" \
        "All images already at or below that number will not be modified.";
    width_selections[5].data = ImageEditor::UPSCALE;
    width_selections[5].name = "Upscale Width To:";
    width_selections[5].desc = "All images below entered width will be modified to that specific number.\n" \
        "All images already at or above that number will not be modified.";

    // comboBox_HeightMod Item Selections
    height_selections[0].data = ImageEditor::NO_CHANGE;
    height_selections[0].name = "No Change";
    height_selections[0].desc = "Image heights may still be modified if 'keep aspect ratio' is checked.";
    height_selections[1].data = ImageEditor::CHANGE_TO;
    height_selections[1].name = "Change Height To:";
    height_selections[1].desc = "All images heights will be modified to a specific number.";
    height_selections[2].data = ImageEditor::MODIFY_BY;
    height_selections[2].name = "Modify Height By:";
    height_selections[2].desc = "This adds to or subtracts from an image's current height. Ex. 1080 + '220' = 1300";
    height_selections[3].data = ImageEditor::MODIFY_BY_PCT;
    height_selections[3].name = "Modify Height By (%):";
    height_selections[3].desc = "This modifies an image's current height by percentage. Ex. 720 x '200%' = 1440";
    height_selections[4].data = ImageEditor::DOWNSCALE;
    height_selections[4].name = "Downscale Height To:";
    height_selections[4].desc = "All images above entered height will be modified to that specific number.\n" \
        "All images already at or below that number will not be modified.";
    height_selections[5].data = ImageEditor::UPSCALE;
    height_selections[5].name = "Upscale Height To:";
    height_selections[5].desc = "All images below entered height will be modified to that specific number.\n" \
        "All images already at or above that number will not be modified.";


    // TODO: create UIData for this as well as other edit image widgets
    ui.checkBox_KeepAspectRatio->setText("Keep Aspect Ratio");
    ui.checkBox_KeepAspectRatio->setToolTip("In order to keep aspect ratio, either width or height must be set to \"No Change\" or \"0\"");


    file_path_options[FilePathOptions::groupBox_FileRename].data = 1;
    file_path_options[FilePathOptions::groupBox_FileRename].name = "Modify Image File Names:";
    file_path_options[FilePathOptions::groupBox_FileRename].desc = "";
    file_path_options[FilePathOptions::radioButton_Overwrite].data = 0;
    file_path_options[FilePathOptions::radioButton_Overwrite].name = "Overwrite Original File";
    file_path_options[FilePathOptions::radioButton_Overwrite].desc = "If selected, this will overwrite the original image file with the new edited image file.";
    file_path_options[FilePathOptions::radioButton_RenameOriginal].data = 0;
    file_path_options[FilePathOptions::radioButton_RenameOriginal].name = "Rename Original File";
    file_path_options[FilePathOptions::radioButton_RenameOriginal].desc = "If selected, this will rename the original image file, and use it's name for the new edited image file.";
    file_path_options[FilePathOptions::radioButton_NewFileName].data = 1;
    file_path_options[FilePathOptions::radioButton_NewFileName].name = "Create New File Name";
    file_path_options[FilePathOptions::radioButton_NewFileName].desc = "If selected, a new file name will be used for the new edited image file.";
    file_path_options[FilePathOptions::label_Add].data = 0;
    file_path_options[FilePathOptions::label_Add].name = "Add:";
    file_path_options[FilePathOptions::label_Add].desc = "Add metadata to file name edit text box (to be included in a new file name).";
    file_path_options[FilePathOptions::groupBox_SaveDir].data = 1;
    file_path_options[FilePathOptions::groupBox_SaveDir].name = "Save All Image Files In:";
    file_path_options[FilePathOptions::groupBox_SaveDir].desc = "";
    file_path_options[FilePathOptions::radioButton_RelativePath].data = 1;
    file_path_options[FilePathOptions::radioButton_RelativePath].name = "Relative Paths:";
    file_path_options[FilePathOptions::radioButton_RelativePath].desc = "Relative to the currently edited image file path.";
    file_path_options[FilePathOptions::radioButton_AbsolutePath].data = 0;
    file_path_options[FilePathOptions::radioButton_AbsolutePath].name = "Absolute Path:";
    file_path_options[FilePathOptions::radioButton_AbsolutePath].desc = "The absolute path all edited images will be saved to.";
    file_path_options[FilePathOptions::pushButton_AddBackOneDir].data = 0;
    file_path_options[FilePathOptions::pushButton_AddBackOneDir].name = "Add \"Back One Directory\" For Relative Paths";
    file_path_options[FilePathOptions::pushButton_AddBackOneDir].desc = "Start relative path up one directory level.";
    file_path_options[FilePathOptions::pushButton_FindAbsolutePath].data = 0;
    file_path_options[FilePathOptions::pushButton_FindAbsolutePath].name = "Search For An Absolute Path";
    file_path_options[FilePathOptions::pushButton_FindAbsolutePath].desc = "Open dialog window to select a directory adding it to the absolute path text box.";

    // comboBox_Resample Item Selections (Resampling Filters) 
    resampling_selections[0].data = cv::InterpolationFlags::INTER_NEAREST;
    resampling_selections[0].name = "Resampling: Nearest";
    resampling_selections[0].desc = "Nearest Neighbor Interpolation";
    resampling_selections[1].data = cv::InterpolationFlags::INTER_LINEAR;
    resampling_selections[1].name = "Resampling: Bilinear";
    resampling_selections[1].desc = "Bilinear Interpolation";
    resampling_selections[2].data = cv::InterpolationFlags::INTER_CUBIC;
    resampling_selections[2].name = "Resampling: Bicubic";
    resampling_selections[2].desc = "Bicubic Interpolation";
    // TODO: add more Resampling Filters

    // comboBox_AddText
    file_name_creation[0].data = ADD_FILE_NAME;
    file_name_creation[0].name = "Original File Name";
    file_name_creation[0].desc = "Add the original file name into the creation of a new file name.";
    file_name_creation[1].data = ADD_COUNTER;
    file_name_creation[1].name = "Incrementing Counter";
    file_name_creation[1].desc = "Add an incrementing number into the creation of a new file name.";
    file_name_creation[2].data = ADD_WIDTH;
    file_name_creation[2].name = "Image Width";
    file_name_creation[2].desc = "Add the width of the modified image into the creation of a new file name.";
    file_name_creation[3].data = ADD_HEIGHT;
    file_name_creation[3].name = "Image Height";
    file_name_creation[3].desc = "Add the height of the modified image into the creation of a new file name.";

    // comboBox_ImageFormat
    uint i = 0;
    image_formats[i].data = ".jpeg";
    image_formats[i].name = "JPEG Files - *.jpeg";
    image_formats[i].desc = "JPEG (Joint Photographic Experts Group) is a commonly used method of lossy compression\n" \
        "for digital images, particularly for those images produced by digital photography.\n" \
        "The degree of compression can be adjusted, allowing a selectable tradeoff between\n" \
        "storage size and image quality.JPEG typically achieves 10:1 compression with little\n" \
        "perceptible loss in image quality.";
    image_formats[++i].data = ".jpg";
    image_formats[i].name = "JPEG Files - *.jpg";
    image_formats[i].desc = image_formats[i - 1].desc;
    image_formats[++i].data = ".jpe";
    image_formats[i].name = "JPEG Files - *.jpe";
    image_formats[i].desc = image_formats[i - 2].desc;
    image_formats[++i].data = ".jp2";
    image_formats[i].name = "JPEG 2000 Files - *.jp2";
    image_formats[i].desc = "JPEG 2000 (Joint Photographic Experts Group) is an image compression standard based on a\n" \
        "discrete wavelet transform (DWT). Note that it is still not widely supported in web\n" \
        "browsers (other than Safari) and hence is not generally used on the World Wide Web.";
    image_formats[++i].data = ".png";
    image_formats[i].name = "Portable Network Graphics - *.png";
    image_formats[i].desc = "Portable Network Graphics (PNG) is a raster-graphics file format that supports lossless\n" \
        "data compression. PNG supports palette-based images (with palettes of 24-bit RGB or\n" \
        "32-bit RGBA colors), grayscale images (with or without an alpha channel for transparency),\n" \
        "and full-color non-palette-based RGB or RGBA images.";
    image_formats[++i].data = ".webp";
    image_formats[i].name = "WebP - *.webp";
    image_formats[i].desc = "WebP is a raster graphics file format developed by Google intended as a replacement for\n" \
        "JPEG, PNG, and GIF file formats. It supports both lossy and lossless compression, as well\n" \
        "as animation and alpha transparency.";
    image_formats[++i].data = ".bmp";
    image_formats[i].name = "Windows Bitmaps - *.bmp";
    image_formats[i].desc = "The BMP file format or bitmap, is a raster graphics image file format used to store\n" \
        "bitmap digital images, independently of the display device (such as a graphics adapter),\n" \
        "especially on Microsoft Windows and OS/2 operating systems.";
    image_formats[++i].data = ".dib";
    image_formats[i].name = "Windows Bitmaps - *.dib";
    image_formats[i].desc = image_formats[i - 1].desc;
    image_formats[++i].data = ".avif";
    image_formats[i].name = "AVIF - *.avif";
    image_formats[i].desc = "AV1 Image File Format (AVIF) is an open, royalty-free image file format specification\n" \
        "for storing images or image sequences compressed with AV1 in the HEIF container format.\n" \
        "AV1 Supports:\n" \
        "* Multiple color spaces (HDR, SDR, color space signaling via CICP or ICC)\n" \
        "* Lossless and lossy compression\n" \
        "* 8-, 10-, and 12-bit color depths\n" \
        "* Monochrome (alpha/depth) or multi-components\n" \
        "* 4:2:0, 4:2:2, 4:4:4 chroma subsampling and RGB\n" \
        "* Film grain synthesis\n" \
        "* Image sequences/animation";
    image_formats[++i].data = ".pbm";
    image_formats[i].name = "Netpbm Formats - *.pbm";
    image_formats[i].desc = "Netpbm (formerly Pbmplus) is an open-source package of graphics programs and a programming\n" \
        "library. It is used mainly in the Unix world, but also works on Microsoft Windows, macOS,\n" \
        "and other operating systems.  Graphics formats used and defined by the Netpbm project:\n" \
        "portable pixmap format (PPM), portable graymap format (PGM), and portable bitmap format (PBM).\n" \
        "They are also sometimes referred to collectively as the portable anymap format (PNM).";
    image_formats[++i].data = ".pgm";
    image_formats[i].name = "Netpbm Formats - *.pgm";
    image_formats[i].desc = image_formats[i - 1].desc;
    image_formats[++i].data = ".ppm";
    image_formats[i].name = "Netpbm Formats - *.ppm";
    image_formats[i].desc = image_formats[i - 2].desc;
    image_formats[++i].data = ".pxm";
    image_formats[i].name = "Netpbm Formats - *.pxm";
    image_formats[i].desc = image_formats[i - 3].desc;
    image_formats[++i].data = ".pnm";
    image_formats[i].name = "Netpbm Formats - *.pnm";
    image_formats[i].desc = image_formats[i - 4].desc;
    image_formats[++i].data = ".pfm";
    image_formats[i].name = "Netpbm Formats - *.pfm";
    image_formats[i].desc = "The PFM (Portable Floatmap) is supported by the de facto reference implementation Netpbm\n" \
        "and is the unofficial four byte IEEE 754 single precision floating point extension.\n" \
        "PFM is supported by the programs Photoshop, GIMP, and ImageMagick.";
    // TODO: Test support for this format
    image_formats[++i].data = ".pam";
    image_formats[i].name = "Netpbm Formats - *.pam";
    image_formats[i].desc = "Portable Arbitrary Map (PAM) is an extension of the older binary P4...P6 graphics formats,\n" \
        "introduced with Netpbm version 9.7. PAM generalizes all features of PBM, PGM and PPM, and\n" \
        "provides for extensions. PAM is supported by XnView and FFmpeg; and defines two new\n" \
        "attributes: depth and tuple type.";
    image_formats[++i].data = ".sr";
    image_formats[i].name = "Sun Rasters - *.sr";
    image_formats[i].desc = "Sun Raster was a raster graphics file format used on SunOS by Sun Microsystems. ACDSee,\n" \
        "FFmpeg, GIMP, ImageMagick, IrfanView, LibreOffice, Netpbm, PaintShop Pro, PMView, and\n" \
        "XnView among others support Sun Raster image files. The format does not support transparency.";
    image_formats[++i].data = ".ras";
    image_formats[i].name = "Sun Rasters - *.ras";
    image_formats[i].desc = image_formats[i - 1].desc;
    image_formats[++i].data = ".tiff";
    image_formats[i].name = "TIFF Files - *.tiff";
    image_formats[i].desc = "Tag Image File Format (TIFF or TIF), is an image file format for storing raster graphics\n" \
        "images, popular among graphic artists, the publishing industry, and photographers. TIFF is\n" \
        "widely supported by scanning, faxing, word processing, optical character recognition,\n" \
        "image manipulation, desktop publishing, and page-layout applications.";
    image_formats[++i].data = ".tif";
    image_formats[i].name = "TIFF Files - *.tif";
    image_formats[i].desc = image_formats[i - 1].desc;
    image_formats[++i].data = ".exr";
    image_formats[i].name = "OpenEXR Image Files - *.exr";
    image_formats[i].desc = "OpenEXR is a high-dynamic range, multi-channel raster file format, created under a free\n" \
        "software license similar to the BSD license.  It supports multiple channels of potentially\n" \
        "different pixel sizes, including 32-bit unsigned integer, 32-bit and 16-bit floating point\n" \
        "values, as well as various compression techniques which include lossless and lossy\n" \
        "compression algorithms. It also has arbitrary channels and encodes multiple points of view\n" \
        "such as left- and right-camera images.";
    image_formats[++i].data = ".hdr";
    image_formats[i].name = "Radiance HDR - *.hdr";
    image_formats[i].desc = "RGBE or Radiance HDR is an image format that stores pixels as one byte each for RGB (red,\n" \
        "green, and blue) values with a one byte shared exponent. Thus it stores four bytes per pixel.\n" \
        "RGBE allows pixels to have the dynamic range and precision of floating-point values in a\n" \
        "relatively compact data structure (32 bits per pixel).";
    image_formats[++i].data = ".pic";
    image_formats[i].name = "Radiance HDR - *.pic";
    image_formats[i].desc = image_formats[i - 1].desc;

    // Specific Format Widget Options (multiple sets)
    format_jpeg_options[FormatJpegOptions::label_FormatFlags].data = 1;
    format_jpeg_options[FormatJpegOptions::label_FormatFlags].name = "Subsampling:";
    format_jpeg_options[FormatJpegOptions::label_FormatFlags].desc = "Chroma subsampling is the practice of encoding images by implementing less resolution\n" \
        "for chroma information than for luma information, taking advantage of the human visual\n" \
        "system's lower acuity for color differences than for luminance.";
    format_jpeg_options[FormatJpegOptions::label_Quality].data = 95;
    format_jpeg_options[FormatJpegOptions::label_Quality].name = "Quality:";
    format_jpeg_options[FormatJpegOptions::label_Quality].desc = "JPEG quality can be between 0 and 100 (the higher the better). Default value is 95.";
    format_jpeg_options[FormatJpegOptions::checkBox_Optimize].data = 0;
    format_jpeg_options[FormatJpegOptions::checkBox_Optimize].name = "Optimize";
    format_jpeg_options[FormatJpegOptions::checkBox_Optimize].desc = "JPEG optimize can lower file sizes by striping unnecessary metadata, but only noticeable\n" \
        "at higher quality ranges (95+). Default is unchecked.";
    format_jpeg_options[FormatJpegOptions::checkBox_Progressive].data = 0;
    format_jpeg_options[FormatJpegOptions::checkBox_Progressive].name = "Progressive";
    format_jpeg_options[FormatJpegOptions::checkBox_Progressive].desc = "Using the JPEG interlaced progressive format data is compressed in multiple passes of\n" \
        "progressively higher detail.This is ideal for large images that will be displayed while\n" \
        "downloading over a slow connection, allowing a reasonable preview after receiving only a\n" \
        "portion of the data. Default is unchecked.";
    format_jpeg_options[FormatJpegOptions::label_Compression].data = 0;
    format_jpeg_options[FormatJpegOptions::label_Compression].name = "Restart Interval:";
    format_jpeg_options[FormatJpegOptions::label_Compression].desc = "Restart interval specifies the interval between restart markers, in Minimum Coded Units\n" \
        "(MCUs).They were designed to allow resynchronization after an error, but also now serve a\n" \
        "new purpose, to allow for multi - threaded JPEG encoders and decoders. Default value is 0.";
    format_jpeg_options[FormatJpegOptions::label_ExtraSetting1].data = -1;
    format_jpeg_options[FormatJpegOptions::label_ExtraSetting1].name = "Luma:";
    format_jpeg_options[FormatJpegOptions::label_ExtraSetting1].desc = "Separate and adjust the luma quality between to 0 and 100 (-1 don't use, default). When an\n" \
        "image is converted from RGB to Y'CBCR, the luma component is the (Y'), representing brightness.";
    format_jpeg_options[FormatJpegOptions::label_ExtraSetting2].data = -1;
    format_jpeg_options[FormatJpegOptions::label_ExtraSetting2].name = "Chroma:";
    format_jpeg_options[FormatJpegOptions::label_ExtraSetting2].desc = "Separate and adjust the chroma quality between to 0 and 100 (-1 don't use, default). When an\n" \
        "image is converted from RGB to Y'CBCR, the chroma component is the (CB and CR), representing color.";

    format_jp2_options[FormatJp2Options::label_Compression].data = 1000;
    format_jp2_options[FormatJp2Options::label_Compression].name = "Compression Level:";
    format_jp2_options[FormatJp2Options::label_Compression].desc = "Use to specify the JPEG 2000 target compression rate with values from 0 to 1000. Default value is 1000.";

    format_png_options[FormatPngOptions::label_FormatFlags].data = 0;
    format_png_options[FormatPngOptions::label_FormatFlags].name = "Compression Strategy:";
    format_png_options[FormatPngOptions::label_FormatFlags].desc = "PNG compression strategies are passed to the underlying zlib processing stage, and only affect the\n" \
        "compression ratio, but not the correctness of the compressed output even if it is not set appropriately\n" \
        "The effect of the [Filter] strategy is to force more Huffman coding and less string matching; it is\n" \
        "somewhat intermediate between [Default] and [Huffman Only]. [Run-Length Encoding] is designed to be\n" \
        "almost as fast as [Huffman Only], but gives better compression for PNG image data. [Fixed (No Huffman)]\n" \
        "prevents the use of dynamic Huffman codes, allowing for a simpler decoder for special applications.";
    format_png_options[FormatPngOptions::checkBox_Optimize].data = 0;
    format_png_options[FormatPngOptions::checkBox_Optimize].name = "Binary Level";
    format_png_options[FormatPngOptions::checkBox_Optimize].desc = "If binary level (bi-level) is checked a grayscale PNG image will be created. Default is unchecked.";
    format_png_options[FormatPngOptions::label_Compression].data = 1;
    format_png_options[FormatPngOptions::label_Compression].name = "Compression Level";
    format_png_options[FormatPngOptions::label_Compression].desc = "PNG compression levels are from 0 to 9 with higher values meaning a smaller file size and longer\n" \
        "compression time. If specified, the strategy is changed to [Default].\n" \
        "Default value is 1 (best speed setting).";

    format_webp_options[FormatWebpOptions::label_Quality].data = 100;
    format_webp_options[FormatWebpOptions::label_Quality].name = "Quality:";
    format_webp_options[FormatWebpOptions::label_Quality].desc = "WebP quality can be between 0 and 100 (the higher the better). With 100 quality, the lossless\n" \
        "compression is used. Default value is 100.";

    format_avif_options[FormatAvifOptions::label_Quality].data = 95;
    format_avif_options[FormatAvifOptions::label_Quality].name = "Quality:";
    format_avif_options[FormatAvifOptions::label_Quality].desc = "AVIF quality can be between 0 and 100 (the higher the better). Default value is 95.";
    format_avif_options[FormatAvifOptions::label_Compression].data = 9;
    format_avif_options[FormatAvifOptions::label_Compression].name = "Speed:";
    format_avif_options[FormatAvifOptions::label_Compression].desc = "The AVIF creation speed can be between 0 (slowest) and 9 (fastest). Default value is 9.";
    format_avif_options[FormatAvifOptions::label_ExtraSetting2].data = 8;
    format_avif_options[FormatAvifOptions::label_ExtraSetting2].name = "Color Depth:";
    format_avif_options[FormatAvifOptions::label_ExtraSetting2].desc = "AVIF can have 8-, 10- or 12-bit color depths. If greater than 8, it is stored/read as a float with\n" \
        "pixels having any value between 0 and 1.0. Default value is 8.";

    format_pbm_options[FormatPbmOptions::checkBox_Optimize].data = 1;
    format_pbm_options[FormatPbmOptions::checkBox_Optimize].name = "Binary Format";
    format_pbm_options[FormatPbmOptions::checkBox_Optimize].desc = "For PBM, PGM, or PPM, using a binary format creates a grayscale image. Default is checked.";

    format_pam_options[FormatPamOptions::label_FormatFlags].data = 0;
    format_pam_options[FormatPamOptions::label_FormatFlags].name = "Tuple Type:";
    format_pam_options[FormatPamOptions::label_FormatFlags].desc = "The tuple type attribute specifies what kind of image the PAM file represents, thus enabling it to\n" \
        "stand for the older Netpbm formats, as well as to be extended to new uses, like transparency.";

    format_tiff_options[FormatTiffOptions::label_FormatFlags].data = 4;
    format_tiff_options[FormatTiffOptions::label_FormatFlags].name = "Compression Scheme:";
    format_tiff_options[FormatTiffOptions::label_FormatFlags].desc = "The compression scheme used on the image data. Note that this is a complete list of schemes from the\n" \
        "libtiff documents and some may not be currently supported in this app.\n" \
        "Compression schemes that do work: LWZ (default), Adobe Deflate, and Deflate.";
    format_tiff_options[FormatTiffOptions::label_Quality].data = 2;
    format_tiff_options[FormatTiffOptions::label_Quality].name = "Resolution Unit:";
    format_tiff_options[FormatTiffOptions::label_Quality].desc = "The resolution unit of measurement for X and Y directions.";
    format_tiff_options[FormatTiffOptions::label_ExtraSetting1].data = 0;
    format_tiff_options[FormatTiffOptions::label_ExtraSetting1].name = "X:";
    format_tiff_options[FormatTiffOptions::label_ExtraSetting1].desc = "The number of pixels per \"resolution unit\" in the image's width or X direction. Default value is 0.";
    format_tiff_options[FormatTiffOptions::label_ExtraSetting2].data = 0;
    format_tiff_options[FormatTiffOptions::label_ExtraSetting2].name = "Y:";
    format_tiff_options[FormatTiffOptions::label_ExtraSetting2].desc = "The number of pixels per \"resolution unit\" in the image's height (length) or Y direction. Default value is 0.";

    format_exr_options[FormatExrOptions::label_FormatFlags].data = 3;
    format_exr_options[FormatExrOptions::label_FormatFlags].name = "Compression Type:";
    format_exr_options[FormatExrOptions::label_FormatFlags].desc = "Override EXR compression type (ZLib is default)";
    format_exr_options[FormatExrOptions::checkBox_Optimize].data = 0;
    format_exr_options[FormatExrOptions::checkBox_Optimize].name = "Store as FP16 (HALF)";
    format_exr_options[FormatExrOptions::checkBox_Optimize].desc = "Override EXR Storage Type";
    format_exr_options[FormatExrOptions::checkBox_Progressive].data = 1;
    format_exr_options[FormatExrOptions::checkBox_Progressive].name = "Store as FP32 (Default)";
    format_exr_options[FormatExrOptions::checkBox_Progressive].desc = "Override EXR Storage Type";
    format_exr_options[FormatExrOptions::label_Compression].data = 45;
    format_exr_options[FormatExrOptions::label_Compression].name = "Compression Level:";
    format_exr_options[FormatExrOptions::label_Compression].desc = "Override EXR DWA Compression Level. Default value is 45.";

    format_hdr_options[FormatHdrOptions::label_FormatFlags].data = 0;
    format_hdr_options[FormatHdrOptions::label_FormatFlags].name = "Compression Strategy:";
    format_hdr_options[FormatHdrOptions::label_FormatFlags].desc = "For HDR files there are only really two strategies: compress or don't compress.";

    // comboBox_FormatFlags (multiple sets)
    format_jpeg_subsamplings[0].data = cv::IMWRITE_JPEG_SAMPLING_FACTOR_411;
    format_jpeg_subsamplings[0].name = "4x1, 1x1, 1x1";
    format_jpeg_subsamplings[0].desc = "JPEG Sampling 4:1:1.";
    format_jpeg_subsamplings[1].data = cv::IMWRITE_JPEG_SAMPLING_FACTOR_420;
    format_jpeg_subsamplings[1].name = "2x2, 1x1, 1x1 (Default)";
    format_jpeg_subsamplings[1].desc = "JPEG Sampling 4:2:0. In most cases this is the best option.";
    format_jpeg_subsamplings[2].data = cv::IMWRITE_JPEG_SAMPLING_FACTOR_422;
    format_jpeg_subsamplings[2].name = "2x1, 1x1, 1x1";
    format_jpeg_subsamplings[2].desc = "JPEG Sampling 4:2:2.";
    format_jpeg_subsamplings[3].data = cv::IMWRITE_JPEG_SAMPLING_FACTOR_440;
    format_jpeg_subsamplings[3].name = "1x2, 1x1, 1x1";
    format_jpeg_subsamplings[3].desc = "JPEG Sampling 4:4:0.";
    format_jpeg_subsamplings[4].data = cv::IMWRITE_JPEG_SAMPLING_FACTOR_444;
    format_jpeg_subsamplings[4].name = "1x1, 1x1, 1x1 (No Subsampling)";
    format_jpeg_subsamplings[4].desc = "JPEG Sampling 4:4:4. It can help to turn off subsampling when using more than one quality\n" \
        "setting (Luma/Chroma).";

    format_png_compression[0].data = cv::IMWRITE_PNG_STRATEGY_DEFAULT;
    format_png_compression[0].name = "Default";
    format_png_compression[0].desc = "Use this value for normal data.";
    format_png_compression[1].data = cv::IMWRITE_PNG_STRATEGY_FILTERED;
    format_png_compression[1].name = "Filtered";
    format_png_compression[1].desc = "Use this value for data produced by a filter (or predictor). Filtered data consists mostly of small\n" \
        "values with a somewhat random distribution.In this case, the compression algorithm is tuned to\n" \
        "compress them better.";
    format_png_compression[2].data = cv::IMWRITE_PNG_STRATEGY_HUFFMAN_ONLY;
    format_png_compression[2].name = "Huffman Only";
    format_png_compression[2].desc = "Use this value to force Huffman encoding only (no string match).";
    format_png_compression[3].data = cv::IMWRITE_PNG_STRATEGY_RLE;
    format_png_compression[3].name = "Run-Length Encoding";
    format_png_compression[3].desc = "Use this value to limit match distances to one (run-length encoding).";
    format_png_compression[4].data = cv::IMWRITE_PNG_STRATEGY_FIXED;
    format_png_compression[4].name = "Fixed (No Huffman)";
    format_png_compression[4].desc = "Using this value prevents the use of dynamic Huffman codes, allowing for a simpler decoder for\n" \
        "special applications.";

    format_pam_tupletype[0].data = cv::IMWRITE_PAM_FORMAT_NULL; // TODO: test what this does
    format_pam_tupletype[0].name = "Format Null";
    format_pam_tupletype[0].desc = "No tuple type used.";
    format_pam_tupletype[1].data = cv::IMWRITE_PAM_FORMAT_BLACKANDWHITE;
    format_pam_tupletype[1].name = "Black and White";
    format_pam_tupletype[1].desc = "Black and white is a special case of grayscale, with only 1 byte per pixel (1 depth).";
    format_pam_tupletype[2].data = cv::IMWRITE_PAM_FORMAT_GRAYSCALE;
    format_pam_tupletype[2].name = "Grayscale";
    format_pam_tupletype[2].desc = "Grayscale is equivalent to PGM (portable graymap), with 2 bytes per pixel (1 depth).";
    format_pam_tupletype[3].data = cv::IMWRITE_PAM_FORMAT_GRAYSCALE_ALPHA;
    format_pam_tupletype[3].name = "Grayscale Alpha";
    format_pam_tupletype[3].desc = "Grayscale alpha is equivalent to PGM (portable graymap), but has transparency not directly possible\n" \
        "in PGM, with 4 bytes per pixel (2 depth).";
    format_pam_tupletype[4].data = cv::IMWRITE_PAM_FORMAT_RGB;
    format_pam_tupletype[4].name = "RGB";
    format_pam_tupletype[4].desc = "RGB (red, green, blue) uses a greater depth of colors, with 6 bytes per pixel (3 depth).";
    format_pam_tupletype[5].data = cv::IMWRITE_PAM_FORMAT_RGB_ALPHA;
    format_pam_tupletype[5].name = "RGB Alpha";
    format_pam_tupletype[5].desc = "RGB Alpha uses the colors red, green, and blue with a transparency layer, adding up to 8 bytes per\n" \
        "pixel (4 depth).";

    // TODO: Find more info on Tiff Compression Schemes https://www.awaresystems.be/imaging/tiff/tifftags/baseline.html
    format_tiff_compression[0].data = ImageEditor::COMPRESSION_NONE;
    format_tiff_compression[0].name = "None";
    format_tiff_compression[0].desc = "No Compression.";
    format_tiff_compression[1].data = ImageEditor::COMPRESSION_CCITTRLE;
    format_tiff_compression[1].name = "CCITT RLE";
    format_tiff_compression[1].desc = "";
    format_tiff_compression[2].data = ImageEditor::COMPRESSION_CCITTFAX3;
    format_tiff_compression[2].name = "CCITT FAX3/T4";
    format_tiff_compression[2].desc = "";
    format_tiff_compression[3].data = ImageEditor::COMPRESSION_CCITTFAX4;
    format_tiff_compression[3].name = "CCITT FAX4/T6";
    format_tiff_compression[3].desc = "";
    format_tiff_compression[4].data = ImageEditor::COMPRESSION_LZW;
    format_tiff_compression[4].name = "LZW (Default)";
    format_tiff_compression[4].desc = "LZW is the default compression scheme.";
    format_tiff_compression[5].data = ImageEditor::COMPRESSION_OJPEG;
    format_tiff_compression[5].name = "OJPEG";
    format_tiff_compression[5].desc = "";
    format_tiff_compression[6].data = ImageEditor::COMPRESSION_JPEG;
    format_tiff_compression[6].name = "JPEG";
    format_tiff_compression[6].desc = "";
    format_tiff_compression[7].data = ImageEditor::COMPRESSION_ADOBE_DEFLATE;
    format_tiff_compression[7].name = "ADOBE DEFLATE";
    format_tiff_compression[7].desc = "";
    format_tiff_compression[8].data = ImageEditor::COMPRESSION_NEXT;
    format_tiff_compression[8].name = "NEXT";
    format_tiff_compression[8].desc = "";
    format_tiff_compression[9].data = ImageEditor::COMPRESSION_CCITTRLEW;
    format_tiff_compression[9].name = "CCITT RLE-W";
    format_tiff_compression[9].desc = "";
    format_tiff_compression[10].data = ImageEditor::COMPRESSION_PACKBITS;
    format_tiff_compression[10].name = "PACK BITS";
    format_tiff_compression[10].desc = "";
    format_tiff_compression[11].data = ImageEditor::COMPRESSION_THUNDERSCAN;
    format_tiff_compression[11].name = "THUNDER SCAN";
    format_tiff_compression[11].desc = "";
    format_tiff_compression[12].data = ImageEditor::COMPRESSION_IT8CTPAD;
    format_tiff_compression[12].name = "IT8 CTPAD";
    format_tiff_compression[12].desc = "";
    format_tiff_compression[13].data = ImageEditor::COMPRESSION_IT8LW;
    format_tiff_compression[13].name = "IT8 LW";
    format_tiff_compression[13].desc = "";
    format_tiff_compression[14].data = ImageEditor::COMPRESSION_IT8MP;
    format_tiff_compression[14].name = "IT8 MP";
    format_tiff_compression[14].desc = "";
    format_tiff_compression[15].data = ImageEditor::COMPRESSION_IT8BL;
    format_tiff_compression[15].name = "IT8 BL";
    format_tiff_compression[15].desc = "";
    format_tiff_compression[16].data = ImageEditor::COMPRESSION_PIXARFILM;
    format_tiff_compression[16].name = "PIXAR FILM";
    format_tiff_compression[16].desc = "";
    format_tiff_compression[17].data = ImageEditor::COMPRESSION_PIXARLOG;
    format_tiff_compression[17].name = "PIXAR LOG";
    format_tiff_compression[17].desc = "";
    format_tiff_compression[18].data = ImageEditor::COMPRESSION_DEFLATE;
    format_tiff_compression[18].name = "DEFLATE";
    format_tiff_compression[18].desc = "";
    format_tiff_compression[19].data = ImageEditor::COMPRESSION_DCS;
    format_tiff_compression[19].name = "DCS";
    format_tiff_compression[19].desc = "";
    format_tiff_compression[20].data = ImageEditor::COMPRESSION_JBIG;
    format_tiff_compression[20].name = "JBIG";
    format_tiff_compression[20].desc = "";
    format_tiff_compression[21].data = ImageEditor::COMPRESSION_SGILOG;
    format_tiff_compression[21].name = "SGI LOG";
    format_tiff_compression[21].desc = "";
    format_tiff_compression[22].data = ImageEditor::COMPRESSION_SGILOG24;
    format_tiff_compression[22].name = "SGI LOG24";
    format_tiff_compression[22].desc = "";
    format_tiff_compression[23].data = ImageEditor::COMPRESSION_JP2000;
    format_tiff_compression[23].name = "JP2000";
    format_tiff_compression[23].desc = "";

    format_tiff_resolution_unit[0].data = ImageEditor::RESUNIT_NONE;
    format_tiff_resolution_unit[0].name = "Resolution Unit:";
    format_tiff_resolution_unit[0].desc = "No absolute unit of measurement. Used for images that may have a non-square aspect ratio, but no\n" \
        "meaningful absolute dimensions.";
    format_tiff_resolution_unit[1].data = ImageEditor::RESUNIT_INCH;
    format_tiff_resolution_unit[1].name = "Resolution Unit:";
    format_tiff_resolution_unit[1].desc = "Inch (Default)";
    format_tiff_resolution_unit[2].data = ImageEditor::RESUNIT_CENTIMETER;
    format_tiff_resolution_unit[2].name = "Resolution Unit:";
    format_tiff_resolution_unit[2].desc = "Centimeter";

    format_exr_compression[0].data = cv::IMWRITE_EXR_COMPRESSION_NO;
    format_exr_compression[0].name = "None";
    format_exr_compression[0].desc = "No Compression.";
    format_exr_compression[1].data = cv::IMWRITE_EXR_COMPRESSION_RLE;
    format_exr_compression[1].name = "Run-Length Encoding";
    format_exr_compression[1].desc = "Run-length encoding compression.";
    format_exr_compression[2].data = cv::IMWRITE_EXR_COMPRESSION_ZIPS;
    format_exr_compression[2].name = "ZLib 1 SL";
    format_exr_compression[2].desc = "ZLib compression, one scan line at a time.";
    format_exr_compression[3].data = cv::IMWRITE_EXR_COMPRESSION_ZIP;
    format_exr_compression[3].name = "ZLib 16 SL (Default)";
    format_exr_compression[3].desc = "ZLib compression in blocks of 16 scan lines (default).";
    format_exr_compression[4].data = cv::IMWRITE_EXR_COMPRESSION_PIZ;
    format_exr_compression[4].name = "Piz Wavelet";
    format_exr_compression[4].desc = "Piz-based wavelet compression.";
    format_exr_compression[5].data = cv::IMWRITE_EXR_COMPRESSION_PXR24;
    format_exr_compression[5].name = "PXR24 (Lossy)";
    format_exr_compression[5].desc = "Lossy 24-bit float compression.";
    format_exr_compression[6].data = cv::IMWRITE_EXR_COMPRESSION_B44;
    format_exr_compression[6].name = "4-by-4 Fixed (Lossy)";
    format_exr_compression[6].desc = "B44 lossy 4-by-4 pixel block compression, fixed compression rate.";
    format_exr_compression[7].data = cv::IMWRITE_EXR_COMPRESSION_B44A;
    format_exr_compression[7].name = "4-by-4 Flat (Lossy)";
    format_exr_compression[7].desc = "B44A lossy 4-by-4 pixel block compression, flat fields are compressed more.";
    format_exr_compression[8].data = cv::IMWRITE_EXR_COMPRESSION_DWAA;
    format_exr_compression[8].name = "DWAA 32 SL (Lossy)";
    format_exr_compression[8].desc = "DWAA Lossy DCT based compression, in blocks of 32 scanlines. More efficient for partial buffer access.";
    format_exr_compression[9].data = cv::IMWRITE_EXR_COMPRESSION_DWAB;
    format_exr_compression[9].name = "DWAB 256 SL (Lossy)";
    format_exr_compression[9].desc = "DWAB Lossy DCT based compression, in blocks of 256 scanlines. More efficient space wise and faster\n" \
        "to decode full frames than DWAA.";

    format_hdr_compression[0].data = cv::IMWRITE_HDR_COMPRESSION_NONE;
    format_hdr_compression[0].name = "None (Default)";
    format_hdr_compression[0].desc = "No compression.";
    format_hdr_compression[1].data = cv::IMWRITE_HDR_COMPRESSION_RLE;
    format_hdr_compression[1].name = "Run-Length Encoding";
    format_hdr_compression[1].desc = "The only compression option.";

    // Various Other Widget data
    other_options[OtherOptions::tab_1].data = 0; // Default current tab index, ignores other tab.data.
    other_options[OtherOptions::tab_1].name = "Images";
    //other_options[OtherOptions::tab_1].desc = "Image File Viewer Tab";
    other_options[OtherOptions::tab_2].data = 0;
    other_options[OtherOptions::tab_2].name = "Image Edits";
    //other_options[OtherOptions::tab_2].desc = "Image Edit Tools Tab";
    other_options[OtherOptions::tab_3].data = 0;
    other_options[OtherOptions::tab_3].name = "Save Options";
    //other_options[OtherOptions::tab_3].desc = "File Save Options Tab";
    other_options[OtherOptions::checkBox_SearchSubDirs].data = 1;
    other_options[OtherOptions::checkBox_SearchSubDirs].name = "When Searching Directories Search Sub-Directories As Well";
    other_options[OtherOptions::checkBox_SearchSubDirs].desc = "When file directories/folders are dropped into the image file viewer an image file search will begin\n" \
                                                               "in the directory, and it this is checked, all its sub-directories too.";
    other_options[OtherOptions::pushButton_EditAndSave].data = 0;
    other_options[OtherOptions::pushButton_EditAndSave].name = "Start Editing And Saving Images";
    other_options[OtherOptions::pushButton_EditAndSave].desc = "";
    /*other_options[OtherOptions::].data = 0;
    other_options[OtherOptions::].name = "";
    other_options[OtherOptions::].desc = "";*/

}

/// <summary>
/// Setup file tree with initial settings and header titles.
/// </summary>
void BatchItImage::SetupFileTree()
{
    ui.treeWidget_FileInfo->clear();
    for (int col = 0; col < FileColumn::COUNT; col++) {
        ui.treeWidget_FileInfo->headerItem()->setText(col, QString::fromStdString(file_tree_headers[col].name));
        ui.treeWidget_FileInfo->headerItem()->setToolTip(col, QString::fromStdString(file_tree_headers[col].desc));
    }
    ui.treeWidget_FileInfo->setColumnWidth(FileColumn::FILE_SELECTED, ui.treeWidget_FileInfo->minimumWidth());
    ui.treeWidget_FileInfo->header()->setSectionsClickable(true);
    ui.treeWidget_FileInfo->header()->sortIndicatorOrder();
    ui.treeWidget_FileInfo->installEventFilter(this); // Keep watch of all events happening in file tree. -> eventFilter()
    ui.treeWidget_FileInfo->setMouseTracking(true);
}

/// <summary>
/// Add display text, tooltip descriptions, and other data to various ui objects/widgets.
/// </summary>
/// <param name="object">--Pointer to an object/widget.</param>
/// <param name="object_data">--Pointer to a UIData.</param>
void BatchItImage::AddUiObjectData(QObject* object, UIData* object_data)
{
    std::string object_class = object->metaObject()->className();
    DEBUG2("AddUiObjectData: ", object_class);
    
    if ("QCheckBox" == object_class) {
        QCheckBox* cb = qobject_cast<QCheckBox*>(object);
        cb->setChecked(std::get<int>(object_data->data));
        cb->setText(QString::fromStdString(object_data->name));
        cb->setStatusTip(QString::fromStdString(object_data->desc));
        cb->setToolTip(QString::fromStdString(object_data->desc));
    }
    else if ("QGroupBox" == object_class) {
        QGroupBox* gb = qobject_cast<QGroupBox*>(object);
        gb->setChecked(std::get<int>(object_data->data));
        gb->setTitle(QString::fromStdString(object_data->name));
        gb->setStatusTip(QString::fromStdString(object_data->desc));
        gb->setToolTip(QString::fromStdString(object_data->desc));
    }else if ("QLabel" == object_class) {
        QLabel* lbl = qobject_cast<QLabel*>(object);
        lbl->setText(QString::fromStdString(object_data->name));
        lbl->setStatusTip(QString::fromStdString(object_data->desc));
        lbl->setToolTip(QString::fromStdString(object_data->desc));
    }
    else if ("QRadioButton" == object_class) {
        QRadioButton* rb = qobject_cast<QRadioButton*>(object);
        rb->setChecked(std::get<int>(object_data->data));
        rb->setText(QString::fromStdString(object_data->name));
        rb->setStatusTip(QString::fromStdString(object_data->desc));
        rb->setToolTip(QString::fromStdString(object_data->desc));
    }
    else if ("QPushButton" == object_class) {
        QPushButton* pb = qobject_cast<QPushButton*>(object);
        pb->setText(QString::fromStdString(object_data->name));
        pb->setStatusTip(QString::fromStdString(object_data->desc));
        pb->setToolTip(QString::fromStdString(object_data->desc));
    }
}

/// <summary>
/// Add items to various combo boxes which include titles, tooltip descriptions, and other data.
/// </summary>
/// <param name="cb">--Pointer to a QComboBox.</param>
/// <param name="items">--Data array to enter into a combo box.</param>
/// <param name="items_size">--Size/length of item data array.</param>
void BatchItImage::PopulateComboBox(QComboBox* cb, UIData items[], int items_size)
{
    cb->clear();
    for (int i = 0; i < items_size; i++) {
        if (const int* data = std::get_if<int>(&items[i].data)) { // variant<int>
            cb->addItem(
                QString::fromStdString(items[i].name),
                QVariant::fromValue(*data)
            );
        }
        else if (const std::string* data = std::get_if<std::string>(&items[i].data)) { // variant<string>
            cb->addItem(
                QString::fromStdString(items[i].name),
                QString::fromStdString(*data)
            );
        }
        cb->setItemData( i,
            QString::fromStdString(items[i].desc),
            Qt::ToolTipRole
        );
        cb->setItemData( i,
            QString::fromStdString(items[i].desc),
            Qt::StatusTipRole
        );
    }
    cb->setStatusTip(cb->currentData(Qt::StatusTipRole).toString());
    cb->setToolTip(cb->currentData(Qt::ToolTipRole).toString());
}

/// <summary>
/// Slot: Update a combo box tooltip when index changes.
/// </summary>
void BatchItImage::UpdateComboBoxToolTip()
{
    QComboBox* cb = qobject_cast<QComboBox*>(sender());
    cb->setToolTip(cb->currentData(Qt::ToolTipRole).toString());
    //DEBUG2("UpdateComboBoxToolTip: ", cb->objectName().toStdString());
}

/// <summary>
/// Enable format specific ui options depending on which format change option is currently selected.
/// Each format makes use of a different set of options.
/// </summary>
/// <param name="loading_preset">--If loading presets do not insert any default values. Default is false.</param>
void BatchItImage::EnableSpecificFormatOptions(bool loading_preset)
{
    DEBUG2("EnableSpecificFormatOptions: ", loading_preset);
    const std::string format = ui.comboBox_ImageFormat->currentData().toString().toStdString();

    int NONE = 0;
    int FORMATFLAGS = 1;
    int QUALITY = 2;
    int OPTIMIZE = 4;
    int PROGRESSIVE = 8;
    int COMPRESSION = 16;
    int EXTRASETTING1 = 32;
    int EXTRASETTING2 = 64;
    auto enableOptions = [&](int options)
        {
            if (options & FORMATFLAGS) {
                ui.label_FormatFlags->setFont(*font_default);
                ui.comboBox_FormatFlags->setEnabled(true);
                ui.comboBox_FormatFlags->setFont(*font_default);
            }
            else {
                ui.label_FormatFlags->setFont(*font_default_light);
                ui.comboBox_FormatFlags->setEnabled(false);
                ui.comboBox_FormatFlags->setFont(*font_default_light);
            }
            if (options & QUALITY) {
                ui.label_Quality->setFont(*font_default);
                ui.horizontalSlider_Quality->setEnabled(true);
            }
            else {
                ui.label_Quality->setFont(*font_default_light);
                ui.horizontalSlider_Quality->setEnabled(false);
            }
            if (options & OPTIMIZE) {
                ui.checkBox_Optimize->setAutoExclusive(false);
                ui.checkBox_Optimize->setFont(*font_default);
                ui.checkBox_Optimize->setEnabled(true);
            }
            else {
                ui.checkBox_Optimize->setFont(*font_default_light);
                ui.checkBox_Optimize->setEnabled(false);
            }
            if (options & PROGRESSIVE) {
                ui.checkBox_Progressive->setAutoExclusive(false);
                ui.checkBox_Progressive->setFont(*font_default);
                ui.checkBox_Progressive->setEnabled(true);
            }
            else {
                ui.checkBox_Progressive->setFont(*font_default_light);
                ui.checkBox_Progressive->setEnabled(false);
            }
            if (options & COMPRESSION) {
                ui.label_Compression->setFont(*font_default);
                ui.spinBox_Compression->setEnabled(true);
                ui.spinBox_Compression->setFont(*font_default);
                ui.spinBox_ExtraSetting1->setSingleStep(1);
            }
            else {
                ui.label_Compression->setFont(*font_default_light);
                ui.spinBox_Compression->setEnabled(false);
                ui.spinBox_Compression->setFont(*font_default_light);
            }
            if (options & EXTRASETTING1) {
                ui.label_ExtraSetting1->setFont(*font_default);
                ui.spinBox_ExtraSetting1->setEnabled(true);
                ui.spinBox_ExtraSetting1->setFont(*font_default);
                ui.spinBox_ExtraSetting1->setSingleStep(1);
            }
            else {
                ui.label_ExtraSetting1->setFont(*font_default_light);
                ui.spinBox_ExtraSetting1->setEnabled(false);
                ui.spinBox_ExtraSetting1->setFont(*font_default_light);
            }
            if (options & EXTRASETTING2) {
                ui.label_ExtraSetting2->setFont(*font_default);
                ui.spinBox_ExtraSetting2->setEnabled(true);
                ui.spinBox_ExtraSetting2->setFont(*font_default);
                ui.spinBox_ExtraSetting2->setSingleStep(1);
                ui.spinBox_ExtraSetting2->disconnect(
                    ui.spinBox_ExtraSetting2, &QAbstractSpinBox::editingFinished, this, nullptr
                );
            }
            else {
                ui.label_ExtraSetting2->setFont(*font_default_light);
                ui.spinBox_ExtraSetting2->setEnabled(false);
                ui.spinBox_ExtraSetting2->setFont(*font_default_light);
            }
        };

    // IMWRITE_JPEG_QUALITY  IMWRITE_JPEG_PROGRESSIVE  IMWRITE_JPEG_OPTIMIZE  IMWRITE_JPEG_RST_INTERVAL  IMWRITE_JPEG_LUMA_QUALITY  IMWRITE_JPEG_CHROMA_QUALITY  IMWRITE_JPEG_SAMPLING_FACTOR
    if (format == ".jpeg" or format == ".jpg" or format == ".jpe") { 
        
        enableOptions(FORMATFLAGS + QUALITY + OPTIMIZE + PROGRESSIVE + COMPRESSION + EXTRASETTING1 + EXTRASETTING2);
        
        ui.label_FormatFlags->setText(format_jpeg_options[FormatJpegOptions::label_FormatFlags].name.c_str());
        ui.label_FormatFlags->setToolTip(format_jpeg_options[FormatJpegOptions::label_FormatFlags].desc.c_str());
        ui.label_Quality->setText(format_jpeg_options[FormatJpegOptions::label_Quality].name.c_str());
        ui.label_Quality->setToolTip(format_jpeg_options[FormatJpegOptions::label_Quality].desc.c_str());
        ui.checkBox_Optimize->setText(format_jpeg_options[FormatJpegOptions::checkBox_Optimize].name.c_str());
        ui.checkBox_Optimize->setToolTip(format_jpeg_options[FormatJpegOptions::checkBox_Optimize].desc.c_str());
        ui.checkBox_Progressive->setText(format_jpeg_options[FormatJpegOptions::checkBox_Progressive].name.c_str());
        ui.checkBox_Progressive->setToolTip(format_jpeg_options[FormatJpegOptions::checkBox_Progressive].desc.c_str());
        ui.label_Compression->setText(format_jpeg_options[FormatJpegOptions::label_Compression].name.c_str());
        ui.label_Compression->setToolTip(format_jpeg_options[FormatJpegOptions::label_Compression].desc.c_str());
        ui.label_ExtraSetting1->setText(format_jpeg_options[FormatJpegOptions::label_ExtraSetting1].name.c_str());
        ui.label_ExtraSetting1->setToolTip(format_jpeg_options[FormatJpegOptions::label_ExtraSetting1].desc.c_str());
        ui.label_ExtraSetting2->setText(format_jpeg_options[FormatJpegOptions::label_ExtraSetting2].name.c_str());
        ui.label_ExtraSetting2->setToolTip(format_jpeg_options[FormatJpegOptions::label_ExtraSetting2].desc.c_str());

        if (loading_preset)
            PopulateComboBox(ui.comboBox_FormatFlags, format_jpeg_subsamplings, sizeof(format_jpeg_subsamplings) / sizeof(UIData));
        
        int default_quality_value = std::get<int>(format_jpeg_options[FormatJpegOptions::label_Quality].data);
        ui.horizontalSlider_Quality->setRange(0, 100);
        ui.horizontalSlider_Quality->addTextValue(default_quality_value, default_quality_value, "Default", true);
        ui.spinBox_Compression->setRange(0, 65535);
        ui.spinBox_ExtraSetting1->setRange(-1, 100);
        ui.spinBox_ExtraSetting2->setRange(-1, 100);

        if (not loading_preset and last_selected_format != ".jpeg" and last_selected_format != ".jpg" and last_selected_format != ".jpe") {
            PopulateComboBox(ui.comboBox_FormatFlags, format_jpeg_subsamplings, sizeof(format_jpeg_subsamplings) / sizeof(UIData));
            ui.comboBox_FormatFlags->setCurrentIndex(std::get<int>(format_jpeg_options[FormatJpegOptions::label_FormatFlags].data));
            ui.horizontalSlider_Quality->setValue(default_quality_value);
            ui.checkBox_Optimize->setChecked(std::get<int>(format_jpeg_options[FormatJpegOptions::checkBox_Optimize].data));
            ui.checkBox_Progressive->setChecked(std::get<int>(format_jpeg_options[FormatJpegOptions::checkBox_Progressive].data));
            ui.spinBox_Compression->setValue(std::get<int>(format_jpeg_options[FormatJpegOptions::label_Compression].data));
            ui.spinBox_ExtraSetting1->setValue(std::get<int>(format_jpeg_options[FormatJpegOptions::label_ExtraSetting1].data));
            ui.spinBox_ExtraSetting2->setValue(std::get<int>(format_jpeg_options[FormatJpegOptions::label_ExtraSetting2].data));
        }
    }
    else if (format == ".jp2") { // cv::IMWRITE_JPEG2000_COMPRESSION_X1000
        
        enableOptions(COMPRESSION);

        ui.label_Compression->setText(format_jp2_options[FormatJp2Options::label_Compression].name.c_str());
        ui.label_Compression->setToolTip(format_jp2_options[FormatJp2Options::label_Compression].desc.c_str());

        ui.spinBox_Compression->setRange(0, 1000);

        if (not loading_preset and last_selected_format != ".jp2") {
            ui.spinBox_Compression->setValue(std::get<int>(format_jp2_options[FormatJp2Options::label_Compression].data));
        }
    }
    else if (format == ".png") { // cv::IMWRITE_PNG_COMPRESSION  cv::IMWRITE_PNG_STRATEGY  cv::IMWRITE_PNG_BILEVEL
        
        enableOptions(FORMATFLAGS + OPTIMIZE + COMPRESSION);

        ui.label_FormatFlags->setText(format_png_options[FormatPngOptions::label_FormatFlags].name.c_str());
        ui.label_FormatFlags->setToolTip(format_png_options[FormatPngOptions::label_FormatFlags].desc.c_str());
        ui.checkBox_Optimize->setText(format_png_options[FormatPngOptions::checkBox_Optimize].name.c_str());
        ui.checkBox_Optimize->setToolTip(format_png_options[FormatPngOptions::checkBox_Optimize].desc.c_str());
        ui.label_Compression->setText(format_png_options[FormatPngOptions::label_Compression].name.c_str());
        ui.label_Compression->setToolTip(format_png_options[FormatPngOptions::label_Compression].desc.c_str());

        PopulateComboBox(ui.comboBox_FormatFlags, format_png_compression, sizeof(format_png_compression) / sizeof(UIData));
        ui.spinBox_Compression->setRange(0, 9);

        if (not loading_preset and last_selected_format != ".png") {
            ui.comboBox_FormatFlags->setCurrentIndex(std::get<int>(format_png_options[FormatPngOptions::label_FormatFlags].data));
            ui.checkBox_Optimize->setChecked(std::get<int>(format_png_options[FormatPngOptions::checkBox_Optimize].data));
            ui.spinBox_Compression->setValue(std::get<int>(format_png_options[FormatPngOptions::label_Compression].data));
        }
    }
    else if (format == ".webp") { // cv::IMWRITE_WEBP_QUALITY 

        enableOptions(QUALITY);

        ui.label_Quality->setText(format_webp_options[FormatWebpOptions::label_Quality].name.c_str());
        ui.label_Quality->setToolTip(format_webp_options[FormatWebpOptions::label_Quality].desc.c_str());

        int default_quality_value = std::get<int>(format_webp_options[FormatWebpOptions::label_Quality].data);
        ui.horizontalSlider_Quality->setRange(1, 100);
        ui.horizontalSlider_Quality->addTextValue(default_quality_value, default_quality_value, "Default", true);

        if (not loading_preset and last_selected_format != ".webp") {
            ui.horizontalSlider_Quality->setValue(default_quality_value);
        }
    }
    else if (format == ".avif") { // cv::IMWRITE_AVIF_QUALITY  cv::IMWRITE_AVIF_DEPTH  cv::IMWRITE_AVIF_SPEED   TODO: test  IMWRITE_JPEG_SAMPLING_FACTOR

        enableOptions(QUALITY + COMPRESSION + EXTRASETTING2);

        //ui.label_FormatFlags->setText(":");
        ui.label_Quality->setText(format_avif_options[FormatAvifOptions::label_Quality].name.c_str());
        ui.label_Quality->setToolTip(format_avif_options[FormatAvifOptions::label_Quality].desc.c_str());
        ui.label_Compression->setText(format_avif_options[FormatAvifOptions::label_Compression].name.c_str());
        ui.label_Compression->setToolTip(format_avif_options[FormatAvifOptions::label_Compression].desc.c_str());
        ui.label_ExtraSetting2->setText(format_avif_options[FormatAvifOptions::label_ExtraSetting2].name.c_str());
        ui.label_ExtraSetting2->setToolTip(format_avif_options[FormatAvifOptions::label_ExtraSetting2].desc.c_str());

        int default_quality_value = std::get<int>(format_avif_options[FormatAvifOptions::label_Quality].data);
        ui.horizontalSlider_Quality->setRange(0, 100);
        ui.horizontalSlider_Quality->addTextValue(default_quality_value, default_quality_value, "Default", true);
        ui.spinBox_Compression->setRange(0, 9);
        ui.spinBox_ExtraSetting2->setRange(8, 12);
        ui.spinBox_ExtraSetting2->connect(ui.spinBox_ExtraSetting2, &QAbstractSpinBox::editingFinished, this,
            [this] { // Only 8, 10, 12
                int val = ui.spinBox_ExtraSetting2->value();
                if (val == 9 or val == 11) {
                    ui.spinBox_ExtraSetting2->setValue(10);
                }
            });
        ui.spinBox_ExtraSetting2->setSingleStep(2);

        if (not loading_preset and last_selected_format != ".avif") {
            //ui.comboBox_FormatFlags->setCurrentIndex(0);
            ui.horizontalSlider_Quality->setValue(default_quality_value);
            ui.spinBox_Compression->setValue(std::get<int>(format_avif_options[FormatAvifOptions::label_Compression].data));
            ui.spinBox_ExtraSetting2->setValue(std::get<int>(format_avif_options[FormatAvifOptions::label_ExtraSetting2].data));
        }
    }
    else if (format == ".pbm" or format == ".pgm" or format == ".ppm") { // IMWRITE_PXM_BINARY

        enableOptions(OPTIMIZE);

        ui.checkBox_Optimize->setText(format_pbm_options[FormatPbmOptions::checkBox_Optimize].name.c_str());
        ui.checkBox_Optimize->setToolTip(format_pbm_options[FormatPbmOptions::checkBox_Optimize].desc.c_str());

        if (not loading_preset and last_selected_format != ".pbm" and last_selected_format != ".pgm" and last_selected_format != ".ppm") {
            ui.checkBox_Optimize->setChecked(std::get<int>(format_pbm_options[FormatPbmOptions::checkBox_Optimize].data));
        }
    }
    else if (format == ".pam") { // IMWRITE_PAM_TUPLETYPE

        enableOptions(FORMATFLAGS);

        ui.label_FormatFlags->setText(format_pam_options[FormatPamOptions::label_FormatFlags].name.c_str());
        ui.label_FormatFlags->setToolTip(format_pam_options[FormatPamOptions::label_FormatFlags].desc.c_str());

        PopulateComboBox(ui.comboBox_FormatFlags, format_pam_tupletype, sizeof(format_pam_tupletype) / sizeof(UIData));

        if (not loading_preset and last_selected_format != ".pam") {
            ui.comboBox_FormatFlags->setCurrentIndex(std::get<int>(format_pam_options[FormatPamOptions::label_FormatFlags].data));
        }
    }
    else if (format == ".tiff" or format == ".tif") {

        enableOptions(FORMATFLAGS + QUALITY + EXTRASETTING1 + EXTRASETTING2);

        ui.label_FormatFlags->setText(format_tiff_options[FormatTiffOptions::label_FormatFlags].name.c_str());
        ui.label_FormatFlags->setToolTip(format_tiff_options[FormatTiffOptions::label_FormatFlags].desc.c_str());
        ui.label_Quality->setText(format_tiff_options[FormatTiffOptions::label_Quality].name.c_str());
        ui.label_Quality->setToolTip(format_tiff_options[FormatTiffOptions::label_Quality].desc.c_str());
        ui.label_ExtraSetting1->setText(format_tiff_options[FormatTiffOptions::label_ExtraSetting1].name.c_str());
        ui.label_ExtraSetting1->setToolTip(format_tiff_options[FormatTiffOptions::label_ExtraSetting1].desc.c_str());
        ui.label_ExtraSetting2->setText(format_tiff_options[FormatTiffOptions::label_ExtraSetting2].name.c_str());
        ui.label_ExtraSetting2->setToolTip(format_tiff_options[FormatTiffOptions::label_ExtraSetting2].desc.c_str());

        if (loading_preset)
            PopulateComboBox(ui.comboBox_FormatFlags, format_tiff_compression, sizeof(format_tiff_compression) / sizeof(UIData));
        ui.horizontalSlider_Quality->setRange(1, 3);
        for (auto& tiff_ru : format_tiff_resolution_unit) {
            ui.horizontalSlider_Quality->addTextValue(
                std::get<int>(tiff_ru.data),
                std::get<int>(tiff_ru.data),
                QString::fromStdString(tiff_ru.name + " " + tiff_ru.desc),
                true
            );
        }
        ui.spinBox_ExtraSetting1->setRange(0, INT_MAX);
        ui.spinBox_ExtraSetting2->setRange(0, INT_MAX);

        if (not loading_preset and last_selected_format != ".tiff" and last_selected_format != ".tif") {
            PopulateComboBox(ui.comboBox_FormatFlags, format_tiff_compression, sizeof(format_tiff_compression) / sizeof(UIData));
            ui.comboBox_FormatFlags->setCurrentIndex(std::get<int>(format_tiff_options[FormatTiffOptions::label_FormatFlags].data));
            ui.horizontalSlider_Quality->setValue(std::get<int>(format_tiff_options[FormatTiffOptions::label_Quality].data));
            ui.spinBox_ExtraSetting1->setValue(std::get<int>(format_tiff_options[FormatTiffOptions::label_ExtraSetting1].data));
            ui.spinBox_ExtraSetting2->setValue(std::get<int>(format_tiff_options[FormatTiffOptions::label_ExtraSetting2].data));
        }

    }
    else if (format == ".exr") { // IMWRITE_EXR_TYPE  IMWRITE_EXR_COMPRESSION  IMWRITE_EXR_DWA_COMPRESSION_LEVEL

        enableOptions(FORMATFLAGS + OPTIMIZE + PROGRESSIVE + COMPRESSION);

        ui.label_FormatFlags->setText(format_exr_options[FormatExrOptions::label_FormatFlags].name.c_str());
        ui.label_FormatFlags->setToolTip(format_exr_options[FormatExrOptions::label_FormatFlags].desc.c_str());
        ui.checkBox_Optimize->setText(format_exr_options[FormatExrOptions::checkBox_Optimize].name.c_str());
        ui.checkBox_Optimize->setToolTip(format_exr_options[FormatExrOptions::checkBox_Optimize].desc.c_str());
        ui.checkBox_Progressive->setText(format_exr_options[FormatExrOptions::checkBox_Progressive].name.c_str());
        ui.checkBox_Progressive->setToolTip(format_exr_options[FormatExrOptions::checkBox_Progressive].desc.c_str());
        ui.label_Compression->setText(format_exr_options[FormatExrOptions::label_Compression].name.c_str());
        ui.label_Compression->setToolTip(format_exr_options[FormatExrOptions::label_Compression].desc.c_str());

        PopulateComboBox(ui.comboBox_FormatFlags, format_exr_compression, sizeof(format_exr_compression) / sizeof(UIData));
        ui.checkBox_Optimize->setAutoExclusive(true);
        ui.checkBox_Progressive->setAutoExclusive(true);

        if (not loading_preset and last_selected_format != ".exr") {
            ui.comboBox_FormatFlags->setCurrentIndex(std::get<int>(format_exr_options[FormatExrOptions::label_FormatFlags].data));
            ui.checkBox_Optimize->setChecked(std::get<int>(format_exr_options[FormatExrOptions::checkBox_Optimize].data));
            ui.checkBox_Progressive->setChecked(std::get<int>(format_exr_options[FormatExrOptions::checkBox_Progressive].data));
            ui.spinBox_Compression->setValue(std::get<int>(format_exr_options[FormatExrOptions::label_Compression].data));
        }

    }
    else if (format == ".hdr" or format == ".pic") { // IMWRITE_HDR_COMPRESSION 

        enableOptions(FORMATFLAGS);

        ui.label_FormatFlags->setText(format_hdr_options[FormatHdrOptions::label_FormatFlags].name.c_str());
        ui.label_FormatFlags->setToolTip(format_hdr_options[FormatHdrOptions::label_FormatFlags].desc.c_str());

        if (loading_preset)
            PopulateComboBox(ui.comboBox_FormatFlags, format_hdr_compression, sizeof(format_hdr_compression) / sizeof(UIData));

        if (not loading_preset and last_selected_format != ".hdr" and last_selected_format != ".pic") {
            PopulateComboBox(ui.comboBox_FormatFlags, format_hdr_compression, sizeof(format_hdr_compression) / sizeof(UIData));
            ui.comboBox_FormatFlags->setCurrentIndex(std::get<int>(format_hdr_options[FormatHdrOptions::label_FormatFlags].data));
        }
    }
    else {
        enableOptions(NONE);
    }
    // If change format check box unchecked, make sure to disable all options after adding data/text and preset loaded.
    if (not ui.groupBox_ChangeFormat->isChecked()) {
        enableOptions(NONE);
    }
    if (not loading_preset)
        last_selected_format = format;
}

/// <summary>
/// Build a "right click" context menu for the file tree.
/// </summary>
void BatchItImage::SetupFileTreeContextMenu()
{
    action_add = new QAction(QString::fromStdString(file_tree_menu_items[ActionMenu::action_add].name), this);
    action_delete = new QAction(QString::fromStdString(file_tree_menu_items[ActionMenu::action_delete].name), this);
    action_clear = new QAction(QString::fromStdString(file_tree_menu_items[ActionMenu::action_clear].name), this);
    action_select = new QAction(QString::fromStdString(file_tree_menu_items[ActionMenu::action_select].name), this);
    action_view = new QAction(QString::fromStdString(file_tree_menu_items[ActionMenu::action_view].name), this);
    action_preview = new QAction(QString::fromStdString(file_tree_menu_items[ActionMenu::action_preview].name), this);

    action_add->setToolTip(QString::fromStdString(file_tree_menu_items[ActionMenu::action_add].desc));
    action_delete->setToolTip(QString::fromStdString(file_tree_menu_items[ActionMenu::action_delete].desc));
    action_clear->setToolTip(QString::fromStdString(file_tree_menu_items[ActionMenu::action_clear].desc));
    action_select->setToolTip(QString::fromStdString(file_tree_menu_items[ActionMenu::action_select].desc));
    action_view->setToolTip(QString::fromStdString(file_tree_menu_items[ActionMenu::action_view].desc));
    action_preview->setToolTip(QString::fromStdString(file_tree_menu_items[ActionMenu::action_preview].desc));

    auto action_line_1 = new QAction(this);
    action_line_1->setSeparator(true);
    auto action_line_2 = new QAction(this);
    action_line_2->setSeparator(true);

    connect(action_add, SIGNAL(triggered()), this, SLOT(LoadImageFiles()));
    connect(action_delete, SIGNAL(triggered()), this, SLOT(DeleteConfirmationPopup()));
    connect(action_clear, &QAction::triggered, [this] { DeleteConfirmationPopup(true); });
    connect(action_view, SIGNAL(triggered()), this, SLOT(Test())); // TODO
    connect(action_preview, SIGNAL(triggered()), this, SLOT(Test())); // TODO
    connect(action_select, &QAction::triggered,
        [this] {
            int current_file_tree_row = GetCurrentFileTreeRow();
            if (current_file_tree_row > -1) {
                Qt::CheckState toggle = (current_file_metadata_list.at(current_file_tree_row).selected) ? Qt::Unchecked : Qt::Checked;
                qobject_cast<QCheckBox*>(ui.treeWidget_FileInfo->itemWidget(ui.treeWidget_FileInfo->currentItem(), 0))->setCheckState(toggle);
            }
        });

    // Enabled later when files are loaded into tree.
    ToggleFileTreeContextMenuItems(false);

    // Create the Context Menu
    ui.treeWidget_FileInfo->setContextMenuPolicy(Qt::ActionsContextMenu);
    //ui.treeWidget_FileInfo->setContextMenuPolicy(Qt::CustomContextMenu);
    ui.treeWidget_FileInfo->addActions({ 
        action_add, action_delete, action_clear, action_line_1, 
        action_select, action_line_2, action_view, action_preview 
        });

#ifdef DEBUG
    DEBUG("Adding: action_debug_quick_load");
    QAction* action_debug_quick_load = new QAction("Debug Quick Load", this);
    QStringList testing_file_list;
    //testing_file_list.append(qdefault_path + R"(/test_images/01.jpg)"); // large file
    testing_file_list.append(qdefault_path + R"(/test_images/79.jpg)");
    testing_file_list.append(qdefault_path + R"(/test_images/evil_monkey.png)");
    testing_file_list.append(qdefault_path + R"(/test_images/AC01.png)");
    testing_file_list.append(qdefault_path + R"(/test_images/AC02.png)");
    connect(action_debug_quick_load, &QAction::triggered, [this, testing_file_list] { AddNewFiles(testing_file_list); });
    ui.treeWidget_FileInfo->addAction(action_debug_quick_load);
#endif // DEBUG
}

/// <summary>
/// Toggle usability of certain action context menu items.
/// </summary>
/// <param name="enable">--True enabled, false disabled</param>
void BatchItImage::ToggleFileTreeContextMenuItems(bool enable)
{
    action_delete->setEnabled(enable);
    action_clear->setEnabled(enable);
    action_select->setEnabled(enable);
    action_view->setEnabled(enable);
    action_preview->setEnabled(enable);
    if (enable) {
        action_delete->setFont(*font_default);
        action_clear->setFont(*font_default);
        action_select->setFont(*font_default);
        action_view->setFont(*font_default);
        action_preview->setFont(*font_default);
    }
    else {
        action_delete->setFont(*font_default_light);
        action_clear->setFont(*font_default_light);
        action_select->setFont(*font_default_light);
        action_view->setFont(*font_default_light);
        action_preview->setFont(*font_default_light);
    }
}

/// <summary>
/// Change the currently selected preset.
/// </summary>
/// <param name="index">--The index of the preset.</param>
void BatchItImage::ChangePreset(int index)
{
    ui.comboBox_Preset_1->blockSignals(true);
    ui.comboBox_Preset_2->blockSignals(true);
    ui.comboBox_Preset_3->blockSignals(true);

    DEBUG2("current_selected_preset: ", current_selected_preset);
    if (index != current_selected_preset) {
        // TODO: ask user before saving?
        SavePreset(); // Save previous preset before change
        current_selected_preset = index;
        //DEBUG2("current_selected_preset: ", current_selected_preset);
        
        ui.comboBox_Preset_1->setCurrentIndex(current_selected_preset);
        ui.comboBox_Preset_2->setCurrentIndex(current_selected_preset);
        ui.comboBox_Preset_3->setCurrentIndex(current_selected_preset);
        
        LoadPreset(preset_list.at(current_selected_preset));
    }

    ui.comboBox_Preset_1->blockSignals(false);
    ui.comboBox_Preset_2->blockSignals(false);
    ui.comboBox_Preset_3->blockSignals(false);
}

/// <summary>
/// Save current (or all) preset to the preset list and to the user settings file.
/// </summary>
/// <param name="save_all">--Save all presets to settings file.</param>
void BatchItImage::SavePreset(bool save_all)
{
    DEBUG2("SavePresets->current_selected_preset: ", current_selected_preset);

    //QStringList recent_image_files;
    //recent_image_files.resize(20);
    // TODO: record time file added, sort descending, when adding to "recent_image_files" create new QStringList and join with old.

    QSettings settings(preset_settings_file, QSettings::IniFormat);
    settings.beginGroup("Settings");
    settings.setValue("current_selected_preset", ui.comboBox_Preset_1->currentIndex());
    settings.endGroup();

    if (save_all) {
        DEBUG("Saving All Settings Presets (Defaults)");

        for (int i = 0; i < preset_list.size(); i++) {
            settings.beginGroup("Preset" + std::to_string(i));
            settings.setValue("description", preset_list.at(i).description);
            settings.setValue("width_change_selection", preset_list.at(i).width_change_selection);
            settings.setValue("width_number", preset_list.at(i).width_number);
            settings.setValue("height_change_selection", preset_list.at(i).height_change_selection);
            settings.setValue("height_number", preset_list.at(i).height_number);
            settings.setValue("keep_aspect_ratio", preset_list.at(i).keep_aspect_ratio);
            settings.setValue("resampling_filter", preset_list.at(i).resampling_filter);
            settings.setValue("rotation_degrees", preset_list.at(i).rotation_degrees);
            settings.setValue("increase_boundaries", preset_list.at(i).increase_boundaries);
            settings.setValue("flip_image", preset_list.at(i).flip_image);
            settings.setValue("format_change", preset_list.at(i).format_change);
            settings.setValue("format", preset_list.at(i).format_extension);
            settings.setValue("format_format_flag", preset_list.at(i).format_format_flag);
            settings.setValue("format_optimize", preset_list.at(i).format_optimize);
            settings.setValue("format_progressive", preset_list.at(i).format_progressive);
            settings.setValue("format_quality", preset_list.at(i).format_quality);
            settings.setValue("format_compression", preset_list.at(i).format_compression);
            settings.setValue("format_extra1", preset_list.at(i).format_extra1);
            settings.setValue("format_extra2", preset_list.at(i).format_extra2);
            settings.setValue("save_file_policy_option", preset_list.at(i).save_file_policy_option);
            settings.setValue("save_file_name_change", preset_list.at(i).save_file_name_change.c_str());
            settings.setValue("relative_save_path", preset_list.at(i).relative_save_path);
            settings.setValue("save_file_path_change", preset_list.at(i).save_file_path_change.c_str());
            settings.endGroup();
        }
    }
    else {
        DEBUG2("Update Settings Preset #", current_selected_preset);

        int save_option;
        if (ui.radioButton_Overwrite->isChecked()) {
            save_option = SaveOption::OVERWRITE;
        }
        else if (ui.radioButton_RenameOriginal->isChecked()) {
            save_option = SaveOption::RENAME_ORG;
        }
        else {
            save_option = SaveOption::NEW_NAME;
        }
        bool relative_save_path = ui.radioButton_RelativePath->isChecked();

        // Update current preset in list then save it in settings
        preset_list.at(current_selected_preset).index = current_selected_preset;
        //preset_list.at(current_selected_preset).description = ui.comboBox_Preset_1->itemText(current_selected_preset); // Updated in ChangePresetDescription()
        preset_list.at(current_selected_preset).width_change_selection = ui.comboBox_WidthMod->currentIndex();
        preset_list.at(current_selected_preset).width_number = ui.spinBox_WidthNumber->value();
        preset_list.at(current_selected_preset).height_change_selection = ui.comboBox_HeightMod->currentIndex();
        preset_list.at(current_selected_preset).height_number = ui.spinBox_HeightNumber->value();
        preset_list.at(current_selected_preset).keep_aspect_ratio = ui.checkBox_KeepAspectRatio->isChecked();
        preset_list.at(current_selected_preset).resampling_filter = ui.comboBox_Resample->currentIndex();
        preset_list.at(current_selected_preset).rotation_degrees = ui.dial_Rotation->value();
        preset_list.at(current_selected_preset).increase_boundaries = ui.checkBox_IncreaseBounds->isChecked();
        preset_list.at(current_selected_preset).flip_image = ui.checkBox_FlipImage->isChecked();
        preset_list.at(current_selected_preset).format_change = ui.groupBox_ChangeFormat->isChecked();
        preset_list.at(current_selected_preset).format_extension = ui.comboBox_ImageFormat->currentIndex();
        preset_list.at(current_selected_preset).format_format_flag = ui.comboBox_FormatFlags->currentIndex();
        preset_list.at(current_selected_preset).format_optimize = ui.checkBox_Optimize->isChecked();
        preset_list.at(current_selected_preset).format_progressive = ui.checkBox_Progressive->isChecked();
        preset_list.at(current_selected_preset).format_quality = ui.horizontalSlider_Quality->value();
        preset_list.at(current_selected_preset).format_compression = ui.spinBox_Compression->value();
        preset_list.at(current_selected_preset).format_extra1 = ui.spinBox_ExtraSetting1->value();
        preset_list.at(current_selected_preset).format_extra2 = ui.spinBox_ExtraSetting2->value();
        preset_list.at(current_selected_preset).save_file_policy_option = save_option;
        preset_list.at(current_selected_preset).save_file_name_change = ui.lineEdit_FileName->text().toStdString();
        preset_list.at(current_selected_preset).relative_save_path = relative_save_path;
        if (relative_save_path)
            preset_list.at(current_selected_preset).save_file_path_change = ui.lineEdit_RelativePath->text().toStdString();
        else
            preset_list.at(current_selected_preset).save_file_path_change = ui.lineEdit_AbsolutePath->text().toStdString();

        settings.beginGroup("Preset" + std::to_string(current_selected_preset));
        settings.setValue("description", preset_list.at(current_selected_preset).description);
        settings.setValue("width_change_selection", preset_list.at(current_selected_preset).width_change_selection);
        settings.setValue("width_number", preset_list.at(current_selected_preset).width_number);
        settings.setValue("height_change_selection", preset_list.at(current_selected_preset).height_change_selection);
        settings.setValue("height_number", preset_list.at(current_selected_preset).height_number);
        settings.setValue("keep_aspect_ratio", preset_list.at(current_selected_preset).keep_aspect_ratio);
        settings.setValue("resampling_filter", preset_list.at(current_selected_preset).resampling_filter);
        settings.setValue("rotation_degrees", preset_list.at(current_selected_preset).rotation_degrees);
        settings.setValue("increase_boundaries", preset_list.at(current_selected_preset).increase_boundaries);
        settings.setValue("flip_image", preset_list.at(current_selected_preset).flip_image);
        settings.setValue("format_change", preset_list.at(current_selected_preset).format_change);
        settings.setValue("format_extension", preset_list.at(current_selected_preset).format_extension);
        settings.setValue("format_format_flag", preset_list.at(current_selected_preset).format_format_flag);
        settings.setValue("format_optimize", preset_list.at(current_selected_preset).format_optimize);
        settings.setValue("format_progressive", preset_list.at(current_selected_preset).format_progressive);
        settings.setValue("format_quality", preset_list.at(current_selected_preset).format_quality);
        settings.setValue("format_compression", preset_list.at(current_selected_preset).format_compression);
        settings.setValue("format_extra1", preset_list.at(current_selected_preset).format_extra1);
        settings.setValue("format_extra2", preset_list.at(current_selected_preset).format_extra2);
        settings.setValue("save_file_policy_option", preset_list.at(current_selected_preset).save_file_policy_option);
        settings.setValue("save_file_name_change", preset_list.at(current_selected_preset).save_file_name_change.c_str());
        settings.setValue("relative_save_path", preset_list.at(current_selected_preset).relative_save_path);
        settings.setValue("save_file_path_change", preset_list.at(current_selected_preset).save_file_path_change.c_str());
        settings.endGroup();
    }
}

/// <summary>
/// Load a preset's data into various UI elements.
/// </summary>
/// <param name="preset">--A preset with all image edits to be made.</param>
void BatchItImage::LoadPreset(Preset preset)
{
    // TODO: load "ALL" selected preset settings into ui

    ui.comboBox_WidthMod->setCurrentIndex(preset.width_change_selection);
    ui.spinBox_WidthNumber->setValue(preset.width_number);
    ui.comboBox_HeightMod->setCurrentIndex(preset.height_change_selection);
    ui.spinBox_HeightNumber->setValue(preset.height_number);
    ui.comboBox_Resample->setCurrentIndex(preset.resampling_filter);
    ui.checkBox_KeepAspectRatio->setChecked(preset.keep_aspect_ratio);
    ui.dial_Rotation->setValue(preset.rotation_degrees);
    ui.lcdNumber_Rotation->display(preset.rotation_degrees);
    ui.checkBox_IncreaseBounds->setChecked(preset.increase_boundaries);
    ui.checkBox_FlipImage->setChecked(preset.flip_image);
    ui.groupBox_ChangeFormat->setChecked(preset.format_change);
    ui.comboBox_ImageFormat->setCurrentIndex(preset.format_extension);
    ui.comboBox_FormatFlags->setCurrentIndex(preset.format_format_flag);
    ui.checkBox_Optimize->setChecked(preset.format_optimize);
    ui.checkBox_Progressive->setChecked(preset.format_progressive);
    ui.horizontalSlider_Quality->setValue(preset.format_quality);
    ui.spinBox_Compression->setValue(preset.format_compression);
    ui.spinBox_ExtraSetting1->setValue(preset.format_extra1);
    ui.spinBox_ExtraSetting2->setValue(preset.format_extra2);
    ui.lineEdit_FileName->setText(QString::fromStdString(preset.save_file_name_change));
    int save_option = preset.save_file_policy_option;
    if (save_option == SaveOption::OVERWRITE) {
        ui.radioButton_Overwrite->setChecked(true);
    }
    else if (save_option == SaveOption::RENAME_ORG) {
        ui.radioButton_RenameOriginal->setChecked(true);
    }
    else { // save_option == NEW_NAME
        ui.radioButton_NewFileName->setChecked(true);
    }
    bool relative_save_path = preset.relative_save_path;
    ui.radioButton_RelativePath->setChecked(relative_save_path);
    ui.radioButton_AbsolutePath->setChecked(not relative_save_path);
    if (relative_save_path)
        ui.lineEdit_RelativePath->setText(QString::fromStdString(preset.save_file_path_change));
    else
        ui.lineEdit_AbsolutePath->setText(QString::fromStdString(preset.save_file_path_change));

    EnableSpecificFormatOptions(true);
}

/// <summary>
/// Load presets from settings file or get defaults if none found.
/// </summary>
void BatchItImage::LoadPresets()
{
    QSettings settings(preset_settings_file, QSettings::IniFormat);
    preset_list.clear();
    int cspi = 0;

    // Default Presets (will be used when none are found in the settings file.)
    Preset preset1;
    preset1.index = 0;
    preset1.description = "(Default) Create New 600x600 Image.";
    preset1.width_change_selection = ImageEditor::CHANGE_TO;
    preset1.width_number = 600;
    preset1.height_change_selection = ImageEditor::CHANGE_TO;
    preset1.height_number = 600;
    //preset1.keep_aspect_ratio = true;
    preset1.resampling_filter = cv::InterpolationFlags::INTER_CUBIC;
    //preset1.rotation_degrees = 0;
    //preset1.format_change = false;
    //preset1.format_extension = 0;
    //preset1.format_format_flag = 1;
    //preset1.format_optimize = false;
    //preset1.format_progressive = false;
    //preset1.format_quality = 95;
    //preset1.format_compression = 0;
    //preset1.format_extra1 = -1;
    //preset1.format_extra2 = -1;
    //preset1.save_file_policy_option = NEW_NAME;
    //preset1.save_file_name_change = "<FILE_NAME>__new";
    //preset1.relative_save_path = true;
    //preset1.save_file_path_change = "";

    Preset preset2;
    preset2.index = 1;
    preset2.description = "(Default) Resize Image 200x200 and Rename Original.";
    preset2.width_change_selection = ImageEditor::CHANGE_TO;
    preset2.width_number = 200;
    preset2.height_change_selection = ImageEditor::CHANGE_TO;
    preset2.height_number = 200;
    preset2.keep_aspect_ratio = false;
    preset2.resampling_filter = cv::InterpolationFlags::INTER_CUBIC;
    //preset2.rotation_degrees = 0;
    //preset2.format_change = false;
    //preset2.format_extension = 0;
    //preset2.format_format_flag = 1;
    //preset2.format_optimize = false;
    //preset2.format_progressive = false;
    //preset2.format_quality = 95;
    //preset2.format_compression = 0;
    //preset2.format_extra1 = -1;
    //preset2.format_extra2 = -1;
    preset2.save_file_policy_option = SaveOption::RENAME_ORG;
    preset2.save_file_name_change = "<FILE_NAME>__org";
    //preset2.relative_save_path = true;
    //preset2.save_file_path_change = "";

    if (settings.contains("Preset0")) {
        // TODO: load presets from settings to preset_list
        int i = 0;
        do {
            //settings.value("width_change_selection");
            DEBUG("Found Preset#" + std::to_string(i) + " in Settings");
            settings.beginGroup("Preset" + std::to_string(i));
            Preset preset;
            preset.index = i;
            preset.description = settings.value("description").toString();
            preset.width_change_selection = settings.value("width_change_selection").toInt();
            preset.width_number = settings.value("width_number").toInt();
            preset.height_change_selection = settings.value("height_change_selection").toInt();
            preset.height_number = settings.value("height_number").toInt();
            preset.keep_aspect_ratio = settings.value("keep_aspect_ratio").toBool();
            preset.resampling_filter = settings.value("resampling_filter").toInt();
            preset.rotation_degrees = settings.value("rotation_degrees").toInt();
            preset.increase_boundaries = settings.value("increase_boundaries").toBool();
            preset.flip_image = settings.value("flip_image").toBool();
            preset.format_change = settings.value("format_change").toBool();
            preset.format_extension = settings.value("format_extension").toInt();
            preset.format_format_flag = settings.value("format_format_flag").toInt();
            preset.format_optimize = settings.value("format_optimize").toBool();
            preset.format_progressive = settings.value("format_progressive").toBool();
            preset.format_quality = settings.value("format_quality").toInt();
            preset.format_compression = settings.value("format_compression").toInt();
            preset.format_extra1 = settings.value("format_extra1").toInt();
            preset.format_extra2 = settings.value("format_extra2").toInt();
            preset.save_file_policy_option = settings.value("save_file_policy_option").toInt();
            preset.save_file_name_change = settings.value("save_file_name_change").toString().toStdString();
            preset.relative_save_path = settings.value("relative_save_path").toBool();
            preset.save_file_path_change = settings.value("save_file_path_change").toString().toStdString();
            //preset_list.push_back({ preset });
            settings.endGroup();

        } while (settings.contains("Preset" + i));

        settings.beginGroup("Settings");
        cspi = settings.value("current_selected_preset").toInt();
        settings.endGroup();

        //qDebug() << settings.allKeys();

        // TEMP: load only defaults, keep presets as defaults between sessions.
        preset_list.push_back({ preset1 });
        preset_list.push_back({ preset2 });
    }
    else {
        DEBUG("Loading Default Presets");
        preset_list.push_back({ preset1 });
        preset_list.push_back({ preset2 });
        SavePreset(true);
    }

    // Add preset titles into all preset combo boxes.
    QComboBox* preset_comboboxes[]{ ui.comboBox_Preset_1, ui.comboBox_Preset_2, ui.comboBox_Preset_3 };
    AddPresetsToComboBox(&preset_list, preset_comboboxes, 3);
    //AddPresetsToComboBox(&preset_list, &ui.comboBox_Preset_1);
    ChangePreset(cspi);

    // Load selected preset data into ui.
    LoadPreset(preset_list.at(current_selected_preset));
}

/// <summary>
/// Open dialog allowing user to change any preset description.
/// </summary>
void BatchItImage::ChangePresetDescription()
{
    DEBUG("ChangePresetDescription");
    DialogEditPresetDesc* epc_window = new DialogEditPresetDesc(
        "Change Preset Description",
        "Change Title Description of Currently Selected Preset.",
        &preset_list,
        current_selected_preset,
        this
    );
    epc_window->exec();
    QComboBox* preset_comboboxes[]{ ui.comboBox_Preset_1, ui.comboBox_Preset_2, ui.comboBox_Preset_3 };
    AddPresetsToComboBox(&preset_list, preset_comboboxes, 3);
    SavePreset(true);
    delete epc_window;
}

/// <summary>
/// [Static] Add preset descriptions to one or more combo boxes.
/// </summary>
/// <param name="preset_list">--Pointer to the preset list.</param>
/// <param name="preset_cb">--Pointer to one or more combo boxes.</param>
/// <param name="count">--The amount of combo boxes.</param>
void BatchItImage::AddPresetsToComboBox(std::vector<Preset>* preset_list, QComboBox* preset_cb[], uint count)
{
    for (uint x = 0; x < count; x++) {
        preset_cb[x]->blockSignals(true);
        preset_cb[x]->clear();

        for (int i = 0; i < preset_list->size(); i++) {

            QString preset_text = "[Preset #" + QVariant(i + 1).toString() + "] "; // TODO: add to UI Text/Data?

            preset_cb[x]->insertItem(i,
                preset_text + preset_list->at(i).description,
                QString::fromStdString("Preset" + std::to_string(i))
            );
            preset_cb[x]->setItemData(i,
                preset_list->at(i).description,
                Qt::ToolTipRole
            );
            preset_cb[x]->setItemData(i,
                preset_list->at(i).description,
                Qt::StatusTipRole
            );
        }
        preset_cb[x]->setStatusTip(preset_cb[x]->currentData(Qt::StatusTipRole).toString());
        preset_cb[x]->setToolTip(preset_cb[x]->currentData(Qt::ToolTipRole).toString());
        preset_cb[x]->blockSignals(false);
    }
}

/// <summary>
/// Dialog asking user to save or discard any changes made to current preset before editing images.
/// </summary>
/// <returns>True if editing of images aborted by user.</returns>
bool BatchItImage::SavePresetDialog()
{
    m_window = new MessageWindow(
        "Save Current Preset",
        "Before editing any images, with the current settings, would you like to \"Save/Discard\" those settings to the currently selected preset?\nYou may also choose to \"Abort\" the editing of images.",
        QDialogButtonBox::Save | QDialogButtonBox::Discard | QDialogButtonBox::Abort,
        this
    );
    bool abort = false;
    bool* abort_p = &abort;
    connect(m_window, &MessageWindow::ButtonClicked, 
        [=](QDialogButtonBox::StandardButton button_clicked) {
            if (QDialogButtonBox::Save & button_clicked)
                SavePreset();
            if (QDialogButtonBox::Discard & button_clicked)
                m_window->close();
            else if (QDialogButtonBox::Abort & button_clicked or QDialogButtonBox::Close & button_clicked) {
                bool abort = true;
                *abort_p = abort;
            }
        }
    );
    m_window->exec();
    delete m_window;
    return abort;
}

/// <summary>
/// Start editing and saving images in file tree (in another thread).
/// </summary>
void BatchItImage::EditAndSave()
{
    DEBUG("Edit And Save...");

    // TODO: if current preset settings are not saved, ask to save them now before editing images
    // Save Current, Save New, Cancel... all preset ui elements would need to trigger a "changed and not saved" flag
    bool abort = SavePresetDialog();
    if (abort) return;
    DEBUG("...Edit And Save...");

    for (int i = 0; i < current_file_metadata_list.size(); i++) {
        DEBUG(current_file_metadata_list.at(i).to_string());

        // Create pointer to what will be the new edited image
        cv::Mat* img_p = new cv::Mat();

        // Setup the image editor with a file path and all the edits to be done.
        ImageEditor* new_ie = new ImageEditor(current_file_metadata_list.at(i).path, img_p);
        new_ie->width_modifier = ui.comboBox_WidthMod->currentIndex();
        new_ie->width = ui.spinBox_WidthNumber->value();
        new_ie->height_modifier = ui.comboBox_HeightMod->currentIndex();
        new_ie->height = ui.spinBox_HeightNumber->value();
        new_ie->keep_aspect_ratio = ui.checkBox_KeepAspectRatio->isChecked();
        new_ie->interpolation = ui.comboBox_Resample->currentData().toInt();
        new_ie->rotation_degrees = ui.dial_Rotation->value();
        new_ie->increase_boundaries = ui.checkBox_IncreaseBounds->isChecked();
        new_ie->flip_image = ui.checkBox_FlipImage->isChecked();

        // Start the image edit process on another thread
        std::future<uint> worker_thread = std::async(&ImageEditor::StartEditProcess, new_ie);

        // Add Callback function when edit finishes, send data to SaveImageFile()
        new_ie->AddExitCallback(std::bind(&BatchItImage::SaveImageFile, this, std::placeholders::_1, std::placeholders::_2, i, new_ie));

        //int image_edits_made = worker_thread.get();
        //DEBUG2("Done: ", image_edits_made);
    }
}

/// <summary>
/// Save edited image file using current preset options.
/// </summary>
/// <param name="image">--Pointer to edited image.</param>
/// <param name="image_edits_made">--Edit code representing each edit made to image.</param>
/// <param name="image_index">--Image index in current list of image to be edited.</param>
/// <param name="ie">--Pointer to the ImageEditor used to edit image and must be deleted once finished.</param>
void BatchItImage::SaveImageFile(cv::Mat* image, uint image_edits_made, int image_index, ImageEditor* ie)
{
    DEBUG4("SaveImageFile: ", image_index, ", Edit-Code: ", image_edits_made);
    
    // Save new image only if edits were made.
    if (image_edits_made) {

        cv::Mat image_edited = *image;

        /*std::string windowName = "Image Window";
        namedWindow(windowName);
        imshow(windowName, *img_p);*/

        // CREATE FILE NAME/PATH
        int save_option = preset_list.at(current_selected_preset).save_file_policy_option;
        std::filesystem::path org_file_path = std::filesystem::path(current_file_metadata_list.at(image_index).path);
        std::filesystem::path new_file_path;

        // Get the full root save path whether it's the relative or new or original absolute path.
        std::filesystem::path new_root_save_path;
        if (preset_list.at(current_selected_preset).relative_save_path) {
            new_root_save_path = std::filesystem::weakly_canonical(
                org_file_path.parent_path() / std::filesystem::path(preset_list.at(current_selected_preset).save_file_path_change)
            );
            try { // Create missing directories in path
                std::filesystem::create_directories(new_root_save_path);
            }
            catch (const std::exception& err) {
                DEBUG(err.what());
                // TODO: log and show error message to user. [Ignore][Ignore All?][Retry][Abort]
            }
        }
        else {
            std::filesystem::path absolute_save_path = std::filesystem::path(preset_list.at(current_selected_preset).save_file_path_change);
            try {
                new_root_save_path = std::filesystem::canonical(absolute_save_path);
            }
            // Note: This should never be called because absolute paths are checked before adding them, but if it errors anyways...
            catch (const std::exception& err) {
                DEBUG(err.what());
                // TODO: log and show error message to user, and maybe give option to create missing directories? [Yes][Abort]
            }
        }

        if (save_option == SaveOption::RENAME_ORG or save_option == SaveOption::NEW_NAME) {
            std::string file_name_changes = preset_list.at(current_selected_preset).save_file_name_change;

            std::string metadata_inserts[4] = {
                org_file_path.stem().string(), // File Name
                std::to_string(image_index + 1), // Counter
                std::to_string(image_edited.cols), // Width
                std::to_string(image_edited.rows) // Height
            };
            file_name_changes = CreateNewFileName(file_name_changes, metadata_inserts);

            // TODO: if file already exists ask to overwrite?
            // TODO: how to handle permissions?
            //if (std::filesystem::exists(file_path_rename))

            if (save_option == SaveOption::RENAME_ORG) {
                std::filesystem::path org_file_path_rename = org_file_path.parent_path() / file_name_changes;
                std::filesystem::rename(org_file_path, org_file_path_rename);
                // TODO: If RENAME_ORG and an absolute or a relative path that isn't empty or just "\",
                // Inform user that they are renaming original but not saving the new file with the same name/path, is this what they intended to do?
                // Inform user before starting the edit process. (No need to rename org if not intending to overwrite it, or is there?)
                new_file_path = std::filesystem::path(new_root_save_path / org_file_path.stem());
            }
            else if (save_option == SaveOption::NEW_NAME) {
                new_file_path = std::filesystem::path(new_root_save_path / file_name_changes);
                //std::filesystem::path new_file_path_rename = new_root_save_path / file_name_changes;
                //new_file_path = new_file_path_rename.generic_string();
            }
        }
        else { // save_option == SaveOption::OVERWRITE
            new_file_path = org_file_path.parent_path() / org_file_path.stem();
        }

        //DEBUG2("Save File Path: ", new_file_path.generic_string());

        // Add Extension and Get/Apply Format Specific Parameters
        std::vector<int> params;
        DEBUG2("Change Format: ", preset_list.at(current_selected_preset).format_change);
        if (ui.groupBox_ChangeFormat->isChecked()) {

            std::string extension = ui.comboBox_ImageFormat->currentData().toString().toStdString();
            new_file_path.replace_extension(extension);

            int format_flag = ui.comboBox_FormatFlags->currentData().toInt();
            DEBUG(format_flag);
            int quality = ui.horizontalSlider_Quality->value();
            bool optimize = ui.checkBox_Optimize->isChecked();
            bool progressive = ui.checkBox_Progressive->isChecked();
            int compression = ui.spinBox_Compression->value();
            int extra1 = ui.spinBox_ExtraSetting1->value();
            int extra2 = ui.spinBox_ExtraSetting2->value();
            
            ie->GetFormatParameters(&params, extension, format_flag, quality, optimize, progressive, compression, extra1, extra2);
        }
        else {
            new_file_path.replace_extension(org_file_path.extension());
        }

        // SAVE IMAGE
        bool image_saved = false;
        try {
            image_saved = imwrite(new_file_path.generic_string(), image_edited, params);
        }
        catch (const cv::Exception& err) {
            DEBUG(":Exception:");
            DEBUG_ERR(err.what());
            // TODO: log
        }
        if (image_saved) {
            DEBUG2("New edited image is successfully saved at: ", new_file_path.generic_string());
        }
        else {
            DEBUG2("Failed to save the edited image: ", new_file_path.generic_string());
        }
    }
    delete image;
    delete ie;

    // TODO: update log
    DEBUG("TODO: LOG");
}

/// <summary>
/// Build a new file name from user entered text and which metadata to insert.
/// </summary>
/// <param name="file_name_changes">--Text from the preset file rename option including which type of metadata to insert.</param>
/// <param name="metadata_inserts">--The file metadata to insert into new file name.</param>
/// <returns>--The new file name string.</returns>
std::string BatchItImage::CreateNewFileName(std::string file_name_changes, std::string metadata_inserts[4])
{
    std::string metadata_selections[4] = { ADD_FILE_NAME, ADD_COUNTER, ADD_WIDTH, ADD_HEIGHT };
    for (int i = 0; i < 4; i++) {
        file_name_changes = ReplaceAll(file_name_changes, metadata_selections[i], metadata_inserts[i]);
    }
    // Clean up any illegal characters that got through. Only these should be possible: <>.
    file_name_changes = ReplaceAll(file_name_changes, "<", "");
    file_name_changes = ReplaceAll(file_name_changes, ">", "");
    DEBUG(file_name_changes);
    // TODO: I could retest for metadata_selections in case an illegal character was placed into a metadata by mistake.
    // Or just let the user mistake happen? "<FIL<E_NAME>"

    return file_name_changes;
}

/// <summary>
/// Check for any directories and add any files found to the list before sending it too BuildFileMetadataList(). 
/// </summary>
/// <param name="file_list">--List of file and/or directory paths.</param>
void BatchItImage::AddNewFiles(QStringList file_list)
{
    // Create new updated file list from files found in directories.
    QStringList updated_file_list;
    for (const auto& file : file_list)
    {
        std::filesystem::path file_path = std::filesystem::u8path(file.toStdString());
        // TODO: handle windows shortcut files. https://stackoverflow.com/questions/22986845/windows-read-the-target-of-shortcut-file-in-c
        if (std::filesystem::is_symlink(file_path)) {
            file_path = std::filesystem::read_symlink(file_path);
            DEBUG2("symlink target: ", file_path);
        }
        if (std::filesystem::is_directory(file_path)) {
            QStringList files_from_dir;
            if (ui.checkBox_SearchSubDirs->isChecked())
                files_from_dir = IterateDirectory(std::filesystem::recursive_directory_iterator(file_path));
            else
                files_from_dir = IterateDirectory(std::filesystem::directory_iterator(file_path));
            updated_file_list.append(files_from_dir);
        }
        else {
            updated_file_list.append(file);
        }
    }
    BuildFileMetadataList(updated_file_list);
    //DebugPrintList(current_file_metadata_list, "current_file_metadata_list");
}

/// <summary>
/// Iterate over a directory and append every supported image file found to a list.
/// </summary>
/// <typeparam name="DirectoryIter"></typeparam>
/// <param name="iterator">--"directory_iterator" or "recursive_directory_iterator"</param>
/// <returns>QStringList of file paths.</returns>
template<typename DirectoryIter>QStringList BatchItImage::IterateDirectory(DirectoryIter iterator)
{
    QStringList updated_file_list;
    for (auto const& dir_entry : iterator) {
        if (std::filesystem::is_regular_file(dir_entry)) {
            DEBUG2("File: ", dir_entry);
            std::string file_ext = dir_entry.path().extension().string();
            for (const auto& img_format : image_formats) {
                std::string ext = std::get<std::string>(img_format.data);
                if (ext == file_ext) {
                    QString file = QString::fromStdString(dir_entry.path().string());
                    updated_file_list.append(file);
                    break;
                }
            }
            /*if (std::find(supported_images.begin(), supported_images.end(), file_ext) != supported_images.end()) {
                QString file = QString::fromStdString(dir_entry.path().string());
                updated_file_list.append(file);
            }
            else {
                DEBUG4("-> ", file_ext, " files not supported: ", dir_entry);
            }*/
        }
        else {
            DEBUG2("Not a file: ", dir_entry);
        }
    }
    return updated_file_list;
}

/// <summary>
/// Build a list of files with it's metadata each on another thread. Results will be sent to HandleFileMetadata().
/// </summary>
/// <param name="file_list">--List of file paths.</param>
void BatchItImage::BuildFileMetadataList(const QStringList file_list)
{
    std::string file_path_str;
    int load_order = last_load_count; // Get highest load_order in all file lists. 
    int file_count = file_list.size();
    InitiateProgressBar(file_count, 3.0f);

    //DEBUG2("file_list (count): ", file_list.count());

    for (const auto& file : file_list) {

        file_path_str = file.toStdString();
        load_order += 1;

        if (IsFileInList(file_path_str, current_file_metadata_list) > -1) {
            UpdateProgressBar(function_ResizeFileTreeColumns, 3.0f);
        }
        else {
            // TODO: Which is faster?
            bool use_qthread = true;
            if (use_qthread) {
                // Start a new thread to build a file's metadata. Results will be sent to HandleFileMetadata(). https://wiki.qt.io/QThreads_general_usage
                FileMetadataWorker* new_fm_worker = new FileMetadataWorker(file_path_str, load_order, this);
                QThread* thread = new QThread();
                connect(new_fm_worker, &FileMetadataWorker::FileMetadataReady, new_fm_worker, &FileMetadataWorker::deleteLater);
                connect(new_fm_worker, &FileMetadataWorker::FileMetadataReady, thread, &QThread::quit); // (thread, &QThread::finished,...) could maybe work too
                connect(thread, SIGNAL(started()), new_fm_worker, SLOT(GetFileMetadata()));
                //connect(new_fm_worker, SIGNAL(FileMetadataReady(std::vector<struct FileMetadata>)), this, SLOT(HandleFileMetadata(std::vector<struct FileMetadata>)));
                connect(new_fm_worker, SIGNAL(FileMetadataReady(FileMetadata*)), this, SLOT(HandleFileMetadata(FileMetadata*)));
                new_fm_worker->moveToThread(thread);
                thread->start();
                DEBUG("- Thread Worker Created -");
            }
            else {
                // TODO: how to move back to main thread after callback? There's more work to be done after callback, and I can't use this until I figure that out.
                //MetadataBuilder* mb = new MetadataBuilder(file_path_str);
                // Start the process on another thread and add a callback function when finished.
                //std::future<void> worker_thread = std::async(&MetadataBuilder::GetFileMetadata, mb);
                //mb->AddExitCallback(std::bind(&BatchItImage::HandleFileMetadata, this, std::placeholders::_1));
            }
            UpdateProgressBar();
        }
    }
}

/// <summary>
/// A callback function that handles a file's metedata after it is made by adding it to the current list and then file tree.
/// </summary>
/// <param name="file_metadata">--A pointer to the metadata.</param>
void BatchItImage::HandleFileMetadata(FileMetadata* file_metadata)
{
    DEBUG2("HandleFileMetadata", file_metadata->to_string());

    if (file_metadata->load_order) {
        int item_count = ui.treeWidget_FileInfo->topLevelItemCount();
        //DEBUG(item_count);

        // If a file is deleted and then added back, remove it from deleted list.
        const auto i = IsFileInList(file_metadata->path, deleted_file_metadata_list);
        if (i > -1) {
            deleted_file_metadata_list.erase(deleted_file_metadata_list.begin() + i);
        }
        last_load_count = file_metadata->load_order;
        current_file_metadata_list.push_back(*file_metadata);
        LoadFileIntoTree(item_count);
    }
    else {
        UpdateProgressBar(function_ResizeFileTreeColumns, 2.0f);
        DEBUG("----> Bad/Non Image File");

        // TODO: return bad file path? then add to vector of bad files, show m_window for each or ignore all
        if (not m_window_shown) {
            m_window = new MessageWindow(
                "Warning",
                "Unsupported or non-image files were not added to list.",
                QDialogButtonBox::Ignore,
                this
            );
            connect(m_window, &MessageWindow::ButtonClicked,
                [this](QDialogButtonBox::StandardButton button) {
                    DEBUG2("Button: ", button);
                    m_window_shown = false;
                    delete m_window;
                });
            m_window->show();
            m_window_shown = true;
        }
    }

    if (ui.treeWidget_FileInfo->topLevelItemCount() > 0) {
        ToggleFileTreeContextMenuItems(true);
    }

    delete file_metadata;
    //DEBUG(current_file_metadata_list.back().to_string());
    //DebugPrintList(current_file_metadata_list, "current_file_metadata_list");
    //DebugPrintList(deleted_file_metadata_list, "deleted_file_metadata_list");
}

/// <summary>
/// Insert a file into the tree widget from the list of file metadata.
/// </summary>
/// <param name="file_index">--An index from "current_file_metadata_list"</param>
/// <param name="sorted_column">--The column index that was last sorted. Default value is -1/none.</param>
void BatchItImage::LoadFileIntoTree(int file_index, int sorted_column)
{
    UpdateProgressBar();

    // TODO: Styles?
    //QTreeWidget::item{
    //    padding: 0px 25px 10px 22px;
    //}
    
    FileMetadata file = current_file_metadata_list.at(file_index);
    std::filesystem::path file_path = std::filesystem::u8path(file.path);

    // Get File Metadata:
    struct stat t_stat;
    stat(file.path.data(), &t_stat);

    // Formatted File Size:
    size_t file_size = t_stat.st_size;
    std::string file_size_str = BytesToFileSizeString(file_size);

    // Formatted File Creation and Last Modified Dates: https://en.cppreference.com/w/c/chrono
    char date_created[50];
    char date_modified[50];
    struct tm* file_ctime = localtime(&t_stat.st_ctime);
    strftime(date_created, sizeof(date_created), "%x %I:%M%p", file_ctime);
    struct tm* file_mtime = localtime(&t_stat.st_mtime);
    strftime(date_modified, sizeof(date_modified), "%x %I:%M%p", file_mtime);
    
    QTreeWidgetItem* new_item = new QTreeWidgetItem(ui.treeWidget_FileInfo);
    
    QCheckBox* file_selected_check_box = new QCheckBox(ui.treeWidget_FileInfo);
    file_selected_check_box->setText("");
    file_selected_check_box->setChecked(file.selected);
    file_selected_check_box->setStatusTip(QString::fromStdString(file_tree_other_text[FileColumn::FILE_LOAD_ORDER] + std::to_string(file.load_order)));
    
    connect(file_selected_check_box, SIGNAL(toggled(bool)), this, SLOT(FileSelectionChange(bool)));

    ui.treeWidget_FileInfo->setItemWidget(new_item, FileColumn::FILE_SELECTED, file_selected_check_box);
    new_item->setToolTip(FileColumn::FILE_SELECTED, QString::fromStdString(file_tree_other_text[FileColumn::FILE_LOAD_ORDER] + std::to_string(file.load_order)));
    //new_item->setData(FileColumn::FILE_SELECTED, Qt::ToolTipRole, QVariant(file.load_order));
    //new_item->setData(FileColumn::FILE_SELECTED, Qt::StatusTipRole, QVariant(file.load_order)); // Won't show for widget column
    new_item->setText(FileColumn::FILE_NAME, QString::fromStdString(file_path.filename().string()));
    new_item->setStatusTip(FileColumn::FILE_NAME, QString::fromStdString(file_tree_other_text[FileColumn::FILE_NAME] + file.path));
    new_item->setToolTip(FileColumn::FILE_NAME, QString::fromStdString(file_tree_other_text[FileColumn::FILE_NAME] + file.path));
    new_item->setText(FileColumn::IMAGE_DIMENSIONS, QString::fromStdString(std::to_string(file.width) + " x " + std::to_string(file.height)));
    new_item->setTextAlignment(FileColumn::IMAGE_DIMENSIONS, Qt::AlignCenter);
    new_item->setStatusTip(FileColumn::IMAGE_DIMENSIONS, QString::fromStdString(file_tree_other_text[FileColumn::IMAGE_DIMENSIONS] + std::to_string(file.width) + " x " + std::to_string(file.height)));
    //new_item->setToolTip(FileColumn::IMAGE_DIMENSIONS, QString::fromStdString("Image Width: " + std::to_string(file.width) + ",   Image Height: " + std::to_string(file.height)));
    new_item->setToolTip(FileColumn::IMAGE_DIMENSIONS, QString::fromStdString(file_tree_other_text[FileColumn::IMAGE_DIMENSIONS] + std::to_string(file.width) + " x " + std::to_string(file.height)));
    new_item->setText(FileColumn::FILE_SIZE, QString::fromStdString(file_size_str));
    new_item->setTextAlignment(FileColumn::FILE_SIZE, Qt::AlignRight);
    new_item->setStatusTip(FileColumn::FILE_SIZE, QString::fromStdString(file_tree_other_text[FileColumn::FILE_SIZE] + file_size_str));
    new_item->setToolTip(FileColumn::FILE_SIZE, QString::fromStdString(file_tree_other_text[FileColumn::FILE_SIZE] + file_size_str));
    new_item->setText(FileColumn::DATE_CREATED, QString::fromStdString(date_created));
    new_item->setStatusTip(FileColumn::DATE_CREATED, QString::fromStdString(file_tree_other_text[FileColumn::DATE_CREATED]).append(date_created));
    new_item->setToolTip(FileColumn::DATE_CREATED, QString::fromStdString(file_tree_other_text[FileColumn::DATE_CREATED]).append(date_created));
    new_item->setText(FileColumn::DATE_MODIFIED, QString::fromStdString(date_modified));
    new_item->setStatusTip(FileColumn::DATE_MODIFIED, QString::fromStdString(file_tree_other_text[FileColumn::DATE_MODIFIED]).append(date_modified));
    new_item->setToolTip(FileColumn::DATE_MODIFIED, QString::fromStdString(file_tree_other_text[FileColumn::DATE_MODIFIED]).append(date_modified));
    
    new_item->setFont(FileColumn::FILE_SELECTED, *font_mono);
    new_item->setFont(FileColumn::FILE_NAME, *font_mono);
    new_item->setFont(FileColumn::IMAGE_DIMENSIONS, *font_mono);
    new_item->setFont(FileColumn::FILE_SIZE, *font_mono);
    new_item->setFont(FileColumn::DATE_CREATED, *font_mono);
    new_item->setFont(FileColumn::DATE_MODIFIED, *font_mono);
    if (sorted_column > -1 and sorted_column < FileColumn::COUNT)
        new_item->setFont(sorted_column, *font_mono_bold);

    QTreeWidgetItem* item_file_path = new QTreeWidgetItem(new_item);
    item_file_path->setText(0, QString::fromStdString(file_path.string()));
    item_file_path->setFirstColumnSpanned(true);

    new_item->addChild(item_file_path);
    ui.treeWidget_FileInfo->addTopLevelItem(new_item);

    UpdateProgressBar(function_ResizeFileTreeColumns);
}

/// <summary>
/// Get the top level index of the current row highlighted in the file tree.
/// </summary>
/// <returns>Only the index of the top level row is returned.</returns>
int BatchItImage::GetCurrentFileTreeRow()
{
    int current_file_tree_row = ui.treeWidget_FileInfo->currentIndex().row();
    int current_file_tree_p_row = ui.treeWidget_FileInfo->currentIndex().parent().row();
    if (current_file_tree_p_row > -1)
        current_file_tree_row = current_file_tree_p_row;
    return current_file_tree_row;
}

/// <summary>
/// Slot event called when file tree check box changed.
/// </summary>
/// <param name="checked">--Check box toggle.</param>
void BatchItImage::FileSelectionChange(bool checked)
{
    int current_file_tree_row = ui.treeWidget_FileInfo->currentIndex().row();
    DEBUG4("Row: ", current_file_tree_row, " Checked: ", checked);
    current_file_metadata_list.at(current_file_tree_row).selected = checked;
}

/// <summary>
/// Resize all columns to fit content in file tree.
/// </summary>
void BatchItImage::ResizeFileTreeColumns()
{
    DEBUG("ResizeFileTreeColumns");
    for (int i = 0; i < FileColumn::COUNT; i++) {
        ui.treeWidget_FileInfo->resizeColumnToContents(i);
        //ui.treeWidget_FileInfo->header()->ResizeToContents;
    }
}

/// <summary>
/// Sort a column in the file tree widget by sorting the list of file metadata then reinserting it back into the file tree.
/// </summary>
/// <param name="index">--The selected column index.</param>
void BatchItImage::SortFileTreeByColumn(int index)
{
    DEBUG4("Column Clicked", index, ", Sort Order: ", current_file_sort_order);
    InitiateProgressBar(ui.treeWidget_FileInfo->topLevelItemCount(), 2.0f);

    Qt::SortOrder qsort_indicator = Qt::SortOrder::AscendingOrder;
    
    // Get sort order
    if (current_file_column_sorted != index) {
        current_file_column_sorted = index;
        current_file_sort_order = SortOrder::ASCENDING1;
    }
    else {
        current_file_sort_order = (current_file_sort_order < SortOrder::DESCENDING2) ? ++current_file_sort_order : SortOrder::ASCENDING1;
        qsort_indicator = (fmod(current_file_sort_order, 2) == 0) ? Qt::SortOrder::AscendingOrder : Qt::SortOrder::DescendingOrder;
    }
    
    // Sort list of file metadata based on column clicked.
    if (current_file_column_sorted == FileColumn::FILE_LOAD_ORDER and
        (current_file_sort_order == SortOrder::ASCENDING1 or current_file_sort_order == SortOrder::ASCENDING2)) {
        std::sort(begin(current_file_metadata_list), end(current_file_metadata_list), [](auto const& left, auto const& right) {
                return left.load_order < right.load_order;
            });
    }
    else if (current_file_column_sorted == FileColumn::FILE_LOAD_ORDER and
        (current_file_sort_order == SortOrder::DESCENDING1 or current_file_sort_order == SortOrder::DESCENDING2)) {
        std::sort(begin(current_file_metadata_list), end(current_file_metadata_list), [](auto const& left, auto const& right) {
                return left.load_order > right.load_order;
            });
    }
    else if (current_file_column_sorted == FileColumn::FILE_NAME and
        (current_file_sort_order == SortOrder::ASCENDING1 or current_file_sort_order == SortOrder::ASCENDING2)) {
        std::sort(begin(current_file_metadata_list), end(current_file_metadata_list), [](auto const& left, auto const& right) {
                return left.path < right.path;
            });
    }
    else if (current_file_column_sorted == FileColumn::FILE_NAME and
        (current_file_sort_order == SortOrder::DESCENDING1 or current_file_sort_order == SortOrder::DESCENDING2)) {
        std::sort(begin(current_file_metadata_list), end(current_file_metadata_list), [](auto const& left, auto const& right) {
                return left.path > right.path;
            });
    }
    else if (current_file_column_sorted == FileColumn::IMAGE_DIMENSIONS and (current_file_sort_order == SortOrder::ASCENDING1)) {
        std::sort(begin(current_file_metadata_list), end(current_file_metadata_list), [](auto const& left, auto const& right) {
                return left.width < right.width;
            });
    }
    else if (current_file_column_sorted == FileColumn::IMAGE_DIMENSIONS and (current_file_sort_order == SortOrder::DESCENDING1)) {
        std::sort(begin(current_file_metadata_list), end(current_file_metadata_list), [](auto const& left, auto const& right) {
                return left.width > right.width;
            });
    }
    else if (current_file_column_sorted == FileColumn::IMAGE_DIMENSIONS and
        (current_file_sort_order == SortOrder::ASCENDING2)) {
        std::sort(begin(current_file_metadata_list), end(current_file_metadata_list), [](auto const& left, auto const& right) {
                return left.height < right.height;
            });
    }
    else if (current_file_column_sorted == FileColumn::IMAGE_DIMENSIONS and
        (current_file_sort_order == SortOrder::DESCENDING2)) {
        std::sort(begin(current_file_metadata_list), end(current_file_metadata_list), [](auto const& left, auto const& right) {
                return left.height > right.height;
            });
    }
    else if (current_file_column_sorted == FileColumn::FILE_SIZE and
        (current_file_sort_order == SortOrder::ASCENDING1 or current_file_sort_order == SortOrder::ASCENDING2)) {
        std::sort(begin(current_file_metadata_list), end(current_file_metadata_list), [](auto const& left, auto const& right) {
                return left.size < right.size;
            });
    }
    else if (current_file_column_sorted == FileColumn::FILE_SIZE and
        (current_file_sort_order == SortOrder::DESCENDING1 or current_file_sort_order == SortOrder::DESCENDING2)) {
        std::sort(begin(current_file_metadata_list), end(current_file_metadata_list), [](auto const& left, auto const& right) {
                return left.size > right.size;
            });
    }
    else if (current_file_column_sorted == FileColumn::DATE_CREATED and
        (current_file_sort_order == SortOrder::ASCENDING1 or current_file_sort_order == SortOrder::ASCENDING2)) {
        std::sort(begin(current_file_metadata_list), end(current_file_metadata_list), [](auto const& left, auto const& right) {
                return left.date_created < right.date_created;
            });
    }
    else if (current_file_column_sorted == FileColumn::DATE_CREATED and
        (current_file_sort_order == SortOrder::DESCENDING1 or current_file_sort_order == SortOrder::DESCENDING2)) {
        std::sort(begin(current_file_metadata_list), end(current_file_metadata_list), [](auto const& left, auto const& right) {
                return left.date_created > right.date_created;
            });
    }
    else if (current_file_column_sorted == FileColumn::DATE_MODIFIED and
        (current_file_sort_order == SortOrder::ASCENDING1 or current_file_sort_order == SortOrder::ASCENDING2)) {
        std::sort(begin(current_file_metadata_list), end(current_file_metadata_list), [](auto const& left, auto const& right) {
                return left.date_modified < right.date_modified;
            });
    }
    else if (current_file_column_sorted == FileColumn::DATE_MODIFIED and
        (current_file_sort_order == SortOrder::DESCENDING1 or current_file_sort_order == SortOrder::DESCENDING2)) {
        std::sort(begin(current_file_metadata_list), end(current_file_metadata_list), [](auto const& left, auto const& right) {
                return left.date_modified > right.date_modified;
            });
    }

    // Clear file tree and add back sorted files
    ui.treeWidget_FileInfo->clear();
    for (int i = 0; i < current_file_metadata_list.size(); i++) {
        LoadFileIntoTree(i, current_file_column_sorted);
    }

    ui.treeWidget_FileInfo->header()->setSortIndicator(current_file_column_sorted, qsort_indicator);
    ui.treeWidget_FileInfo->header()->setSortIndicatorShown(true);
}

/// <summary>
/// Convert bytes into a formatted file size string -> KB -> MB -> GB -> TB.
/// </summary>
/// <param name="bytes">--File size in bytes.</param>
/// <returns>A Formatted String - Example: " 1.37 MB "</returns>
std::string BatchItImage::BytesToFileSizeString(std::uintmax_t bytes)
{
    std::string file_size_str;
    unsigned int kb = 1024;
    unsigned int mb = 1048576;
    unsigned int gb = 1073741824;
    unsigned long long tb = 1099511627776;

    auto removeTrailingZeros = [&](std::string suffix = "")
        {
            file_size_str.erase(file_size_str.find_last_not_of('0') + 1, std::string::npos);
            file_size_str.erase(file_size_str.find_last_not_of('.') + 1, std::string::npos);
            //size_t last = file_size_str.find_last_not_of("0");
            //file_size_str = file_size_str.substr(0, (last + 1));
            file_size_str += suffix;
        };

    if (bytes > tb) {
        file_size_str = std::to_string(round(static_cast<double>(bytes) / tb * 100) / 100);
        removeTrailingZeros(" TB ");
    }
    else if (bytes > gb) {
        file_size_str = std::to_string(round(static_cast<double>(bytes) / gb * 100) / 100);
        removeTrailingZeros(" GB ");
    }
    else if (bytes > mb) {
        file_size_str = std::to_string(round(static_cast<double>(bytes) / mb * 100) / 100);
        removeTrailingZeros(" MB ");
    }
    else if (bytes > kb) {
        file_size_str = std::to_string(round(static_cast<double>(bytes) / kb * 100) / 100);
        removeTrailingZeros(" KB ");
    }
    else {
        file_size_str = std::to_string(bytes) + " B. ";
    }

    return " " + file_size_str;
}

/// <summary>
/// Show open file dialog allowing user to load one or more image files.
/// </summary>
void BatchItImage::LoadImageFiles()
{
    QStringList files = QFileDialog::getOpenFileNames(
        this,
        "Select one or more image files to load",
        qdefault_path, // TODO: Default config setting, "recent"
        supported_image_extensions_dialog_str
    );
    if (not files.empty()) {
        AddNewFiles(files);
    }
}

/// <summary>
/// Overridden protected function that watches an objects events.
/// </summary>
/// <param name="watched">--Widget</param>
/// <param name="event"></param>
/// <returns>If true the event has been intercepted, no pass-through.</returns>
bool BatchItImage::eventFilter(QObject* watched, QEvent* event) 
{
    //DEBUG4("Object Watched: ", watched->objectName().toStdString(), ", eventFilter'd: ", event->type());
    if (event->type() == QEvent::ContextMenu) {
        DEBUG("ContextMenu Called");
    }
    if (event->type() == QEvent::ChildPolished) { // Used in-place of "no ActionsContextMenu event"
        DEBUG("ActionsContextMenu Called?");
        int current_file_tree_row = ui.treeWidget_FileInfo->currentIndex().row();
        if (current_file_tree_row > -1) {
            if (current_file_metadata_list.at(current_file_tree_row).selected)
                action_select->setText("Unselect Image");
            else
                action_select->setText("Select Image");
        }
    }
    // Pass the event on to the parent class.
    return QWidget::eventFilter(watched, event);
}


// Unused
void BatchItImage::actionEvent(QActionEvent* event)
{
    DEBUG2("actionEvent: ", event->type());
    QWidget::actionEvent(event);
}

/// <summary>
/// Overridden protected function that intercepts file urls dropped.
/// </summary>
/// <param name="event"></param>
void BatchItImage::dropEvent(QDropEvent* event)
{
    //qDebug() << "dropEvent" << event;
    //event->acceptProposedAction();

    if (event->mimeData()->hasUrls()) {
        QStringList file_list;
        QList<QUrl> urls;
        QList<QUrl>::Iterator it;
        urls = event->mimeData()->urls();
        for (it = urls.begin(); it != urls.end(); ++it) {
            // TODO: check if file is of a supported image format/extension or just let opencv detect image later?
            //DEBUG2("File Dropped (local): ", it->toLocalFile().toStdString());
            //DEBUG2("File Dropped (path): ", it->path().toStdString());
            //DEBUG2("File Dropped (display): ", it->toDisplayString().toStdString());
            file_list.append(it->toLocalFile());
            //file_list.append(it->toDisplayString());
        }
        AddNewFiles(file_list);
    }
}
void BatchItImage::dragEnterEvent(QDragEnterEvent* event)
{
    event->acceptProposedAction();
}
void BatchItImage::dragMoveEvent(QDragMoveEvent* event)
{
    event->acceptProposedAction();
}

/// <summary>
/// Overridden protected function that can intercept keyboard key presses.
/// </summary>
/// <param name="event"></param>
void BatchItImage::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Delete) {
        DEBUG("Delete Key Pressed");
        DeleteConfirmationPopup();
    }
}

/// <summary>
/// Confirmation popup asking user how to handle the deleting of files in file tree.
/// </summary>
/// <param name="clear_all">--If true will clear file list entirely, else deletes are selective.</param>
void BatchItImage::DeleteConfirmationPopup(bool clear_all)
{
    if (ui.treeWidget_FileInfo->hasFocus()) {
        int current_file_tree_row = GetCurrentFileTreeRow();
        if (current_file_tree_row > -1) {
            
            if (clear_all) {
                m_window = new MessageWindow(
                    "Clear List?",
                    "The entire list of image files is about to be cleared, is this OK?",
                    QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                    this
                );
            }
            else {
                m_window = new MessageWindow(
                    "OK To Delete?",
                    "Delete currently highlighted image file -or- all image files selected/checked?",
                    QDialogButtonBox::Yes | QDialogButtonBox::YesToAll | QDialogButtonBox::Cancel,
                    this
                );
            }
            connect(m_window, SIGNAL(ButtonClicked(QDialogButtonBox::StandardButton)), this, SLOT(RemoveFileFromTree(QDialogButtonBox::StandardButton)));
            m_window->setModal(true); // Block parent window inputs. exec() = auto modal.
            //DEBUG(m_window->exec()); //QDialog::Accepted ?
            m_window->show();
        }
    }
}

/// <summary>
/// Check if file path is already in a file list.
/// </summary>
/// <param name="path">--File path</param>
/// <param name="list">--List of file metadata</param>
/// <returns>Index pointing to where the path was found in list.</returns>
int BatchItImage::IsFileInList(std::string path, std::vector<FileMetadata> list)
{
    const auto it = std::find_if(list.begin(), list.end(),
        [path](const FileMetadata& a) { return a.path == path; });
    if (it != list.end()) {
        return std::distance(list.begin(), it);
    }
    return -1;
}
/// <summary>
/// Check if file path is already in a file list.
/// </summary>
/// <param name="path">--File path</param>
/// <param name="list">--List of file metadata</param>
/// <param name="search_range">--A range to search, {start, end}.</param>
/// <returns>Index pointing to where the path was found in list.</returns>
int BatchItImage::IsFileInList(std::string path, std::vector<FileMetadata> list, const size_t search_range[2])
{
    const auto it = std::find_if(list.begin() + search_range[0], list.begin() + search_range[1],
        [path](const FileMetadata& a) { return a.path == path; });
    if (it != list.begin() + search_range[1]) {
        return std::distance(list.begin(), it);
    }
    return -1;
}
/// <summary>
/// Check if file path is already in a file list.
/// </summary>
/// <param name="path">--File path</param>
/// <param name="list">--List of file metadata</param>
/// <returns>Iterator where the path was found in list.</returns>
std::vector<FileMetadata>::iterator BatchItImage::IsFileInListIterator(std::string path, std::vector<FileMetadata> list)
{
    const auto it = std::find_if(list.begin(), list.end(),
        [path](const FileMetadata& a) { return a.path == path; });
    return it;
}

/// <summary>
/// Remove one or more file rows from file tree.
/// </summary>
/// <param name="button_clicked">--Based on a clicked button's role delete one, mulitple, or all rows.</param>
void BatchItImage::RemoveFileFromTree(const QDialogButtonBox::StandardButton& button_clicked)
{
    int current_file_tree_row = GetCurrentFileTreeRow(); // this should never be -1, if it is something is changing it between this and DeleteConfirmationPopup
    DEBUG4("RemoveFileFromTree At Index: ", current_file_tree_row, ", Button: ", button_clicked);
    delete m_window;
    int dupe_index = -1;
    std::vector<size_t> d_indexes;

    if (QDialogButtonBox::Yes & button_clicked) { // Delete Highlighted Row
        DEBUG("Yes Button");
        //dupe_index = IsFileInList(current_file_metadata_list.at(current_file_tree_row).path, deleted_file_metadata_list);
        //if (dupe_index == -1) {
            deleted_file_metadata_list.push_back(current_file_metadata_list.at(current_file_tree_row));
        //}
        current_file_metadata_list.erase(current_file_metadata_list.begin() + current_file_tree_row);
        DeleteTreeItemObjects(ui.treeWidget_FileInfo, current_file_tree_row);
    }
    else if (QDialogButtonBox::YesToAll & button_clicked) { // Delete All Selected
        DEBUG("Yes To All Button");
        const size_t org_d_list_size_range[2] = { 0, deleted_file_metadata_list.size() };
        for (std::vector<FileMetadata>::reverse_iterator it = current_file_metadata_list.rbegin(); it != current_file_metadata_list.rend(); ++it) {
            if (it->selected) {
                //dupe_index = IsFileInList(it->path, deleted_file_metadata_list, org_d_list_size_range);
                //if (dupe_index == -1) {
                    deleted_file_metadata_list.push_back(*it);
                //}
                size_t i = std::distance(current_file_metadata_list.begin(), it.base()) - 1;
                //current_file_metadata_list.erase(it.base()); // Can't get this to work, looping d_indexes afterwards instead
                d_indexes.push_back(i);
                DeleteTreeItemObjects(ui.treeWidget_FileInfo, i);
            }
        }
        for (const size_t& i : d_indexes) {
            current_file_metadata_list.erase(current_file_metadata_list.begin() + i);
        }
    }
    else if (QDialogButtonBox::Ok & button_clicked) { // Delete All
        DEBUG("Ok Button");

        // TODO: maybe I should just remove items from deleted_file_metadata_list if re-added to file-tree / current_file_metadata_list.
        // Done, so none of these dupe checks should be needed. Would moving this to add file events make loading even longer (in some cases)? better here?
        const size_t org_d_list_size_range[2] = { 0, deleted_file_metadata_list.size() };
        for (std::vector<FileMetadata>::reverse_iterator it = current_file_metadata_list.rbegin(); it != current_file_metadata_list.rend(); ++it) {
            //dupe_index = IsFileInList(it->path, deleted_file_metadata_list, org_d_list_size_range);
            //if (dupe_index == -1) {
                deleted_file_metadata_list.push_back(*it);
            //}
        }
        DeleteTreeItemObjects(ui.treeWidget_FileInfo);
        current_file_metadata_list.clear();
        //ui.treeWidget_FileInfo->clear();
    }
    else if (QDialogButtonBox::No & button_clicked) {
        DEBUG("No Button");
    }

    // No files, nothing to delete.
    if (ui.treeWidget_FileInfo->topLevelItemCount() == 0) {
        ToggleFileTreeContextMenuItems(false);
    }

    //DebugPrintList(current_file_metadata_list, "current_file_metadata_list");
    //DebugPrintList(deleted_file_metadata_list, "deleted_file_metadata_list");
}

/// <summary>
/// Delete all widget objects in a widget tree's row or every row if "row_index" is not set.
/// </summary>
/// <param name="tree">--The tree widget</param>
/// <param name="row_index">--The row to delete. Default all rows.</param>
void BatchItImage::DeleteTreeItemObjects(QTreeWidget* tree, int row_index)
{
    if (row_index > -1) {
        DEBUG2("Deleting all object in row: ", row_index);
        QList<QTreeWidgetItem*> child_widget_list = tree->topLevelItem(row_index)->takeChildren();
        qDeleteAll(child_widget_list);
        delete tree->takeTopLevelItem(row_index);
    } 
    else { // Clear/Delete All
        while (auto item = tree->takeTopLevelItem(0)) {
            DEBUG2("Deleting all objects in first row: current-count-> ", tree->topLevelItemCount());
            QList<QTreeWidgetItem*> child_widget_list = item->takeChildren();
            qDeleteAll(child_widget_list);
            /*for (auto& child_widget : child_widget_list) {
                DEBUG2("Deleting a Widget in TopLevelItem: current-count->", child_widget_list.count());
                delete child_widget;
            }*/
            delete item;
        }
        tree->clear();
    }
}

/// <summary>
/// Show progress bar in UI and set the amount of ticks to maximum load. This must be called before updating progress bar.
/// </summary>
/// <param name="max_ticks">--The amount of ticks to maximum load. Must be called before starting update progress.</param>
void BatchItImage::InitiateProgressBar(int max_ticks) {
    InitiateProgressBar(max_ticks, 1.0f);
}

/// <summary>
/// Show progress bar in UI and set the amount of ticks to maximum load. This must be called before updating progress bar.
/// </summary>
/// <param name="max_ticks">--The amount of ticks to maximum load.Must be called before starting update progress.</param>
/// <param name="multiplier">--Multiply the ticks when there are multiple UpdateProgressBar calls and some may be skipped. [Default: 1]</param>
void BatchItImage::InitiateProgressBar(int max_ticks, float multiplier) {
    current_load_number = 0.0f;
    load_interval = float(ui.progressBar->maximum()) / float(max_ticks) / multiplier;
    ui.progressBar->setVisible(true);
    ui.progressBar->setValue(ui.progressBar->minimum());
}

/// <summary>
/// Update progress bar in UI, hiding it when maximum load hit. InitiateProgressBar() must have been previously called before updating can proceed.
/// </summary>
void BatchItImage::UpdateProgressBar() {
    UpdateProgressBar(0, 1.0f);
}

/// <summary>
/// Update progress bar in UI, hiding it when maximum load hit. InitiateProgressBar() must have been previously called before updating can proceed.
/// </summary>
/// <param name="multiplier">--Multiply the ticks when there are multiple UpdateProgressBar calls and some may be skipped. [Default: 1]</param>
void BatchItImage::UpdateProgressBar(float multiplier) {
    UpdateProgressBar(0, multiplier);
}

/// <summary>
/// Update progress bar in UI, hiding it when maximum load hit. InitiateProgressBar() must have been previously called before updating can proceed.
/// Only use this overloaded function when adding a function to call when maximum load hit.
/// </summary>
/// <param name="func">--A function to call when maximum load hit. [Default: Null] - [Example: std::bind(class::function, this)]</param>
/// <param name="multiplier">--Multiply the ticks when there are multiple UpdateProgressBar calls and some may be skipped. [Default: 1]</param>
void BatchItImage::UpdateProgressBar(std::function<void()> func, float multiplier)
{
    if (ui.progressBar->isVisible() == false) {
        DEBUG(current_load_number);
        DEBUG_ERR("Progress Bar failed to update because \"InitiateProgressBar(max_ticks)\" has to be called first. Misuse of the \"multiplier\" parameter can also cause issues.");
        current_load_number = 0.0f;
        ui.progressBar->setValue(ui.progressBar->minimum());
    }
    else {
        current_load_number += load_interval * multiplier;
        ui.progressBar->setValue(current_load_number);
        if (current_load_number >= float(ui.progressBar->maximum()) - 0.05) {
            ui.progressBar->setVisible(false);
            DEBUG2("Progress Bar Finished: ", current_load_number);
            if (func != 0) func();
            //(*func)();
            //(this->*func)();
        }
    }
}

/// <summary>
/// Add/insert special data from a combo box representing specific metadata to a line edit.
/// </summary>
void BatchItImage::AddTextToFileName()
{
    // Get Text
    QString cur_qtext = ui.lineEdit_FileName->text();

    // Move currsor to an index right after <> if it is inbetween <>
    DEBUG2("Cursor Posistion: ", ui.lineEdit_FileName->cursorPosition());
    int rb_found = cur_qtext.indexOf(">", ui.lineEdit_FileName->cursorPosition());
    if (rb_found > -1) {
        int lb_found = cur_qtext.indexOf("<", ui.lineEdit_FileName->cursorPosition());
        if (lb_found == -1 or lb_found > rb_found) {
            ui.lineEdit_FileName->setCursorPosition(rb_found + 1);
            DEBUG2("Cursor Posistion Moved: ", ui.lineEdit_FileName->cursorPosition());
        }
    }

    // Edit Text
    cur_qtext.insert(
        ui.lineEdit_FileName->cursorPosition(),
        ui.comboBox_AddText->currentData().toString().toStdString()
    );

    // Replace Text
    ui.lineEdit_FileName->setText(cur_qtext);
}

/// <summary>
/// Makes sure ui lineEdit_RelativePath uses platform specific slashes, converting all in current text. 
/// </summary>
void BatchItImage::CheckRelativePath()
{
    QString existing_text = ui.lineEdit_RelativePath->text();
    if (not existing_text.isEmpty() and
        (existing_text.front().toLatin1() == QString::fromStdString("/") or
        existing_text.front().toLatin1() == QString::fromStdString("\\"))) {
        existing_text.removeFirst();
    }
    std::string new_rel_path = std::filesystem::path(existing_text.toStdString()).make_preferred().string();
    new_rel_path = ReplaceAll(new_rel_path, "\\\\", "\\", true);
    new_rel_path = ReplaceAll(new_rel_path, "//", "/", true);
    ui.lineEdit_RelativePath->setText(QString::fromStdString(new_rel_path));
}

/// <summary>
/// Check if path from ui lineEdit_AbsolutePath exists and if it doesn't, replace text with last existing path or a default path.
/// </summary>
void BatchItImage::CheckAbsolutePath()
{
    std::string path = ui.lineEdit_AbsolutePath->text().toStdString();
    if (std::filesystem::exists(path)) {
        last_existing_save_path = path;
    }
    else {
        m_window = new MessageWindow(
            "Invalid Directory Path",
            "The directory path provided does not exist. Open directory dialog window?",
            QDialogButtonBox::Open | QDialogButtonBox::Abort,
            this
        );
        if (m_window->exec()) {
            ui.lineEdit_AbsolutePath->setText(GetSaveDirectoryPath());
        }
        else {
            ui.lineEdit_AbsolutePath->setText(GetLastExistingSavePath());
        }
        delete m_window;
    }
}

/// <summary>
/// Opens a directory dialog to obtain an existing directory path.
/// </summary>
/// <returns>A QString directory path.</returns>
QString BatchItImage::GetSaveDirectoryPath()
{
    QString qpath = GetLastExistingSavePath();
    QString directory = QFileDialog::getExistingDirectory(
        this,
        "Select a directory path",
        qpath // TODO: Default config setting, "recent"
    );
    DEBUG(directory.toStdString());
    if (directory.length() > 0) {
        last_existing_save_path = directory.toStdString();
        return directory;
    }
    else {
        ui.lineEdit_AbsolutePath->setText(qpath);
        return qpath;
    }
}

/// <summary>
/// Get the last existing directory path or a default path if none.
/// </summary>
/// <returns>A QString directory path.</returns>
QString BatchItImage::GetLastExistingSavePath()
{
    if (last_existing_save_path.empty())
        return qdefault_path;
    else
        return QString::fromStdString(last_existing_save_path);
}

/// <summary>
/// Find and replace all matched text.
/// </summary>
/// <param name="str">--The text to search.</param>
/// <param name="from">--Find this text.</param>
/// <param name="to">--Replace with this text</param>
/// <param name="no_really_all">--If true will include cases where "to" is a substring of "from". Default is false.</param>
/// <returns>An edited string.</returns>
std::string BatchItImage::ReplaceAll(std::string str, const std::string& from, const std::string& to, bool no_really_all) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        if (not no_really_all)
            start_pos += to.length();
    }
    return str;
}
