#include "BatchItImage.h"

/*
TODO: 
    Log Actions
    Holding right click scrolls left to right
    Track changes to undo file list deletes or image edit ui changes
    Image File Filter... show/edit only PNG files, files > 1 MB, etc.
    A cancel button to abort image editing durring process.

*/


DialogMessage::DialogMessage(QString title, QString message,
    QFlags<QDialogButtonBox::StandardButton> buttons, const uint custom_buttons, QWidget* parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    setWindowTitle(title);
    ui.label_Message->setText(message);
    ui.buttonBox->clear();

    if (custom_buttons & CustomButton::SaveContinue)
        ui.buttonBox->addButton(new QPushButton("&Save && Continue", this), QDialogButtonBox::ApplyRole);
    if (custom_buttons & CustomButton::Continue)
        ui.buttonBox->addButton(new QPushButton("&Continue Without Saving", this), QDialogButtonBox::AcceptRole);
    if (custom_buttons & CustomButton::ResetCancel)
        ui.buttonBox->addButton(new QPushButton("&Revert && Cancel", this), QDialogButtonBox::ResetRole);
    //if (custom_buttons & Cancel)
    //    ui.buttonBox->addButton(new QPushButton("Cancel All", this), QDialogButtonBox::DestructiveRole);
    if (custom_buttons & CustomButton::SaveClose)
        ui.buttonBox->addButton(new QPushButton("&Save && Close", this), QDialogButtonBox::ApplyRole);
    if (custom_buttons & CustomButton::Close)
        ui.buttonBox->addButton(new QPushButton("&Close Without Saving", this), QDialogButtonBox::AcceptRole);
    
    ui.buttonBox->setStandardButtons(buttons);

    Q_ASSERT(connect(ui.buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(buttonBoxClicked(QAbstractButton*))));
}
DialogMessage::~DialogMessage()
{
    qDebug() << "DialogMessage Deconstructed";
}
void DialogMessage::changeEvent(QEvent* event)
{
    //qDebug() << "DialogMessage::changeEvent";
    QDialog::changeEvent(event);
}
void DialogMessage::closeEvent(QCloseEvent* event)
{
    if (event->spontaneous()) {
        qDebug() << "The X-close button was clicked.";
        emit buttonClicked(QDialogButtonBox::Close);
        emit buttonRoleClicked(QDialogButtonBox::DestructiveRole);
    }
    QWidget::closeEvent(event);
}
void DialogMessage::buttonBoxClicked(QAbstractButton* button)
{
    const QDialogButtonBox::StandardButton std_button = ui.buttonBox->standardButton(button);
    const QDialogButtonBox::ButtonRole button_role = ui.buttonBox->buttonRole(button);

    qDebug() << "DialogMessage::buttonBoxClicked: " << std_button;
    qDebug() << "DialogMessage::buttonBoxClicked (Role): " << button_role;
    emit buttonClicked(std_button);
    emit buttonRoleClicked(button_role);
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
    QDialogButtonBox::Abort	    0x00040000	An "Abort" button defined with the RejectRole. and ResetRole?
    QDialogButtonBox::Retry	    0x00080000	A "Retry" button defined with the AcceptRole.
    QDialogButtonBox::Ignore	0x00100000	An "Ignore" button defined with the AcceptRole.
    QDialogButtonBox::NoButton	0x00000000	An invalid button.

    QDialogButtonBox::InvalidRole	    -1	The button is invalid.
    QDialogButtonBox::AcceptRole	    0	Clicking the button causes the dialog to be accepted (e.g. OK).
    QDialogButtonBox::RejectRole	    1	Clicking the button causes the dialog to be rejected (e.g. Cancel).
    QDialogButtonBox::DestructiveRole	2	Clicking the button causes a destructive change (e.g. for Discarding Changes) and closes the dialog.
    QDialogButtonBox::ActionRole    	3	Clicking the button causes changes to the elements within the dialog.
    QDialogButtonBox::HelpRole	        4	The button can be clicked to request help.
    QDialogButtonBox::YesRole       	5	The button is a "Yes"-like button.
    QDialogButtonBox::NoRole    	    6	The button is a "No"-like button.
    QDialogButtonBox::ApplyRole     	8	The button applies current changes.
    QDialogButtonBox::ResetRole	        7	The button resets the dialog's fields to default values.
    */
}


DialogEditPresetDesc::DialogEditPresetDesc(QString title, QString message,
    std::vector<Preset>* preset_list, uint preset_index, QWidget* parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    setWindowTitle(title);
    ui.label_Message->setText(message);
    DialogEditPresetDesc::preset_list = preset_list;
    DialogEditPresetDesc:: current_selected_preset = preset_index;
    updateComboBox();
    presetIndexChanged(preset_index);
    ui.comboBox_Preset_4->setCurrentIndex(preset_index);
    Q_ASSERT(connect(ui.buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(buttonBoxClicked(QAbstractButton*))));
    Q_ASSERT(connect(ui.comboBox_Preset_4, SIGNAL(currentIndexChanged(int)), this, SLOT(presetIndexChanged(int))));
}
DialogEditPresetDesc::~DialogEditPresetDesc()
{
    qDebug() << "DialogEditPresetDesc Deconstructed";
}
void DialogEditPresetDesc::presetIndexChanged(int index)
{
    qDebug() << "presetIndexChanged:" << index;
    ui.lineEdit_PresetDesc->setText(preset_list->at(index).description);
    current_selected_preset = index;
}
void DialogEditPresetDesc::updateComboBox()
{
    qDebug() << "updateComboBox";
    BatchItImage::AddPresetsToComboBox(preset_list, std::vector<QComboBox*>{ ui.comboBox_Preset_4 });
    ui.comboBox_Preset_4->setCurrentIndex(current_selected_preset);
}
void DialogEditPresetDesc::closeEvent(QCloseEvent* event)
{
    if (event->spontaneous()) {
        qDebug() << "The X-close button was clicked.";
        emit buttonClicked(QDialogButtonBox::Close);
    }
    emit presetIndexSelected(current_selected_preset);
    QWidget::closeEvent(event);
}
void DialogEditPresetDesc::buttonBoxClicked(QAbstractButton* button)
{
    QDialogButtonBox::StandardButton std_button = ui.buttonBox->standardButton(button);
    qDebug() << "DialogEditPresetDesc::buttonBoxClicked: " << std_button;

    if (QDialogButtonBox::Apply & std_button) {
        QString updated_description = ui.lineEdit_PresetDesc->text();
        preset_list->at(current_selected_preset).description = updated_description;
        qDebug() << updated_description.toStdString();
        updateComboBox();
    }
    else if (QDialogButtonBox::Reset & std_button) {
        presetIndexChanged(current_selected_preset);
    }
    else if (QDialogButtonBox::Close & std_button) {
        emit presetIndexSelected(current_selected_preset);
    }
    emit buttonClicked(std_button);
}


FileMetadataWorker::FileMetadataWorker(std::string file_path, int load_order, QObject* parent)
{
    FileMetadataWorker::file_path = file_path;
    FileMetadataWorker::load_order = load_order;
}
void FileMetadataWorker::getFileMetadata()
{
    qDebug() << "getFileMetadata From: " <<  file_path;

    // Image File Dimensions
    cv::Mat file_image = cv::imread(file_path); // TODO: Find (is there?) another way to get width/height without reading/opening file. This is slow when adding 100+ files.

    if (not file_image.empty()) {

        // Get File Metadata:
        struct stat t_stat;
        stat(file_path.data(), &t_stat);

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
    emit fileMetadataReady(file_metadata);
}
FileMetadataWorker::~FileMetadataWorker()
{
    qDebug() << "FileMetadataWorker Deconstructed";
}


BatchItImage::BatchItImage(QWidget* parent) : QMainWindow(parent)
{
    DEBUG("Debug Build");

    ui.setupUi(this);
    setAcceptDrops(true);

    preset_settings_file = QApplication::applicationDirPath() + "/settings.ini";
    DEBUG(preset_settings_file.toStdString());

    /***************************
        Add UI Text/Data
    ****************************/

    LoadInUiData();
    qDebug() << "LoadInUiData = Loaded";

    ui.tabWidget->setCurrentIndex(other_options->at(OtherOptions::tab_1).data);
    ui.tabWidget->setTabText(OtherOptions::tab_1, other_options->at(OtherOptions::tab_1).name);
    ui.tab_1->setToolTip(other_options->at(OtherOptions::tab_1).desc);
    ui.tabWidget->setTabText(OtherOptions::tab_2, other_options->at(OtherOptions::tab_2).name);
    ui.tab_2->setToolTip(other_options->at(OtherOptions::tab_2).desc);
    ui.tabWidget->setTabText(OtherOptions::tab_3, other_options->at(OtherOptions::tab_3).name);
    ui.tab_3->setToolTip(other_options->at(OtherOptions::tab_3).desc);

    AddUiDataTo(*resize_options, std::vector<QWidget*>{
        ui.groupBox_Resize, ui.checkBox_KeepAspectRatio });
    AddUiDataTo(*background_options, std::vector<QWidget*>{
        ui.groupBox_Background, ui.pushButton_ColorPicker, ui.label_ColorPreview });
    AddUiDataTo(*blur_options, std::vector<QWidget*>{
        ui.groupBox_Blur, ui.checkBox_BlurNormalize, ui.label_BlurX1, ui.label_BlurY1, ui.label_BlurX2, ui.label_BlurY2, ui.label_BlurD });
    //AddUiDataTo(blur_depth_selections, std::vector<QWidget*>{ui.verticalSlider_BlurD});
    AddUiDataTo(*rotation_options, std::vector<QWidget*>{
        ui.groupBox_Rotation, ui.checkBox_IncreaseBounds, ui.checkBox_FlipImage });

    AddUiDataTo(ui.groupBox_FileRename, file_path_options->at(FilePathOptions::groupBox_FileRename));
    AddUiDataTo(ui.radioButton_Overwrite, file_path_options->at(FilePathOptions::radioButton_Overwrite));
    AddUiDataTo(ui.radioButton_RenameOriginal, file_path_options->at(FilePathOptions::radioButton_RenameOriginal));
    AddUiDataTo(ui.radioButton_NewFileName, file_path_options->at(FilePathOptions::radioButton_NewFileName));
    AddUiDataTo(ui.label_Add, file_path_options->at(FilePathOptions::label_Add));
    AddUiDataTo(ui.groupBox_SaveDir, file_path_options->at(FilePathOptions::groupBox_SaveDir));
    AddUiDataTo(ui.radioButton_RelativePath, file_path_options->at(FilePathOptions::radioButton_RelativePath));
    AddUiDataTo(ui.radioButton_AbsolutePath, file_path_options->at(FilePathOptions::radioButton_AbsolutePath));
    AddUiDataTo(ui.pushButton_AddBackOneDir, file_path_options->at(FilePathOptions::pushButton_AddBackOneDir));
    AddUiDataTo(ui.pushButton_FindAbsolutePath, file_path_options->at(FilePathOptions::pushButton_FindAbsolutePath));
    AddUiDataTo(ui.checkBox_SearchSubDirs, other_options->at(OtherOptions::checkBox_SearchSubDirs));
    AddUiDataTo(ui.pushButton_EditAndSave, other_options->at(OtherOptions::pushButton_EditAndSave));

    PopulateComboBox(ui.comboBox_WidthMod, *width_selections);
    PopulateComboBox(ui.comboBox_HeightMod, *height_selections);
    PopulateComboBox(ui.comboBox_Resample, *resampling_selections);
    PopulateComboBox(ui.comboBox_BorderType, *border_types);
    PopulateComboBox(ui.comboBox_BlurFilter, *blur_filters);
    PopulateComboBox(ui.comboBox_AddText, *file_name_creation, ImageSaver::Metadata);
    PopulateComboBox(ui.comboBox_ImageFormat, *image_formats, extension_list);

    /***************************
        Prep UI Widgets, Etc
    ****************************/

    preset_list.reserve(10);
    current_file_metadata_list.reserve(30);
    deleted_file_metadata_list.reserve(10);

    ui.enhancedProgressBar->setVisible(false);

    ui.dial_Rotation->setInvertedAppearance(true);
    ui.dial_Rotation->setInvertedControls(true);

    // TODO: get settings for search sub dirs, recent images loaded (last 10?), others?
    //ui.checkBox_SearchSubDirs->setChecked(search_subdirs);

    SetupFileTree();
    SetupFileTreeContextMenu();
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

    // Create image extension string for "open file dialog".
    supported_image_extensions_dialog_str.append("Images: (");
    for (const auto& ext : extension_list) {
        supported_image_extensions_dialog_str.append("*" + ext + " ");
    }
    supported_image_extensions_dialog_str.insert(supported_image_extensions_dialog_str.size() - 1, ")");
    supported_image_extensions_dialog_str.append("\nAll Files (*)");
    DEBUG(supported_image_extensions_dialog_str.toStdString());

    /***************************
        UI Events
    ****************************/
    UiConnections();
   
    // Delete all arrays on the heap that will not be reused after loading.
    delete file_tree_headers;
    delete file_tree_menu_items;
    delete file_path_options;
    delete resize_options;
    delete background_options;
    delete blur_options;
    delete rotation_options;
    delete other_options;
    delete width_selections;
    delete height_selections;
    delete resampling_selections;
    delete border_types;
    delete blur_filters;
    delete file_name_creation;
    delete image_formats;

    /***************************
        Testing
    ****************************/

    // TODO: Combine Tree
    /*ui.treeWidget_Combine->clear();
    QTreeWidgetItem* new_item = new QTreeWidgetItem(ui.treeWidget_Combine);
    QComboBox* comboBox = new QComboBox(this);
    comboBox->addItems(QStringList() << "item1" << "item2");
    ui.treeWidget_Combine->setItemWidget(new_item, 1, comboBox);
    ui.treeWidget_Combine->addTopLevelItem(new_item);*/

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

void BatchItImage::LoadInUiData()
{
    // treeWidget_FileInfo
    file_tree_headers->at(FileColumn::FILE_SELECTED).data = 1; // TODO: 1 = Initial Sort/Bold Text (after files loaded)?
    file_tree_headers->at(FileColumn::FILE_SELECTED).name = "";
    file_tree_headers->at(FileColumn::FILE_SELECTED).desc = "The load order of image files.";
    file_tree_headers->at(FileColumn::FILE_NAME).data = 0;
    file_tree_headers->at(FileColumn::FILE_NAME).name = "File Name";
    file_tree_headers->at(FileColumn::FILE_NAME).desc = "The name of an image file, click an arrow to show full path.";
    file_tree_headers->at(FileColumn::IMAGE_DIMENSIONS).data = 0;
    file_tree_headers->at(FileColumn::IMAGE_DIMENSIONS).name = "Dimensions";
    file_tree_headers->at(FileColumn::IMAGE_DIMENSIONS).desc = "Image Dimensions/Size (Width x Height).";
    file_tree_headers->at(FileColumn::FILE_SIZE).data = 0;
    file_tree_headers->at(FileColumn::FILE_SIZE).name = "File Size";
    file_tree_headers->at(FileColumn::FILE_SIZE).desc = "The file size in bytes.";
    file_tree_headers->at(FileColumn::DATE_CREATED).data = 0;
    file_tree_headers->at(FileColumn::DATE_CREATED).name = "Date Created";
    file_tree_headers->at(FileColumn::DATE_CREATED).desc = "The date a file was created.";
    file_tree_headers->at(FileColumn::DATE_MODIFIED).data = 0;
    file_tree_headers->at(FileColumn::DATE_MODIFIED).name = "Date Modified";
    file_tree_headers->at(FileColumn::DATE_MODIFIED).desc = "The date a file was last modified.";

    file_tree_menu_items->at(ActionMenu::action_add).data = 0; // TODO: 1 = Bold, default double click action?
    file_tree_menu_items->at(ActionMenu::action_add).name = "Add Images";
    file_tree_menu_items->at(ActionMenu::action_add).desc = "Add more images to this file viewer.";
    file_tree_menu_items->at(ActionMenu::action_delete).data = 0;
    file_tree_menu_items->at(ActionMenu::action_delete).name = "Delete Images";
    file_tree_menu_items->at(ActionMenu::action_delete).desc = "Delete images from this file viewer (does not delete from system).";
    file_tree_menu_items->at(ActionMenu::action_clear).data = 0;
    file_tree_menu_items->at(ActionMenu::action_clear).name = "Clear List";
    file_tree_menu_items->at(ActionMenu::action_clear).desc = "Clear entire list of files from file viewer.";
    file_tree_menu_items->at(ActionMenu::action_select).data = 0;
    file_tree_menu_items->at(ActionMenu::action_select).name = "Select Image";
    file_tree_menu_items->at(ActionMenu::action_select).desc = "Select or check image file currently highlighted.";
    file_tree_menu_items->at(ActionMenu::action_view).data = 0;
    file_tree_menu_items->at(ActionMenu::action_view).name = "View Image";
    file_tree_menu_items->at(ActionMenu::action_view).desc = "View image file currently highlighted.";
    file_tree_menu_items->at(ActionMenu::action_preview).data = 0;
    file_tree_menu_items->at(ActionMenu::action_preview).name = "Preview Modified Image";
    file_tree_menu_items->at(ActionMenu::action_preview).desc = "Preview a modified version of the image file currently highlighted using the current selected preset.\n" \
        "This modified image will only be a preview the edits will not be saved to file.";

    file_tree_other_text.at(FileColumn::FILE_LOAD_ORDER) = "Load Order: ";
    file_tree_other_text.at(FileColumn::FILE_NAME) = "File Path: ";
    file_tree_other_text.at(FileColumn::IMAGE_DIMENSIONS) = "Image [Width x Height]: ";
    file_tree_other_text.at(FileColumn::FILE_SIZE) = "File Size: ";
    file_tree_other_text.at(FileColumn::DATE_CREATED) = "Date File Created: ";
    file_tree_other_text.at(FileColumn::DATE_MODIFIED) = "Date File Modified: ";

    resize_options->at(ResizeOptions::groupBox_Resize).data = 0;
    resize_options->at(ResizeOptions::groupBox_Resize).name = "Resize:";
    resize_options->at(ResizeOptions::groupBox_Resize).desc = "Options to increase or decrease the dimensions of an image.";
    resize_options->at(ResizeOptions::checkBox_KeepAspectRatio).data = 1;
    resize_options->at(ResizeOptions::checkBox_KeepAspectRatio).name = "Keep Aspect Ratio";
    resize_options->at(ResizeOptions::checkBox_KeepAspectRatio).desc = "In order to keep aspect ratio, either width or height must be set to \"No Change\" or \"0\".";

    // comboBox_WidthMod
    width_selections->at(0).data = ImageEditor::NO_CHANGE;
    width_selections->at(0).name = "No Change";
    width_selections->at(0).desc = "Image widths may still be modified if 'keep aspect ratio' is checked.";
    width_selections->at(1).data = ImageEditor::CHANGE_TO;
    width_selections->at(1).name = "Change Width To:";
    width_selections->at(1).desc = "All image widths will be modified to a specific number.";
    width_selections->at(2).data = ImageEditor::MODIFY_BY;
    width_selections->at(2).name = "Modify Width By:";
    width_selections->at(2).desc = "This adds to or subtracts from an image's current width. Ex. 1080 + '220' = 1300";
    width_selections->at(3).data = ImageEditor::MODIFY_BY_PCT;
    width_selections->at(3).name = "Modify Width By (%):";
    width_selections->at(3).desc = "This modifies an image's current width by percentage. Ex. 720 x '200%' = 1440";
    width_selections->at(4).data = ImageEditor::DOWNSCALE;
    width_selections->at(4).name = "Downscale Width To:";
    width_selections->at(4).desc = "All images above entered width will be modified to that specific number.\n" \
        "All images already at or below that number will not be modified.";
    width_selections->at(5).data = ImageEditor::UPSCALE;
    width_selections->at(5).name = "Upscale Width To:";
    width_selections->at(5).desc = "All images below entered width will be modified to that specific number.\n" \
        "All images already at or above that number will not be modified.";

    // comboBox_HeightMod
    height_selections->at(0).data = ImageEditor::NO_CHANGE;
    height_selections->at(0).name = "No Change";
    height_selections->at(0).desc = "Image heights may still be modified if 'keep aspect ratio' is checked.";
    height_selections->at(1).data = ImageEditor::CHANGE_TO;
    height_selections->at(1).name = "Change Height To:";
    height_selections->at(1).desc = "All images heights will be modified to a specific number.";
    height_selections->at(2).data = ImageEditor::MODIFY_BY;
    height_selections->at(2).name = "Modify Height By:";
    height_selections->at(2).desc = "This adds to or subtracts from an image's current height. Ex. 1080 + '220' = 1300";
    height_selections->at(3).data = ImageEditor::MODIFY_BY_PCT;
    height_selections->at(3).name = "Modify Height By (%):";
    height_selections->at(3).desc = "This modifies an image's current height by percentage. Ex. 720 x '200%' = 1440";
    height_selections->at(4).data = ImageEditor::DOWNSCALE;
    height_selections->at(4).name = "Downscale Height To:";
    height_selections->at(4).desc = "All images above entered height will be modified to that specific number.\n" \
        "All images already at or below that number will not be modified.";
    height_selections->at(5).data = ImageEditor::UPSCALE;
    height_selections->at(5).name = "Upscale Height To:";
    height_selections->at(5).desc = "All images below entered height will be modified to that specific number.\n" \
        "All images already at or above that number will not be modified.";

    // comboBox_Resample
    resampling_selections->at(0).data = cv::InterpolationFlags::INTER_NEAREST;
    resampling_selections->at(0).name = "Resampling: Nearest";
    resampling_selections->at(0).desc = "Nearest Neighbor Interpolation";
    resampling_selections->at(1).data = cv::InterpolationFlags::INTER_LINEAR;
    resampling_selections->at(1).name = "Resampling: Bilinear";
    resampling_selections->at(1).desc = "Bilinear Interpolation";
    resampling_selections->at(2).data = cv::InterpolationFlags::INTER_CUBIC;
    resampling_selections->at(2).name = "Resampling: Bicubic";
    resampling_selections->at(2).desc = "Bicubic Interpolation";
    // TODO: add more Resampling Filters

    background_options->at(BackgroundOptions::groupBox_Background).data = 0;
    background_options->at(BackgroundOptions::groupBox_Background).name = "Border Type / Background Color:";
    background_options->at(BackgroundOptions::groupBox_Background).desc = "";
    background_options->at(BackgroundOptions::pushButton_ColorPicker).data = 0;
    background_options->at(BackgroundOptions::pushButton_ColorPicker).name = "Background Color Picker";
    background_options->at(BackgroundOptions::pushButton_ColorPicker).desc = "";
    background_options->at(BackgroundOptions::label_ColorPreview).data = 0;
    background_options->at(BackgroundOptions::label_ColorPreview).name = "Background Color Preview";
    background_options->at(BackgroundOptions::label_ColorPreview).desc = "";

    // comboBox_BorderType
    border_types->at(0).data = cv::BORDER_CONSTANT;
    border_types->at(0).name = "Constant Border (Default)";
    border_types->at(0).desc = "[ iiiiii|abcdefgh|iiiiiii ] A consistent |border| that uses a selected background color (i).";
    border_types->at(1).data = cv::BORDER_REPLICATE;
    border_types->at(1).name = "Replicate Border";
    border_types->at(1).desc = "[ aaaaaa|abcdefgh|hhhhhhh ] Replicates the |border| of an image.";
    border_types->at(2).data = cv::BORDER_REFLECT;
    border_types->at(2).name = "Reflect Border";
    border_types->at(2).desc = "[ fedcba|abcdefgh|hgfedcb ] Reflects the |border| of an image.";
    border_types->at(3).data = cv::BORDER_WRAP;
    border_types->at(3).name = "Wrap Border";
    border_types->at(3).desc = "[ cdefgh|abcdefgh|abcdefg ] Warp or uses the opposite |border| side of an image.";
    border_types->at(4).data = cv::BORDER_REFLECT_101;
    border_types->at(4).name = "Reflect Border 101";
    border_types->at(4).desc = "[ gfedcb|abcdefgh|gfedcba ] Slightly different reflection of the |border| of an image.";
    border_types->at(5).data = cv::BORDER_TRANSPARENT;
    border_types->at(5).name = "[Not] Transparent Border";
    border_types->at(5).desc = "[ uvwxyz|abcdefgh|ijklmno ] Not really transparent, but differing shades of light/dark colors.";
    border_types->at(6).data = cv::BORDER_ISOLATED;
    border_types->at(6).name = "Isolated Border";
    border_types->at(6).desc = "Does not look outside of ROI |border|?";
    
    blur_options->at(BlurOptions::groupBox_Blur).data = 0;
    blur_options->at(BlurOptions::groupBox_Blur).name = "Blur Filters:";
    blur_options->at(BlurOptions::groupBox_Blur).desc = "";
    blur_options->at(BlurOptions::checkBox_BlurNormalize).data = 0;
    blur_options->at(BlurOptions::checkBox_BlurNormalize).name = "Normalize";
    blur_options->at(BlurOptions::checkBox_BlurNormalize).desc = "";
    blur_options->at(BlurOptions::label_BlurX1).data = 0;
    blur_options->at(BlurOptions::label_BlurX1).name = "  X";
    blur_options->at(BlurOptions::label_BlurX1).desc = "";
    blur_options->at(BlurOptions::label_BlurY1).data = 0;
    blur_options->at(BlurOptions::label_BlurY1).name = "  Y";
    blur_options->at(BlurOptions::label_BlurY1).desc = "";
    blur_options->at(BlurOptions::label_BlurX2).data = 0;
    blur_options->at(BlurOptions::label_BlurX2).name = " sX";
    blur_options->at(BlurOptions::label_BlurX2).desc = "";
    blur_options->at(BlurOptions::label_BlurY2).data = 0;
    blur_options->at(BlurOptions::label_BlurY2).name = " sY";
    blur_options->at(BlurOptions::label_BlurY2).desc = "";
    blur_options->at(BlurOptions::label_BlurD).data = 0;
    blur_options->at(BlurOptions::label_BlurD).name = "  D";
    blur_options->at(BlurOptions::label_BlurD).desc = "";

    // comboBox_BlurFilter
    blur_filters->at(0).data = ImageEditor::NO_FILTER;
    blur_filters->at(0).name = "None";
    blur_filters->at(0).desc = "Does not apply a blur filter.";
    //blur_filters->at(1).data = ImageEditor::BLUR_FILTER;
    //blur_filters->at(1).name = "Simple Blur Filter";
    //blur_filters->at(1).desc = "Blurs an image using the normalized box filter. Also known as homogeneous smoothing.";
    blur_filters->at(1).data = ImageEditor::BOX_FILTER;
    blur_filters->at(1).name = "Box Filter";
    blur_filters->at(1).desc = "Blurs an image using the box filter. An unnormalized box filter is useful for computing\n" \
                           "various integral characteristics over each pixel neighborhood, such as covariance\n" \
                           "matrices of image derivatives (used in dense optical flow algorithms, and so on).";
    blur_filters->at(2).data = ImageEditor::BILATERAL_FILTER;
    blur_filters->at(2).name = "Bilateral Filter";
    blur_filters->at(2).desc = "Applies the bilateral filter to an image, which can reduce unwanted noise very well while\n" \
                           "keeping edges fairly sharp. However, it is very slow compared to most other filters.";
    blur_filters->at(3).data = ImageEditor::GAUSSIAN_BLUR;
    blur_filters->at(3).name = "Gaussian Blur Filter";
    blur_filters->at(3).desc = "Applies the Gaussian blur filter to an image, which convolves the source image with the\n" \
                           "specified Gaussian kernel.";
    blur_filters->at(4).data = ImageEditor::MEDIAN_BLUR;
    blur_filters->at(4).name = "Median Blur Filter";
    blur_filters->at(4).desc = "Blurs an image using the median filter, which smooths an image with the x/y size aperture.\n" \
                           "Each channel of a multi-channel image is processed independently.";
    blur_filters->at(5).data = ImageEditor::PYR_DOWN_BLUR;
    blur_filters->at(5).name = "PYR Down-Sample Blur";
    blur_filters->at(5).desc = "Blurs an image and down-samples it. This performs the down-sampling step of the Gaussian\n" \
                           "pyramid construction. First, it convolves the source image with the Gaussian kernel, then\n" \
                           "down-samples the image by rejecting even rows and columns.";
    blur_filters->at(6).data = ImageEditor::PYR_UP_BLUR;
    blur_filters->at(6).name = "PYR Up-Sample Blur";
    blur_filters->at(6).desc = "Up-samples an image and then blurs it. This performs the up-sampling step of the Gaussian\n" \
                           "pyramid construction, though it can actually be used to construct the Laplacian pyramid.\n" \
                           "First, it up-samples the source image by injecting even zero rows and columns and then\n" \
                           "convolves the result with the same kernel as in \"PYR Down-Sample\" multiplied by 4.";
    
    // verticalSlider_BlurD
    blur_depth_selections.at(0).data = -1;
    blur_depth_selections.at(0).name = "Unchanged Bit Depth";
    blur_depth_selections.at(0).desc = "";
    blur_depth_selections.at(1).data = CV_8U;
    blur_depth_selections.at(1).name = "Unsigned 8-bit Integer";
    blur_depth_selections.at(1).desc = "";
    blur_depth_selections.at(2).data = CV_16U;
    blur_depth_selections.at(2).name = "Unsigned 16-bit Integer";
    blur_depth_selections.at(2).desc = "";
    blur_depth_selections.at(3).data = CV_16S;
    blur_depth_selections.at(3).name = "Signed 16-bit Integer";
    blur_depth_selections.at(3).desc = "";
    blur_depth_selections.at(4).data = CV_32F;
    blur_depth_selections.at(4).name = "32-bit Floating Point";
    blur_depth_selections.at(4).desc = "";
    blur_depth_selections.at(5).data = CV_64F;
    blur_depth_selections.at(5).name = "64-bit Floating Point";
    blur_depth_selections.at(5).desc = "";

    rotation_options->at(RotationOptions::groupBox_Rotation).data = 0;
    rotation_options->at(RotationOptions::groupBox_Rotation).name = "Rotation:";
    rotation_options->at(RotationOptions::groupBox_Rotation).desc = "";
    rotation_options->at(RotationOptions::checkBox_IncreaseBounds).data = 0;
    rotation_options->at(RotationOptions::checkBox_IncreaseBounds).name = "Increase Image\nBoundaries";
    rotation_options->at(RotationOptions::checkBox_IncreaseBounds).desc = "";
    rotation_options->at(RotationOptions::checkBox_FlipImage).data = 0;
    rotation_options->at(RotationOptions::checkBox_FlipImage).name = "Flip / Mirror";
    rotation_options->at(RotationOptions::checkBox_FlipImage).desc = "";

    file_path_options->at(FilePathOptions::groupBox_FileRename).data = 1;
    file_path_options->at(FilePathOptions::groupBox_FileRename).name = "Modify Image File Names:";
    file_path_options->at(FilePathOptions::groupBox_FileRename).desc = "";
    file_path_options->at(FilePathOptions::radioButton_Overwrite).data = 0;
    file_path_options->at(FilePathOptions::radioButton_Overwrite).name = "Overwrite Original File";
    file_path_options->at(FilePathOptions::radioButton_Overwrite).desc = "If selected, this will overwrite the original image file with the new edited image file.";
    file_path_options->at(FilePathOptions::radioButton_RenameOriginal).data = 0;
    file_path_options->at(FilePathOptions::radioButton_RenameOriginal).name = "Rename Original File";
    file_path_options->at(FilePathOptions::radioButton_RenameOriginal).desc = "If selected, this will rename the original image file, and use it's name for the new edited image file.";
    file_path_options->at(FilePathOptions::radioButton_NewFileName).data = 1;
    file_path_options->at(FilePathOptions::radioButton_NewFileName).name = "Create New File Name";
    file_path_options->at(FilePathOptions::radioButton_NewFileName).desc = "If selected, a new file name will be used for the new edited image file.";
    file_path_options->at(FilePathOptions::label_Add).data = 0;
    file_path_options->at(FilePathOptions::label_Add).name = "Add:";
    file_path_options->at(FilePathOptions::label_Add).desc = "Add metadata to file name edit text box (to be included in a new file name).";
    file_path_options->at(FilePathOptions::groupBox_SaveDir).data = 1;
    file_path_options->at(FilePathOptions::groupBox_SaveDir).name = "Save All Image Files In:";
    file_path_options->at(FilePathOptions::groupBox_SaveDir).desc = "";
    file_path_options->at(FilePathOptions::radioButton_RelativePath).data = 1;
    file_path_options->at(FilePathOptions::radioButton_RelativePath).name = "Relative Paths:";
    file_path_options->at(FilePathOptions::radioButton_RelativePath).desc = "Relative to the currently edited image file path.";
    file_path_options->at(FilePathOptions::radioButton_AbsolutePath).data = 0;
    file_path_options->at(FilePathOptions::radioButton_AbsolutePath).name = "Absolute Path:";
    file_path_options->at(FilePathOptions::radioButton_AbsolutePath).desc = "The absolute path all edited images will be saved to.";
    file_path_options->at(FilePathOptions::pushButton_AddBackOneDir).data = 0;
    file_path_options->at(FilePathOptions::pushButton_AddBackOneDir).name = "Add \"Back One Directory\" For Relative Paths";
    file_path_options->at(FilePathOptions::pushButton_AddBackOneDir).desc = "Start relative path up one directory level.";
    file_path_options->at(FilePathOptions::pushButton_FindAbsolutePath).data = 0;
    file_path_options->at(FilePathOptions::pushButton_FindAbsolutePath).name = "Search For An Absolute Path";
    file_path_options->at(FilePathOptions::pushButton_FindAbsolutePath).desc = "Open dialog window to select a directory adding it to the absolute path text box.";

    // comboBox_AddText
    file_name_creation->at(ImageSaver::FILE_NAME).data = ImageSaver::FILE_NAME;
    file_name_creation->at(ImageSaver::FILE_NAME).name = "Original File Name";
    file_name_creation->at(ImageSaver::FILE_NAME).desc = "Add the original file name into the creation of a new file name.";
    file_name_creation->at(ImageSaver::COUNTER).data = ImageSaver::COUNTER;
    file_name_creation->at(ImageSaver::COUNTER).name = "Incrementing Counter";
    file_name_creation->at(ImageSaver::COUNTER).desc = "Add an incrementing number into the creation of a new file name.";
    file_name_creation->at(ImageSaver::WIDTH).data = ImageSaver::WIDTH;
    file_name_creation->at(ImageSaver::WIDTH).name = "Image Width";
    file_name_creation->at(ImageSaver::WIDTH).desc = "Add the width of the modified image into the creation of a new file name.";
    file_name_creation->at(ImageSaver::HEIGHT).data = ImageSaver::HEIGHT;
    file_name_creation->at(ImageSaver::HEIGHT).name = "Image Height";
    file_name_creation->at(ImageSaver::HEIGHT).desc = "Add the height of the modified image into the creation of a new file name.";

    extension_list = { ".jpeg", ".jpg", ".jpe", ".jp2", ".png", ".webp", ".bmp", ".dib", ".avif", ".pbm",
        ".pgm", ".ppm", ".pxm", ".pnm", ".pfm", ".pam", ".sr", ".ras", ".tiff", ".tif", ".exr", ".hdr", ".pic" };

    // comboBox_ImageFormat
    image_formats->at(ImageFormats::jpeg).data = ImageFormats::jpeg;
    image_formats->at(ImageFormats::jpeg).name = "JPEG Files - *.jpeg";
    image_formats->at(ImageFormats::jpeg).desc = "JPEG (Joint Photographic Experts Group) is a commonly used method of lossy compression\n" \
        "for digital images, particularly for those images produced by digital photography.\n" \
        "The degree of compression can be adjusted, allowing a selectable tradeoff between\n" \
        "storage size and image quality.JPEG typically achieves 10:1 compression with little\n" \
        "perceptible loss in image quality.";
    image_formats->at(ImageFormats::jpg).data = ImageFormats::jpg;
    image_formats->at(ImageFormats::jpg).name = "JPEG Files - *.jpg";
    image_formats->at(ImageFormats::jpg).desc = image_formats->at(ImageFormats::jpeg).desc;
    image_formats->at(ImageFormats::jpe).data = ImageFormats::jpe;
    image_formats->at(ImageFormats::jpe).name = "JPEG Files - *.jpe";
    image_formats->at(ImageFormats::jpe).desc = image_formats->at(ImageFormats::jpeg).desc;
    image_formats->at(ImageFormats::jp2).data = ImageFormats::jp2;
    image_formats->at(ImageFormats::jp2).name = "JPEG 2000 Files - *.jp2";
    image_formats->at(ImageFormats::jp2).desc = "JPEG 2000 (Joint Photographic Experts Group) is an image compression standard based on a\n" \
        "discrete wavelet transform (DWT). Note that it is still not widely supported in web\n" \
        "browsers (other than Safari) and hence is not generally used on the World Wide Web.";
    image_formats->at(ImageFormats::png).data = ImageFormats::png;
    image_formats->at(ImageFormats::png).name = "Portable Network Graphics - *.png";
    image_formats->at(ImageFormats::png).desc = "Portable Network Graphics (PNG) is a raster-graphics file format that supports lossless\n" \
        "data compression. PNG supports palette-based images (with palettes of 24-bit RGB or\n" \
        "32-bit RGBA colors), grayscale images (with or without an alpha channel for transparency),\n" \
        "and full-color non-palette-based RGB or RGBA images.";
    image_formats->at(ImageFormats::webp).data = ImageFormats::webp;
    image_formats->at(ImageFormats::webp).name = "WebP - *.webp";
    image_formats->at(ImageFormats::webp).desc = "WebP is a raster graphics file format developed by Google intended as a replacement for\n" \
        "JPEG, PNG, and GIF file formats. It supports both lossy and lossless compression, as well\n" \
        "as animation and alpha transparency.";
    image_formats->at(ImageFormats::bmp).data = ImageFormats::bmp;
    image_formats->at(ImageFormats::bmp).name = "Windows Bitmaps - *.bmp";
    image_formats->at(ImageFormats::bmp).desc = "The BMP file format or bitmap, is a raster graphics image file format used to store\n" \
        "bitmap digital images, independently of the display device (such as a graphics adapter),\n" \
        "especially on Microsoft Windows and OS/2 operating systems.";
    image_formats->at(ImageFormats::dib).data = ImageFormats::dib;
    image_formats->at(ImageFormats::dib).name = "Windows Bitmaps - *.dib";
    image_formats->at(ImageFormats::dib).desc = image_formats->at(ImageFormats::bmp).desc;
    image_formats->at(ImageFormats::avif).data = ImageFormats::avif;
    image_formats->at(ImageFormats::avif).name = "AVIF - *.avif";
    image_formats->at(ImageFormats::avif).desc = "AV1 Image File Format (AVIF) is an open, royalty-free image file format specification\n" \
        "for storing images or image sequences compressed with AV1 in the HEIF container format.\n" \
        "AV1 Supports:\n" \
        "* Multiple color spaces (HDR, SDR, color space signaling via CICP or ICC)\n" \
        "* Lossless and lossy compression\n" \
        "* 8-, 10-, and 12-bit color depths\n" \
        "* Monochrome (alpha/depth) or multi-components\n" \
        "* 4:2:0, 4:2:2, 4:4:4 chroma subsampling and RGB\n" \
        "* Film grain synthesis\n" \
        "* Image sequences/animation";
    image_formats->at(ImageFormats::pbm).data = ImageFormats::pbm;
    image_formats->at(ImageFormats::pbm).name = "Netpbm Formats - *.pbm";
    image_formats->at(ImageFormats::pbm).desc = "Netpbm (formerly Pbmplus) is an open-source package of graphics programs and a programming\n" \
        "library. It is used mainly in the Unix world, but also works on Microsoft Windows, macOS,\n" \
        "and other operating systems.  Graphics formats used and defined by the Netpbm project:\n" \
        "portable pixmap format (PPM), portable graymap format (PGM), and portable bitmap format (PBM).\n" \
        "They are also sometimes referred to collectively as the portable anymap format (PNM).";
    image_formats->at(ImageFormats::pgm).data = ImageFormats::pgm;
    image_formats->at(ImageFormats::pgm).name = "Netpbm Formats - *.pgm";
    image_formats->at(ImageFormats::pgm).desc = image_formats->at(ImageFormats::pbm).desc;
    image_formats->at(ImageFormats::ppm).data = ImageFormats::ppm;
    image_formats->at(ImageFormats::ppm).name = "Netpbm Formats - *.ppm";
    image_formats->at(ImageFormats::ppm).desc = image_formats->at(ImageFormats::pbm).desc;
    image_formats->at(ImageFormats::pxm).data = ImageFormats::pxm;
    image_formats->at(ImageFormats::pxm).name = "Netpbm Formats - *.pxm";
    image_formats->at(ImageFormats::pxm).desc = image_formats->at(ImageFormats::pbm).desc;
    image_formats->at(ImageFormats::pnm).data = ImageFormats::pnm;
    image_formats->at(ImageFormats::pnm).name = "Netpbm Formats - *.pnm";
    image_formats->at(ImageFormats::pnm).desc = image_formats->at(ImageFormats::pbm).desc;
    image_formats->at(ImageFormats::pfm).data = ImageFormats::pfm;
    image_formats->at(ImageFormats::pfm).name = "Netpbm Formats - *.pfm";
    image_formats->at(ImageFormats::pfm).desc = "The PFM (Portable Floatmap) is supported by the de facto reference implementation Netpbm\n" \
        "and is the unofficial four byte IEEE 754 single precision floating point extension.\n" \
        "PFM is supported by the programs Photoshop, GIMP, and ImageMagick.";
    // TODO: Test support for this format
    image_formats->at(ImageFormats::pam).data = ImageFormats::pam;
    image_formats->at(ImageFormats::pam).name = "Netpbm Formats - *.pam";
    image_formats->at(ImageFormats::pam).desc = "Portable Arbitrary Map (PAM) is an extension of the older binary P4...P6 graphics formats,\n" \
        "introduced with Netpbm version 9.7. PAM generalizes all features of PBM, PGM and PPM, and\n" \
        "provides for extensions. PAM is supported by XnView and FFmpeg; and defines two new\n" \
        "attributes: depth and tuple type.";
    image_formats->at(ImageFormats::sr).data = ImageFormats::sr;
    image_formats->at(ImageFormats::sr).name = "Sun Rasters - *.sr";
    image_formats->at(ImageFormats::sr).desc = "Sun Raster was a raster graphics file format used on SunOS by Sun Microsystems. ACDSee,\n" \
        "FFmpeg, GIMP, ImageMagick, IrfanView, LibreOffice, Netpbm, PaintShop Pro, PMView, and\n" \
        "XnView among others support Sun Raster image files. The format does not support transparency.";
    image_formats->at(ImageFormats::ras).data = ImageFormats::ras;
    image_formats->at(ImageFormats::ras).name = "Sun Rasters - *.ras";
    image_formats->at(ImageFormats::ras).desc = image_formats->at(ImageFormats::sr).desc;
    image_formats->at(ImageFormats::tiff).data = ImageFormats::tiff;
    image_formats->at(ImageFormats::tiff).name = "TIFF Files - *.tiff";
    image_formats->at(ImageFormats::tiff).desc = "Tag Image File Format (TIFF or TIF), is an image file format for storing raster graphics\n" \
        "images, popular among graphic artists, the publishing industry, and photographers. TIFF is\n" \
        "widely supported by scanning, faxing, word processing, optical character recognition,\n" \
        "image manipulation, desktop publishing, and page-layout applications.";
    image_formats->at(ImageFormats::tif).data = ImageFormats::tif;
    image_formats->at(ImageFormats::tif).name = "TIFF Files - *.tif";
    image_formats->at(ImageFormats::tif).desc = image_formats->at(ImageFormats::tiff).desc;
    image_formats->at(ImageFormats::exr).data = ImageFormats::exr;
    image_formats->at(ImageFormats::exr).name = "OpenEXR Image Files - *.exr";
    image_formats->at(ImageFormats::exr).desc = "OpenEXR is a high-dynamic range, multi-channel raster file format, created under a free\n" \
        "software license similar to the BSD license.  It supports multiple channels of potentially\n" \
        "different pixel sizes, including 32-bit unsigned integer, 32-bit and 16-bit floating point\n" \
        "values, as well as various compression techniques which include lossless and lossy\n" \
        "compression algorithms. It also has arbitrary channels and encodes multiple points of view\n" \
        "such as left- and right-camera images.";
    image_formats->at(ImageFormats::hdr).data = ImageFormats::hdr;
    image_formats->at(ImageFormats::hdr).name = "Radiance HDR - *.hdr";
    image_formats->at(ImageFormats::hdr).desc = "RGBE or Radiance HDR is an image format that stores pixels as one byte each for RGB (red,\n" \
        "green, and blue) values with a one byte shared exponent. Thus it stores four bytes per pixel.\n" \
        "RGBE allows pixels to have the dynamic range and precision of floating-point values in a\n" \
        "relatively compact data structure (32 bits per pixel).";
    image_formats->at(ImageFormats::pic).data = ImageFormats::pic;
    image_formats->at(ImageFormats::pic).name = "Radiance HDR - *.pic";
    image_formats->at(ImageFormats::pic).desc = image_formats->at(ImageFormats::hdr).desc;

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
    other_options->at(OtherOptions::tab_1).data = 0; // Default current tab index, ignores other tab.data.
    other_options->at(OtherOptions::tab_1).name = "Images";
    //other_options->at(OtherOptions::tab_1).desc = "Image File Viewer Tab";
    other_options->at(OtherOptions::tab_2).data = 0;
    other_options->at(OtherOptions::tab_2).name = "Image Edits";
    //other_options->at(OtherOptions::tab_2).desc = "Image Edit Tools Tab";
    other_options->at(OtherOptions::tab_3).data = 0;
    other_options->at(OtherOptions::tab_3).name = "Save Options";
    //other_options->at(OtherOptions::tab_3).desc = "File Save Options Tab";
    other_options->at(OtherOptions::checkBox_SearchSubDirs).data = 1;
    other_options->at(OtherOptions::checkBox_SearchSubDirs).name = "When Searching Directories Search Sub-Directories As Well";
    other_options->at(OtherOptions::checkBox_SearchSubDirs).desc = "When file directories/folders are dropped into the image file viewer an image file search will begin\n" \
                                                                   "in the directory, and it this is checked, all its sub-directories too.";
    other_options->at(OtherOptions::pushButton_EditAndSave).data = 0;
    other_options->at(OtherOptions::pushButton_EditAndSave).name = "Start Editing And Saving Images";
    other_options->at(OtherOptions::pushButton_EditAndSave).desc = "";
    /*other_options->at(OtherOptions::).data = 0;
    other_options->at(OtherOptions::).name = "";
    other_options->at(OtherOptions::).desc = "";*/
    
    dialog_messages.at(DialogMessages::delete_dialog).data = 0;
    dialog_messages.at(DialogMessages::delete_dialog).name = "Delete?";
    dialog_messages.at(DialogMessages::delete_dialog).desc = "Delete currently highlighted image file -or- all image files selected/checked?";
    dialog_messages.at(DialogMessages::delete_dialog_clear).data = 0;
    dialog_messages.at(DialogMessages::delete_dialog_clear).name = "Clear List?";
    dialog_messages.at(DialogMessages::delete_dialog_clear).desc = "The entire list of image files is about to be cleared, is this OK?";
    dialog_messages.at(DialogMessages::CreateNewPreset).data = 0;
    dialog_messages.at(DialogMessages::CreateNewPreset).name = "Add New Preset";
    dialog_messages.at(DialogMessages::CreateNewPreset).desc = "Add Description for New Preset.";
    dialog_messages.at(DialogMessages::ChangePresetDescription).data = 0;
    dialog_messages.at(DialogMessages::ChangePresetDescription).name = "Change Preset Description";
    dialog_messages.at(DialogMessages::ChangePresetDescription).desc = "Change Title Description of Currently Selected Preset.";
    dialog_messages.at(DialogMessages::save_preset_dialog).data = 0;
    dialog_messages.at(DialogMessages::save_preset_dialog).name = "Save Current Preset?";
    dialog_messages.at(DialogMessages::save_preset_dialog).desc = "There are unsaved preset options, would you like to save these options before proceeding?";
    dialog_messages.at(DialogMessages::save_preset_dialog_closing).data = 0;
    dialog_messages.at(DialogMessages::save_preset_dialog_closing).name = "Closing...";
    dialog_messages.at(DialogMessages::save_preset_dialog_closing).desc = "There are unsaved preset options, would you like to save these options before closing?";
    dialog_messages.at(DialogMessages::non_image_file_dialog).data = 0;
    dialog_messages.at(DialogMessages::non_image_file_dialog).name = "Warning";
    dialog_messages.at(DialogMessages::non_image_file_dialog).desc = "Unsupported or non-image files were not added to list.";
    dialog_messages.at(DialogMessages::check_path_dialog).data = 0;
    dialog_messages.at(DialogMessages::check_path_dialog).name = "Invalid Directory Path";
    dialog_messages.at(DialogMessages::check_path_dialog).desc = "The directory path provided does not exist. Open directory dialog window?";
    /*dialog_messages.at(DialogMessages::).data = 0;
    dialog_messages.at(DialogMessages::).name = "";
    dialog_messages.at(DialogMessages::).desc = "";*/

}

void BatchItImage::SetupFileTree()
{
    ui.treeWidget_FileInfo->clear();
    for (int col = 0; col < FileColumn::COUNT; col++) {
        ui.treeWidget_FileInfo->headerItem()->setText(col, file_tree_headers->at(col).name);
        ui.treeWidget_FileInfo->headerItem()->setToolTip(col, file_tree_headers->at(col).desc);
    }
    ui.treeWidget_FileInfo->setColumnWidth(FileColumn::FILE_SELECTED, ui.treeWidget_FileInfo->minimumWidth());
    ui.treeWidget_FileInfo->header()->setSectionsClickable(true);
    ui.treeWidget_FileInfo->header()->sortIndicatorOrder();
    ui.treeWidget_FileInfo->installEventFilter(this); // Keep watch of all events happening in file tree. -> eventFilter()
    ui.treeWidget_FileInfo->setMouseTracking(true);
}

template<std::size_t ui_data_size>
void BatchItImage::AddUiDataTo(const std::array<UIData, ui_data_size>& ui_data, const std::vector<QWidget*>& objects)
{
    for (uint i = 0; i < objects.size(); i++) {
        AddUiDataTo(objects.at(i), ui_data.at(i));
    }
}

void BatchItImage::AddUiDataTo(QObject* object, const UIData& ui_data)
{
    std::string object_class = object->metaObject()->className();
    //DEBUG2("AddUiDataTo: ", object_class);
    
    if ("QCheckBox" == object_class) {
        QCheckBox* cb = qobject_cast<QCheckBox*>(object);
        cb->setChecked(ui_data.data);
        cb->setText(ui_data.name);
        cb->setStatusTip(ui_data.desc);
        cb->setToolTip(ui_data.desc);
    }
    else if ("QGroupBox" == object_class) {
        QGroupBox* gb = qobject_cast<QGroupBox*>(object);
        gb->setChecked(ui_data.data);
        gb->setTitle(ui_data.name);
        gb->setStatusTip(ui_data.desc);
        gb->setToolTip(ui_data.desc);
    }else if ("QLabel" == object_class) {
        QLabel* lbl = qobject_cast<QLabel*>(object);
        lbl->setText(ui_data.name);
        lbl->setStatusTip(ui_data.desc);
        lbl->setToolTip(ui_data.desc);
    }
    else if ("QRadioButton" == object_class) {
        QRadioButton* rb = qobject_cast<QRadioButton*>(object);
        rb->setChecked(ui_data.data);
        rb->setText(ui_data.name);
        rb->setStatusTip(ui_data.desc);
        rb->setToolTip(ui_data.desc);
    }
    else if ("QPushButton" == object_class) {
        QPushButton* pb = qobject_cast<QPushButton*>(object);
        pb->setText(ui_data.name);
        pb->setStatusTip(ui_data.desc);
        pb->setToolTip(ui_data.desc);
    }
    /*else if ("EnhancedSlider" == object_class) {
        EnhancedSlider* es = qobject_cast<EnhancedSlider*>(object);
        es->addTextTip(
            ui_data.data,
            ui_data.data,
            ui_data.name,
            false
        );
    }*/
}

template<std::size_t ui_data_size, std::size_t string_data_size>
void BatchItImage::PopulateComboBox(QComboBox* cb, const std::array<UIData, ui_data_size>& ui_data,
    const std::array <std::string, string_data_size>& string_data)
{
    // Convert std::string array to QString array
    std::array <QString, string_data_size> qstring_data;
    for (int i = 0; i < ui_data_size; i++) {
        qstring_data.at(i) = QString::fromStdString(string_data.at(i));
    }
    PopulateComboBox(cb, ui_data, qstring_data);
}
template<std::size_t ui_data_size, std::size_t string_data_size>
void BatchItImage::PopulateComboBox(QComboBox* cb, const std::array<UIData, ui_data_size>& ui_data,
    const std::array <QString, string_data_size>& string_data)
{
    cb->clear();
    for (int i = 0; i < ui_data.size(); i++) {
        if (string_data.empty()) {
            cb->addItem(ui_data.at(i).name, ui_data.at(i).data);
        }
        else {
            cb->addItem(ui_data.at(i).name, string_data.at(i));
        }
        cb->setItemData(i, ui_data.at(i).desc, Qt::StatusTipRole);
        cb->setItemData(i, ui_data.at(i).desc, Qt::ToolTipRole);
    }
    cb->setStatusTip(cb->currentData(Qt::StatusTipRole).toString());
    cb->setToolTip(cb->currentData(Qt::ToolTipRole).toString());
}

void BatchItImage::UpdateComboBoxTextTips()
{
    QComboBox* cb = qobject_cast<QComboBox*>(sender());
    cb->setStatusTip(cb->currentData(Qt::StatusTipRole).toString());
    cb->setToolTip(cb->currentData(Qt::ToolTipRole).toString());
    //DEBUG2("UpdateComboBoxTextTips: ", cb->objectName().toStdString());
}

// Unused
void BatchItImage::UpdateComboBoxTextTips(QComboBox* cb)
{
    cb->setStatusTip(cb->currentData(Qt::StatusTipRole).toString());
    cb->setToolTip(cb->currentData(Qt::ToolTipRole).toString());
}

void BatchItImage::UiConnections()
{
    // Main Window Menu
    Q_ASSERT(connect(ui.action_AddImages, SIGNAL(triggered(bool)), this, SLOT(LoadImageFiles())));
    Q_ASSERT(connect(ui.action_SaveLogAs, SIGNAL(triggered(bool)), this, SLOT(Test()))); // TODO
    Q_ASSERT(connect(ui.action_Close, &QAction::triggered, this, &BatchItImage::close));
    Q_ASSERT(connect(ui.action_AddNewPreset, SIGNAL(triggered(bool)), this, SLOT(CreateNewPreset())));
    Q_ASSERT(connect(ui.action_RemovePreset, SIGNAL(triggered(bool)), this, SLOT(Test()))); // TODO
    Q_ASSERT(connect(ui.action_SavePresets, &QAction::triggered, this, [this] { SavePreset(true); })); // TODO: Show a DialogMessage?
    Q_ASSERT(connect(ui.action_ChangePresetDesc, &QAction::triggered,
        this, [this] {
            ChangePresetDescription(
                CurrentSelectedPreset(),
                dialog_messages.at(DialogMessages::ChangePresetDescription).name,
                dialog_messages.at(DialogMessages::ChangePresetDescription).desc
            );
        }));
    Q_ASSERT(connect(ui.action_About, SIGNAL(triggered(bool)), this, SLOT(Test()))); // TODO
    Q_ASSERT(connect(ui.action_AboutQt, &QAction::triggered, this, [this] { QApplication::aboutQt(); }));
    Q_ASSERT(connect(ui.action_Help, &QAction::triggered, this, [this] { Test(); })); // TODO

    // Image File Tree Widgets
    Q_ASSERT(connect(ui.treeWidget_FileInfo->header(), SIGNAL(sectionClicked(int)), this, SLOT(SortFileTreeByColumn(int))));
    //connect(ui.checkBox_SearchSubDirs, &QCheckBox::stateChanged, this, [this] { search_subdirs = ui.checkBox_SearchSubDirs->isChecked(); });

    // Preset Combo Boxes
    Q_ASSERT(connect(ui.comboBox_Preset_1, SIGNAL(currentIndexChanged(int)), this, SLOT(ChangePreset(int))));
    //connect(ui.comboBox_Preset_1, SIGNAL(currentIndexChanged(int)), ui.comboBox_Preset_2, SLOT(ChangePresets(int))); // Done in the ui xml + 9 Others

    // Image Edit Widgets
    Q_ASSERT(connect(ui.comboBox_WidthMod, &QComboBox::currentIndexChanged, this,
        [=](int index) {
            edit_options_change_tracker = TrackOptionChanges(
                edit_options_change_tracker, Option.comboBox_WidthMod,
                preset_list.at(CurrentSelectedPreset()).width_change_selection, index
            );
            UpdateComboBoxTextTips();
        }));
    Q_ASSERT(connect(ui.spinBox_WidthNumber, &QSpinBox::valueChanged, this,
        [=](int value) {
            edit_options_change_tracker = TrackOptionChanges(
                edit_options_change_tracker, Option.spinBox_WidthNumber,
                preset_list.at(CurrentSelectedPreset()).width_number, value
            );
        }));
    Q_ASSERT(connect(ui.comboBox_HeightMod, &QComboBox::currentIndexChanged, this,
        [=](int index) {
            edit_options_change_tracker = TrackOptionChanges(
                edit_options_change_tracker, Option.comboBox_HeightMod,
                preset_list.at(CurrentSelectedPreset()).height_change_selection, index
            );
            UpdateComboBoxTextTips();
        }));
    Q_ASSERT(connect(ui.spinBox_HeightNumber, &QSpinBox::valueChanged, this,
        [=](int value) {
            edit_options_change_tracker = TrackOptionChanges(
                edit_options_change_tracker, Option.spinBox_HeightNumber,
                preset_list.at(CurrentSelectedPreset()).height_number, value
            );
        }));
    Q_ASSERT(connect(ui.comboBox_Resample, &QComboBox::currentIndexChanged, this,
        [=](int index) {
            edit_options_change_tracker = TrackOptionChanges(
                edit_options_change_tracker, Option.comboBox_Resample,
                preset_list.at(CurrentSelectedPreset()).resampling_filter, index
            );
            UpdateComboBoxTextTips();
        }));
    Q_ASSERT(connect(ui.checkBox_KeepAspectRatio, &QCheckBox::stateChanged, this,
        [=](int value) {
            edit_options_change_tracker = TrackOptionChanges(
                edit_options_change_tracker, Option.checkBox_KeepAspectRatio,
                preset_list.at(CurrentSelectedPreset()).keep_aspect_ratio, (value) ? 1 : 0
            );
        }));
    Q_ASSERT(connect(ui.comboBox_BorderType, &QComboBox::currentIndexChanged, this,
        [=](int index) {
            edit_options_change_tracker = TrackOptionChanges(
                edit_options_change_tracker, Option.comboBox_BorderType,
                preset_list.at(CurrentSelectedPreset()).border_type, index
            );
            UpdateComboBoxTextTips();
        }));

    // TODO background color

    Q_ASSERT(connect(ui.comboBox_BlurFilter, &QComboBox::currentIndexChanged, this,
        [=](int index) {
            int preset_value = preset_list.at(CurrentSelectedPreset()).blur_filter;
            edit_options_change_tracker = TrackOptionChanges(edit_options_change_tracker,
                Option.comboBox_BlurFilter, preset_value, index
            );
            UpdateComboBoxTextTips();
            EnableSpecificBlurOptions();
            // If already "off/None" remove below changed options.
            // TODO: other options once changed and changed back, could modify an option it doesn't use and flag a false change. fix?
            if (index == 0 and index == preset_value) {
                edit_options_change_tracker = RemoveOptionsChanged(edit_options_change_tracker, std::vector<uint>{
                    Option.checkBox_BlurNormalize, Option.verticalSlider_BlurX1, Option.verticalSlider_BlurY1,
                        Option.verticalSlider_BlurX2, Option.verticalSlider_BlurY2, Option.verticalSlider_BlurD
                });
            }
        }));
    Q_ASSERT(connect(ui.checkBox_BlurNormalize, &QCheckBox::stateChanged, this,
        [=](int value) {
            edit_options_change_tracker = TrackOptionChanges(
                edit_options_change_tracker, Option.checkBox_BlurNormalize,
                preset_list.at(CurrentSelectedPreset()).blur_normalize, (value) ? 1 : 0
            );
        }));
    Q_ASSERT(connect(ui.verticalSlider_BlurX1, &QSlider::valueChanged, this,
        [=](int value) {
            edit_options_change_tracker = TrackOptionChanges(
                edit_options_change_tracker, Option.verticalSlider_BlurX1,
                preset_list.at(CurrentSelectedPreset()).blur_x, value
            );
        }));
    Q_ASSERT(connect(ui.verticalSlider_BlurY1, &QSlider::valueChanged, this,
        [=](int value) {
            edit_options_change_tracker = TrackOptionChanges(
                edit_options_change_tracker, Option.verticalSlider_BlurY1,
                preset_list.at(CurrentSelectedPreset()).blur_y, value
            );
        }));
    Q_ASSERT(connect(ui.verticalSlider_BlurX2, &QSlider::valueChanged, this,
        [=](int value) {
            edit_options_change_tracker = TrackOptionChanges(
                edit_options_change_tracker, Option.verticalSlider_BlurX2,
                preset_list.at(CurrentSelectedPreset()).blur_sx, value
            );
        }));
    Q_ASSERT(connect(ui.verticalSlider_BlurY2, &QSlider::valueChanged, this,
        [=](int value) {
            edit_options_change_tracker = TrackOptionChanges(
                edit_options_change_tracker, Option.verticalSlider_BlurY2,
                preset_list.at(CurrentSelectedPreset()).blur_sy, value
            );
        }));
    Q_ASSERT(connect(ui.verticalSlider_BlurD, &QSlider::valueChanged, this,
        [=](int value) {
            edit_options_change_tracker = TrackOptionChanges(
                edit_options_change_tracker, Option.verticalSlider_BlurD,
                preset_list.at(CurrentSelectedPreset()).blur_depth, value
            );
        }));
    Q_ASSERT(connect(ui.dial_Rotation, &QSlider::valueChanged, this,
        [=](int value) {
            ui.lcdNumber_Rotation->display(value);
            edit_options_change_tracker = TrackOptionChanges(
                edit_options_change_tracker, Option.dial_Rotation,
                preset_list.at(CurrentSelectedPreset()).rotation_degrees, value
            );
        }));
    Q_ASSERT(connect(ui.checkBox_IncreaseBounds, &QCheckBox::stateChanged, this,
        [=](int value) {
            edit_options_change_tracker = TrackOptionChanges(
                edit_options_change_tracker, Option.checkBox_IncreaseBounds,
                preset_list.at(CurrentSelectedPreset()).increase_boundaries, (value) ? 1 : 0
            );
        }));
    Q_ASSERT(connect(ui.checkBox_FlipImage, &QCheckBox::stateChanged, this,
        [=](int value) {
            edit_options_change_tracker = TrackOptionChanges(
                edit_options_change_tracker, Option.checkBox_FlipImage,
                preset_list.at(CurrentSelectedPreset()).flip_image, (value) ? 1 : 0
            );
        }));

    // Image Save Widgets
    Q_ASSERT(connect(ui.radioButton_Overwrite, &QRadioButton::toggled, this,
        [=](int value) {
            int preset_value = preset_list.at(CurrentSelectedPreset()).save_file_policy_option;
            save_options_change_tracker = TrackOptionChanges(
                save_options_change_tracker, Option.radioButton_Overwrite,
                preset_value, (value) ? ImageSaver::OVERWRITE : preset_value
            );
        }));
    Q_ASSERT(connect(ui.radioButton_RenameOriginal, &QRadioButton::toggled, this,
        [=](int value) {
            int preset_value = preset_list.at(CurrentSelectedPreset()).save_file_policy_option;
            save_options_change_tracker = TrackOptionChanges(
                save_options_change_tracker, Option.radioButton_RenameOriginal,
                preset_value, (value) ? ImageSaver::RENAME_ORIGINAL : preset_value
            );
        }));
    Q_ASSERT(connect(ui.radioButton_NewFileName, &QRadioButton::toggled, this,
        [=](int value) {
            int preset_value = preset_list.at(CurrentSelectedPreset()).save_file_policy_option;
            save_options_change_tracker = TrackOptionChanges(
                save_options_change_tracker, Option.radioButton_NewFileName,
                preset_value, (value) ? ImageSaver::NEW_NAME : preset_value
            );
        }));
    Q_ASSERT(connect(ui.comboBox_AddText, &QComboBox::activated, this,
        [this] {
            AddTextToFileName();
            save_options_change_tracker = TrackOptionChanges(
                save_options_change_tracker,
                Option.lineEdit_FileName,
                preset_list.at(CurrentSelectedPreset()).save_file_name_change,
                ui.lineEdit_FileName->text().toStdString()
            );
        }));
    Q_ASSERT(connect(ui.comboBox_AddText, SIGNAL(currentIndexChanged(int)), this, SLOT(UpdateComboBoxTextTips())));
    Q_ASSERT(connect(ui.lineEdit_FileName, &QLineEdit::editingFinished, this,
        [this] {
            save_options_change_tracker = TrackOptionChanges(
                save_options_change_tracker,
                Option.lineEdit_FileName, 
                preset_list.at(CurrentSelectedPreset()).save_file_name_change,
                ui.lineEdit_FileName->text().toStdString()
            );
        }));
    Q_ASSERT(connect(ui.radioButton_RelativePath, &QRadioButton::toggled, this,
        [=](int value) {
            int preset_value = preset_list.at(CurrentSelectedPreset()).relative_save_path;
            save_options_change_tracker = TrackOptionChanges(
                save_options_change_tracker, Option.radioButton_RelativePath,
                preset_value, (value) ? 1 : preset_value
            );
            if (value) {
                save_options_change_tracker = TrackOptionChanges(
                    save_options_change_tracker,
                    Option.lineEdit_RelativePath,
                    preset_list.at(CurrentSelectedPreset()).save_file_path_change,
                    ui.lineEdit_RelativePath->text().toStdString()
                );
                save_options_change_tracker = RemoveOptionsChanged(save_options_change_tracker, std::vector<uint>{
                    Option.lineEdit_AbsolutePath
                });
            }
        }));
    Q_ASSERT(connect(ui.radioButton_AbsolutePath, &QRadioButton::toggled, this,
        [=](int value) {
            int preset_value = preset_list.at(CurrentSelectedPreset()).relative_save_path;
            save_options_change_tracker = TrackOptionChanges(
                save_options_change_tracker, Option.radioButton_AbsolutePath,
                preset_value, (value) ? 0 : preset_value
            );
            if (value) {
                save_options_change_tracker = TrackOptionChanges(
                    save_options_change_tracker,
                    Option.lineEdit_AbsolutePath,
                    preset_list.at(CurrentSelectedPreset()).save_file_path_change,
                    ui.lineEdit_AbsolutePath->text().toStdString()
                );
                save_options_change_tracker = RemoveOptionsChanged(save_options_change_tracker, std::vector<uint>{
                    Option.lineEdit_RelativePath
                });
            }
        }));
    Q_ASSERT(connect(ui.lineEdit_RelativePath, &QLineEdit::editingFinished, this,
        [this] {
            CheckRelativePath();
            save_options_change_tracker = TrackOptionChanges(
                save_options_change_tracker,
                Option.lineEdit_RelativePath,
                preset_list.at(CurrentSelectedPreset()).save_file_path_change,
                ui.lineEdit_RelativePath->text().toStdString()
            );
        }));
    Q_ASSERT(connect(ui.pushButton_AddBackOneDir, &QAbstractButton::pressed, this,
        [this] {
            ui.lineEdit_RelativePath->setText(ui.lineEdit_RelativePath->text().prepend("../"));
            CheckRelativePath();
            if (ui.radioButton_RelativePath->isChecked()) {
                save_options_change_tracker = TrackOptionChanges(
                    save_options_change_tracker,
                    Option.lineEdit_RelativePath,
                    preset_list.at(CurrentSelectedPreset()).save_file_path_change,
                    ui.lineEdit_RelativePath->text().toStdString()
                );
            }
        }));
    Q_ASSERT(connect(ui.lineEdit_AbsolutePath, &QLineEdit::editingFinished, this,
        [this] {
            CheckAbsolutePath();
            save_options_change_tracker = TrackOptionChanges(
                save_options_change_tracker,
                Option.lineEdit_AbsolutePath,
                preset_list.at(CurrentSelectedPreset()).save_file_path_change,
                ui.lineEdit_AbsolutePath->text().toStdString()
            );
        }));
    Q_ASSERT(connect(ui.pushButton_FindAbsolutePath, &QAbstractButton::pressed, this,
        [this] {
            ui.lineEdit_AbsolutePath->setText(GetSaveDirectoryPath());
            if (ui.radioButton_AbsolutePath->isChecked()) {
                save_options_change_tracker = TrackOptionChanges(
                    save_options_change_tracker,
                    Option.lineEdit_AbsolutePath,
                    preset_list.at(CurrentSelectedPreset()).save_file_path_change,
                    ui.lineEdit_AbsolutePath->text().toStdString()
                );
            }
        }));
    Q_ASSERT(connect(ui.groupBox_ChangeFormat, &QGroupBox::toggled, this,
        [=](int value) {
            int preset_value = preset_list.at(CurrentSelectedPreset()).format_change;
            save_options_change_tracker = TrackOptionChanges(save_options_change_tracker,
                Option.comboBox_BlurFilter, preset_value, (value) ? 1 : 0
            );
            EnableSpecificFormatOptions();
            // If already "off/None" remove below changed options.
            // TODO: other options once changed and changed back, could modify an option it doesn't use and flag a false change. fix?
            if (value == 0 and value == preset_value) {
                save_options_change_tracker = RemoveOptionsChanged(save_options_change_tracker, std::vector<uint>{
                    Option.comboBox_ImageFormat, Option.comboBox_FormatFlags, Option.horizontalSlider_Quality, Option.checkBox_Optimize,
                        Option.checkBox_Progressive, Option.spinBox_Compression, Option.spinBox_ExtraSetting1, Option.spinBox_ExtraSetting2
                });
            }
        }));
    Q_ASSERT(connect(ui.comboBox_ImageFormat, &QComboBox::currentIndexChanged, this,
        [=](int index) {
            UpdateComboBoxTextTips();
            EnableSpecificFormatOptions();
            save_options_change_tracker = TrackOptionChanges(
                save_options_change_tracker, Option.comboBox_ImageFormat,
                preset_list.at(CurrentSelectedPreset()).format_extension, index
            );
        }));
    Q_ASSERT(connect(ui.comboBox_FormatFlags, &QComboBox::currentIndexChanged, this,
        [=](int index) {
            UpdateComboBoxTextTips();
            if (ui.comboBox_ImageFormat->currentData() == ".exr") { // TODO: track this special case? .exr currently doesn't work/not suported anyways.
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
            save_options_change_tracker = TrackOptionChanges(
                save_options_change_tracker, Option.comboBox_FormatFlags,
                preset_list.at(CurrentSelectedPreset()).format_format_flag, index
            );

        }));
    Q_ASSERT(connect(ui.horizontalSlider_Quality, &QSlider::valueChanged, this,
        [=](int value) {
            save_options_change_tracker = TrackOptionChanges(
                save_options_change_tracker, Option.horizontalSlider_Quality,
                preset_list.at(CurrentSelectedPreset()).format_quality, value
            );
        }));
    Q_ASSERT(connect(ui.checkBox_Optimize, &QCheckBox::stateChanged, this,
        [=](int value) {
            save_options_change_tracker = TrackOptionChanges(
                save_options_change_tracker, Option.checkBox_Optimize,
                preset_list.at(CurrentSelectedPreset()).format_optimize, (value) ? 1 : 0
            );
        }));
    Q_ASSERT(connect(ui.checkBox_Progressive, &QCheckBox::stateChanged, this,
        [=](int value) {
            save_options_change_tracker = TrackOptionChanges(
                save_options_change_tracker, Option.checkBox_Progressive,
                preset_list.at(CurrentSelectedPreset()).format_progressive, (value) ? 1 : 0
            );
        }));
    Q_ASSERT(connect(ui.spinBox_Compression, &QSpinBox::valueChanged, this,
        [=](int value) {
            save_options_change_tracker = TrackOptionChanges(
                save_options_change_tracker, Option.spinBox_Compression,
                preset_list.at(CurrentSelectedPreset()).format_compression, value
            );
        }));
    Q_ASSERT(connect(ui.spinBox_ExtraSetting1, &QSpinBox::valueChanged, this,
        [=](int value) {
            save_options_change_tracker = TrackOptionChanges(
                save_options_change_tracker, Option.spinBox_ExtraSetting1,
                preset_list.at(CurrentSelectedPreset()).format_extra1, value
            );
        }));
    Q_ASSERT(connect(ui.spinBox_ExtraSetting2, &QSpinBox::valueChanged, this,
        [=](int value) {
            save_options_change_tracker = TrackOptionChanges(
                save_options_change_tracker, Option.spinBox_ExtraSetting2,
                preset_list.at(CurrentSelectedPreset()).format_extra2, value
            );
        }));

    // Other Widgets
    Q_ASSERT(connect(ui.pushButton_EditAndSave, SIGNAL(clicked(bool)), this, SLOT(EditAndSave())));
    Q_ASSERT(connect(this, SIGNAL(progressMade(float)), ui.enhancedProgressBar, SLOT(updateProgressBar(float))));

    //qDebug().noquote() << Option.printAllTrackerFlags();
}

ulong BatchItImage::TrackOptionChanges(ulong tracker, uint tracked_option, int preset_value, int changed_option_value)
{
    bool option_changed = tracker & tracked_option;
    if (changed_option_value != preset_value and not option_changed) {
        tracker += tracked_option;
    }
    else if (changed_option_value == preset_value and option_changed) {
        tracker -= tracked_option;
    }
    qDebug() << "Options Tracker: " << tracker;
    return tracker;
}

ulong BatchItImage::TrackOptionChanges(ulong tracker, uint tracked_option, std::string preset_string, std::string changed_option_string)
{
    bool option_changed = tracker & tracked_option;
    if (changed_option_string != preset_string and not option_changed) {
        tracker += tracked_option;
    }
    else if (changed_option_string == preset_string and option_changed) {
        tracker -= tracked_option;
    }
    qDebug() << "Options Tracker: " << tracker;
    return tracker;
}

ulong BatchItImage::RemoveOptionsChanged(ulong tracker, std::vector<uint> tracked_options)
{
    for (auto& tracked_option : tracked_options) {
        if (tracker & tracked_option) {
            tracker -= tracked_option;
        }
    }
    qDebug() << "Options Tracker (group removed): " << tracker;
    return tracker;
}

void BatchItImage::RemoveOptionsChanged()
{
    edit_options_change_tracker = 0;
    save_options_change_tracker = 0;
    qDebug() << "Options Tracker (all removed): 0";
}

void BatchItImage::EnableSpecificBlurOptions(bool loading_preset)
{
    int blur_filter_selected = ui.comboBox_BlurFilter->currentData().toInt();

    int NONE = 0;
    int NORMALIZE = 1;
    int X1 = 2;
    int Y1 = 4;
    int X2 = 8;
    int Y2 = 16;
    int DEPTH = 32;
    auto enableOptions = [&](int options)
        {
            if (options & NORMALIZE) {
                ui.checkBox_BlurNormalize->setEnabled(true);
                ui.checkBox_BlurNormalize->setFont(*font_default);
            }
            else {
                ui.checkBox_BlurNormalize->setToolTip("");
                ui.checkBox_BlurNormalize->setEnabled(false);
                ui.checkBox_BlurNormalize->setFont(*font_default_light);
            }
            if (options & X1) {
                ui.label_BlurX1->setFont(*font_default);
                ui.verticalSlider_BlurX1->setEnabled(true, true);
            }
            else {
                ui.label_BlurX1->setStatusTip("");
                ui.label_BlurX1->setToolTip("");
                ui.label_BlurX1->setFont(*font_default_light);
                ui.verticalSlider_BlurX1->setEnabled(false, true);
            }
            if (options & Y1) {
                ui.label_BlurY1->setFont(*font_default);
                ui.verticalSlider_BlurY1->setEnabled(true, true);
            }
            else {
                ui.label_BlurY1->setStatusTip("");
                ui.label_BlurY1->setToolTip("");
                ui.label_BlurY1->setFont(*font_default_light);
                ui.verticalSlider_BlurY1->setEnabled(false, true);
            }
            if (options & X2) {
                ui.label_BlurX2->setFont(*font_default);
                ui.verticalSlider_BlurX2->setEnabled(true, true);
            }
            else {
                ui.label_BlurX2->setStatusTip("");
                ui.label_BlurX2->setToolTip("");
                ui.label_BlurX2->setFont(*font_default_light);
                ui.verticalSlider_BlurX2->setEnabled(false, true);
            }
            if (options & Y2) {
                ui.label_BlurY2->setFont(*font_default);
                ui.verticalSlider_BlurY2->setEnabled(true, true);
                ui.verticalSlider_BlurY2->forceSingleStepInterval(false);
                ui.verticalSlider_BlurY2->setSingleStep(1);
            }
            else {
                ui.label_BlurY2->setStatusTip("");
                ui.label_BlurY2->setToolTip("");
                ui.label_BlurY2->setFont(*font_default_light);
                ui.verticalSlider_BlurY2->setEnabled(false, true);
            }
            if (options & DEPTH) {
                ui.label_BlurD->setFont(*font_default);
                ui.verticalSlider_BlurD->setEnabled(true, true);
            }
            else {
                ui.label_BlurD->setStatusTip("");
                ui.label_BlurD->setToolTip("");
                ui.label_BlurD->setFont(*font_default_light);
                ui.verticalSlider_BlurD->setEnabled(false, true);
            }
        };

    if (blur_filter_selected == ImageEditor::BLUR_FILTER) {
        // Unused, BOX_FILTER covers anything BLUR_FILTER can do.
    }
    else if (blur_filter_selected == ImageEditor::BOX_FILTER) {
        enableOptions(NORMALIZE + X1 + Y1 + X2 + Y2 + DEPTH);
        ui.verticalSlider_BlurX1->setRange(0, 100);
        ui.verticalSlider_BlurY1->setRange(0, 100);
        ui.verticalSlider_BlurX2->setRange(-20, 20);
        ui.verticalSlider_BlurX2->addTextTip(-1, -1, " (Image Center)", true, EnhancedSlider::TextTipPlacement::ToTheRight);
        ui.verticalSlider_BlurY2->setRange(-20, 20);
        ui.verticalSlider_BlurY2->addTextTip(-1, -1, " (Image Center)", true, EnhancedSlider::TextTipPlacement::ToTheRight);
        ui.verticalSlider_BlurD->setRange(-1, 6);
        ui.verticalSlider_BlurD->addTextTip(
            blur_depth_selections[0].data, //-1
            blur_depth_selections[0].data,
            blur_depth_selections[0].name,
            false
        );
        ui.verticalSlider_BlurD->addTextTip(
            blur_depth_selections[1].data, //0
            blur_depth_selections[1].data,
            blur_depth_selections[1].name,
            false
        );
        ui.verticalSlider_BlurD->addTextTip(
            blur_depth_selections[2].data - 1,
            blur_depth_selections[2].data, //2
            blur_depth_selections[2].name,
            false
        );
        ui.verticalSlider_BlurD->addTextTip(
            blur_depth_selections[3].data, //3
            blur_depth_selections[3].data + 1,
            blur_depth_selections[3].name,
            false
        );
        ui.verticalSlider_BlurD->addTextTip(
            blur_depth_selections[4].data, //5
            blur_depth_selections[4].data,
            blur_depth_selections[4].name,
            false
        );
        ui.verticalSlider_BlurD->addTextTip(
            blur_depth_selections[5].data, //6
            blur_depth_selections[5].data,
            blur_depth_selections[5].name,
            false
        );
        if (not loading_preset) {
            ui.checkBox_BlurNormalize->setChecked(true);
            ui.verticalSlider_BlurX1->setValue(1);
            ui.verticalSlider_BlurY1->setValue(1);
            ui.verticalSlider_BlurX2->setValue(-1);
            ui.verticalSlider_BlurY2->setValue(-1);
            ui.verticalSlider_BlurD->setValue(-1);
        }
    }
    else if (blur_filter_selected == ImageEditor::BILATERAL_FILTER) {
        enableOptions(X1 + X2 + Y2);
        ui.verticalSlider_BlurX1->setRange(0, 12);
        ui.verticalSlider_BlurX2->setRange(0, 300);
        ui.verticalSlider_BlurY2->setRange(0, 300);
        if (not loading_preset) {
            ui.verticalSlider_BlurX1->setValue(0);
            ui.verticalSlider_BlurX2->setValue(0);
            ui.verticalSlider_BlurY2->setValue(0);
        }
    }
    else if (blur_filter_selected == ImageEditor::GAUSSIAN_BLUR) {
        enableOptions(X1 + Y1 + X2 + Y2);
        ui.verticalSlider_BlurX1->forceSingleStepInterval(true);
        ui.verticalSlider_BlurX1->setRange(1, 99);
        ui.verticalSlider_BlurX1->setSingleStep(2);
        ui.verticalSlider_BlurY1->forceSingleStepInterval(true);
        ui.verticalSlider_BlurY1->setRange(1, 99);
        ui.verticalSlider_BlurY1->setSingleStep(2);
        ui.verticalSlider_BlurX2->setRange(0.0, 10.0);
        ui.verticalSlider_BlurX2->setSingleStep(0.1);
        ui.verticalSlider_BlurY2->setRange(0.0, 10.0);
        ui.verticalSlider_BlurY2->setSingleStep(0.1);
        if (not loading_preset) {
            ui.verticalSlider_BlurX1->setValue(1);
            ui.verticalSlider_BlurY1->setValue(1);
            ui.verticalSlider_BlurX2->setValue(0.0);
            ui.verticalSlider_BlurY2->setValue(0.0);
        }
    }
    else if (blur_filter_selected == ImageEditor::MEDIAN_BLUR) {
        enableOptions(X1);
        ui.verticalSlider_BlurX1->forceSingleStepInterval(true);
        ui.verticalSlider_BlurX1->setRange(3, 99);
        ui.verticalSlider_BlurX1->setSingleStep(2);
        if (not loading_preset) {
            ui.verticalSlider_BlurX1->setValue(3);
        }
    }
    else if (blur_filter_selected == ImageEditor::PYR_DOWN_BLUR) {
        enableOptions(NONE);
    }
    else if (blur_filter_selected == ImageEditor::PYR_UP_BLUR) {
        enableOptions(NONE);
    }
    else {
        enableOptions(NONE);
    }
}

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
                ui.label_FormatFlags->setText("");
                ui.label_FormatFlags->setStatusTip("");
                ui.label_FormatFlags->setToolTip("");
                ui.label_FormatFlags->setFont(*font_default_light);
                ui.comboBox_FormatFlags->setStatusTip("");
                ui.comboBox_FormatFlags->setToolTip("");
                ui.comboBox_FormatFlags->setEnabled(false);
                ui.comboBox_FormatFlags->setFont(*font_default_light);
                
            }
            if (options & QUALITY) {
                ui.label_Quality->setFont(*font_default);
                ui.horizontalSlider_Quality->setEnabled(true);
            }
            else {
                ui.label_Quality->setText("");
                ui.label_Quality->setStatusTip("");
                ui.label_Quality->setToolTip("");
                ui.label_Quality->setFont(*font_default_light);
                ui.horizontalSlider_Quality->setStatusTip("");
                ui.horizontalSlider_Quality->setToolTip("");
                ui.horizontalSlider_Quality->setEnabled(false);
            }
            if (options & OPTIMIZE) {
                ui.checkBox_Optimize->setAutoExclusive(false);
                ui.checkBox_Optimize->setFont(*font_default);
                ui.checkBox_Optimize->setEnabled(true);
            }
            else {
                ui.checkBox_Optimize->setText("");
                ui.checkBox_Optimize->setStatusTip("");
                ui.checkBox_Optimize->setToolTip("");
                ui.checkBox_Optimize->setFont(*font_default_light);
                ui.checkBox_Optimize->setEnabled(false);
            }
            if (options & PROGRESSIVE) {
                ui.checkBox_Progressive->setAutoExclusive(false);
                ui.checkBox_Progressive->setFont(*font_default);
                ui.checkBox_Progressive->setEnabled(true);
            }
            else {
                ui.checkBox_Progressive->setText("");
                ui.checkBox_Progressive->setStatusTip("");
                ui.checkBox_Progressive->setToolTip("");
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
                ui.label_Compression->setText("");
                ui.label_Compression->setStatusTip("");
                ui.label_Compression->setToolTip("");
                ui.label_Compression->setFont(*font_default_light);
                ui.spinBox_Compression->setStatusTip("");
                ui.spinBox_Compression->setToolTip("");
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
                ui.label_ExtraSetting1->setText("");
                ui.label_ExtraSetting1->setStatusTip("");
                ui.label_ExtraSetting1->setToolTip("");
                ui.label_ExtraSetting1->setFont(*font_default_light);
                ui.spinBox_ExtraSetting1->setStatusTip("");
                ui.spinBox_ExtraSetting1->setToolTip("");
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
                ui.label_ExtraSetting2->setText("");
                ui.label_ExtraSetting2->setStatusTip("");
                ui.label_ExtraSetting2->setToolTip("");
                ui.label_ExtraSetting2->setFont(*font_default_light);
                ui.spinBox_ExtraSetting2->setStatusTip("");
                ui.spinBox_ExtraSetting2->setToolTip("");
                ui.spinBox_ExtraSetting2->setEnabled(false);
                ui.spinBox_ExtraSetting2->setFont(*font_default_light);
            }
        };

    if (format == ".jpeg" or format == ".jpg" or format == ".jpe") { 
        
        enableOptions(FORMATFLAGS + QUALITY + OPTIMIZE + PROGRESSIVE + COMPRESSION + EXTRASETTING1 + EXTRASETTING2);
        
        ui.label_FormatFlags->setText(format_jpeg_options[FormatJpegOptions::label_FormatFlags].name);
        ui.label_FormatFlags->setStatusTip(format_jpeg_options[FormatJpegOptions::label_FormatFlags].desc);
        ui.label_FormatFlags->setToolTip(format_jpeg_options[FormatJpegOptions::label_FormatFlags].desc);
        ui.label_Quality->setText(format_jpeg_options[FormatJpegOptions::label_Quality].name);
        ui.label_Quality->setStatusTip(format_jpeg_options[FormatJpegOptions::label_Quality].desc);
        ui.label_Quality->setToolTip(format_jpeg_options[FormatJpegOptions::label_Quality].desc);
        ui.checkBox_Optimize->setText(format_jpeg_options[FormatJpegOptions::checkBox_Optimize].name);
        ui.checkBox_Optimize->setStatusTip(format_jpeg_options[FormatJpegOptions::checkBox_Optimize].desc);
        ui.checkBox_Optimize->setToolTip(format_jpeg_options[FormatJpegOptions::checkBox_Optimize].desc);
        ui.checkBox_Progressive->setText(format_jpeg_options[FormatJpegOptions::checkBox_Progressive].name);
        ui.checkBox_Progressive->setStatusTip(format_jpeg_options[FormatJpegOptions::checkBox_Progressive].desc);
        ui.checkBox_Progressive->setToolTip(format_jpeg_options[FormatJpegOptions::checkBox_Progressive].desc);
        ui.label_Compression->setText(format_jpeg_options[FormatJpegOptions::label_Compression].name);
        ui.label_Compression->setStatusTip(format_jpeg_options[FormatJpegOptions::label_Compression].desc);
        ui.label_Compression->setToolTip(format_jpeg_options[FormatJpegOptions::label_Compression].desc);
        ui.label_ExtraSetting1->setText(format_jpeg_options[FormatJpegOptions::label_ExtraSetting1].name);
        ui.label_ExtraSetting1->setStatusTip(format_jpeg_options[FormatJpegOptions::label_ExtraSetting1].desc);
        ui.label_ExtraSetting1->setToolTip(format_jpeg_options[FormatJpegOptions::label_ExtraSetting1].desc);
        ui.label_ExtraSetting2->setText(format_jpeg_options[FormatJpegOptions::label_ExtraSetting2].name);
        ui.label_ExtraSetting2->setStatusTip(format_jpeg_options[FormatJpegOptions::label_ExtraSetting2].desc);
        ui.label_ExtraSetting2->setToolTip(format_jpeg_options[FormatJpegOptions::label_ExtraSetting2].desc);

        if (loading_preset)
            //PopulateComboBox(ui.comboBox_FormatFlags, format_jpeg_subsamplings, sizeof(format_jpeg_subsamplings) / sizeof(UIData));
            PopulateComboBox(ui.comboBox_FormatFlags, format_jpeg_subsamplings);
        
        int default_quality_value = format_jpeg_options[FormatJpegOptions::label_Quality].data;
        ui.horizontalSlider_Quality->setRange(0, 100);
        ui.horizontalSlider_Quality->addTextTip(default_quality_value, default_quality_value, " : Default", true);
        ui.spinBox_Compression->setRange(0, 65535);
        ui.spinBox_ExtraSetting1->setRange(-1, 100);
        ui.spinBox_ExtraSetting2->setRange(-1, 100);

        if (not loading_preset and last_selected_format != ".jpeg" and last_selected_format != ".jpg" and last_selected_format != ".jpe") {
            PopulateComboBox(ui.comboBox_FormatFlags, format_jpeg_subsamplings);
            ui.comboBox_FormatFlags->setCurrentIndex(format_jpeg_options[FormatJpegOptions::label_FormatFlags].data);
            ui.horizontalSlider_Quality->setValue(default_quality_value);
            ui.checkBox_Optimize->setChecked(format_jpeg_options[FormatJpegOptions::checkBox_Optimize].data);
            ui.checkBox_Progressive->setChecked(format_jpeg_options[FormatJpegOptions::checkBox_Progressive].data);
            ui.spinBox_Compression->setValue(format_jpeg_options[FormatJpegOptions::label_Compression].data);
            ui.spinBox_ExtraSetting1->setValue(format_jpeg_options[FormatJpegOptions::label_ExtraSetting1].data);
            ui.spinBox_ExtraSetting2->setValue(format_jpeg_options[FormatJpegOptions::label_ExtraSetting2].data);
        }
    }
    else if (format == ".jp2") {
        
        enableOptions(COMPRESSION);

        ui.label_Compression->setText(format_jp2_options[FormatJp2Options::label_Compression].name);
        ui.label_Compression->setStatusTip(format_jp2_options[FormatJp2Options::label_Compression].desc);
        ui.label_Compression->setToolTip(format_jp2_options[FormatJp2Options::label_Compression].desc);

        ui.spinBox_Compression->setRange(0, 1000);

        if (not loading_preset and last_selected_format != ".jp2") {
            ui.spinBox_Compression->setValue(format_jp2_options[FormatJp2Options::label_Compression].data);
        }
    }
    else if (format == ".png") {
        
        enableOptions(FORMATFLAGS + OPTIMIZE + COMPRESSION);

        ui.label_FormatFlags->setText(format_png_options[FormatPngOptions::label_FormatFlags].name);
        ui.label_FormatFlags->setStatusTip(format_png_options[FormatPngOptions::label_FormatFlags].desc);
        ui.label_FormatFlags->setToolTip(format_png_options[FormatPngOptions::label_FormatFlags].desc);
        ui.checkBox_Optimize->setText(format_png_options[FormatPngOptions::checkBox_Optimize].name);
        ui.checkBox_Optimize->setStatusTip(format_png_options[FormatPngOptions::checkBox_Optimize].desc);
        ui.checkBox_Optimize->setToolTip(format_png_options[FormatPngOptions::checkBox_Optimize].desc);
        ui.label_Compression->setText(format_png_options[FormatPngOptions::label_Compression].name);
        ui.label_Compression->setStatusTip(format_png_options[FormatPngOptions::label_Compression].desc);
        ui.label_Compression->setToolTip(format_png_options[FormatPngOptions::label_Compression].desc);

        PopulateComboBox(ui.comboBox_FormatFlags, format_png_compression);
        ui.spinBox_Compression->setRange(0, 9);

        if (not loading_preset and last_selected_format != ".png") {
            ui.comboBox_FormatFlags->setCurrentIndex(format_png_options[FormatPngOptions::label_FormatFlags].data);
            ui.checkBox_Optimize->setChecked(format_png_options[FormatPngOptions::checkBox_Optimize].data);
            ui.spinBox_Compression->setValue(format_png_options[FormatPngOptions::label_Compression].data);
        }
    }
    else if (format == ".webp") { 

        enableOptions(QUALITY);

        ui.label_Quality->setText(format_webp_options[FormatWebpOptions::label_Quality].name);
        ui.label_Quality->setStatusTip(format_webp_options[FormatWebpOptions::label_Quality].desc);
        ui.label_Quality->setToolTip(format_webp_options[FormatWebpOptions::label_Quality].desc);

        int default_quality_value = format_webp_options[FormatWebpOptions::label_Quality].data;
        ui.horizontalSlider_Quality->setRange(1, 100);
        ui.horizontalSlider_Quality->addTextTip(default_quality_value, default_quality_value, " : Default", true);

        if (not loading_preset and last_selected_format != ".webp") {
            ui.horizontalSlider_Quality->setValue(default_quality_value);
        }
    }
    else if (format == ".avif") { // TODO: test  IMWRITE_JPEG_SAMPLING_FACTOR

        enableOptions(QUALITY + COMPRESSION + EXTRASETTING2);

        //ui.label_FormatFlags->setText(":");
        ui.label_Quality->setText(format_avif_options[FormatAvifOptions::label_Quality].name);
        ui.label_Quality->setStatusTip(format_avif_options[FormatAvifOptions::label_Quality].desc);
        ui.label_Quality->setToolTip(format_avif_options[FormatAvifOptions::label_Quality].desc);
        ui.label_Compression->setText(format_avif_options[FormatAvifOptions::label_Compression].name);
        ui.label_Compression->setStatusTip(format_avif_options[FormatAvifOptions::label_Compression].desc);
        ui.label_Compression->setToolTip(format_avif_options[FormatAvifOptions::label_Compression].desc);
        ui.label_ExtraSetting2->setText(format_avif_options[FormatAvifOptions::label_ExtraSetting2].name);
        ui.label_ExtraSetting2->setStatusTip(format_avif_options[FormatAvifOptions::label_ExtraSetting2].desc);
        ui.label_ExtraSetting2->setToolTip(format_avif_options[FormatAvifOptions::label_ExtraSetting2].desc);

        int default_quality_value = format_avif_options[FormatAvifOptions::label_Quality].data;
        ui.horizontalSlider_Quality->setRange(0, 100);
        ui.horizontalSlider_Quality->addTextTip(default_quality_value, default_quality_value, " : Default", true);
        ui.spinBox_Compression->setRange(0, 9);
        ui.spinBox_ExtraSetting2->setRange(8, 12);
        Q_ASSERT(ui.spinBox_ExtraSetting2->connect(ui.spinBox_ExtraSetting2, &QAbstractSpinBox::editingFinished, this,
            [this] { // Only 8, 10, 12
                int val = ui.spinBox_ExtraSetting2->value();
                if (val == 9 or val == 11) {
                    ui.spinBox_ExtraSetting2->setValue(10);
                }
            }));
        ui.spinBox_ExtraSetting2->setSingleStep(2);

        if (not loading_preset and last_selected_format != ".avif") {
            //ui.comboBox_FormatFlags->setCurrentIndex(0);
            ui.horizontalSlider_Quality->setValue(default_quality_value);
            ui.spinBox_Compression->setValue(format_avif_options[FormatAvifOptions::label_Compression].data);
            ui.spinBox_ExtraSetting2->setValue(format_avif_options[FormatAvifOptions::label_ExtraSetting2].data);
        }
    }
    else if (format == ".pbm" or format == ".pgm" or format == ".ppm") {

        enableOptions(OPTIMIZE);

        ui.checkBox_Optimize->setText(format_pbm_options[FormatPbmOptions::checkBox_Optimize].name);
        ui.checkBox_Optimize->setStatusTip(format_pbm_options[FormatPbmOptions::checkBox_Optimize].desc);
        ui.checkBox_Optimize->setToolTip(format_pbm_options[FormatPbmOptions::checkBox_Optimize].desc);

        if (not loading_preset and last_selected_format != ".pbm" and last_selected_format != ".pgm" and last_selected_format != ".ppm") {
            ui.checkBox_Optimize->setChecked(format_pbm_options[FormatPbmOptions::checkBox_Optimize].data);
        }
    }
    else if (format == ".pam") {

        enableOptions(FORMATFLAGS);

        ui.label_FormatFlags->setText(format_pam_options[FormatPamOptions::label_FormatFlags].name);
        ui.label_FormatFlags->setStatusTip(format_pam_options[FormatPamOptions::label_FormatFlags].desc);
        ui.label_FormatFlags->setToolTip(format_pam_options[FormatPamOptions::label_FormatFlags].desc);

        PopulateComboBox(ui.comboBox_FormatFlags, format_pam_tupletype);

        if (not loading_preset and last_selected_format != ".pam") {
            ui.comboBox_FormatFlags->setCurrentIndex(format_pam_options[FormatPamOptions::label_FormatFlags].data);
        }
    }
    else if (format == ".tiff" or format == ".tif") {

        enableOptions(FORMATFLAGS + QUALITY + EXTRASETTING1 + EXTRASETTING2);

        ui.label_FormatFlags->setText(format_tiff_options[FormatTiffOptions::label_FormatFlags].name);
        ui.label_FormatFlags->setStatusTip(format_tiff_options[FormatTiffOptions::label_FormatFlags].desc);
        ui.label_FormatFlags->setToolTip(format_tiff_options[FormatTiffOptions::label_FormatFlags].desc);
        ui.label_Quality->setText(format_tiff_options[FormatTiffOptions::label_Quality].name);
        ui.label_Quality->setStatusTip(format_tiff_options[FormatTiffOptions::label_Quality].desc);
        ui.label_Quality->setToolTip(format_tiff_options[FormatTiffOptions::label_Quality].desc);
        ui.label_ExtraSetting1->setText(format_tiff_options[FormatTiffOptions::label_ExtraSetting1].name);
        ui.label_ExtraSetting1->setStatusTip(format_tiff_options[FormatTiffOptions::label_ExtraSetting1].desc);
        ui.label_ExtraSetting1->setToolTip(format_tiff_options[FormatTiffOptions::label_ExtraSetting1].desc);
        ui.label_ExtraSetting2->setText(format_tiff_options[FormatTiffOptions::label_ExtraSetting2].name);
        ui.label_ExtraSetting2->setStatusTip(format_tiff_options[FormatTiffOptions::label_ExtraSetting2].desc);
        ui.label_ExtraSetting2->setToolTip(format_tiff_options[FormatTiffOptions::label_ExtraSetting2].desc);

        if (loading_preset)
            PopulateComboBox(ui.comboBox_FormatFlags, format_tiff_compression);
        ui.horizontalSlider_Quality->setRange(1, 3);
        for (auto& tiff_ru : format_tiff_resolution_unit) {
            ui.horizontalSlider_Quality->addTextTip(
                tiff_ru.data,
                tiff_ru.data,
                " : " + tiff_ru.name + " " + tiff_ru.desc,
                true,
                EnhancedSlider::ToTheRight
            );
        }
        ui.spinBox_ExtraSetting1->setRange(0, INT_MAX);
        ui.spinBox_ExtraSetting2->setRange(0, INT_MAX);

        if (not loading_preset and last_selected_format != ".tiff" and last_selected_format != ".tif") {
            PopulateComboBox(ui.comboBox_FormatFlags, format_tiff_compression);
            ui.comboBox_FormatFlags->setCurrentIndex(format_tiff_options[FormatTiffOptions::label_FormatFlags].data);
            ui.horizontalSlider_Quality->setValue(format_tiff_options[FormatTiffOptions::label_Quality].data);
            ui.spinBox_ExtraSetting1->setValue(format_tiff_options[FormatTiffOptions::label_ExtraSetting1].data);
            ui.spinBox_ExtraSetting2->setValue(format_tiff_options[FormatTiffOptions::label_ExtraSetting2].data);
        }
    }
    else if (format == ".exr") {

        enableOptions(FORMATFLAGS + OPTIMIZE + PROGRESSIVE + COMPRESSION);

        ui.label_FormatFlags->setText(format_exr_options[FormatExrOptions::label_FormatFlags].name);
        ui.label_FormatFlags->setStatusTip(format_exr_options[FormatExrOptions::label_FormatFlags].desc);
        ui.label_FormatFlags->setToolTip(format_exr_options[FormatExrOptions::label_FormatFlags].desc);
        ui.checkBox_Optimize->setText(format_exr_options[FormatExrOptions::checkBox_Optimize].name);
        ui.checkBox_Optimize->setStatusTip(format_exr_options[FormatExrOptions::checkBox_Optimize].desc);
        ui.checkBox_Optimize->setToolTip(format_exr_options[FormatExrOptions::checkBox_Optimize].desc);
        ui.checkBox_Progressive->setText(format_exr_options[FormatExrOptions::checkBox_Progressive].name);
        ui.checkBox_Progressive->setStatusTip(format_exr_options[FormatExrOptions::checkBox_Progressive].desc);
        ui.checkBox_Progressive->setToolTip(format_exr_options[FormatExrOptions::checkBox_Progressive].desc);
        ui.label_Compression->setText(format_exr_options[FormatExrOptions::label_Compression].name);
        ui.label_Compression->setStatusTip(format_exr_options[FormatExrOptions::label_Compression].desc);
        ui.label_Compression->setToolTip(format_exr_options[FormatExrOptions::label_Compression].desc);

        PopulateComboBox(ui.comboBox_FormatFlags, format_exr_compression);
        ui.checkBox_Optimize->setAutoExclusive(true);
        ui.checkBox_Progressive->setAutoExclusive(true);

        if (not loading_preset and last_selected_format != ".exr") {
            ui.comboBox_FormatFlags->setCurrentIndex(format_exr_options[FormatExrOptions::label_FormatFlags].data);
            ui.checkBox_Optimize->setChecked(format_exr_options[FormatExrOptions::checkBox_Optimize].data);
            ui.checkBox_Progressive->setChecked(format_exr_options[FormatExrOptions::checkBox_Progressive].data);
            ui.spinBox_Compression->setValue(format_exr_options[FormatExrOptions::label_Compression].data);
        }

    }
    else if (format == ".hdr" or format == ".pic") { 

        enableOptions(FORMATFLAGS);

        ui.label_FormatFlags->setText(format_hdr_options[FormatHdrOptions::label_FormatFlags].name);
        ui.label_FormatFlags->setStatusTip(format_hdr_options[FormatHdrOptions::label_FormatFlags].desc);
        ui.label_FormatFlags->setToolTip(format_hdr_options[FormatHdrOptions::label_FormatFlags].desc);

        if (loading_preset)
            PopulateComboBox(ui.comboBox_FormatFlags, format_hdr_compression);

        if (not loading_preset and last_selected_format != ".hdr" and last_selected_format != ".pic") {
            PopulateComboBox(ui.comboBox_FormatFlags, format_hdr_compression);
            ui.comboBox_FormatFlags->setCurrentIndex(format_hdr_options[FormatHdrOptions::label_FormatFlags].data);
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

void BatchItImage::SetupFileTreeContextMenu()
{
    action_add = new QAction(file_tree_menu_items->at(ActionMenu::action_add).name, this);
    action_delete = new QAction(file_tree_menu_items->at(ActionMenu::action_delete).name, this);
    action_clear = new QAction(file_tree_menu_items->at(ActionMenu::action_clear).name, this);
    action_select = new QAction(file_tree_menu_items->at(ActionMenu::action_select).name, this);
    action_view = new QAction(file_tree_menu_items->at(ActionMenu::action_view).name, this);
    action_preview = new QAction(file_tree_menu_items->at(ActionMenu::action_preview).name, this);

    action_add->setToolTip(file_tree_menu_items->at(ActionMenu::action_add).desc);
    action_delete->setToolTip(file_tree_menu_items->at(ActionMenu::action_delete).desc);
    action_clear->setToolTip(file_tree_menu_items->at(ActionMenu::action_clear).desc);
    action_select->setToolTip(file_tree_menu_items->at(ActionMenu::action_select).desc);
    action_view->setToolTip(file_tree_menu_items->at(ActionMenu::action_view).desc);
    action_preview->setToolTip(file_tree_menu_items->at(ActionMenu::action_preview).desc);

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

#ifdef _DEBUG
    QAction* action_debug_quick_load = new QAction("Debug Quick Load", this);
    QStringList testing_file_list;
    //testing_file_list.append(qdefault_path + R"(/test_images/01.jpg)"); // large file
    testing_file_list.append(qdefault_path + R"(/test_images/79.jpg)");
    testing_file_list.append(qdefault_path + R"(/test_images/evil_monkey.png)");
    testing_file_list.append(qdefault_path + R"(/test_images/AC01.png)");
    testing_file_list.append(qdefault_path + R"(/test_images/AC02.png)");
    connect(action_debug_quick_load, &QAction::triggered, [this, testing_file_list] { AddNewFiles(testing_file_list); });
    ui.treeWidget_FileInfo->addAction(action_debug_quick_load);
    
    QAction* action_debug_large_load = new QAction("Debug Large Load", this);
    QStringList testing_folder_list;
    testing_folder_list.append(qdefault_path + R"(/test_images)");
    connect(action_debug_large_load, &QAction::triggered, [this, testing_folder_list] { AddNewFiles(testing_folder_list); });
    ui.treeWidget_FileInfo->addAction(action_debug_large_load);
#endif // _DEBUG
}

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

void BatchItImage::ChangePreset(int index)
{
    SavePresetDialog();
    RemoveOptionsChanged();
    
    ui.comboBox_Preset_1->blockSignals(true);
    ui.comboBox_Preset_2->blockSignals(true);
    ui.comboBox_Preset_3->blockSignals(true);

    //if (index != current_selected_preset) {
    current_selected_preset = index;
    DEBUG2("current_selected_preset: ", current_selected_preset);

    ui.comboBox_Preset_1->setCurrentIndex(current_selected_preset);
    ui.comboBox_Preset_2->setCurrentIndex(current_selected_preset);
    ui.comboBox_Preset_3->setCurrentIndex(current_selected_preset);

    LoadPreset(preset_list.at(current_selected_preset));
    //}
    ui.comboBox_Preset_1->blockSignals(false);
    ui.comboBox_Preset_2->blockSignals(false);
    ui.comboBox_Preset_3->blockSignals(false);
}

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
            SavePresetToSettingsFile(i);
        }
        ui.statusbar->showMessage("All Presets Saved!", 5000);
    }
    else {
        DEBUG2("Update Settings Preset #", current_selected_preset);

        int save_option;
        if (ui.radioButton_Overwrite->isChecked()) {
            save_option = ImageSaver::OVERWRITE;
        }
        else if (ui.radioButton_RenameOriginal->isChecked()) {
            save_option = ImageSaver::RENAME_ORIGINAL;
        }
        else {
            save_option = ImageSaver::NEW_NAME;
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
        preset_list.at(current_selected_preset).border_type = ui.comboBox_BorderType->currentIndex();

        // TODO: 
        //ui.label_ColorPreview->;
        //QColor background_color = QColor(0, 0, 0, 0);
        /*preset_list.at(current_selected_preset).background_color_blue;
        preset_list.at(current_selected_preset).background_color_green;
        preset_list.at(current_selected_preset).background_color_red;
        preset_list.at(current_selected_preset).background_color_alpha;*/

        preset_list.at(current_selected_preset).blur_filter = ui.comboBox_BlurFilter->currentIndex();
        preset_list.at(current_selected_preset).blur_normalize = ui.checkBox_BlurNormalize->isChecked();
        preset_list.at(current_selected_preset).blur_x = ui.verticalSlider_BlurX1->value();
        preset_list.at(current_selected_preset).blur_y = ui.verticalSlider_BlurY1->value();
        preset_list.at(current_selected_preset).blur_sx = ui.verticalSlider_BlurX2->value();
        preset_list.at(current_selected_preset).blur_sy = ui.verticalSlider_BlurY2->value();
        preset_list.at(current_selected_preset).blur_depth = ui.verticalSlider_BlurD->value();
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

        SavePresetToSettingsFile(current_selected_preset);
    }
    RemoveOptionsChanged();
}

void BatchItImage::SavePresetToSettingsFile(int index)
{
    QSettings settings(preset_settings_file, QSettings::IniFormat);

    settings.beginGroup("Preset" + std::to_string(index));
    settings.setValue("description", preset_list.at(index).description);
    settings.setValue("width_change_selection", preset_list.at(index).width_change_selection);
    settings.setValue("width_number", preset_list.at(index).width_number);
    settings.setValue("height_change_selection", preset_list.at(index).height_change_selection);
    settings.setValue("height_number", preset_list.at(index).height_number);
    settings.setValue("keep_aspect_ratio", preset_list.at(index).keep_aspect_ratio);
    settings.setValue("resampling_filter", preset_list.at(index).resampling_filter);
    settings.setValue("border_type", preset_list.at(index).border_type);

    // TODO
    //settings.setValue("", preset_list.at(index).background_color_blue);
    //settings.setValue("", preset_list.at(index).background_color_green);
    //settings.setValue("", preset_list.at(index).background_color_red);
    //settings.setValue("", preset_list.at(index).background_color_alpha);

    settings.setValue("blur_filter", preset_list.at(index).blur_filter);
    settings.setValue("blur_normalize", preset_list.at(index).blur_normalize);
    settings.setValue("blur_x", preset_list.at(index).blur_x);
    settings.setValue("blur_y", preset_list.at(index).blur_y);
    settings.setValue("blur_sx", preset_list.at(index).blur_sx);
    settings.setValue("blur_sy", preset_list.at(index).blur_sy);
    settings.setValue("blur_depth", preset_list.at(index).blur_depth);
    settings.setValue("rotation_degrees", preset_list.at(index).rotation_degrees);
    settings.setValue("increase_boundaries", preset_list.at(index).increase_boundaries);
    settings.setValue("flip_image", preset_list.at(index).flip_image);
    settings.setValue("format_change", preset_list.at(index).format_change);
    settings.setValue("format_extension", preset_list.at(index).format_extension);
    settings.setValue("format_format_flag", preset_list.at(index).format_format_flag);
    settings.setValue("format_optimize", preset_list.at(index).format_optimize);
    settings.setValue("format_progressive", preset_list.at(index).format_progressive);
    settings.setValue("format_quality", preset_list.at(index).format_quality);
    settings.setValue("format_compression", preset_list.at(index).format_compression);
    settings.setValue("format_extra1", preset_list.at(index).format_extra1);
    settings.setValue("format_extra2", preset_list.at(index).format_extra2);
    settings.setValue("save_file_policy_option", preset_list.at(index).save_file_policy_option);
    settings.setValue("save_file_name_change", QString::fromStdString(preset_list.at(index).save_file_name_change));
    settings.setValue("relative_save_path", preset_list.at(index).relative_save_path);
    settings.setValue("save_file_path_change", QString::fromStdString(preset_list.at(index).save_file_path_change));
    settings.endGroup();
}

void BatchItImage::LoadPreset(Preset preset)
{
    // TODO: load "ALL" selected preset settings into ui

    ui.comboBox_WidthMod->setCurrentIndex(preset.width_change_selection);
    ui.spinBox_WidthNumber->setValue(preset.width_number);
    ui.comboBox_HeightMod->setCurrentIndex(preset.height_change_selection);
    ui.spinBox_HeightNumber->setValue(preset.height_number);
    ui.comboBox_Resample->setCurrentIndex(preset.resampling_filter);
    ui.checkBox_KeepAspectRatio->setChecked(preset.keep_aspect_ratio);
    ui.comboBox_BorderType->setCurrentIndex(preset.border_type);

    // TODO: 
    //ui.label_ColorPreview->setCurrentIndex(preset.border_type);
    //QColor background_color = QColor(0, 0, 0, 0);

    ui.comboBox_BlurFilter->setCurrentIndex(preset.blur_filter);
    ui.checkBox_BlurNormalize->setChecked(preset.blur_normalize);
    ui.verticalSlider_BlurX1->setValue(preset.blur_x);
    ui.verticalSlider_BlurY1->setValue(preset.blur_y);
    ui.verticalSlider_BlurX2->setValue(preset.blur_sx);
    ui.verticalSlider_BlurY2->setValue(preset.blur_sy);
    ui.verticalSlider_BlurD->setValue(preset.blur_depth);
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
    if (save_option == ImageSaver::OVERWRITE) {
        ui.radioButton_Overwrite->setChecked(true);
    }
    else if (save_option == ImageSaver::RENAME_ORIGINAL) {
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

    EnableSpecificBlurOptions(true);
    EnableSpecificFormatOptions(true);
}

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
    preset2.save_file_policy_option = ImageSaver::RENAME_ORIGINAL;
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
            preset.resampling_filter = settings.value("resampling_filter").toInt();
            preset.keep_aspect_ratio = settings.value("keep_aspect_ratio").toBool();
            preset.border_type = settings.value("border_type").toInt();
            //TODO:
            /*preset.background_color_blue = settings.value("background_color_blue").toInt();
            preset.background_color_green = settings.value("background_color_green").toInt();
            preset.background_color_red = settings.value("background_color_red").toInt();
            preset.background_color_alpha = settings.value("background_color_alpha").toInt();*/
            preset.blur_filter = settings.value("blur_filter").toInt();
            preset.blur_normalize = settings.value("blur_normalize").toBool();
            preset.blur_x = settings.value("blur_x").toInt();
            preset.blur_y = settings.value("blur_y").toInt();
            preset.blur_sx = settings.value("blur_sx").toInt();
            preset.blur_sx = settings.value("blur_sx").toInt();
            preset.blur_depth = settings.value("blur_depth").toInt();
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
    AddPresetsToComboBox(&preset_list, std::vector<QComboBox*>{ ui.comboBox_Preset_1, ui.comboBox_Preset_2, ui.comboBox_Preset_3 });
    ChangePreset(cspi);

    // Load selected preset data into ui.
    LoadPreset(preset_list.at(current_selected_preset));
}

void BatchItImage::CreateNewPreset()
{
    SavePresetDialog();
    RemoveOptionsChanged();
    int new_preset_index = preset_list.size();
    qDebug() << "CreateNewPreset: " << new_preset_index;
    
    Preset new_preset;
    new_preset.index = new_preset_index;
    new_preset.description = "New Preset";
    preset_list.push_back({ new_preset });

    ChangePresetDescription(
        new_preset_index,
        dialog_messages.at(DialogMessages::CreateNewPreset).name,
        dialog_messages.at(DialogMessages::CreateNewPreset).name
    );
}

bool BatchItImage::SavePresetDialog(bool include_cancel_buttons, bool closing)
{
    if (edit_options_change_tracker or save_options_change_tracker) {
        QString title;
        QString message;
        QDialogButtonBox::StandardButtons buttons = QDialogButtonBox::NoButton;
        uint custom_buttons = DialogMessage::CustomButton::NoCustomButton;

        if (closing) {
            title = dialog_messages.at(DialogMessages::save_preset_dialog_closing).name;
            message = dialog_messages.at(DialogMessages::save_preset_dialog_closing).desc;
            buttons = QDialogButtonBox::Cancel;
            custom_buttons = DialogMessage::CustomButton::SaveClose
                | DialogMessage::CustomButton::Close;
        }
        else {
            title = dialog_messages.at(DialogMessages::save_preset_dialog).name;
            message = dialog_messages.at(DialogMessages::save_preset_dialog).desc;
            if (include_cancel_buttons) {
                buttons = QDialogButtonBox::Cancel;
                custom_buttons = DialogMessage::CustomButton::SaveContinue
                    | DialogMessage::CustomButton::Continue
                    | DialogMessage::CustomButton::ResetCancel;
            }
            else {
                custom_buttons = DialogMessage::CustomButton::SaveContinue
                    | DialogMessage::CustomButton::Continue;
            }
        }
        auto* save_preset_dialog = new DialogMessage(title, message, buttons, custom_buttons, this);
        bool abort = false;
        bool* abort_p = &abort;
        Q_ASSERT(connect(save_preset_dialog, &DialogMessage::buttonRoleClicked, this,
            [=](QDialogButtonBox::ButtonRole button_role_clicked) {
                if (QDialogButtonBox::ApplyRole == button_role_clicked) { // Save and Continue
                    qDebug() << "ApplyRole";
                    SavePreset();
                    //RemoveOptionsChanged();
                }
                else if (QDialogButtonBox::AcceptRole == button_role_clicked) { // Don't Save and Continue
                    qDebug() << "AcceptRole";
                    //save_preset_dialog->close();
                    //RemoveOptionsChanged();
                }
                else if (QDialogButtonBox::ResetRole == button_role_clicked) { // Revert Changes and Don't Continue
                    qDebug() << "ResetRole";
                    // Reload saved preset
                    LoadPreset(preset_list.at(CurrentSelectedPreset()));
                    RemoveOptionsChanged();
                    //save_preset_dialog->close();
                    bool abort = true;
                    *abort_p = abort;
                }
                else if (QDialogButtonBox::RejectRole == button_role_clicked or 
                    QDialogButtonBox::DestructiveRole == button_role_clicked) { // Don't Save and Don't Continue
                    qDebug() << "DestructiveRole";
                    bool abort = true;
                    *abort_p = abort;
                }
                save_preset_dialog->deleteLater();
            }
        ));
        save_preset_dialog->exec();
        return abort;
    }
    return false;
}

void BatchItImage::ChangePresetDescription(int selected_preset_index, QString title, QString message)
{
    SavePresetDialog();
    RemoveOptionsChanged();

    int* preset_index = new int(selected_preset_index);
    qDebug() << "ChangePresetDescription: " << *preset_index;

    auto* change_preset_desc_dialog = new DialogEditPresetDesc(title, message, &preset_list, *preset_index, this);
    Q_ASSERT(connect(change_preset_desc_dialog, &DialogEditPresetDesc::presetIndexSelected, this,
        [=](int index) { *preset_index = index; change_preset_desc_dialog->deleteLater(); }));

    change_preset_desc_dialog->exec();

    AddPresetsToComboBox(&preset_list, std::vector<QComboBox*>{
        ui.comboBox_Preset_1, ui.comboBox_Preset_2, ui.comboBox_Preset_3
    });
    ChangePreset(*preset_index);
    SavePreset(true);
    delete preset_index;
}

void BatchItImage::AddPresetsToComboBox(std::vector<Preset>* preset_list, std::vector<QComboBox*> preset_cb) // Static
{
    for (uint x = 0; x < preset_cb.size(); x++) {
        preset_cb.at(x)->blockSignals(true);
        preset_cb.at(x)->clear();

        for (int i = 0; i < preset_list->size(); i++) {

            QString preset_text = "[Preset #" + QVariant(i + 1).toString() + "] ";

            preset_cb.at(x)->insertItem(i,
                preset_text + preset_list->at(i).description,
                QString::fromStdString("Preset" + std::to_string(i))
            );
            preset_cb.at(x)->setItemData(i,
                preset_list->at(i).description,
                Qt::ToolTipRole
            );
            preset_cb.at(x)->setItemData(i,
                preset_list->at(i).description,
                Qt::StatusTipRole
            );
        }
        preset_cb.at(x)->setStatusTip(preset_cb.at(x)->currentData(Qt::StatusTipRole).toString());
        preset_cb.at(x)->setToolTip(preset_cb.at(x)->currentData(Qt::ToolTipRole).toString());
        preset_cb.at(x)->blockSignals(false);
    }
}

uint BatchItImage::CurrentSelectedPreset()
{
    return current_selected_preset;
}

void BatchItImage::EditAndSave()
{
    DEBUG("Edit And Save...");
    int file_count = current_file_metadata_list.size();
    
    // TODO: if current preset settings are not saved, ask to save them now before editing images
    // Save Current, Save New, Cancel... all preset ui elements would need to trigger a "changed and not saved" flag
    if (SavePresetDialog(true) or file_count == 0) {
        DEBUG("Edit And Save... Aborted");
        return;
    }
    ui.enhancedProgressBar->configure(file_count, 3.0f);

    for (int i = 0; i < file_count; i++) {
        //DEBUG(current_file_metadata_list.at(i).to_string());

        // Setup the image editor with a file path and all the edits to be done.
        ImageEditor* new_ie = new ImageEditor(
            current_file_metadata_list.at(i).path,
            ui.comboBox_WidthMod->currentIndex(),
            ui.spinBox_WidthNumber->value(),
            ui.comboBox_HeightMod->currentIndex(),
            ui.spinBox_HeightNumber->value(),
            ui.checkBox_KeepAspectRatio->isChecked(),
            ui.comboBox_Resample->currentData().toInt(),
            ui.dial_Rotation->value(),
            ui.checkBox_IncreaseBounds->isChecked(),
            ui.checkBox_FlipImage->isChecked(),
            ui.comboBox_BorderType->currentData().toInt(),
            // TODO: background color
            ui.comboBox_BlurFilter->currentData().toInt(),
            ui.checkBox_BlurNormalize->isChecked(),
            ui.verticalSlider_BlurX1->value(),
            ui.verticalSlider_BlurY1->value(),
            ui.verticalSlider_BlurX2->value(),
            ui.verticalSlider_BlurY2->value(),
            ui.verticalSlider_BlurD->value()
        );

        // Add Callback function when edit finishes, send data to SaveImageFile()
        //new_ie->AddFinishedCallback(std::bind(&BatchItImage::SaveImageFile, this, std::placeholders::_1, std::placeholders::_2, i, new_ie));
        new_ie->AddFinishedCallback(std::bind(&BatchItImage::SaveImageFile, this, i, new_ie));

        // Start the image edit process on another thread
        //std::future<uint> worker_thread = std::async(&ImageEditor::StartEditProcess, new_ie);
        auto worker_thread = std::thread(&ImageEditor::StartEditProcess, new_ie);
 
        //int image_edits_made = worker_thread.get();
        //DEBUG2("Done: ", image_edits_made);

        emit progressMade();

        worker_thread.detach();
    }
}

void BatchItImage::SaveImageFile(int image_index, ImageEditor* image_editor)
{
    DEBUG4("SaveImageFile: ", image_index, ", Edit-Code: ", image_editor->GetImageEditsMade());
    
    emit progressMade();

    int save_file_option_flag = ImageSaver::OVERWRITE;
    std::string save_file_path_change;
    bool use_relative_save_path = ui.radioButton_RelativePath->isChecked();
    std::string extension = "";
    int format_flag = ui.comboBox_FormatFlags->currentData().toInt();
    int quality = ui.horizontalSlider_Quality->value();
    bool optimize = ui.checkBox_Optimize->isChecked();
    bool progressive = ui.checkBox_Progressive->isChecked();
    int compression = ui.spinBox_Compression->value();
    int extra1 = ui.spinBox_ExtraSetting1->value();
    int extra2 = ui.spinBox_ExtraSetting2->value();
    
    if (ui.radioButton_RenameOriginal->isChecked()) {
        save_file_option_flag = ImageSaver::RENAME_ORIGINAL;
    }
    else if (ui.radioButton_NewFileName->isChecked()) {
        save_file_option_flag = ImageSaver::NEW_NAME;
    }
    if (use_relative_save_path) {
        save_file_path_change = ui.lineEdit_RelativePath->text().toStdString();
    }
    else {
        save_file_path_change = ui.lineEdit_AbsolutePath->text().toStdString();
    }
    if (ui.groupBox_ChangeFormat->isChecked()) {
        extension = ui.comboBox_ImageFormat->currentData().toString().toStdString();
    }
    ImageSaver* image_saver = new ImageSaver(
        image_editor->GetImage(),
        current_file_metadata_list.at(image_index).path,
        save_file_option_flag,
        use_relative_save_path,
        save_file_path_change,
        ui.lineEdit_FileName->text().toStdString(),
        extension,
        image_index + 1
    );
    image_saver->PushFormatParameters(format_flag, quality, optimize, progressive, compression, extra1, extra2);

    // Add Callback function when saving finishes, send data to UpdateLog().
    image_saver->AddFinishedCallback(std::bind(&BatchItImage::UpdateLog, this, image_editor, image_saver));

    // Start the image saving process on another thread
    //std::future<bool> worker_thread = std::async(&ImageSaver::SaveImageFile, image_saver);
    auto worker_thread = std::thread(&ImageSaver::SaveImageFile, image_saver);
    
    worker_thread.detach();
}

void BatchItImage::UpdateLog(ImageEditor* edited_image, ImageSaver* saved_image)
{
    DEBUG("UpdateLog: TODO");
    
    std::vector<std::string>* edit_errors = edited_image->GetErrors();
    if (not edit_errors->empty()) {
        DEBUG(edit_errors->at(0));

    }

    std::string save_error = saved_image->GetErrorMessage();
    if (save_error != "") {
        DEBUG(save_error);

    }

    emit progressMade();
    delete edited_image;
    delete saved_image;
}

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

template<typename DirectoryIter>QStringList BatchItImage::IterateDirectory(DirectoryIter iterator)
{
    QStringList updated_file_list;
    for (auto const& dir_entry : iterator) {
        if (std::filesystem::is_regular_file(dir_entry)) {
            DEBUG2("File: ", dir_entry);
            std::string file_ext = dir_entry.path().extension().string();
            for (const auto& img_format : extension_list) {
                std::string ext = img_format.toStdString();
                if (ext == file_ext) {
                    QString file = QString::fromStdString(dir_entry.path().string());
                    updated_file_list.append(file);
                    break;
                }
            }
        }
        else {
            DEBUG2("Not a file: ", dir_entry);
        }
    }
    return updated_file_list;
}

void BatchItImage::BuildFileMetadataList(const QStringList file_list)
{
    std::string file_path_str;
    int load_order = last_load_count; // Get highest load_order in all file lists. 
    int file_count = file_list.size();

    ui.enhancedProgressBar->configure(file_count, 3.0f, function_ResizeFileTreeColumns);

    //DEBUG2("file_list (count): ", file_list.count());

    for (const auto& file : file_list) {

        file_path_str = file.toStdString();
        load_order += 1;

        if (IsFileInList(file_path_str, current_file_metadata_list) > -1) {
            emit progressMade(3.0f);
        }
        else {
            // TODO: Which is faster?
            bool use_qthread = true;
            if (use_qthread) {
                // Start a new thread to build a file's metadata. Results will be sent to HandleFileMetadata(). https://wiki.qt.io/QThreads_general_usage
                FileMetadataWorker* new_fm_worker = new FileMetadataWorker(file_path_str, load_order, this);
                QThread* thread = new QThread();
                Q_ASSERT(connect(new_fm_worker, &FileMetadataWorker::fileMetadataReady, new_fm_worker, &FileMetadataWorker::deleteLater));
                Q_ASSERT(connect(new_fm_worker, &FileMetadataWorker::fileMetadataReady, thread, &QThread::quit)); // (thread, &QThread::finished,...) could maybe work too
                Q_ASSERT(connect(thread, SIGNAL(started()), new_fm_worker, SLOT(getFileMetadata())));
                //connect(new_fm_worker, SIGNAL(fileMetadataReady(std::vector<struct FileMetadata>)), this, SLOT(HandleFileMetadata(std::vector<struct FileMetadata>)));
                Q_ASSERT(connect(new_fm_worker, SIGNAL(fileMetadataReady(FileMetadata*)), this, SLOT(HandleFileMetadata(FileMetadata*))));
                new_fm_worker->moveToThread(thread);
                thread->start();
                DEBUG("- QThread Worker Created -");
            }
            else {
                // TODO: Test this
                //MetadataBuilder* mb = new MetadataBuilder(file_path_str, load_order);
                // Start the process on another thread and add a callback function when finished.
                //std::future<void> worker_thread = std::async(&MetadataBuilder::getFileMetadata, mb);
                //auto worker_thread = std::thread(&MetadataBuilder::getFileMetadata, mb);
                //mb->AddExitCallback(std::bind(&BatchItImage::HandleFileMetadata, this, std::placeholders::_1));
            }
            emit progressMade();
        }
    }
}

void BatchItImage::HandleFileMetadata(FileMetadata* file_metadata)
{
    DEBUG2("HandleFileMetadata", file_metadata->to_string());

    if (file_metadata->load_order) {
        emit progressMade();
        
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
        emit progressMade(2.0f);
        DEBUG("----> Bad/Non Image File");

        // TODO: return bad file path? then add to vector of bad files, show non_image_file_dialog for each or ignore all
        if (not non_image_file_dialog_shown) {
            auto* non_image_file_dialog = new DialogMessage(
                dialog_messages.at(DialogMessages::non_image_file_dialog).name,
                dialog_messages.at(DialogMessages::non_image_file_dialog).desc,
                QDialogButtonBox::Ignore,
                DialogMessage::CustomButton::NoCustomButton,
                this
            );
            Q_ASSERT(connect(non_image_file_dialog, &DialogMessage::buttonClicked,
                [=](QDialogButtonBox::StandardButton button) {
                    //DEBUG2("Button: ", button);
                    non_image_file_dialog_shown = false;
                    non_image_file_dialog->deleteLater();
                }));
            non_image_file_dialog->show();
            non_image_file_dialog_shown = true;
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
//void BatchItImage::HandleFileMetadata(FileMetadata* file_metadata, MetadataBuilder* mb)
//{
//    HandleFileMetadata(file_metadata);
//    delete mb;
//}

void BatchItImage::LoadFileIntoTree(int file_index, int sorted_column)
{
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
    //QString qdate_created = date_created;
    //QString qdate_modified = date_modified;


    QTreeWidgetItem* new_item = new QTreeWidgetItem(ui.treeWidget_FileInfo);
    
    QCheckBox* file_selected_check_box = new QCheckBox(ui.treeWidget_FileInfo);
    file_selected_check_box->setText("");
    file_selected_check_box->setChecked(file.selected);
    file_selected_check_box->setStatusTip(file_tree_other_text[FileColumn::FILE_LOAD_ORDER] + QVariant(file.load_order).toString());
    
    connect(file_selected_check_box, SIGNAL(toggled(bool)), this, SLOT(FileSelectionChange(bool)));

    ui.treeWidget_FileInfo->setItemWidget(new_item, FileColumn::FILE_SELECTED, file_selected_check_box);
    new_item->setToolTip(FileColumn::FILE_SELECTED, file_tree_other_text[FileColumn::FILE_LOAD_ORDER] + QVariant(file.load_order).toString());
    //new_item->setData(FileColumn::FILE_SELECTED, Qt::ToolTipRole, QVariant(file.load_order));
    //new_item->setData(FileColumn::FILE_SELECTED, Qt::StatusTipRole, QVariant(file.load_order)); // Won't show for widget column
    new_item->setText(FileColumn::FILE_NAME, QString::fromStdString(file_path.filename().string()));
    new_item->setStatusTip(FileColumn::FILE_NAME, file_tree_other_text[FileColumn::FILE_NAME] + QString::fromStdString(file.path));
    new_item->setToolTip(FileColumn::FILE_NAME, file_tree_other_text[FileColumn::FILE_NAME] + QString::fromStdString(file.path));
    new_item->setText(FileColumn::IMAGE_DIMENSIONS, QString::fromStdString(std::to_string(file.width) + " x " + std::to_string(file.height)));
    new_item->setTextAlignment(FileColumn::IMAGE_DIMENSIONS, Qt::AlignCenter);
    new_item->setStatusTip(FileColumn::IMAGE_DIMENSIONS, file_tree_other_text[FileColumn::IMAGE_DIMENSIONS] + QVariant(file.width).toString() + " x " + QVariant(file.height).toString());
    //new_item->setToolTip(FileColumn::IMAGE_DIMENSIONS, QString::fromStdString("Image Width: " + std::to_string(file.width) + ",   Image Height: " + std::to_string(file.height)));
    new_item->setToolTip(FileColumn::IMAGE_DIMENSIONS, file_tree_other_text[FileColumn::IMAGE_DIMENSIONS] + QVariant(file.width).toString() + " x " + QVariant(file.height).toString());
    new_item->setText(FileColumn::FILE_SIZE, QString::fromStdString(file_size_str));
    new_item->setTextAlignment(FileColumn::FILE_SIZE, Qt::AlignRight);
    new_item->setStatusTip(FileColumn::FILE_SIZE, file_tree_other_text[FileColumn::FILE_SIZE] + QString::fromStdString(file_size_str));
    new_item->setToolTip(FileColumn::FILE_SIZE, file_tree_other_text[FileColumn::FILE_SIZE] + QString::fromStdString(file_size_str));
    new_item->setText(FileColumn::DATE_CREATED, QString::fromStdString(date_created));
    new_item->setStatusTip(FileColumn::DATE_CREATED, file_tree_other_text[FileColumn::DATE_CREATED] + QString(date_created));
    new_item->setToolTip(FileColumn::DATE_CREATED, file_tree_other_text[FileColumn::DATE_CREATED] + QString(date_created));
    new_item->setText(FileColumn::DATE_MODIFIED, QString::fromStdString(date_modified));
    new_item->setStatusTip(FileColumn::DATE_MODIFIED, file_tree_other_text[FileColumn::DATE_MODIFIED] + QString(date_modified));
    new_item->setToolTip(FileColumn::DATE_MODIFIED, file_tree_other_text[FileColumn::DATE_MODIFIED] + QString(date_modified));
    
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

    emit progressMade();
}

int BatchItImage::GetCurrentFileTreeRow()
{
    int current_file_tree_row = ui.treeWidget_FileInfo->currentIndex().row();
    int current_file_tree_p_row = ui.treeWidget_FileInfo->currentIndex().parent().row();
    if (current_file_tree_p_row > -1)
        current_file_tree_row = current_file_tree_p_row;
    return current_file_tree_row;
}

void BatchItImage::FileSelectionChange(bool checked)
{
    int current_file_tree_row = ui.treeWidget_FileInfo->currentIndex().row();
    DEBUG4("Row: ", current_file_tree_row, " Checked: ", checked);
    current_file_metadata_list.at(current_file_tree_row).selected = checked;
}

void BatchItImage::ResizeFileTreeColumns()
{
    DEBUG("ResizeFileTreeColumns");
    for (int i = 0; i < FileColumn::COUNT; i++) {
        ui.treeWidget_FileInfo->resizeColumnToContents(i);
        //ui.treeWidget_FileInfo->header()->ResizeToContents;
    }
}

void BatchItImage::SortFileTreeByColumn(int index)
{
    DEBUG4("Column Clicked", index, ", Sort Order: ", current_file_sort_order);
    ui.enhancedProgressBar->configure(ui.treeWidget_FileInfo->topLevelItemCount(), 2.0f, function_ResizeFileTreeColumns);

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
        emit progressMade();
        LoadFileIntoTree(i, current_file_column_sorted);
    }

    ui.treeWidget_FileInfo->header()->setSortIndicator(current_file_column_sorted, qsort_indicator);
    ui.treeWidget_FileInfo->header()->setSortIndicatorShown(true);
}

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

void BatchItImage::closeEvent(QCloseEvent* event)
{
    if (event->spontaneous()) {
        qDebug() << "The X-close button was clicked.";
    }
    if (SavePresetDialog(true, true)) {
        event->ignore();
    }
    else {
        QWidget::closeEvent(event);
    }
}

void BatchItImage::dropEvent(QDropEvent* event)
{
    if (event->mimeData()->hasUrls()) {
        QStringList file_list;
        QList<QUrl> urls;
        QList<QUrl>::Iterator it;
        urls = event->mimeData()->urls();
        for (it = urls.begin(); it != urls.end(); ++it) {
            // TODO: check if file is of a supported image format/extension or just let opencv detect image later?
            file_list.append(it->toLocalFile());
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

void BatchItImage::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Delete) {
        DEBUG("Delete Key Pressed");
        DeleteConfirmationPopup();
    }
}

void BatchItImage::DeleteConfirmationPopup(bool clear_all)
{
    if (ui.treeWidget_FileInfo->hasFocus()) {
        int current_file_tree_row = GetCurrentFileTreeRow();
        if (current_file_tree_row > -1) {
            
            DialogMessage* delete_dialog;

            if (clear_all) {
                delete_dialog = new DialogMessage(
                    dialog_messages.at(DialogMessages::delete_dialog_clear).name,
                    dialog_messages.at(DialogMessages::delete_dialog_clear).desc,
                    QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                    DialogMessage::CustomButton::NoCustomButton,
                    this
                );
            }
            else {
                delete_dialog = new DialogMessage(
                    dialog_messages.at(DialogMessages::delete_dialog).name,
                    dialog_messages.at(DialogMessages::delete_dialog).name,
                    QDialogButtonBox::Yes | QDialogButtonBox::YesToAll | QDialogButtonBox::Cancel,
                    DialogMessage::CustomButton::NoCustomButton,
                    this
                );
            }
            Q_ASSERT(connect(delete_dialog, &DialogMessage::buttonClicked, this,
                [=](QDialogButtonBox::StandardButton button) {
                    RemoveFileFromTree(button);
                    delete_dialog->deleteLater();
                }));
            delete_dialog->setModal(true); // Block parent window inputs. exec() = auto modal.
            //DEBUG(delete_dialog->exec()); //QDialog::Accepted ?
            delete_dialog->show();
        }
    }
}

int BatchItImage::IsFileInList(std::string path, std::vector<FileMetadata> list)
{
    const auto it = std::find_if(list.begin(), list.end(),
        [path](const FileMetadata& a) { return a.path == path; });
    if (it != list.end()) {
        return std::distance(list.begin(), it);
    }
    return -1;
}

int BatchItImage::IsFileInList(std::string path, std::vector<FileMetadata> list, const size_t search_range[2])
{
    const auto it = std::find_if(list.begin() + search_range[0], list.begin() + search_range[1],
        [path](const FileMetadata& a) { return a.path == path; });
    if (it != list.begin() + search_range[1]) {
        return std::distance(list.begin(), it);
    }
    return -1;
}

std::vector<FileMetadata>::iterator BatchItImage::IsFileInListIterator(std::string path, std::vector<FileMetadata> list)
{
    const auto it = std::find_if(list.begin(), list.end(),
        [path](const FileMetadata& a) { return a.path == path; });
    return it;
}

void BatchItImage::RemoveFileFromTree(const QDialogButtonBox::StandardButton& button_clicked)
{
    int current_file_tree_row = GetCurrentFileTreeRow(); // this should never be -1, if it is something is changing it between this and DeleteConfirmationPopup
    DEBUG4("RemoveFileFromTree At Index: ", current_file_tree_row, ", Button: ", button_clicked);
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

void BatchItImage::AddTextToFileName()
{
    // Get Text
    QString cur_qtext = ui.lineEdit_FileName->text();

    // Move cursor to an index right after <> if it is in-between <>
    DEBUG2("Cursor Position: ", ui.lineEdit_FileName->cursorPosition());
    int rb_found = cur_qtext.indexOf(">", ui.lineEdit_FileName->cursorPosition());
    if (rb_found > -1) {
        int lb_found = cur_qtext.indexOf("<", ui.lineEdit_FileName->cursorPosition());
        if (lb_found == -1 or lb_found > rb_found) {
            ui.lineEdit_FileName->setCursorPosition(rb_found + 1);
            DEBUG2("Cursor Position Moved: ", ui.lineEdit_FileName->cursorPosition());
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

void BatchItImage::CheckAbsolutePath()
{
    std::string path = ui.lineEdit_AbsolutePath->text().toStdString();
    if (std::filesystem::exists(path)) {
        last_existing_save_path = path;
    }
    else {
        auto* check_path_dialog = new DialogMessage(
            dialog_messages.at(DialogMessages::check_path_dialog).name,
            dialog_messages.at(DialogMessages::check_path_dialog).desc,
            QDialogButtonBox::Open | QDialogButtonBox::Cancel,
            DialogMessage::CustomButton::NoCustomButton,
            this
        );
        if (check_path_dialog->exec()) {
            ui.lineEdit_AbsolutePath->setText(GetSaveDirectoryPath());
        }
        else {
            ui.lineEdit_AbsolutePath->setText(GetLastExistingSavePath());
        }
        delete check_path_dialog;
    }
}

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

QString BatchItImage::GetLastExistingSavePath()
{
    if (last_existing_save_path.empty())
        return qdefault_path;
    else
        return QString::fromStdString(last_existing_save_path);
}

const std::string BatchItImage::ReplaceAll(std::string str, const std::string& from, const std::string& to, bool no_really_all) { // Static
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        if (not no_really_all)
            start_pos += to.length();
    }
    return str;
}
