#include "BatchItImage.h"

/*
TODOs: 
    Log Actions
    Holding right click scrolls left to right
    Track changes to undo file list deletes or image edit ui changes
    Image File Filters... select: files > 100 width, < 1 MB, < 1/1/2021 date, etc. Via right click context menu?
    Application Settings Dialog
    Priority Sort those selected to top. Context menu?
    Handle extensions not matching the format used?

Long-Term TODOs:
    Image Viewer/Previewer
    Image Combiner
    Shear Image
    Crop Image
    Add Text To Image
    Custom Reordering of Images Files (Drag'n'Drop)

*/


DialogMessage::DialogMessage(QString title, QString message,
    const QFlags<QDialogButtonBox::StandardButton> buttons,
    const DialogMessage::CustomButtons custom_buttons,
    QWidget* parent, bool bold_message_text)
    : QDialog(parent)
{
    ui.setupUi(this);
    setWindowTitle(title);
    
    if (bold_message_text)
        message_font.setBold(true);
    ui.label_Message->setFont(message_font);
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
    if (custom_buttons & CustomButton::Delete)
        ui.buttonBox->addButton(new QPushButton("&Delete", this), QDialogButtonBox::AcceptRole);
    if (custom_buttons & CustomButton::OpenLog)
        ui.buttonBox->addButton(new QPushButton("&Open Log", this), QDialogButtonBox::ActionRole);
    if (custom_buttons & CustomButton::SaveLogAs)
        ui.buttonBox->addButton(new QPushButton("&Save Log As", this), QDialogButtonBox::ApplyRole);

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
    ui.lineEdit_PresetDesc->setText(preset_list->at(index).presetDescription());
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
        preset_list->at(current_selected_preset).setPresetDescription(updated_description);
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
    FileMetadataWorker::file_path = std::filesystem::path(file_path).make_preferred().string();
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
    qDebug() << "Debug Build";

    ui.setupUi(this);
    setAcceptDrops(true);

    preset_settings_file = QApplication::applicationDirPath() + "/settings.ini";
    qDebug() << preset_settings_file.toStdString();

    preset_list.reserve(10);
    current_file_metadata_list.reserve(30);
    deleted_file_metadata_list.reserve(10);
    recent_file_paths_loaded.reserve(recent_file_paths_loaded_max);

    /***************************
        Add UI Text/Data
    ****************************/

    LoadInUiData();
    qDebug() << "LoadInUiData = Successful";

    AddUiDataTo(*menu_bar_file, std::vector<QObject*>{
        ui.menu_File, ui.action_AddImages, ui.menu_RecentImageFiles, action_load_all_files,
            action_clear_all_files, ui.action_OpenLogDirectory, ui.action_Close
    });
    AddUiDataTo(*menu_bar_edit, std::vector<QObject*>{
        ui.menu_Edit, ui.action_AddNewPreset, ui.action_RemovePreset, ui.action_SavePresets,
            ui.action_ChangePresetDesc, ui.action_ShowFormatFilter
    });
    AddUiDataTo(*menu_bar_help, std::vector<QObject*>{ ui.menu_Help, ui.action_About, ui.action_AboutQt, ui.action_Help });

    ui.tabWidget->setCurrentIndex(other_options->at(UI::Other::tab_1).data);
    ui.tabWidget->setTabText(UI::Other::tab_1, other_options->at(UI::Other::tab_1).name);
    ui.tab_1->setToolTip(other_options->at(UI::Other::tab_1).desc);
    ui.tabWidget->setTabText(UI::Other::tab_2, other_options->at(UI::Other::tab_2).name);
    ui.tab_2->setToolTip(other_options->at(UI::Other::tab_2).desc);
    ui.tabWidget->setTabText(UI::Other::tab_3, other_options->at(UI::Other::tab_3).name);
    ui.tab_3->setToolTip(other_options->at(UI::Other::tab_3).desc);

    AddUiDataTo(*resize_options, std::vector<QWidget*>{ ui.groupBox_Resize, ui.checkBox_KeepAspectRatio });
    AddUiDataTo(background_options, std::vector<QWidget*>{ ui.groupBox_Background, ui.pushButton_ColorDialog, ui.label_ColorPreview });
    AddUiDataTo(*blur_options, std::vector<QWidget*>{
        ui.groupBox_Blur, ui.checkBox_BlurNormalize, ui.label_BlurX1, ui.label_BlurY1, ui.label_BlurX2, ui.label_BlurY2, ui.label_BlurD
    });
    AddUiDataTo(*rotation_options, std::vector<QWidget*>{ ui.groupBox_Rotation, ui.checkBox_IncreaseBounds, ui.checkBox_FlipImage });
    AddUiDataTo(*watermark_options, std::vector<QWidget*>{
        ui.groupBox_Watermark, ui.comboBox_WatermarkLocation, ui.label_WatermarkLocation,
            ui.label_WatermarkTransparency, ui.label_WatermarkOffset
    });
    AddUiDataTo(ui.groupBox_FileRename, file_path_options->at(UI::FileOption::FilePath::groupBox_FileRename));
    AddUiDataTo(ui.radioButton_Overwrite, file_path_options->at(UI::FileOption::FilePath::radioButton_Overwrite));
    AddUiDataTo(ui.radioButton_RenameOriginal, file_path_options->at(UI::FileOption::FilePath::radioButton_RenameOriginal));
    AddUiDataTo(ui.radioButton_NewFileName, file_path_options->at(UI::FileOption::FilePath::radioButton_NewFileName));
    AddUiDataTo(ui.label_Add, file_path_options->at(UI::FileOption::FilePath::label_Add));
    AddUiDataTo(ui.groupBox_SaveDir, file_path_options->at(UI::FileOption::FilePath::groupBox_SaveDir));
    AddUiDataTo(ui.radioButton_RelativePath, file_path_options->at(UI::FileOption::FilePath::radioButton_RelativePath));
    AddUiDataTo(ui.radioButton_AbsolutePath, file_path_options->at(UI::FileOption::FilePath::radioButton_AbsolutePath));
    AddUiDataTo(ui.pushButton_AddBackOneDir, file_path_options->at(UI::FileOption::FilePath::pushButton_AddBackOneDir));
    AddUiDataTo(ui.pushButton_FindAbsolutePath, file_path_options->at(UI::FileOption::FilePath::pushButton_FindAbsolutePath));
    AddUiDataTo(ui.checkBox_SearchSubDirs, other_options->at(UI::Other::checkBox_SearchSubDirs));
    AddUiDataTo(ui.pushButton_EditSaveAll, other_options->at(UI::Other::pushButton_EditSaveAll));

    PopulateComboBox(ui.comboBox_WidthMod, *width_selections);
    PopulateComboBox(ui.comboBox_HeightMod, *height_selections);
    PopulateComboBox(ui.comboBox_Resample, *resampling_selections);
    PopulateComboBox(ui.comboBox_BorderType, *border_types);
    PopulateComboBox(ui.comboBox_BlurFilter, *blur_filters);
    PopulateComboBox(ui.comboBox_WatermarkLocation, *watermark_locations,
        watermark_options->at(UI::EditOption::Watermark::label_WatermarkLocation).data);
    PopulateComboBox(ui.comboBox_AddText, *file_name_creation, 0, ImageSaver::MetadataIdentifiers);
    PopulateComboBox(ui.comboBox_ImageFormat, *image_formats, 0, extension_list);

    /***************************
        Prep UI Widgets, Etc.
    ****************************/

    action_line_recent_top->setSeparator(true);
    action_line_recent_bottom->setSeparator(true);

    ui.enhancedProgressBar->setParentContainer(ui.widget_EnhancedProgressBar);
    ui.enhancedProgressBar->setCancelButton(ui.pushButton_CancelProgress);
    ui.pushButton_CancelProgress->setIcon(QIcon(":/BatchItImage/res/x-24850.svg"));

    ui.dial_Rotation->setInvertedAppearance(true);
    ui.dial_Rotation->setInvertedControls(true);

    SetupFileTree();
    SetupFileTreeContextMenu();
    LoadRecentFiles();
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
    supported_image_extensions_dialog_str.append(
        file_dialog_titles.at(Dialog::FileSearch::supported_image_extensions_dialog_str) + " (");
    for (const auto& ext : extension_list) {
        supported_image_extensions_dialog_str.append("*" + ext + " ");
    }
    supported_image_extensions_dialog_str.insert(supported_image_extensions_dialog_str.size() - 1, ")");
    supported_image_extensions_dialog_str.append("\n" + file_dialog_titles.at(Dialog::FileSearch::all_files_dialog_str) + " (*)");
    qDebug() << supported_image_extensions_dialog_str.toStdString();

    UiConnections();
   
    // Delete all arrays on the heap that will not be reused after loading.
    delete menu_bar_file;
    delete menu_bar_edit;
    delete menu_bar_help;
    delete file_tree_headers;
    delete file_tree_menu_items;
    delete file_tree_sub_menu_formats;
    delete file_path_options;
    delete resize_options;
    //delete background_options;
    delete blur_options;
    delete rotation_options;
    delete other_options;
    delete width_selections;
    delete height_selections;
    delete resampling_selections;
    delete border_types;
    delete blur_filters;
    delete watermark_locations;
    delete file_name_creation;
    delete image_formats;

    session_start_time = std::chrono::system_clock::now();
    qDebug().noquote() << std::format("{0:%Y.%m.%d %H:%M:%OS}", session_start_time);
    std::chrono::zoned_time session_start_time_zt{ std::chrono::current_zone(), session_start_time };
    qDebug().noquote() << "Session Start Time (Local Time-Zone):"
        << std::format("{0:%Y.%m.%d %H:%M:%OS}", session_start_time_zt) << '\n';

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

    //qDebug().noquote() << Option.printAllTrackerFlags();

}

BatchItImage::~BatchItImage() {}

// Just for quick testing
void BatchItImage::Test()
{
    qDebug() << "--TEST--";
    QObject* obj = sender();
    qDebug() << obj->objectName().toStdString();
}

void BatchItImage::LoadInUiData()
{
    menu_bar_file->at(UI::MenuBar::File::menu_File) = "File";
    menu_bar_file->at(UI::MenuBar::File::action_AddImages) = "Add Image Files...";
    menu_bar_file->at(UI::MenuBar::File::menu_RecentImageFiles) = "Recent Image Files";
    menu_bar_file->at(UI::MenuBar::File::action_load_all_files) = "Load All Files Below";
    menu_bar_file->at(UI::MenuBar::File::action_clear_all_files) = "Clear Recent History";
    menu_bar_file->at(UI::MenuBar::File::action_OpenLogDirectory) = "Open Log File Directory";
    menu_bar_file->at(UI::MenuBar::File::action_Close) = "Close";

    menu_bar_edit->at(UI::MenuBar::Edit::menu_Edit) = "Edit";
    menu_bar_edit->at(UI::MenuBar::Edit::action_AddNewPreset) = "Add New Preset...";
    menu_bar_edit->at(UI::MenuBar::Edit::action_RemovePreset) = "Remove Preset...";
    menu_bar_edit->at(UI::MenuBar::Edit::action_SavePresets) = "Save All Presets";
    menu_bar_edit->at(UI::MenuBar::Edit::action_ChangePresetDesc) = "Change Preset Description...";
    menu_bar_edit->at(UI::MenuBar::Edit::action_ShowFormatFilter) = "Show Extension Filter";

    menu_bar_help->at(UI::MenuBar::Help::menu_Help) = "Help";
    menu_bar_help->at(UI::MenuBar::Help::action_About) = "About";
    menu_bar_help->at(UI::MenuBar::Help::action_AboutQt) = "About Qt";
    menu_bar_help->at(UI::MenuBar::Help::action_Help) = "Help";

    // treeWidget_FileInfo
    file_tree_headers->at(FileTree::Column::FILE_SELECTED).data = 1; // TODO: 1 = Initial Sort/Bold Text (after files loaded)?
    file_tree_headers->at(FileTree::Column::FILE_SELECTED).name = "";
    file_tree_headers->at(FileTree::Column::FILE_SELECTED).desc = "The load order of image files.";
    file_tree_headers->at(FileTree::Column::FILE_NAME).data = 0;
    file_tree_headers->at(FileTree::Column::FILE_NAME).name = "File Name";
    file_tree_headers->at(FileTree::Column::FILE_NAME).desc = "The name of an image file, click an arrow to show full path.";
    file_tree_headers->at(FileTree::Column::IMAGE_DIMENSIONS).data = 0;
    file_tree_headers->at(FileTree::Column::IMAGE_DIMENSIONS).name = "Dimensions";
    file_tree_headers->at(FileTree::Column::IMAGE_DIMENSIONS).desc = "Image Dimensions/Size (Width x Height).";
    file_tree_headers->at(FileTree::Column::FILE_SIZE).data = 0;
    file_tree_headers->at(FileTree::Column::FILE_SIZE).name = "File Size";
    file_tree_headers->at(FileTree::Column::FILE_SIZE).desc = "The file size in bytes.";
    file_tree_headers->at(FileTree::Column::DATE_CREATED).data = 0;
    file_tree_headers->at(FileTree::Column::DATE_CREATED).name = "Date Created";
    file_tree_headers->at(FileTree::Column::DATE_CREATED).desc = "The date a file was created.";
    file_tree_headers->at(FileTree::Column::DATE_MODIFIED).data = 0;
    file_tree_headers->at(FileTree::Column::DATE_MODIFIED).name = "Date Modified";
    file_tree_headers->at(FileTree::Column::DATE_MODIFIED).desc = "The date a file was last modified.";

    file_tree_other_text.at(FileTree::Column::FILE_LOAD_ORDER) = "Load Order: ";
    file_tree_other_text.at(FileTree::Column::FILE_NAME) = "File Path: ";
    file_tree_other_text.at(FileTree::Column::IMAGE_DIMENSIONS) = "Image [Width x Height]: ";
    file_tree_other_text.at(FileTree::Column::FILE_SIZE) = "File Size: ";
    file_tree_other_text.at(FileTree::Column::DATE_CREATED) = "Date File Created: ";
    file_tree_other_text.at(FileTree::Column::DATE_MODIFIED) = "Date File Modified: ";

    file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_add).data = 0; // TODO: 1 = Bold, default double click action?
    file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_add).name = "Add Images";
    file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_add).desc = "Add more images to this file viewer.";
    file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_delete).data = 0;
    file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_delete).name = "Delete Images";
    file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_delete).desc = "Delete images from this file viewer (does not delete from system).";
    file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_clear).data = 0;
    file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_clear).name = "Clear List";
    file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_clear).desc = "Clear entire list of files from file viewer.";
    file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_select).data = 0;
    file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_select).name = "Select Image";
    file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_select).desc = "Select or check image file currently highlighted.";
    file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_unselect).data = 0;
    file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_unselect).name = "Unselect Image";
    file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_unselect).desc = "Unselect or uncheck image file currently highlighted.";
    file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_select_all).data = 0;
    file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_select_all).name = "Select All Images";
    file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_select_all).desc = "Select or check all image files.";
    file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_select_none).data = 0;
    file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_select_none).name = "Unselect All Image";
    file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_select_none).desc = "Unselect or uncheck all image files.";
    file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_view).data = 0;
    file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_view).name = "View Image";
    file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_view).desc = "View image file currently highlighted.";
    file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_preview).data = 0;
    file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_preview).name = "Preview Modified Image";
    file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_preview).desc = "Preview a modified version of the image file currently highlighted using the current selected preset.\n" \
        "This modified image will only be a preview the edits will not be saved to file.";

    select_text = file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_select).name;
    unselect_text = file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_unselect).name;

    file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_submenu_filter).data = 0;
    file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_submenu_filter).name = "Format Selection Filter";
    file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_submenu_filter).desc = "Filter specific formats, selecting only those \"checked\".";
    file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_submenu_undock).data = 0;
    file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_submenu_undock).name = "[Undock Filter Menu]";
    file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_submenu_undock).desc = "Undock this sub-menu allowing it to float above and stay on screen until closed.";
    file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_jpeg).data = 0;
    file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_jpeg).name = "Select All JPEG Images";
    file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_jpeg).desc = "";
    file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_jp2).data = 0;
    file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_jp2).name = "Select All JPEG 2000 Images";
    file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_jp2).desc = "";
    file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_png).data = 0;
    file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_png).name = "Select All PNG Images";
    file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_png).desc = "";
    file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_webp).data = 0;
    file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_webp).name = "Select All WebP Images";
    file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_webp).desc = "";
    file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_bmp).data = 0;
    file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_bmp).name = "Select All Bitmap Images";
    file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_bmp).desc = "";
    file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_avif).data = 0;
    file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_avif).name = "Select All AVIF Images";
    file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_avif).desc = "";
    file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_pbm).data = 0;
    file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_pbm).name = "Select All Netpbm Images";
    file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_pbm).desc = "";
    file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_sr).data = 0;
    file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_sr).name = "Select All Sun Raster Images";
    file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_sr).desc = "";
    file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_tiff).data = 0;
    file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_tiff).name = "Select All TIFF Images";
    file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_tiff).desc = "";
    file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_exr).data = 0;
    file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_exr).name = "Select All OpenEXR Images";
    file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_exr).desc = "";
    file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_hdr).data = 0;
    file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_hdr).name = "Select All Radiance HDR Images";
    file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_hdr).desc = "";

    resize_options->at(UI::EditOption::Resize::groupBox_Resize).data = 0;
    resize_options->at(UI::EditOption::Resize::groupBox_Resize).name = "Resize:";
    resize_options->at(UI::EditOption::Resize::groupBox_Resize).desc = "Options to increase or decrease the dimensions of an image.";
    resize_options->at(UI::EditOption::Resize::checkBox_KeepAspectRatio).data = 1;
    resize_options->at(UI::EditOption::Resize::checkBox_KeepAspectRatio).name = "Keep Aspect Ratio";
    resize_options->at(UI::EditOption::Resize::checkBox_KeepAspectRatio).desc = "In order to keep aspect ratio, either width or height must be set to \"No Change\" or \"0\".";

    // comboBox_WidthMod
    width_selections->at(0).data = ImageEditor::SizeModifier::NO_CHANGE;
    width_selections->at(0).name = "No Change";
    width_selections->at(0).desc = "Image widths may still be modified if 'keep aspect ratio' is checked.";
    width_selections->at(1).data = ImageEditor::SizeModifier::CHANGE_TO;
    width_selections->at(1).name = "Change Width To:";
    width_selections->at(1).desc = "All image widths will be modified to a specific number.";
    width_selections->at(2).data = ImageEditor::SizeModifier::MODIFY_BY;
    width_selections->at(2).name = "Modify Width By:";
    width_selections->at(2).desc = "This adds to or subtracts from an image's current width. Ex. 1080 + '220' = 1300";
    width_selections->at(3).data = ImageEditor::SizeModifier::MODIFY_BY_PCT;
    width_selections->at(3).name = "Modify Width By (%):";
    width_selections->at(3).desc = "This modifies an image's current width by percentage. Ex. 720 x '200%' = 1440";
    width_selections->at(4).data = ImageEditor::SizeModifier::DOWNSCALE;
    width_selections->at(4).name = "Downscale Width To:";
    width_selections->at(4).desc = "All images above entered width will be modified to that specific number.\n" \
        "All images already at or below that number will not be modified.";
    width_selections->at(5).data = ImageEditor::SizeModifier::UPSCALE;
    width_selections->at(5).name = "Upscale Width To:";
    width_selections->at(5).desc = "All images below entered width will be modified to that specific number.\n" \
        "All images already at or above that number will not be modified.";

    // comboBox_HeightMod
    height_selections->at(0).data = ImageEditor::SizeModifier::NO_CHANGE;
    height_selections->at(0).name = "No Change";
    height_selections->at(0).desc = "Image heights may still be modified if 'keep aspect ratio' is checked.";
    height_selections->at(1).data = ImageEditor::SizeModifier::CHANGE_TO;
    height_selections->at(1).name = "Change Height To:";
    height_selections->at(1).desc = "All images heights will be modified to a specific number.";
    height_selections->at(2).data = ImageEditor::SizeModifier::MODIFY_BY;
    height_selections->at(2).name = "Modify Height By:";
    height_selections->at(2).desc = "This adds to or subtracts from an image's current height. Ex. 1080 + '220' = 1300";
    height_selections->at(3).data = ImageEditor::SizeModifier::MODIFY_BY_PCT;
    height_selections->at(3).name = "Modify Height By (%):";
    height_selections->at(3).desc = "This modifies an image's current height by percentage. Ex. 720 x '200%' = 1440";
    height_selections->at(4).data = ImageEditor::SizeModifier::DOWNSCALE;
    height_selections->at(4).name = "Downscale Height To:";
    height_selections->at(4).desc = "All images above entered height will be modified to that specific number.\n" \
        "All images already at or below that number will not be modified.";
    height_selections->at(5).data = ImageEditor::SizeModifier::UPSCALE;
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

    background_options.at(UI::EditOption::Background::groupBox_Background).data = 0;
    background_options.at(UI::EditOption::Background::groupBox_Background).name = "Border Type / Background Color:";
    background_options.at(UI::EditOption::Background::groupBox_Background).desc = "Options on how to handle images when a background must be shown.";
    background_options.at(UI::EditOption::Background::pushButton_ColorDialog).data = 0;
    background_options.at(UI::EditOption::Background::pushButton_ColorDialog).name = "Background Color Picker";
    background_options.at(UI::EditOption::Background::pushButton_ColorDialog).desc = "Choose an Image Background Color (Alpha/Transparency is Format Dependant)";
    background_options.at(UI::EditOption::Background::label_ColorPreview).data = 0;
    background_options.at(UI::EditOption::Background::label_ColorPreview).name = "Background Color Preview";
    background_options.at(UI::EditOption::Background::label_ColorPreview).desc = "Background of this label will be the color used in an image background if needed.";

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
    
    blur_options->at(UI::EditOption::Blur::groupBox_Blur).data = 0;
    blur_options->at(UI::EditOption::Blur::groupBox_Blur).name = "Blur Filters:";
    blur_options->at(UI::EditOption::Blur::groupBox_Blur).desc = "";
    blur_options->at(UI::EditOption::Blur::checkBox_BlurNormalize).data = 0;
    blur_options->at(UI::EditOption::Blur::checkBox_BlurNormalize).name = "Normalize";
    blur_options->at(UI::EditOption::Blur::checkBox_BlurNormalize).desc = "";
    blur_options->at(UI::EditOption::Blur::label_BlurX1).data = 0;
    blur_options->at(UI::EditOption::Blur::label_BlurX1).name = "  X";
    blur_options->at(UI::EditOption::Blur::label_BlurX1).desc = "";
    blur_options->at(UI::EditOption::Blur::label_BlurY1).data = 0;
    blur_options->at(UI::EditOption::Blur::label_BlurY1).name = "  Y";
    blur_options->at(UI::EditOption::Blur::label_BlurY1).desc = "";
    blur_options->at(UI::EditOption::Blur::label_BlurX2).data = 0;
    blur_options->at(UI::EditOption::Blur::label_BlurX2).name = " sX";
    blur_options->at(UI::EditOption::Blur::label_BlurX2).desc = "";
    blur_options->at(UI::EditOption::Blur::label_BlurY2).data = 0;
    blur_options->at(UI::EditOption::Blur::label_BlurY2).name = " sY";
    blur_options->at(UI::EditOption::Blur::label_BlurY2).desc = "";
    blur_options->at(UI::EditOption::Blur::label_BlurD).data = 0;
    blur_options->at(UI::EditOption::Blur::label_BlurD).name = "  D";
    blur_options->at(UI::EditOption::Blur::label_BlurD).desc = "";

    // comboBox_BlurFilter
    blur_filters->at(0).data = ImageEditor::BlurFilter::NO_FILTER;
    blur_filters->at(0).name = "None";
    blur_filters->at(0).desc = "Does not apply a blur filter.";
    //blur_filters->at(1).data = ImageEditor::BlurFilter::BLUR_FILTER;
    //blur_filters->at(1).name = "Simple Blur Filter";
    //blur_filters->at(1).desc = "Blurs an image using the normalized box filter. Also known as homogeneous smoothing.";
    blur_filters->at(1).data = ImageEditor::BlurFilter::BOX_FILTER;
    blur_filters->at(1).name = "Box Filter";
    blur_filters->at(1).desc = "Blurs an image using the box filter. An unnormalized box filter is useful for computing\n" \
                           "various integral characteristics over each pixel neighborhood, such as covariance\n" \
                           "matrices of image derivatives (used in dense optical flow algorithms, and so on).";
    blur_filters->at(2).data = ImageEditor::BlurFilter::BILATERAL_FILTER;
    blur_filters->at(2).name = "Bilateral Filter";
    blur_filters->at(2).desc = "Applies the bilateral filter to an image, which can reduce unwanted noise very well while\n" \
                           "keeping edges fairly sharp. However, it is very slow compared to most other filters.";
    blur_filters->at(3).data = ImageEditor::BlurFilter::GAUSSIAN_BLUR;
    blur_filters->at(3).name = "Gaussian Blur Filter";
    blur_filters->at(3).desc = "Applies the Gaussian blur filter to an image, which convolves the source image with the\n" \
                           "specified Gaussian kernel.";
    blur_filters->at(4).data = ImageEditor::BlurFilter::MEDIAN_BLUR;
    blur_filters->at(4).name = "Median Blur Filter";
    blur_filters->at(4).desc = "Blurs an image using the median filter, which smooths an image with the x/y size aperture.\n" \
                           "Each channel of a multi-channel image is processed independently.";
    blur_filters->at(5).data = ImageEditor::BlurFilter::PYR_DOWN_BLUR;
    blur_filters->at(5).name = "PYR Down-Sample Blur";
    blur_filters->at(5).desc = "Blurs an image and down-samples it. This performs the down-sampling step of the Gaussian\n" \
                           "pyramid construction. First, it convolves the source image with the Gaussian kernel, then\n" \
                           "down-samples the image by rejecting even rows and columns.";
    blur_filters->at(6).data = ImageEditor::BlurFilter::PYR_UP_BLUR;
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

    rotation_options->at(UI::EditOption::Rotation::groupBox_Rotation).data = 0;
    rotation_options->at(UI::EditOption::Rotation::groupBox_Rotation).name = "Rotation:";
    rotation_options->at(UI::EditOption::Rotation::groupBox_Rotation).desc = "";
    rotation_options->at(UI::EditOption::Rotation::checkBox_IncreaseBounds).data = 0;
    rotation_options->at(UI::EditOption::Rotation::checkBox_IncreaseBounds).name = "Increase Image\nBoundaries";
    rotation_options->at(UI::EditOption::Rotation::checkBox_IncreaseBounds).desc = "";
    rotation_options->at(UI::EditOption::Rotation::checkBox_FlipImage).data = 0;
    rotation_options->at(UI::EditOption::Rotation::checkBox_FlipImage).name = "Flip / Mirror";
    rotation_options->at(UI::EditOption::Rotation::checkBox_FlipImage).desc = "";

    file_path_options->at(UI::FileOption::FilePath::groupBox_FileRename).data = 1;
    file_path_options->at(UI::FileOption::FilePath::groupBox_FileRename).name = "Modify Image File Names:";
    file_path_options->at(UI::FileOption::FilePath::groupBox_FileRename).desc = "";
    file_path_options->at(UI::FileOption::FilePath::radioButton_Overwrite).data = 0;
    file_path_options->at(UI::FileOption::FilePath::radioButton_Overwrite).name = "Overwrite Original File";
    file_path_options->at(UI::FileOption::FilePath::radioButton_Overwrite).desc = "If selected, this will overwrite the original image file with the new edited image file.";
    file_path_options->at(UI::FileOption::FilePath::radioButton_RenameOriginal).data = 0;
    file_path_options->at(UI::FileOption::FilePath::radioButton_RenameOriginal).name = "Rename Original File";
    file_path_options->at(UI::FileOption::FilePath::radioButton_RenameOriginal).desc = "If selected, this will rename the original image file, and use it's name for the new edited image file.";
    file_path_options->at(UI::FileOption::FilePath::radioButton_NewFileName).data = 1;
    file_path_options->at(UI::FileOption::FilePath::radioButton_NewFileName).name = "Create New File Name";
    file_path_options->at(UI::FileOption::FilePath::radioButton_NewFileName).desc = "If selected, a new file name will be used for the new edited image file.";
    file_path_options->at(UI::FileOption::FilePath::label_Add).data = 0;
    file_path_options->at(UI::FileOption::FilePath::label_Add).name = "Add:";
    file_path_options->at(UI::FileOption::FilePath::label_Add).desc = "Add metadata to file name edit text box (to be included in a new file name).";
    file_path_options->at(UI::FileOption::FilePath::groupBox_SaveDir).data = 1;
    file_path_options->at(UI::FileOption::FilePath::groupBox_SaveDir).name = "Save All Image Files In:";
    file_path_options->at(UI::FileOption::FilePath::groupBox_SaveDir).desc = "";
    file_path_options->at(UI::FileOption::FilePath::radioButton_RelativePath).data = 1;
    file_path_options->at(UI::FileOption::FilePath::radioButton_RelativePath).name = "Relative Paths:";
    file_path_options->at(UI::FileOption::FilePath::radioButton_RelativePath).desc = "Relative to the currently edited image file path.";
    file_path_options->at(UI::FileOption::FilePath::radioButton_AbsolutePath).data = 0;
    file_path_options->at(UI::FileOption::FilePath::radioButton_AbsolutePath).name = "Absolute Path:";
    file_path_options->at(UI::FileOption::FilePath::radioButton_AbsolutePath).desc = "The absolute path all edited images will be saved to.";
    file_path_options->at(UI::FileOption::FilePath::pushButton_AddBackOneDir).data = 0;
    file_path_options->at(UI::FileOption::FilePath::pushButton_AddBackOneDir).name = "Add \"Back One Directory\" For Relative Paths";
    file_path_options->at(UI::FileOption::FilePath::pushButton_AddBackOneDir).desc = "Start relative path up one directory level.";
    file_path_options->at(UI::FileOption::FilePath::pushButton_FindAbsolutePath).data = 0;
    file_path_options->at(UI::FileOption::FilePath::pushButton_FindAbsolutePath).name = "Search For An Absolute Path";
    file_path_options->at(UI::FileOption::FilePath::pushButton_FindAbsolutePath).desc = "Open dialog window to select a directory adding it to the absolute path text box.";

    watermark_options->at(UI::EditOption::Watermark::groupBox_Watermark).data = 0;
    watermark_options->at(UI::EditOption::Watermark::groupBox_Watermark).name = "Add Watermark:";
    watermark_options->at(UI::EditOption::Watermark::groupBox_Watermark).desc = "";
    watermark_options->at(UI::EditOption::Watermark::pushButton_Watermark).data = 0;
    watermark_options->at(UI::EditOption::Watermark::pushButton_Watermark).name = "Add Watermark";
    watermark_options->at(UI::EditOption::Watermark::pushButton_Watermark).desc = "";
    watermark_options->at(UI::EditOption::Watermark::label_WatermarkLocation).data = ImageEditor::WatermarkLocation::BottomRight;
    watermark_options->at(UI::EditOption::Watermark::label_WatermarkLocation).name = "Location:";
    watermark_options->at(UI::EditOption::Watermark::label_WatermarkLocation).desc = "Choose where to place the watermark image.";
    watermark_options->at(UI::EditOption::Watermark::label_WatermarkTransparency).data = 100;
    watermark_options->at(UI::EditOption::Watermark::label_WatermarkTransparency).name = "Transparency:";
    watermark_options->at(UI::EditOption::Watermark::label_WatermarkTransparency).desc = "Decreasing the transparency (alpha channel) of the watermark will blend it into the background making it \"see-through\".";
    watermark_options->at(UI::EditOption::Watermark::label_WatermarkOffset).data = 0;
    watermark_options->at(UI::EditOption::Watermark::label_WatermarkOffset).name = "X / Y Offset:";
    watermark_options->at(UI::EditOption::Watermark::label_WatermarkOffset).desc = "Offset the watermark position relative to the chosen location.";

    // comboBox_WatermarkLocation
    watermark_locations->at(ImageEditor::WatermarkLocation::TopLeft).data = ImageEditor::WatermarkLocation::TopLeft;
    watermark_locations->at(ImageEditor::WatermarkLocation::TopLeft).name = "Top Left";
    watermark_locations->at(ImageEditor::WatermarkLocation::TopLeft).desc = "Place a watermark at the top left position of each edited image.";
    watermark_locations->at(ImageEditor::WatermarkLocation::TopCenter).data = ImageEditor::WatermarkLocation::TopCenter;
    watermark_locations->at(ImageEditor::WatermarkLocation::TopCenter).name = "Top Center";
    watermark_locations->at(ImageEditor::WatermarkLocation::TopCenter).desc = "Place a watermark at the top center position of each edited image.";
    watermark_locations->at(ImageEditor::WatermarkLocation::TopRight).data = ImageEditor::WatermarkLocation::TopRight;
    watermark_locations->at(ImageEditor::WatermarkLocation::TopRight).name = "Top Right";
    watermark_locations->at(ImageEditor::WatermarkLocation::TopRight).desc = "Place a watermark at the top right position of each edited image.";
    watermark_locations->at(ImageEditor::WatermarkLocation::CenterLeft).data = ImageEditor::WatermarkLocation::CenterLeft;
    watermark_locations->at(ImageEditor::WatermarkLocation::CenterLeft).name = "Center Left";
    watermark_locations->at(ImageEditor::WatermarkLocation::CenterLeft).desc = "Place a watermark at the center left position of each edited image.";
    watermark_locations->at(ImageEditor::WatermarkLocation::Center).data = ImageEditor::WatermarkLocation::Center;
    watermark_locations->at(ImageEditor::WatermarkLocation::Center).name = "Center";
    watermark_locations->at(ImageEditor::WatermarkLocation::Center).desc = "Place a watermark at the center (middle) position of each edited image.";
    watermark_locations->at(ImageEditor::WatermarkLocation::CenterRight).data = ImageEditor::WatermarkLocation::CenterRight;
    watermark_locations->at(ImageEditor::WatermarkLocation::CenterRight).name = "Center Right";
    watermark_locations->at(ImageEditor::WatermarkLocation::CenterRight).desc = "Place a watermark at the center right position of each edited image.";
    watermark_locations->at(ImageEditor::WatermarkLocation::BottomLeft).data = ImageEditor::WatermarkLocation::BottomLeft;
    watermark_locations->at(ImageEditor::WatermarkLocation::BottomLeft).name = "Bottom Left";
    watermark_locations->at(ImageEditor::WatermarkLocation::BottomLeft).desc = "Place a watermark at the bottom left position of each edited image.";
    watermark_locations->at(ImageEditor::WatermarkLocation::BottomCenter).data = ImageEditor::WatermarkLocation::BottomCenter;
    watermark_locations->at(ImageEditor::WatermarkLocation::BottomCenter).name = "Bottom Center";
    watermark_locations->at(ImageEditor::WatermarkLocation::BottomCenter).desc = "Place a watermark at the bottom center position of each edited image.";
    watermark_locations->at(ImageEditor::WatermarkLocation::BottomRight).data = ImageEditor::WatermarkLocation::BottomRight;
    watermark_locations->at(ImageEditor::WatermarkLocation::BottomRight).name = "Bottom Right";
    watermark_locations->at(ImageEditor::WatermarkLocation::BottomRight).desc = "Place a watermark at the bottom right position of each edited image.";

    // comboBox_AddText
    file_name_creation->at(ImageSaver::MetadataFlags::FILE_NAME).data = ImageSaver::MetadataFlags::FILE_NAME;
    file_name_creation->at(ImageSaver::MetadataFlags::FILE_NAME).name = "Original File Name";
    file_name_creation->at(ImageSaver::MetadataFlags::FILE_NAME).desc = "Add the original file name into the creation of a new file name.";
    file_name_creation->at(ImageSaver::MetadataFlags::COUNTER).data = ImageSaver::MetadataFlags::COUNTER;
    file_name_creation->at(ImageSaver::MetadataFlags::COUNTER).name = "Incrementing Counter";
    file_name_creation->at(ImageSaver::MetadataFlags::COUNTER).desc = "Add an incrementing number into the creation of a new file name.";
    file_name_creation->at(ImageSaver::MetadataFlags::WIDTH).data = ImageSaver::MetadataFlags::WIDTH;
    file_name_creation->at(ImageSaver::MetadataFlags::WIDTH).name = "Image Width";
    file_name_creation->at(ImageSaver::MetadataFlags::WIDTH).desc = "Add the width of the modified image into the creation of a new file name.";
    file_name_creation->at(ImageSaver::MetadataFlags::HEIGHT).data = ImageSaver::MetadataFlags::HEIGHT;
    file_name_creation->at(ImageSaver::MetadataFlags::HEIGHT).name = "Image Height";
    file_name_creation->at(ImageSaver::MetadataFlags::HEIGHT).desc = "Add the height of the modified image into the creation of a new file name.";
    
    // Cast a list of std::strings to QStrings
    for (uint i = 0; i < ImageSaver::ExtensionList.size(); i++) {
        extension_list.at(i) = QString::fromStdString(ImageSaver::ExtensionList.at(i));
    }

    // comboBox_ImageFormat
    image_formats->at(ImageSaver::ImageExtension::jpeg).data = ImageSaver::ImageExtension::jpeg;
    image_formats->at(ImageSaver::ImageExtension::jpeg).name = "JPEG Files - *.jpeg";
    image_formats->at(ImageSaver::ImageExtension::jpeg).desc = "JPEG (Joint Photographic Experts Group) is a commonly used method of lossy compression\n" \
        "for digital images, particularly for those images produced by digital photography.\n" \
        "The degree of compression can be adjusted, allowing a selectable tradeoff between\n" \
        "storage size and image quality.JPEG typically achieves 10:1 compression with little\n" \
        "perceptible loss in image quality.";
    image_formats->at(ImageSaver::ImageExtension::jpg).data = ImageSaver::ImageExtension::jpg;
    image_formats->at(ImageSaver::ImageExtension::jpg).name = "JPEG Files - *.jpg";
    image_formats->at(ImageSaver::ImageExtension::jpg).desc = image_formats->at(ImageSaver::ImageExtension::jpeg).desc;
    image_formats->at(ImageSaver::ImageExtension::jpe).data = ImageSaver::ImageExtension::jpe;
    image_formats->at(ImageSaver::ImageExtension::jpe).name = "JPEG Files - *.jpe";
    image_formats->at(ImageSaver::ImageExtension::jpe).desc = image_formats->at(ImageSaver::ImageExtension::jpeg).desc;
    image_formats->at(ImageSaver::ImageExtension::jp2).data = ImageSaver::ImageExtension::jp2;
    image_formats->at(ImageSaver::ImageExtension::jp2).name = "JPEG 2000 Files - *.jp2";
    image_formats->at(ImageSaver::ImageExtension::jp2).desc = "JPEG 2000 (Joint Photographic Experts Group) is an image compression standard based on a\n" \
        "discrete wavelet transform (DWT). Note that it is still not widely supported in web\n" \
        "browsers (other than Safari) and hence is not generally used on the World Wide Web.";
    image_formats->at(ImageSaver::ImageExtension::png).data = ImageSaver::ImageExtension::png;
    image_formats->at(ImageSaver::ImageExtension::png).name = "Portable Network Graphics - *.png";
    image_formats->at(ImageSaver::ImageExtension::png).desc = "Portable Network Graphics (PNG) is a raster-graphics file format that supports lossless\n" \
        "data compression. PNG supports palette-based images (with palettes of 24-bit RGB or\n" \
        "32-bit RGBA colors), grayscale images (with or without an alpha channel for transparency),\n" \
        "and full-color non-palette-based RGB or RGBA images.";
    image_formats->at(ImageSaver::ImageExtension::webp).data = ImageSaver::ImageExtension::webp;
    image_formats->at(ImageSaver::ImageExtension::webp).name = "WebP - *.webp";
    image_formats->at(ImageSaver::ImageExtension::webp).desc = "WebP is a raster graphics file format developed by Google intended as a replacement for\n" \
        "JPEG, PNG, and GIF file formats. It supports both lossy and lossless compression, as well\n" \
        "as animation and alpha transparency.";
    image_formats->at(ImageSaver::ImageExtension::bmp).data = ImageSaver::ImageExtension::bmp;
    image_formats->at(ImageSaver::ImageExtension::bmp).name = "Windows Bitmaps - *.bmp";
    image_formats->at(ImageSaver::ImageExtension::bmp).desc = "The BMP file format or bitmap, is a raster graphics image file format used to store\n" \
        "bitmap digital images, independently of the display device (such as a graphics adapter),\n" \
        "especially on Microsoft Windows and OS/2 operating systems.";
    image_formats->at(ImageSaver::ImageExtension::dib).data = ImageSaver::ImageExtension::dib;
    image_formats->at(ImageSaver::ImageExtension::dib).name = "Windows Bitmaps - *.dib";
    image_formats->at(ImageSaver::ImageExtension::dib).desc = image_formats->at(ImageSaver::ImageExtension::bmp).desc;
    image_formats->at(ImageSaver::ImageExtension::avif).data = ImageSaver::ImageExtension::avif;
    image_formats->at(ImageSaver::ImageExtension::avif).name = "AVIF - *.avif";
    image_formats->at(ImageSaver::ImageExtension::avif).desc = "AV1 Image File Format (AVIF) is an open, royalty-free image file format specification\n" \
        "for storing images or image sequences compressed with AV1 in the HEIF container format.\n" \
        "AV1 Supports:\n" \
        "* Multiple color spaces (HDR, SDR, color space signaling via CICP or ICC)\n" \
        "* Lossless and lossy compression\n" \
        "* 8-, 10-, and 12-bit color depths\n" \
        "* Monochrome (alpha/depth) or multi-components\n" \
        "* 4:2:0, 4:2:2, 4:4:4 chroma subsampling and RGB\n" \
        "* Film grain synthesis\n" \
        "* Image sequences/animation";
    image_formats->at(ImageSaver::ImageExtension::pbm).data = ImageSaver::ImageExtension::pbm;
    image_formats->at(ImageSaver::ImageExtension::pbm).name = "Netpbm Formats - *.pbm";
    image_formats->at(ImageSaver::ImageExtension::pbm).desc = "Netpbm (formerly Pbmplus) is an open-source package of graphics programs and a programming\n" \
        "library. It is used mainly in the Unix world, but also works on Microsoft Windows, macOS,\n" \
        "and other operating systems.  Graphics formats used and defined by the Netpbm project:\n" \
        "portable pixmap format (PPM), portable graymap format (PGM), and portable bitmap format (PBM).\n" \
        "They are also sometimes referred to collectively as the portable anymap format (PNM).";
    image_formats->at(ImageSaver::ImageExtension::pgm).data = ImageSaver::ImageExtension::pgm;
    image_formats->at(ImageSaver::ImageExtension::pgm).name = "Netpbm Formats - *.pgm";
    image_formats->at(ImageSaver::ImageExtension::pgm).desc = image_formats->at(ImageSaver::ImageExtension::pbm).desc;
    image_formats->at(ImageSaver::ImageExtension::ppm).data = ImageSaver::ImageExtension::ppm;
    image_formats->at(ImageSaver::ImageExtension::ppm).name = "Netpbm Formats - *.ppm";
    image_formats->at(ImageSaver::ImageExtension::ppm).desc = image_formats->at(ImageSaver::ImageExtension::pbm).desc;
    image_formats->at(ImageSaver::ImageExtension::pxm).data = ImageSaver::ImageExtension::pxm;
    image_formats->at(ImageSaver::ImageExtension::pxm).name = "Netpbm Formats - *.pxm";
    image_formats->at(ImageSaver::ImageExtension::pxm).desc = image_formats->at(ImageSaver::ImageExtension::pbm).desc;
    image_formats->at(ImageSaver::ImageExtension::pnm).data = ImageSaver::ImageExtension::pnm;
    image_formats->at(ImageSaver::ImageExtension::pnm).name = "Netpbm Formats - *.pnm";
    image_formats->at(ImageSaver::ImageExtension::pnm).desc = image_formats->at(ImageSaver::ImageExtension::pbm).desc;
    image_formats->at(ImageSaver::ImageExtension::pfm).data = ImageSaver::ImageExtension::pfm;
    image_formats->at(ImageSaver::ImageExtension::pfm).name = "Netpbm Formats - *.pfm";
    image_formats->at(ImageSaver::ImageExtension::pfm).desc = "The PFM (Portable Floatmap) is supported by the de facto reference implementation Netpbm\n" \
        "and is the unofficial four byte IEEE 754 single precision floating point extension.\n" \
        "PFM is supported by the programs Photoshop, GIMP, and ImageMagick.";
    // TODO: Test support for this format
    image_formats->at(ImageSaver::ImageExtension::pam).data = ImageSaver::ImageExtension::pam;
    image_formats->at(ImageSaver::ImageExtension::pam).name = "Netpbm Formats - *.pam";
    image_formats->at(ImageSaver::ImageExtension::pam).desc = "Portable Arbitrary Map (PAM) is an extension of the older binary P4...P6 graphics formats,\n" \
        "introduced with Netpbm version 9.7. PAM generalizes all features of PBM, PGM and PPM, and\n" \
        "provides for extensions. PAM is supported by XnView and FFmpeg; and defines two new\n" \
        "attributes: depth and tuple type.";
    image_formats->at(ImageSaver::ImageExtension::sr).data = ImageSaver::ImageExtension::sr;
    image_formats->at(ImageSaver::ImageExtension::sr).name = "Sun Rasters - *.sr";
    image_formats->at(ImageSaver::ImageExtension::sr).desc = "Sun Raster was a raster graphics file format used on SunOS by Sun Microsystems. ACDSee,\n" \
        "FFmpeg, GIMP, ImageMagick, IrfanView, LibreOffice, Netpbm, PaintShop Pro, PMView, and\n" \
        "XnView among others support Sun Raster image files. The format does not support transparency.";
    image_formats->at(ImageSaver::ImageExtension::ras).data = ImageSaver::ImageExtension::ras;
    image_formats->at(ImageSaver::ImageExtension::ras).name = "Sun Rasters - *.ras";
    image_formats->at(ImageSaver::ImageExtension::ras).desc = image_formats->at(ImageSaver::ImageExtension::sr).desc;
    image_formats->at(ImageSaver::ImageExtension::tiff).data = ImageSaver::ImageExtension::tiff;
    image_formats->at(ImageSaver::ImageExtension::tiff).name = "TIFF Files - *.tiff";
    image_formats->at(ImageSaver::ImageExtension::tiff).desc = "Tag Image File Format (TIFF or TIF), is an image file format for storing raster graphics\n" \
        "images, popular among graphic artists, the publishing industry, and photographers. TIFF is\n" \
        "widely supported by scanning, faxing, word processing, optical character recognition,\n" \
        "image manipulation, desktop publishing, and page-layout applications.";
    image_formats->at(ImageSaver::ImageExtension::tif).data = ImageSaver::ImageExtension::tif;
    image_formats->at(ImageSaver::ImageExtension::tif).name = "TIFF Files - *.tif";
    image_formats->at(ImageSaver::ImageExtension::tif).desc = image_formats->at(ImageSaver::ImageExtension::tiff).desc;
    image_formats->at(ImageSaver::ImageExtension::exr).data = ImageSaver::ImageExtension::exr;
    image_formats->at(ImageSaver::ImageExtension::exr).name = "OpenEXR Image Files - *.exr";
    image_formats->at(ImageSaver::ImageExtension::exr).desc = "OpenEXR is a high-dynamic range, multi-channel raster file format, created under a free\n" \
        "software license similar to the BSD license. It supports multiple channels of potentially\n" \
        "different pixel sizes, including 32-bit unsigned integer, 32-bit and 16-bit floating point\n" \
        "values, as well as various compression techniques which include lossless and lossy\n" \
        "compression algorithms. It also has arbitrary channels and encodes multiple points of view\n" \
        "such as left- and right-camera images.";
    image_formats->at(ImageSaver::ImageExtension::hdr).data = ImageSaver::ImageExtension::hdr;
    image_formats->at(ImageSaver::ImageExtension::hdr).name = "Radiance HDR - *.hdr";
    image_formats->at(ImageSaver::ImageExtension::hdr).desc = "RGBE or Radiance HDR is an image format that stores pixels as one byte each for RGB (red,\n" \
        "green, and blue) values with a one byte shared exponent. Thus it stores four bytes per pixel.\n" \
        "RGBE allows pixels to have the dynamic range and precision of floating-point values in a\n" \
        "relatively compact data structure (32 bits per pixel).";
    image_formats->at(ImageSaver::ImageExtension::pic).data = ImageSaver::ImageExtension::pic;
    image_formats->at(ImageSaver::ImageExtension::pic).name = "Radiance HDR - *.pic";
    image_formats->at(ImageSaver::ImageExtension::pic).desc = image_formats->at(ImageSaver::ImageExtension::hdr).desc;

    // Specific Format Widget Options (multiple sets)
    format_jpeg_options[UI::FileOption::FormatJpeg::label_FormatFlags].data = 1;
    format_jpeg_options[UI::FileOption::FormatJpeg::label_FormatFlags].name = "Subsampling:";
    format_jpeg_options[UI::FileOption::FormatJpeg::label_FormatFlags].desc = "Chroma subsampling is the practice of encoding images by implementing less resolution\n" \
        "for chroma information than for luma information, taking advantage of the human visual\n" \
        "system's lower acuity for color differences than for luminance.";
    format_jpeg_options[UI::FileOption::FormatJpeg::label_Quality].data = 95;
    format_jpeg_options[UI::FileOption::FormatJpeg::label_Quality].name = "Quality:";
    format_jpeg_options[UI::FileOption::FormatJpeg::label_Quality].desc = "JPEG quality can be between 0 and 100 (the higher the better). Default value is 95.";
    format_jpeg_options[UI::FileOption::FormatJpeg::checkBox_Optimize].data = 0;
    format_jpeg_options[UI::FileOption::FormatJpeg::checkBox_Optimize].name = "Optimize";
    format_jpeg_options[UI::FileOption::FormatJpeg::checkBox_Optimize].desc = "JPEG optimize can lower file sizes by striping unnecessary metadata, but only noticeable\n" \
        "at higher quality ranges (95+). Default is unchecked.";
    format_jpeg_options[UI::FileOption::FormatJpeg::checkBox_Progressive].data = 0;
    format_jpeg_options[UI::FileOption::FormatJpeg::checkBox_Progressive].name = "Progressive";
    format_jpeg_options[UI::FileOption::FormatJpeg::checkBox_Progressive].desc = "Using the JPEG interlaced progressive format data is compressed in multiple passes of\n" \
        "progressively higher detail.This is ideal for large images that will be displayed while\n" \
        "downloading over a slow connection, allowing a reasonable preview after receiving only a\n" \
        "portion of the data. Default is unchecked.";
    format_jpeg_options[UI::FileOption::FormatJpeg::label_Compression].data = 0;
    format_jpeg_options[UI::FileOption::FormatJpeg::label_Compression].name = "Restart Interval:";
    format_jpeg_options[UI::FileOption::FormatJpeg::label_Compression].desc = "Restart interval specifies the interval between restart markers, in Minimum Coded Units\n" \
        "(MCUs).They were designed to allow resynchronization after an error, but also now serve a\n" \
        "new purpose, to allow for multi - threaded JPEG encoders and decoders. Default value is 0.";
    format_jpeg_options[UI::FileOption::FormatJpeg::label_ExtraSetting1].data = -1;
    format_jpeg_options[UI::FileOption::FormatJpeg::label_ExtraSetting1].name = "Luma:";
    format_jpeg_options[UI::FileOption::FormatJpeg::label_ExtraSetting1].desc = "Separate and adjust the luma quality between to 0 and 100 (-1 don't use, default). When an\n" \
        "image is converted from RGB to Y'CBCR, the luma component is the (Y'), representing brightness.";
    format_jpeg_options[UI::FileOption::FormatJpeg::label_ExtraSetting2].data = -1;
    format_jpeg_options[UI::FileOption::FormatJpeg::label_ExtraSetting2].name = "Chroma:";
    format_jpeg_options[UI::FileOption::FormatJpeg::label_ExtraSetting2].desc = "Separate and adjust the chroma quality between to 0 and 100 (-1 don't use, default). When an\n" \
        "image is converted from RGB to Y'CBCR, the chroma component is the (CB and CR), representing color.";

    format_jp2_options[UI::FileOption::FormatJp2::label_Compression].data = 1000;
    format_jp2_options[UI::FileOption::FormatJp2::label_Compression].name = "Compression Level:";
    format_jp2_options[UI::FileOption::FormatJp2::label_Compression].desc = "Use to specify the JPEG 2000 target compression rate with values from 0 to 1000. Default value is 1000.";

    format_png_options[UI::FileOption::FormatPng::label_FormatFlags].data = 0;
    format_png_options[UI::FileOption::FormatPng::label_FormatFlags].name = "Compression Strategy:";
    format_png_options[UI::FileOption::FormatPng::label_FormatFlags].desc = "PNG compression strategies are passed to the underlying zlib processing stage, and only affect the\n" \
        "compression ratio, but not the correctness of the compressed output even if it is not set appropriately\n" \
        "The effect of the [Filter] strategy is to force more Huffman coding and less string matching; it is\n" \
        "somewhat intermediate between [Default] and [Huffman Only]. [Run-Length Encoding] is designed to be\n" \
        "almost as fast as [Huffman Only], but gives better compression for PNG image data. [Fixed (No Huffman)]\n" \
        "prevents the use of dynamic Huffman codes, allowing for a simpler decoder for special applications.";
    format_png_options[UI::FileOption::FormatPng::checkBox_Optimize].data = 0;
    format_png_options[UI::FileOption::FormatPng::checkBox_Optimize].name = "Binary Level";
    format_png_options[UI::FileOption::FormatPng::checkBox_Optimize].desc = "If binary level (bi-level) is checked a grayscale PNG image will be created. Default is unchecked.";
    format_png_options[UI::FileOption::FormatPng::label_Compression].data = 1;
    format_png_options[UI::FileOption::FormatPng::label_Compression].name = "Compression Level:";
    format_png_options[UI::FileOption::FormatPng::label_Compression].desc = "PNG compression levels are from 0 to 9 with higher values meaning a smaller file size and longer\n" \
        "compression time. If specified, the strategy is changed to [Default].\n" \
        "Default value is 1 (best speed setting).";

    format_webp_options[UI::FileOption::FormatWebp::label_Quality].data = 100;
    format_webp_options[UI::FileOption::FormatWebp::label_Quality].name = "Quality:";
    format_webp_options[UI::FileOption::FormatWebp::label_Quality].desc = "WebP quality can be between 0 and 100 (the higher the better). With 100 quality, the lossless\n" \
        "compression is used. Default value is 100.";

    format_avif_options[UI::FileOption::FormatAvif::label_Quality].data = 95;
    format_avif_options[UI::FileOption::FormatAvif::label_Quality].name = "Quality:";
    format_avif_options[UI::FileOption::FormatAvif::label_Quality].desc = "AVIF quality can be between 0 and 100 (the higher the better). Default value is 95.";
    format_avif_options[UI::FileOption::FormatAvif::label_Compression].data = 9;
    format_avif_options[UI::FileOption::FormatAvif::label_Compression].name = "Speed:";
    format_avif_options[UI::FileOption::FormatAvif::label_Compression].desc = "The AVIF creation speed can be between 0 (slowest) and 9 (fastest). Default value is 9.";
    format_avif_options[UI::FileOption::FormatAvif::label_ExtraSetting2].data = 8;
    format_avif_options[UI::FileOption::FormatAvif::label_ExtraSetting2].name = "Color Depth:";
    format_avif_options[UI::FileOption::FormatAvif::label_ExtraSetting2].desc = "AVIF can have 8-, 10- or 12-bit color depths. If greater than 8, it is stored/read as a float with\n" \
        "pixels having any value between 0 and 1.0. Default value is 8.";

    format_pbm_options[UI::FileOption::FormatPbm::checkBox_Optimize].data = 1;
    format_pbm_options[UI::FileOption::FormatPbm::checkBox_Optimize].name = "Binary Format";
    format_pbm_options[UI::FileOption::FormatPbm::checkBox_Optimize].desc = "For PBM, PGM, or PPM, using a binary format creates a grayscale image. Default is checked.";

    format_pam_options[UI::FileOption::FormatPam::label_FormatFlags].data = 0;
    format_pam_options[UI::FileOption::FormatPam::label_FormatFlags].name = "Tuple Type:";
    format_pam_options[UI::FileOption::FormatPam::label_FormatFlags].desc = "The tuple type attribute specifies what kind of image the PAM file represents, thus enabling it to\n" \
        "stand for the older Netpbm formats, as well as to be extended to new uses, like transparency.";

    format_tiff_options[UI::FileOption::FormatTiff::label_FormatFlags].data = 4;
    format_tiff_options[UI::FileOption::FormatTiff::label_FormatFlags].name = "Compression Scheme:";
    format_tiff_options[UI::FileOption::FormatTiff::label_FormatFlags].desc = "The compression scheme used on the image data. Note that this is a complete list of schemes from the\n" \
        "libtiff documents and some may not be currently supported in this app.\n" \
        "Compression schemes that do work: LWZ (default), Adobe Deflate, and Deflate.";
    format_tiff_options[UI::FileOption::FormatTiff::label_Quality].data = 2;
    format_tiff_options[UI::FileOption::FormatTiff::label_Quality].name = "Resolution Unit:";
    format_tiff_options[UI::FileOption::FormatTiff::label_Quality].desc = "The resolution unit of measurement for X and Y directions.";
    format_tiff_options[UI::FileOption::FormatTiff::label_ExtraSetting1].data = 0;
    format_tiff_options[UI::FileOption::FormatTiff::label_ExtraSetting1].name = "X:";
    format_tiff_options[UI::FileOption::FormatTiff::label_ExtraSetting1].desc = "The number of pixels per \"resolution unit\" in the image's width or X direction. Default value is 0.";
    format_tiff_options[UI::FileOption::FormatTiff::label_ExtraSetting2].data = 0;
    format_tiff_options[UI::FileOption::FormatTiff::label_ExtraSetting2].name = "Y:";
    format_tiff_options[UI::FileOption::FormatTiff::label_ExtraSetting2].desc = "The number of pixels per \"resolution unit\" in the image's height (length) or Y direction. Default value is 0.";

    format_exr_options[UI::FileOption::FormatExr::label_FormatFlags].data = 3;
    format_exr_options[UI::FileOption::FormatExr::label_FormatFlags].name = "Compression Type:";
    format_exr_options[UI::FileOption::FormatExr::label_FormatFlags].desc = "Override EXR compression type (ZLib is default)";
    format_exr_options[UI::FileOption::FormatExr::checkBox_Optimize].data = 0;
    format_exr_options[UI::FileOption::FormatExr::checkBox_Optimize].name = "Store as FP16 (HALF)";
    format_exr_options[UI::FileOption::FormatExr::checkBox_Optimize].desc = "Override EXR Storage Type";
    format_exr_options[UI::FileOption::FormatExr::checkBox_Progressive].data = 1;
    format_exr_options[UI::FileOption::FormatExr::checkBox_Progressive].name = "Store as FP32 (Default)";
    format_exr_options[UI::FileOption::FormatExr::checkBox_Progressive].desc = "Override EXR Storage Type";
    format_exr_options[UI::FileOption::FormatExr::label_Compression].data = 45;
    format_exr_options[UI::FileOption::FormatExr::label_Compression].name = "Compression Level:";
    format_exr_options[UI::FileOption::FormatExr::label_Compression].desc = "Override EXR DWA Compression Level. Default value is 45.";

    format_hdr_options[UI::FileOption::FormatHdr::label_FormatFlags].data = 0;
    format_hdr_options[UI::FileOption::FormatHdr::label_FormatFlags].name = "Compression Strategy:";
    format_hdr_options[UI::FileOption::FormatHdr::label_FormatFlags].desc = "For HDR files there are only really two strategies: compress or don't compress.";

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

    status_bar_messages.at(UI::StatusBar::SavePreset).data = 5000;
    status_bar_messages.at(UI::StatusBar::SavePreset).name = "All Presets Saved!";
    status_bar_messages.at(UI::StatusBar::clear_all_files).data = 5000;
    status_bar_messages.at(UI::StatusBar::clear_all_files).name = "Recent File History Cleared!";

    // Various Other Widget Data
    other_options->at(UI::Other::tab_1).data = 0; // Default current tab index, ignores other tab.data.
    other_options->at(UI::Other::tab_1).name = "Images";
    //other_options->at(UI::Other::tab_1).desc = "Image File Viewer Tab";
    other_options->at(UI::Other::tab_2).data = 0;
    other_options->at(UI::Other::tab_2).name = "Image Edits";
    //other_options->at(UI::Other::tab_2).desc = "Image Edit Tools Tab";
    other_options->at(UI::Other::tab_3).data = 0;
    other_options->at(UI::Other::tab_3).name = "Save Options";
    //other_options->at(UI::Other::tab_3).desc = "File Save Options Tab";
    other_options->at(UI::Other::checkBox_SearchSubDirs).data = 1;
    other_options->at(UI::Other::checkBox_SearchSubDirs).name = "When Searching Directories Search Sub-Directories As Well";
    other_options->at(UI::Other::checkBox_SearchSubDirs).desc = "When file directories/folders are dropped into the image file viewer an image file search will begin\n" \
                                                                   "in the directory, and it this is checked, all its sub-directories too.";
    other_options->at(UI::Other::label_EditSave).data = 0;
    other_options->at(UI::Other::label_EditSave).name = "Start Editing And Saving:";
    other_options->at(UI::Other::label_EditSave).desc = "\"All Images\" means all images loaded into file tree. \"Only Selected Images\" means only\n" \
                                                           "images in file tree that have their checkboxes checked.";
    other_options->at(UI::Other::pushButton_EditSaveAll).data = 0;
    other_options->at(UI::Other::pushButton_EditSaveAll).name = "All Images";
    other_options->at(UI::Other::pushButton_EditSaveAll).desc = "";
    other_options->at(UI::Other::pushButton_EditSaveSelected).data = 0;
    other_options->at(UI::Other::pushButton_EditSaveSelected).name = "Selected Images";
    other_options->at(UI::Other::pushButton_EditSaveSelected).desc = "";
    
    dialog_messages.at(Dialog::Messages::delete_dialog).data = 0; // 1 = bold message/desc text. | ## = #[current preset number]
    dialog_messages.at(Dialog::Messages::delete_dialog).name = "Delete?";
    dialog_messages.at(Dialog::Messages::delete_dialog).desc = "Delete currently highlighted image file -or- all image files selected/checked?";
    dialog_messages.at(Dialog::Messages::delete_dialog_clear).data = 0;
    dialog_messages.at(Dialog::Messages::delete_dialog_clear).name = "Clear List?";
    dialog_messages.at(Dialog::Messages::delete_dialog_clear).desc = "The entire list of image files is about to be cleared, is this OK?";
    dialog_messages.at(Dialog::Messages::CreateNewPreset).data = 0;
    dialog_messages.at(Dialog::Messages::CreateNewPreset).name = "Add New Preset";
    dialog_messages.at(Dialog::Messages::CreateNewPreset).desc = "Add Description for New Preset.";
    dialog_messages.at(Dialog::Messages::ChangePresetDescription).data = 0;
    dialog_messages.at(Dialog::Messages::ChangePresetDescription).name = "Change Preset Description";
    dialog_messages.at(Dialog::Messages::ChangePresetDescription).desc = "Change Title Description of Currently Selected Preset.";
    dialog_messages.at(Dialog::Messages::save_preset_dialog).data = 0;
    dialog_messages.at(Dialog::Messages::save_preset_dialog).name = "Save Preset ##?";
    dialog_messages.at(Dialog::Messages::save_preset_dialog).desc = "There are unsaved options in [Preset ##], would you like to save these options before proceeding?";
    dialog_messages.at(Dialog::Messages::save_preset_dialog_closing).data = 0;
    dialog_messages.at(Dialog::Messages::save_preset_dialog_closing).name = "Closing...";
    dialog_messages.at(Dialog::Messages::save_preset_dialog_closing).desc = "There are unsaved preset options, would you like to save these options before closing?";
    dialog_messages.at(Dialog::Messages::remove_preset_dialog).data = 1;
    dialog_messages.at(Dialog::Messages::remove_preset_dialog).name = "Remove Preset ##";
    dialog_messages.at(Dialog::Messages::remove_preset_dialog).desc = "Are you sure you want to remove/delete [Preset ##]?";
    dialog_messages.at(Dialog::Messages::remove_preset_dialog_halted).data = 0;
    dialog_messages.at(Dialog::Messages::remove_preset_dialog_halted).name = "Cannot Remove Preset";
    dialog_messages.at(Dialog::Messages::remove_preset_dialog_halted).desc = "You cannot remove/delete the last remaining preset.";
    dialog_messages.at(Dialog::Messages::non_image_file_dialog).data = 0;
    dialog_messages.at(Dialog::Messages::non_image_file_dialog).name = "Warning";
    dialog_messages.at(Dialog::Messages::non_image_file_dialog).desc = "Unsupported or non-image files were not added to list.";
    dialog_messages.at(Dialog::Messages::check_wm_path_dialog).data = 0;
    dialog_messages.at(Dialog::Messages::check_wm_path_dialog).name = "Invalid Image Path";
    dialog_messages.at(Dialog::Messages::check_wm_path_dialog).desc = "The image path provided does not exist. Open file dialog window and find new path?";
    dialog_messages.at(Dialog::Messages::check_path_dialog).data = 0;
    dialog_messages.at(Dialog::Messages::check_path_dialog).name = "Invalid Directory Path";
    dialog_messages.at(Dialog::Messages::check_path_dialog).desc = "The directory path provided does not exist. Open directory dialog window and find new path?";
    dialog_messages.at(Dialog::Messages::log_created_dialog).data = 0;
    dialog_messages.at(Dialog::Messages::log_created_dialog).name = "Image Editing Finished";
    dialog_messages.at(Dialog::Messages::log_created_dialog).desc = "A new log file created with the details of the last batch of images edited.";
    dialog_messages.at(Dialog::Messages::log_created_dialog_updated).data = 0;
    dialog_messages.at(Dialog::Messages::log_created_dialog_updated).name = "Image Editing Finished";
    dialog_messages.at(Dialog::Messages::log_created_dialog_updated).desc = "Log file updated with the details of the latest batch of images edited.";
    dialog_messages.at(Dialog::Messages::log_created_dialog_error).data = 0;
    dialog_messages.at(Dialog::Messages::log_created_dialog_error).name = "Image Editing Finished";
    dialog_messages.at(Dialog::Messages::log_created_dialog_error).desc = "While image editing has finished (possibly with errors), the log file could not be created or written too.";

    file_dialog_titles.at(Dialog::FileSearch::LoadImageFiles) = "Select one or more image files to edit...";
    file_dialog_titles.at(Dialog::FileSearch::GetImageFile) = "Select an image file to use...";
    file_dialog_titles.at(Dialog::FileSearch::GetSaveDirectoryPath) = "Select a directory path...";
    file_dialog_titles.at(Dialog::FileSearch::supported_image_extensions_dialog_str) = "Images";
    file_dialog_titles.at(Dialog::FileSearch::all_files_dialog_str) = "All Files";
    file_dialog_titles.at(Dialog::FileSearch::log_file_new_save_path) = "Save Log File As...";
    file_dialog_titles.at(Dialog::FileSearch::log_file_new_save_path_extensions) = "Log Files";

    // Log File Text:  ##  =  Auto-Generated Date/Time, Number or Error Message
    log_text.at(LogFile::Line::ThickDivider) = "===================================";
    log_text.at(LogFile::Line::ThinDivider) = "-----------------------------------";
    log_text.at(LogFile::Line::Title) = "= BatchItImage Generated Log File =";
    log_text.at(LogFile::Line::SessionStart) = "Session Start: ##";
    log_text.at(LogFile::Line::SessionEnd) = "Session End:   ##";
    log_text.at(LogFile::Line::Batch) = "Batch ##";
    log_text.at(LogFile::Line::SummarySuccesses) = " ## Images Edited and Saved";
    log_text.at(LogFile::Line::SummaryErrors) = " ## Images Not Saved (Errors)";
    log_text.at(LogFile::Line::SummaryTime) = " Time to Completion: ##";
    log_text.at(LogFile::Line::UnsavedSettings) = " Using Unsaved Settings";
    log_text.at(LogFile::Line::ImageNumber) = "Image ##";
    log_text.at(LogFile::Line::SaveSuccess) = "--> [Canceled] The image editing and saving process was canceled before it could finish.";
    log_text.at(LogFile::Line::SaveCanceled) = "--> [Success] The image was edited and saved successfully.";
    log_text.at(LogFile::Line::EditError) = "--> [Edit Error] ##";
    log_text.at(LogFile::Line::SaveError) = "--> [Save Error] ##";
}

void BatchItImage::SetupFileTree()
{
    ui.treeWidget_FileInfo->clear();
    for (int col = 0; col < FileTree::Column::COUNT; col++) {
        ui.treeWidget_FileInfo->headerItem()->setText(col, file_tree_headers->at(col).name);
        ui.treeWidget_FileInfo->headerItem()->setToolTip(col, file_tree_headers->at(col).desc);
    }
    ui.treeWidget_FileInfo->setColumnWidth(FileTree::Column::FILE_SELECTED, ui.treeWidget_FileInfo->minimumWidth());
    ui.treeWidget_FileInfo->header()->setSectionsClickable(true);
    ui.treeWidget_FileInfo->header()->sortIndicatorOrder();
    ui.treeWidget_FileInfo->installEventFilter(this); // Keep watch of all events happening in file tree. -> eventFilter()
    ui.treeWidget_FileInfo->setMouseTracking(true);
}

template<std::size_t ui_data_size>
void BatchItImage::AddUiDataTo(const std::array<QString, ui_data_size>& ui_data, const std::vector<QObject*>& objects)
{
    for (uint i = 0; i < objects.size(); i++) {
        AddUiDataTo(objects.at(i), ui_data.at(i));
    }
}

void BatchItImage::AddUiDataTo(QObject* object, const QString& ui_data)
{
    std::string object_class = object->metaObject()->className();
    if ("QAction" == object_class) {
        QAction* mba = qobject_cast<QAction*>(object);
        mba->setText(ui_data);
    }
    else if ("QMenu" == object_class) {
        QMenu* mbm = qobject_cast<QMenu*>(object);
        mbm->setTitle(ui_data);
    }
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
    //qDebug() << "AddUiDataTo:" <<  object_class;
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
    const uint default_index, const std::array <std::string, string_data_size>& string_data)
{
    // Convert std::string array to QString array
    std::array <QString, string_data_size> qstring_data;
    for (int i = 0; i < ui_data_size; i++) {
        qstring_data.at(i) = QString::fromStdString(string_data.at(i));
    }
    PopulateComboBox(cb, ui_data, default_index, qstring_data);
}

template<std::size_t ui_data_size, std::size_t string_data_size>
void BatchItImage::PopulateComboBox(QComboBox* cb, const std::array<UIData, ui_data_size>& ui_data,
    const uint default_index, const std::array <QString, string_data_size>& string_data)
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
    cb->setCurrentIndex(default_index);
    UpdateComboBoxTextTips(cb);
}

void BatchItImage::UpdateComboBoxTextTips(QComboBox* combo_box)
{
    QComboBox* cb;
    if (combo_box)
        cb = combo_box;
    else
        cb = qobject_cast<QComboBox*>(sender());
    cb->setStatusTip(cb->currentData(Qt::StatusTipRole).toString());
    cb->setToolTip(cb->currentData(Qt::ToolTipRole).toString());
    //qDebug() << "UpdateComboBoxTextTips:" <<  cb->objectName().toStdString();
}

void BatchItImage::UpdateLineEditTextTips(QLineEdit* line_edit)
{
    QLineEdit* le;
    if (line_edit)
        le = line_edit;
    else
        le = qobject_cast<QLineEdit*>(sender());
    le->setStatusTip(le->text());
    le->setToolTip(le->text());
}

void BatchItImage::UiConnections()
{
    // Menu Bar
    Q_ASSERT(connect(ui.action_AddImages, SIGNAL(triggered(bool)), this, SLOT(LoadImageFiles())));
    Q_ASSERT(connect(ui.action_OpenLogDirectory, &QAction::triggered, this,
        [this] {
            bool log_directory_error = CreateDirectories(log_directory_path);
            if (not log_directory_error) {
                // Windows Only (Mac: open, Win: explorer)
                const std::string open_log_dir_path = "start explorer \"" + (log_directory_path).string() + "\"";
                qDebug() << open_log_dir_path;
                std::system(open_log_dir_path.c_str());
            }
        }));
    Q_ASSERT(connect(ui.action_Close, &QAction::triggered, this, &BatchItImage::close));
    Q_ASSERT(connect(ui.action_AddNewPreset, SIGNAL(triggered(bool)), this, SLOT(CreateNewPreset())));
    Q_ASSERT(connect(ui.action_RemovePreset, SIGNAL(triggered(bool)), this, SLOT(RemoveCurrentPreset())));
    Q_ASSERT(connect(ui.action_SavePresets, &QAction::triggered, this, [this] { SavePreset(true); })); // TODO: Show a DialogMessage?
    Q_ASSERT(connect(ui.action_ChangePresetDesc, &QAction::triggered, this,
        [this] {
            ChangePresetDescription(
                CurrentSelectedPreset(),
                dialog_messages.at(Dialog::Messages::ChangePresetDescription).name,
                dialog_messages.at(Dialog::Messages::ChangePresetDescription).desc
            );
        }));
    Q_ASSERT(connect(ui.action_ShowFormatFilter, &QAction::triggered, this,
        [this] {
            submenu_format_filter->actions().at(
                FileTree::ActionMenu::SubMenu::FilterImageFormats::action_submenu_undock)->setVisible(false);
            submenu_format_filter->showTearOffMenu();
        }));
    Q_ASSERT(connect(ui.action_About, SIGNAL(triggered(bool)), this, SLOT(Test()))); // TODO
    Q_ASSERT(connect(ui.action_AboutQt, &QAction::triggered, this, [this] { QApplication::aboutQt(); }));
    Q_ASSERT(connect(ui.action_Help, &QAction::triggered, this, [this] { Test(); })); // TODO

    // Menu Bar: Submenu of ui.menu_RecentImageFiles
    Q_ASSERT(connect(action_load_all_files, &QAction::triggered, this, [this] { AddNewFiles(recent_file_paths_loaded); }));
    Q_ASSERT(connect(action_clear_all_files, &QAction::triggered, this,
        [this] {
            recent_file_paths_loaded.clear();
            QSettings settings(preset_settings_file, QSettings::IniFormat);
            settings.remove("Recent");
            BuildRecentFilesMenu();
            ui.statusbar->showMessage(
                status_bar_messages.at(UI::StatusBar::clear_all_files).name,
                status_bar_messages.at(UI::StatusBar::clear_all_files).data
            );
        }));

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
                preset_list.at(CurrentSelectedPreset()).widthModifierIndex(), index
            );
            UpdateComboBoxTextTips();
        }));
    Q_ASSERT(connect(ui.spinBox_WidthNumber, &QSpinBox::valueChanged, this,
        [=](int value) {
            edit_options_change_tracker = TrackOptionChanges(
                edit_options_change_tracker, Option.spinBox_WidthNumber,
                preset_list.at(CurrentSelectedPreset()).widthNumber(), value
            );
        }));
    Q_ASSERT(connect(ui.comboBox_HeightMod, &QComboBox::currentIndexChanged, this,
        [=](int index) {
            edit_options_change_tracker = TrackOptionChanges(
                edit_options_change_tracker, Option.comboBox_HeightMod,
                preset_list.at(CurrentSelectedPreset()).heightModifierIndex(), index
            );
            UpdateComboBoxTextTips();
        }));
    Q_ASSERT(connect(ui.spinBox_HeightNumber, &QSpinBox::valueChanged, this,
        [=](int value) {
            edit_options_change_tracker = TrackOptionChanges(
                edit_options_change_tracker, Option.spinBox_HeightNumber,
                preset_list.at(CurrentSelectedPreset()).heightNumber(), value
            );
        }));
    Q_ASSERT(connect(ui.comboBox_Resample, &QComboBox::currentIndexChanged, this,
        [=](int index) {
            edit_options_change_tracker = TrackOptionChanges(
                edit_options_change_tracker, Option.comboBox_Resample,
                preset_list.at(CurrentSelectedPreset()).resamplingFilterIndex(), index
            );
            UpdateComboBoxTextTips();
        }));
    Q_ASSERT(connect(ui.checkBox_KeepAspectRatio, &QCheckBox::stateChanged, this,
        [=](int value) {
            edit_options_change_tracker = TrackOptionChanges(
                edit_options_change_tracker, Option.checkBox_KeepAspectRatio,
                preset_list.at(CurrentSelectedPreset()).keepAspectRatio(), (value) ? 1 : 0
            );
        }));
    Q_ASSERT(connect(ui.comboBox_BorderType, &QComboBox::currentIndexChanged, this,
        [=](int index) {
            edit_options_change_tracker = TrackOptionChanges(
                edit_options_change_tracker, Option.comboBox_BorderType,
                preset_list.at(CurrentSelectedPreset()).borderTypeIndex(), index
            );
            UpdateComboBoxTextTips();
        }));
    Q_ASSERT(connect(ui.pushButton_ColorDialog, &QAbstractButton::pressed, this,
        [this] {
            background_color = QColorDialog::getColor(
                background_color, this,
                background_options.at(UI::EditOption::Background::pushButton_ColorDialog).desc,
                QColorDialog::ShowAlphaChannel
            );
            qDebug() << "pushButton_ColorDialog:" << background_color.blue() << "x" << background_color.green()
                << "x" << background_color.red() << ":" << background_color.alpha();
            SetColorPreviewStyleSheet();
            bool changed = true;
            if (background_color == preset_list.at(CurrentSelectedPreset()).backgroundColor()) {
                changed = false;
            }
            /*if (background_color.blue() == preset_list.at(CurrentSelectedPreset()).background_color_blue and
                background_color.green() == preset_list.at(CurrentSelectedPreset()).background_color_green and
                background_color.red() == preset_list.at(CurrentSelectedPreset()).background_color_red and
                background_color.alpha() == preset_list.at(CurrentSelectedPreset()).background_color_alpha) {
                changed = false;
            }*/
            edit_options_change_tracker = TrackOptionChanges(
                edit_options_change_tracker, Option.pushButton_ColorDialog, 0, changed
            );
        }));
    Q_ASSERT(connect(ui.comboBox_BlurFilter, &QComboBox::currentIndexChanged, this,
        [=](int index) {
            int preset_value = preset_list.at(CurrentSelectedPreset()).blurFilterIndex();
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
                preset_list.at(CurrentSelectedPreset()).blurNormalize(), (value) ? 1 : 0
            );
        }));
    Q_ASSERT(connect(ui.verticalSlider_BlurX1, &QSlider::valueChanged, this,
        [=](int value) {
            edit_options_change_tracker = TrackOptionChanges(
                edit_options_change_tracker, Option.verticalSlider_BlurX1,
                preset_list.at(CurrentSelectedPreset()).blurX(), value
            );
        }));
    Q_ASSERT(connect(ui.verticalSlider_BlurY1, &QSlider::valueChanged, this,
        [=](int value) {
            edit_options_change_tracker = TrackOptionChanges(
                edit_options_change_tracker, Option.verticalSlider_BlurY1,
                preset_list.at(CurrentSelectedPreset()).blurY(), value
            );
        }));
    Q_ASSERT(connect(ui.verticalSlider_BlurX2, &QSlider::valueChanged, this,
        [=](int value) {
            edit_options_change_tracker = TrackOptionChanges(
                edit_options_change_tracker, Option.verticalSlider_BlurX2,
                preset_list.at(CurrentSelectedPreset()).blurSX(), value
            );
        }));
    Q_ASSERT(connect(ui.verticalSlider_BlurY2, &QSlider::valueChanged, this,
        [=](int value) {
            edit_options_change_tracker = TrackOptionChanges(
                edit_options_change_tracker, Option.verticalSlider_BlurY2,
                preset_list.at(CurrentSelectedPreset()).blurSY(), value
            );
        }));
    Q_ASSERT(connect(ui.verticalSlider_BlurD, &QSlider::valueChanged, this,
        [=](int value) {
            edit_options_change_tracker = TrackOptionChanges(
                edit_options_change_tracker, Option.verticalSlider_BlurD,
                preset_list.at(CurrentSelectedPreset()).blurDepth(), value
            );
        }));
    Q_ASSERT(connect(ui.dial_Rotation, &QSlider::valueChanged, this,
        [=](int value) {
            ui.lcdNumber_Rotation->display(value);
            edit_options_change_tracker = TrackOptionChanges(
                edit_options_change_tracker, Option.dial_Rotation,
                preset_list.at(CurrentSelectedPreset()).rotationDegrees(), value
            );
        }));
    Q_ASSERT(connect(ui.checkBox_IncreaseBounds, &QCheckBox::stateChanged, this,
        [=](int value) {
            edit_options_change_tracker = TrackOptionChanges(
                edit_options_change_tracker, Option.checkBox_IncreaseBounds,
                preset_list.at(CurrentSelectedPreset()).increaseBoundaries(), (value) ? 1 : 0
            );
        }));
    Q_ASSERT(connect(ui.checkBox_FlipImage, &QCheckBox::stateChanged, this,
        [=](int value) {
            edit_options_change_tracker = TrackOptionChanges(
                edit_options_change_tracker, Option.checkBox_FlipImage,
                preset_list.at(CurrentSelectedPreset()).flipImage(), (value) ? 1 : 0
            );
        }));
    Q_ASSERT(connect(ui.groupBox_Watermark, &QGroupBox::toggled, this,
        [=](int value) {
            //std::string preset_value = preset_list.at(CurrentSelectedPreset()).watermark_path;
            if (value) {
                EnableOptionGroup(ui.groupBox_Watermark->children(), true);
            }
            else {
                EnableOptionGroup(ui.groupBox_Watermark->children(), false);
            }
            // If "off/None" remove below changed options.
            //if (value == 0 and preset_value.length() == 0) {
            if (value == 0 and not preset_list.at(CurrentSelectedPreset()).watermarkAdded()) {
                edit_options_change_tracker = RemoveOptionsChanged(edit_options_change_tracker, std::vector<uint>{
                    Option.lineEdit_WatermarkPath, Option.comboBox_WatermarkLocation, Option.spinBox_WatermarkTransparency,
                        Option.spinBox_WatermarkOffsetX, Option.spinBox_WatermarkOffsetY
                });
            }
            else if (value) { // Trigger all watermark tracking calculations (aka emit signals).
                ui.lineEdit_WatermarkPath->editingFinished();
                ui.comboBox_WatermarkLocation->currentIndexChanged(ui.comboBox_WatermarkLocation->currentIndex());
                ui.spinBox_WatermarkTransparency->valueChanged(ui.spinBox_WatermarkTransparency->value());
                ui.spinBox_WatermarkOffsetX->valueChanged(ui.spinBox_WatermarkOffsetX->value());
                ui.spinBox_WatermarkOffsetY->valueChanged(ui.spinBox_WatermarkOffsetY->value());
            }
        }));
    Q_ASSERT(connect(ui.pushButton_Watermark, &QAbstractButton::pressed, this,
        [this] {
            QString last_verified_watermark_path = (last_existing_wm_path.length() > 0) ? 
                last_existing_wm_path : preset_list.at(CurrentSelectedPreset()).watermarkPath();
            ui.lineEdit_WatermarkPath->setText(GetImageFile(last_verified_watermark_path));
            UpdateLineEditTextTips(ui.lineEdit_WatermarkPath);
            edit_options_change_tracker = TrackOptionChanges(
                edit_options_change_tracker,
                Option.lineEdit_WatermarkPath,
                preset_list.at(CurrentSelectedPreset()).watermarkPath().toStdString(),
                ui.lineEdit_WatermarkPath->text().toStdString()
            );
            
        }));
    Q_ASSERT(connect(ui.lineEdit_WatermarkPath, &QLineEdit::editingFinished, this,
        [this] {
            if (ui.lineEdit_WatermarkPath->text().length() > 0) {
                CheckWatermarkPath();
            }
            UpdateLineEditTextTips();
            edit_options_change_tracker = TrackOptionChanges(
                edit_options_change_tracker,
                Option.lineEdit_WatermarkPath,
                preset_list.at(CurrentSelectedPreset()).watermarkPath().toStdString(),
                ui.lineEdit_WatermarkPath->text().toStdString()
            );
        }));
    Q_ASSERT(connect(ui.comboBox_WatermarkLocation, &QComboBox::currentIndexChanged, this,
        [=](int index) {
            edit_options_change_tracker = TrackOptionChanges(
                edit_options_change_tracker, Option.comboBox_WatermarkLocation,
                preset_list.at(CurrentSelectedPreset()).watermarkLocationIndex(), index
            );
            UpdateComboBoxTextTips();
        }));
    Q_ASSERT(connect(ui.spinBox_WatermarkTransparency, &QSpinBox::valueChanged, this,
        [=](int value) {
            edit_options_change_tracker = TrackOptionChanges(
                edit_options_change_tracker, Option.spinBox_WatermarkTransparency,
                preset_list.at(CurrentSelectedPreset()).watermarkTransparency(), value
            );
        }));
    Q_ASSERT(connect(ui.spinBox_WatermarkOffsetX, &QSpinBox::valueChanged, this,
        [=](int value) {
            edit_options_change_tracker = TrackOptionChanges(
                edit_options_change_tracker, Option.spinBox_WatermarkOffsetX,
                preset_list.at(CurrentSelectedPreset()).watermarkOffsetX(), value
            );
        }));
    Q_ASSERT(connect(ui.spinBox_WatermarkOffsetY, &QSpinBox::valueChanged, this,
        [=](int value) {
            edit_options_change_tracker = TrackOptionChanges(
                edit_options_change_tracker, Option.spinBox_WatermarkOffsetY,
                preset_list.at(CurrentSelectedPreset()).watermarkOffsetY(), value
            );
        }));

    // Image Save Widgets
    Q_ASSERT(connect(ui.radioButton_Overwrite, &QRadioButton::toggled, this,
        [=](int value) {
            int preset_value = preset_list.at(CurrentSelectedPreset()).saveFileProcedureIndex();
            save_options_change_tracker = TrackOptionChanges(
                save_options_change_tracker, Option.radioButton_Overwrite,
                preset_value, (value) ? ImageSaver::SaveOptionFlag::OVERWRITE : preset_value
            );
        }));
    Q_ASSERT(connect(ui.radioButton_RenameOriginal, &QRadioButton::toggled, this,
        [=](int value) {
            int preset_value = preset_list.at(CurrentSelectedPreset()).saveFileProcedureIndex();
            save_options_change_tracker = TrackOptionChanges(
                save_options_change_tracker, Option.radioButton_RenameOriginal,
                preset_value, (value) ? ImageSaver::SaveOptionFlag::RENAME_ORIGINAL : preset_value
            );
        }));
    Q_ASSERT(connect(ui.radioButton_NewFileName, &QRadioButton::toggled, this,
        [=](int value) {
            int preset_value = preset_list.at(CurrentSelectedPreset()).saveFileProcedureIndex();
            save_options_change_tracker = TrackOptionChanges(
                save_options_change_tracker, Option.radioButton_NewFileName,
                preset_value, (value) ? ImageSaver::SaveOptionFlag::NEW_NAME : preset_value
            );
        }));
    Q_ASSERT(connect(ui.comboBox_AddText, &QComboBox::activated, this,
        [this] {
            AddTextToFileName();
            UpdateLineEditTextTips(ui.lineEdit_FileName);
            save_options_change_tracker = TrackOptionChanges(
                save_options_change_tracker,
                Option.lineEdit_FileName,
                preset_list.at(CurrentSelectedPreset()).saveFileNameChange(),
                ui.lineEdit_FileName->text().toStdString()
            );
        }));
    Q_ASSERT(connect(ui.comboBox_AddText, SIGNAL(currentIndexChanged(int)), this, SLOT(UpdateComboBoxTextTips())));
    Q_ASSERT(connect(ui.lineEdit_FileName, &QLineEdit::editingFinished, this,
        [this] {
            UpdateLineEditTextTips();
            save_options_change_tracker = TrackOptionChanges(
                save_options_change_tracker,
                Option.lineEdit_FileName, 
                preset_list.at(CurrentSelectedPreset()).saveFileNameChange(),
                ui.lineEdit_FileName->text().toStdString()
            );
        }));
    Q_ASSERT(connect(ui.radioButton_RelativePath, &QRadioButton::toggled, this,
        [=](int value) {
            int preset_value = preset_list.at(CurrentSelectedPreset()).savePathRelative();
            save_options_change_tracker = TrackOptionChanges(
                save_options_change_tracker, Option.radioButton_RelativePath,
                preset_value, (value) ? 1 : preset_value
            );
            if (value) {
                save_options_change_tracker = TrackOptionChanges(
                    save_options_change_tracker,
                    Option.lineEdit_RelativePath,
                    preset_list.at(CurrentSelectedPreset()).saveFilePathChange().toStdString(),
                    ui.lineEdit_RelativePath->text().toStdString()
                );
                save_options_change_tracker = RemoveOptionsChanged(save_options_change_tracker, std::vector<uint>{
                    Option.lineEdit_AbsolutePath
                });
            }
        }));
    Q_ASSERT(connect(ui.radioButton_AbsolutePath, &QRadioButton::toggled, this,
        [=](int value) {
            int preset_value = preset_list.at(CurrentSelectedPreset()).savePathRelative();
            save_options_change_tracker = TrackOptionChanges(
                save_options_change_tracker, Option.radioButton_AbsolutePath,
                preset_value, (value) ? 0 : preset_value
            );
            if (value) {
                save_options_change_tracker = TrackOptionChanges(
                    save_options_change_tracker,
                    Option.lineEdit_AbsolutePath,
                    preset_list.at(CurrentSelectedPreset()).saveFilePathChange().toStdString(),
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
            UpdateLineEditTextTips();
            save_options_change_tracker = TrackOptionChanges(
                save_options_change_tracker,
                Option.lineEdit_RelativePath,
                preset_list.at(CurrentSelectedPreset()).saveFilePathChange().toStdString(),
                ui.lineEdit_RelativePath->text().toStdString()
            );
        }));
    Q_ASSERT(connect(ui.pushButton_AddBackOneDir, &QAbstractButton::pressed, this,
        [this] {
            ui.lineEdit_RelativePath->setText(ui.lineEdit_RelativePath->text().prepend("../"));
            CheckRelativePath();
            UpdateLineEditTextTips(ui.lineEdit_RelativePath);
            if (ui.radioButton_RelativePath->isChecked()) {
                save_options_change_tracker = TrackOptionChanges(
                    save_options_change_tracker,
                    Option.lineEdit_RelativePath,
                    preset_list.at(CurrentSelectedPreset()).saveFilePathChange().toStdString(),
                    ui.lineEdit_RelativePath->text().toStdString()
                );
            }
        }));
    Q_ASSERT(connect(ui.lineEdit_AbsolutePath, &QLineEdit::editingFinished, this,
        [this] {
            CheckAbsolutePath();
            UpdateLineEditTextTips();
            save_options_change_tracker = TrackOptionChanges(
                save_options_change_tracker,
                Option.lineEdit_AbsolutePath,
                preset_list.at(CurrentSelectedPreset()).saveFilePathChange().toStdString(),
                ui.lineEdit_AbsolutePath->text().toStdString()
            );
        }));
    Q_ASSERT(connect(ui.pushButton_FindAbsolutePath, &QAbstractButton::pressed, this,
        [this] {
            ui.lineEdit_AbsolutePath->setText(GetSaveDirectoryPath());
            UpdateLineEditTextTips(ui.lineEdit_AbsolutePath);
            if (ui.radioButton_AbsolutePath->isChecked()) {
                save_options_change_tracker = TrackOptionChanges(
                    save_options_change_tracker,
                    Option.lineEdit_AbsolutePath,
                    preset_list.at(CurrentSelectedPreset()).saveFilePathChange().toStdString(),
                    ui.lineEdit_AbsolutePath->text().toStdString()
                );
            }
        }));
    Q_ASSERT(connect(ui.groupBox_ChangeFormat, &QGroupBox::toggled, this,
        [=](int value) {
            int preset_value = preset_list.at(CurrentSelectedPreset()).formatChanged();
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
                preset_list.at(CurrentSelectedPreset()).formatExtensionIndex(), index
            );
        }));
    Q_ASSERT(connect(ui.comboBox_FormatFlags, &QComboBox::currentIndexChanged, this,
        [=](int index) {
            UpdateComboBoxTextTips();
            if (ui.comboBox_ImageFormat->currentData() == ".exr") { // TODO: track this special case? .exr currently doesn't work/not suported anyways.
                if (ui.comboBox_FormatFlags->currentData() == cv::IMWRITE_EXR_COMPRESSION_DWAA or
                    ui.comboBox_FormatFlags->currentData() == cv::IMWRITE_EXR_COMPRESSION_DWAB) {
                    qDebug() << "IMWRITE_EXR_COMPRESSION_DWA";
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
                preset_list.at(CurrentSelectedPreset()).formatFormatFlagIndex(), index
            );

        }));
    Q_ASSERT(connect(ui.horizontalSlider_Quality, &QSlider::valueChanged, this,
        [=](int value) {
            save_options_change_tracker = TrackOptionChanges(
                save_options_change_tracker, Option.horizontalSlider_Quality,
                preset_list.at(CurrentSelectedPreset()).formatQuality(), value
            );
        }));
    Q_ASSERT(connect(ui.checkBox_Optimize, &QCheckBox::stateChanged, this,
        [=](int value) {
            save_options_change_tracker = TrackOptionChanges(
                save_options_change_tracker, Option.checkBox_Optimize,
                preset_list.at(CurrentSelectedPreset()).formatOptimize(), (value) ? 1 : 0
            );
        }));
    Q_ASSERT(connect(ui.checkBox_Progressive, &QCheckBox::stateChanged, this,
        [=](int value) {
            save_options_change_tracker = TrackOptionChanges(
                save_options_change_tracker, Option.checkBox_Progressive,
                preset_list.at(CurrentSelectedPreset()).formatProgressive(), (value) ? 1 : 0
            );
        }));
    Q_ASSERT(connect(ui.spinBox_Compression, &QSpinBox::valueChanged, this,
        [=](int value) {
            save_options_change_tracker = TrackOptionChanges(
                save_options_change_tracker, Option.spinBox_Compression,
                preset_list.at(CurrentSelectedPreset()).formatCompression(), value
            );
        }));
    Q_ASSERT(connect(ui.spinBox_ExtraSetting1, &QSpinBox::valueChanged, this,
        [=](int value) {
            save_options_change_tracker = TrackOptionChanges(
                save_options_change_tracker, Option.spinBox_ExtraSetting1,
                preset_list.at(CurrentSelectedPreset()).formatExtra1(), value
            );
        }));
    Q_ASSERT(connect(ui.spinBox_ExtraSetting2, &QSpinBox::valueChanged, this,
        [=](int value) {
            save_options_change_tracker = TrackOptionChanges(
                save_options_change_tracker, Option.spinBox_ExtraSetting2,
                preset_list.at(CurrentSelectedPreset()).formatExtra2(), value
            );
        }));

    // Other Widgets
    Q_ASSERT(connect(ui.pushButton_EditSaveAll, &QPushButton::clicked, this, [this] { EditAndSave(false); }));
    Q_ASSERT(connect(ui.pushButton_EditSaveSelected, &QPushButton::clicked, this, [this] { EditAndSave(true); }));
    Q_ASSERT(connect(this, SIGNAL(progressMade(float)), ui.enhancedProgressBar, SLOT(updateProgressBar(float))));
    Q_ASSERT(connect(ui.pushButton_CancelProgress, SIGNAL(clicked(bool)), this, SLOT(CancelAllImageEditing())));
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
    qDebug() << "Options Tracker:" << tracker;
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
    qDebug() << "Options Tracker:" << tracker;
    return tracker;
}

ulong BatchItImage::RemoveOptionsChanged(ulong tracker, std::vector<uint> tracked_options)
{
    for (auto& tracked_option : tracked_options) {
        if (tracker & tracked_option) {
            tracker -= tracked_option;
        }
    }
    qDebug() << "Options Tracker (group removed):" << tracker;
    return tracker;
}

void BatchItImage::RemoveOptionsChanged()
{
    edit_options_change_tracker = 0;
    save_options_change_tracker = 0;
    qDebug() << "Options Tracker (all removed): 0";
}

void BatchItImage::EnableOptionGroup(QObjectList option_group, bool enabled)
{
    QFont font;
    if (enabled) {
        font = *font_default;
    }
    else {
        font = *font_default_light;
    }
    for (auto& object : option_group) {
        std::string object_class = object->metaObject()->className();
        //qDebug() << "EnableOptionGroup:" << object_class;

        if ("QCheckBox" == object_class) {
            QCheckBox* chb = qobject_cast<QCheckBox*>(object);
            chb->setEnabled(enabled);
            chb->setFont(font);
            //chb->setStatusTip();
            //chb->setToolTip();
        }
        else if ("QComboBox" == object_class) {
            QComboBox* cb = qobject_cast<QComboBox*>(object);
            cb->setEnabled(enabled);
            cb->setFont(font);
            //cb->setStatusTip();
            //cb->setToolTip();
        }
        else if ("QGroupBox" == object_class) {
            QGroupBox* gb = qobject_cast<QGroupBox*>(object);
            gb->setEnabled(enabled);
            gb->setFont(font);
            //gb->setStatusTip();
            //gb->setToolTip();
        }
        else if ("QLabel" == object_class) {
            QLabel* lbl = qobject_cast<QLabel*>(object);
            lbl->setEnabled(enabled);
            lbl->setFont(font);
            //lbl->setStatusTip();
            //lbl->setToolTip();
        }
        else if ("QRadioButton" == object_class) {
            QRadioButton* rb = qobject_cast<QRadioButton*>(object);
            rb->setEnabled(enabled);
            rb->setFont(font);
            //rb->setStatusTip();
            //rb->setToolTip();
        }
        else if ("QPushButton" == object_class) {
            QPushButton* pb = qobject_cast<QPushButton*>(object);
            pb->setEnabled(enabled);
            pb->setFont(font);
            //pb->setStatusTip();
            //pb->setToolTip();
        }
        else if ("QSpinBox" == object_class) {
            QSpinBox* sb = qobject_cast<QSpinBox*>(object);
            sb->setEnabled(enabled);
            sb->setFont(font);
            //sb->setStatusTip();
            //sb->setToolTip();
        }
        else if ("EnhancedSlider" == object_class) {
            EnhancedSlider* es = qobject_cast<EnhancedSlider*>(object);
            es->setEnabled(enabled);
            es->setFont(font);
        }
    }
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

    if (blur_filter_selected == ImageEditor::BlurFilter::BLUR_FILTER) {
        // Unused, BOX_FILTER covers anything BLUR_FILTER can do.
    }
    else if (blur_filter_selected == ImageEditor::BlurFilter::BOX_FILTER) {
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
    else if (blur_filter_selected == ImageEditor::BlurFilter::BILATERAL_FILTER) {
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
    else if (blur_filter_selected == ImageEditor::BlurFilter::GAUSSIAN_BLUR) {
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
    else if (blur_filter_selected == ImageEditor::BlurFilter::MEDIAN_BLUR) {
        enableOptions(X1);
        ui.verticalSlider_BlurX1->forceSingleStepInterval(true);
        ui.verticalSlider_BlurX1->setRange(3, 99);
        ui.verticalSlider_BlurX1->setSingleStep(2);
        if (not loading_preset) {
            ui.verticalSlider_BlurX1->setValue(3);
        }
    }
    else if (blur_filter_selected == ImageEditor::BlurFilter::PYR_DOWN_BLUR) {
        enableOptions(NONE);
    }
    else if (blur_filter_selected == ImageEditor::BlurFilter::PYR_UP_BLUR) {
        enableOptions(NONE);
    }
    else {
        enableOptions(NONE);
    }
}

void BatchItImage::EnableSpecificFormatOptions(bool loading_preset)
{
    qDebug() << "EnableSpecificFormatOptions:" <<  loading_preset;
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
        
        ui.label_FormatFlags->setText(format_jpeg_options[UI::FileOption::FormatJpeg::label_FormatFlags].name);
        ui.label_FormatFlags->setStatusTip(format_jpeg_options[UI::FileOption::FormatJpeg::label_FormatFlags].desc);
        ui.label_FormatFlags->setToolTip(format_jpeg_options[UI::FileOption::FormatJpeg::label_FormatFlags].desc);
        ui.label_Quality->setText(format_jpeg_options[UI::FileOption::FormatJpeg::label_Quality].name);
        ui.label_Quality->setStatusTip(format_jpeg_options[UI::FileOption::FormatJpeg::label_Quality].desc);
        ui.label_Quality->setToolTip(format_jpeg_options[UI::FileOption::FormatJpeg::label_Quality].desc);
        ui.checkBox_Optimize->setText(format_jpeg_options[UI::FileOption::FormatJpeg::checkBox_Optimize].name);
        ui.checkBox_Optimize->setStatusTip(format_jpeg_options[UI::FileOption::FormatJpeg::checkBox_Optimize].desc);
        ui.checkBox_Optimize->setToolTip(format_jpeg_options[UI::FileOption::FormatJpeg::checkBox_Optimize].desc);
        ui.checkBox_Progressive->setText(format_jpeg_options[UI::FileOption::FormatJpeg::checkBox_Progressive].name);
        ui.checkBox_Progressive->setStatusTip(format_jpeg_options[UI::FileOption::FormatJpeg::checkBox_Progressive].desc);
        ui.checkBox_Progressive->setToolTip(format_jpeg_options[UI::FileOption::FormatJpeg::checkBox_Progressive].desc);
        ui.label_Compression->setText(format_jpeg_options[UI::FileOption::FormatJpeg::label_Compression].name);
        ui.label_Compression->setStatusTip(format_jpeg_options[UI::FileOption::FormatJpeg::label_Compression].desc);
        ui.label_Compression->setToolTip(format_jpeg_options[UI::FileOption::FormatJpeg::label_Compression].desc);
        ui.label_ExtraSetting1->setText(format_jpeg_options[UI::FileOption::FormatJpeg::label_ExtraSetting1].name);
        ui.label_ExtraSetting1->setStatusTip(format_jpeg_options[UI::FileOption::FormatJpeg::label_ExtraSetting1].desc);
        ui.label_ExtraSetting1->setToolTip(format_jpeg_options[UI::FileOption::FormatJpeg::label_ExtraSetting1].desc);
        ui.label_ExtraSetting2->setText(format_jpeg_options[UI::FileOption::FormatJpeg::label_ExtraSetting2].name);
        ui.label_ExtraSetting2->setStatusTip(format_jpeg_options[UI::FileOption::FormatJpeg::label_ExtraSetting2].desc);
        ui.label_ExtraSetting2->setToolTip(format_jpeg_options[UI::FileOption::FormatJpeg::label_ExtraSetting2].desc);

        if (loading_preset)
            //PopulateComboBox(ui.comboBox_FormatFlags, format_jpeg_subsamplings, sizeof(format_jpeg_subsamplings) / sizeof(UIData));
            PopulateComboBox(ui.comboBox_FormatFlags, format_jpeg_subsamplings);
        
        int default_quality_value = format_jpeg_options[UI::FileOption::FormatJpeg::label_Quality].data;
        ui.horizontalSlider_Quality->setRange(0, 100);
        ui.horizontalSlider_Quality->addTextTip(default_quality_value, default_quality_value, " : Default", true);
        ui.spinBox_Compression->setRange(0, 65535);
        ui.spinBox_ExtraSetting1->setRange(-1, 100);
        ui.spinBox_ExtraSetting2->setRange(-1, 100);

        if (not loading_preset and last_selected_format != ".jpeg" and last_selected_format != ".jpg" and last_selected_format != ".jpe") {
            PopulateComboBox(ui.comboBox_FormatFlags, format_jpeg_subsamplings);
            ui.comboBox_FormatFlags->setCurrentIndex(format_jpeg_options[UI::FileOption::FormatJpeg::label_FormatFlags].data);
            ui.horizontalSlider_Quality->setValue(default_quality_value);
            ui.checkBox_Optimize->setChecked(format_jpeg_options[UI::FileOption::FormatJpeg::checkBox_Optimize].data);
            ui.checkBox_Progressive->setChecked(format_jpeg_options[UI::FileOption::FormatJpeg::checkBox_Progressive].data);
            ui.spinBox_Compression->setValue(format_jpeg_options[UI::FileOption::FormatJpeg::label_Compression].data);
            ui.spinBox_ExtraSetting1->setValue(format_jpeg_options[UI::FileOption::FormatJpeg::label_ExtraSetting1].data);
            ui.spinBox_ExtraSetting2->setValue(format_jpeg_options[UI::FileOption::FormatJpeg::label_ExtraSetting2].data);
        }
    }
    else if (format == ".jp2") {
        
        enableOptions(COMPRESSION);

        ui.label_Compression->setText(format_jp2_options[UI::FileOption::FormatJp2::label_Compression].name);
        ui.label_Compression->setStatusTip(format_jp2_options[UI::FileOption::FormatJp2::label_Compression].desc);
        ui.label_Compression->setToolTip(format_jp2_options[UI::FileOption::FormatJp2::label_Compression].desc);

        ui.spinBox_Compression->setRange(0, 1000);

        if (not loading_preset and last_selected_format != ".jp2") {
            ui.spinBox_Compression->setValue(format_jp2_options[UI::FileOption::FormatJp2::label_Compression].data);
        }
    }
    else if (format == ".png") {
        
        enableOptions(FORMATFLAGS + OPTIMIZE + COMPRESSION);

        ui.label_FormatFlags->setText(format_png_options[UI::FileOption::FormatPng::label_FormatFlags].name);
        ui.label_FormatFlags->setStatusTip(format_png_options[UI::FileOption::FormatPng::label_FormatFlags].desc);
        ui.label_FormatFlags->setToolTip(format_png_options[UI::FileOption::FormatPng::label_FormatFlags].desc);
        ui.checkBox_Optimize->setText(format_png_options[UI::FileOption::FormatPng::checkBox_Optimize].name);
        ui.checkBox_Optimize->setStatusTip(format_png_options[UI::FileOption::FormatPng::checkBox_Optimize].desc);
        ui.checkBox_Optimize->setToolTip(format_png_options[UI::FileOption::FormatPng::checkBox_Optimize].desc);
        ui.label_Compression->setText(format_png_options[UI::FileOption::FormatPng::label_Compression].name);
        ui.label_Compression->setStatusTip(format_png_options[UI::FileOption::FormatPng::label_Compression].desc);
        ui.label_Compression->setToolTip(format_png_options[UI::FileOption::FormatPng::label_Compression].desc);

        PopulateComboBox(ui.comboBox_FormatFlags, format_png_compression);
        ui.spinBox_Compression->setRange(0, 9);

        if (not loading_preset and last_selected_format != ".png") {
            ui.comboBox_FormatFlags->setCurrentIndex(format_png_options[UI::FileOption::FormatPng::label_FormatFlags].data);
            ui.checkBox_Optimize->setChecked(format_png_options[UI::FileOption::FormatPng::checkBox_Optimize].data);
            ui.spinBox_Compression->setValue(format_png_options[UI::FileOption::FormatPng::label_Compression].data);
        }
    }
    else if (format == ".webp") { 

        enableOptions(QUALITY);

        ui.label_Quality->setText(format_webp_options[UI::FileOption::FormatWebp::label_Quality].name);
        ui.label_Quality->setStatusTip(format_webp_options[UI::FileOption::FormatWebp::label_Quality].desc);
        ui.label_Quality->setToolTip(format_webp_options[UI::FileOption::FormatWebp::label_Quality].desc);

        int default_quality_value = format_webp_options[UI::FileOption::FormatWebp::label_Quality].data;
        ui.horizontalSlider_Quality->setRange(1, 100);
        ui.horizontalSlider_Quality->addTextTip(default_quality_value, default_quality_value, " : Default", true);

        if (not loading_preset and last_selected_format != ".webp") {
            ui.horizontalSlider_Quality->setValue(default_quality_value);
        }
    }
    else if (format == ".avif") { // TODO: test  IMWRITE_JPEG_SAMPLING_FACTOR

        enableOptions(QUALITY + COMPRESSION + EXTRASETTING2);

        //ui.label_FormatFlags->setText(":");
        ui.label_Quality->setText(format_avif_options[UI::FileOption::FormatAvif::label_Quality].name);
        ui.label_Quality->setStatusTip(format_avif_options[UI::FileOption::FormatAvif::label_Quality].desc);
        ui.label_Quality->setToolTip(format_avif_options[UI::FileOption::FormatAvif::label_Quality].desc);
        ui.label_Compression->setText(format_avif_options[UI::FileOption::FormatAvif::label_Compression].name);
        ui.label_Compression->setStatusTip(format_avif_options[UI::FileOption::FormatAvif::label_Compression].desc);
        ui.label_Compression->setToolTip(format_avif_options[UI::FileOption::FormatAvif::label_Compression].desc);
        ui.label_ExtraSetting2->setText(format_avif_options[UI::FileOption::FormatAvif::label_ExtraSetting2].name);
        ui.label_ExtraSetting2->setStatusTip(format_avif_options[UI::FileOption::FormatAvif::label_ExtraSetting2].desc);
        ui.label_ExtraSetting2->setToolTip(format_avif_options[UI::FileOption::FormatAvif::label_ExtraSetting2].desc);

        int default_quality_value = format_avif_options[UI::FileOption::FormatAvif::label_Quality].data;
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
            ui.spinBox_Compression->setValue(format_avif_options[UI::FileOption::FormatAvif::label_Compression].data);
            ui.spinBox_ExtraSetting2->setValue(format_avif_options[UI::FileOption::FormatAvif::label_ExtraSetting2].data);
        }
    }
    else if (format == ".pbm" or format == ".pgm" or format == ".ppm") {

        enableOptions(OPTIMIZE);

        ui.checkBox_Optimize->setText(format_pbm_options[UI::FileOption::FormatPbm::checkBox_Optimize].name);
        ui.checkBox_Optimize->setStatusTip(format_pbm_options[UI::FileOption::FormatPbm::checkBox_Optimize].desc);
        ui.checkBox_Optimize->setToolTip(format_pbm_options[UI::FileOption::FormatPbm::checkBox_Optimize].desc);

        if (not loading_preset and last_selected_format != ".pbm" and last_selected_format != ".pgm" and last_selected_format != ".ppm") {
            ui.checkBox_Optimize->setChecked(format_pbm_options[UI::FileOption::FormatPbm::checkBox_Optimize].data);
        }
    }
    else if (format == ".pam") {

        enableOptions(FORMATFLAGS);

        ui.label_FormatFlags->setText(format_pam_options[UI::FileOption::FormatPam::label_FormatFlags].name);
        ui.label_FormatFlags->setStatusTip(format_pam_options[UI::FileOption::FormatPam::label_FormatFlags].desc);
        ui.label_FormatFlags->setToolTip(format_pam_options[UI::FileOption::FormatPam::label_FormatFlags].desc);

        PopulateComboBox(ui.comboBox_FormatFlags, format_pam_tupletype);

        if (not loading_preset and last_selected_format != ".pam") {
            ui.comboBox_FormatFlags->setCurrentIndex(format_pam_options[UI::FileOption::FormatPam::label_FormatFlags].data);
        }
    }
    else if (format == ".tiff" or format == ".tif") {

        enableOptions(FORMATFLAGS + QUALITY + EXTRASETTING1 + EXTRASETTING2);

        ui.label_FormatFlags->setText(format_tiff_options[UI::FileOption::FormatTiff::label_FormatFlags].name);
        ui.label_FormatFlags->setStatusTip(format_tiff_options[UI::FileOption::FormatTiff::label_FormatFlags].desc);
        ui.label_FormatFlags->setToolTip(format_tiff_options[UI::FileOption::FormatTiff::label_FormatFlags].desc);
        ui.label_Quality->setText(format_tiff_options[UI::FileOption::FormatTiff::label_Quality].name);
        ui.label_Quality->setStatusTip(format_tiff_options[UI::FileOption::FormatTiff::label_Quality].desc);
        ui.label_Quality->setToolTip(format_tiff_options[UI::FileOption::FormatTiff::label_Quality].desc);
        ui.label_ExtraSetting1->setText(format_tiff_options[UI::FileOption::FormatTiff::label_ExtraSetting1].name);
        ui.label_ExtraSetting1->setStatusTip(format_tiff_options[UI::FileOption::FormatTiff::label_ExtraSetting1].desc);
        ui.label_ExtraSetting1->setToolTip(format_tiff_options[UI::FileOption::FormatTiff::label_ExtraSetting1].desc);
        ui.label_ExtraSetting2->setText(format_tiff_options[UI::FileOption::FormatTiff::label_ExtraSetting2].name);
        ui.label_ExtraSetting2->setStatusTip(format_tiff_options[UI::FileOption::FormatTiff::label_ExtraSetting2].desc);
        ui.label_ExtraSetting2->setToolTip(format_tiff_options[UI::FileOption::FormatTiff::label_ExtraSetting2].desc);

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
            ui.comboBox_FormatFlags->setCurrentIndex(format_tiff_options[UI::FileOption::FormatTiff::label_FormatFlags].data);
            ui.horizontalSlider_Quality->setValue(format_tiff_options[UI::FileOption::FormatTiff::label_Quality].data);
            ui.spinBox_ExtraSetting1->setValue(format_tiff_options[UI::FileOption::FormatTiff::label_ExtraSetting1].data);
            ui.spinBox_ExtraSetting2->setValue(format_tiff_options[UI::FileOption::FormatTiff::label_ExtraSetting2].data);
        }
    }
    else if (format == ".exr") {

        enableOptions(FORMATFLAGS + OPTIMIZE + PROGRESSIVE + COMPRESSION);

        ui.label_FormatFlags->setText(format_exr_options[UI::FileOption::FormatExr::label_FormatFlags].name);
        ui.label_FormatFlags->setStatusTip(format_exr_options[UI::FileOption::FormatExr::label_FormatFlags].desc);
        ui.label_FormatFlags->setToolTip(format_exr_options[UI::FileOption::FormatExr::label_FormatFlags].desc);
        ui.checkBox_Optimize->setText(format_exr_options[UI::FileOption::FormatExr::checkBox_Optimize].name);
        ui.checkBox_Optimize->setStatusTip(format_exr_options[UI::FileOption::FormatExr::checkBox_Optimize].desc);
        ui.checkBox_Optimize->setToolTip(format_exr_options[UI::FileOption::FormatExr::checkBox_Optimize].desc);
        ui.checkBox_Progressive->setText(format_exr_options[UI::FileOption::FormatExr::checkBox_Progressive].name);
        ui.checkBox_Progressive->setStatusTip(format_exr_options[UI::FileOption::FormatExr::checkBox_Progressive].desc);
        ui.checkBox_Progressive->setToolTip(format_exr_options[UI::FileOption::FormatExr::checkBox_Progressive].desc);
        ui.label_Compression->setText(format_exr_options[UI::FileOption::FormatExr::label_Compression].name);
        ui.label_Compression->setStatusTip(format_exr_options[UI::FileOption::FormatExr::label_Compression].desc);
        ui.label_Compression->setToolTip(format_exr_options[UI::FileOption::FormatExr::label_Compression].desc);

        PopulateComboBox(ui.comboBox_FormatFlags, format_exr_compression);
        ui.checkBox_Optimize->setAutoExclusive(true);
        ui.checkBox_Progressive->setAutoExclusive(true);

        if (not loading_preset and last_selected_format != ".exr") {
            ui.comboBox_FormatFlags->setCurrentIndex(format_exr_options[UI::FileOption::FormatExr::label_FormatFlags].data);
            ui.checkBox_Optimize->setChecked(format_exr_options[UI::FileOption::FormatExr::checkBox_Optimize].data);
            ui.checkBox_Progressive->setChecked(format_exr_options[UI::FileOption::FormatExr::checkBox_Progressive].data);
            ui.spinBox_Compression->setValue(format_exr_options[UI::FileOption::FormatExr::label_Compression].data);
        }

    }
    else if (format == ".hdr" or format == ".pic") { 

        enableOptions(FORMATFLAGS);

        ui.label_FormatFlags->setText(format_hdr_options[UI::FileOption::FormatHdr::label_FormatFlags].name);
        ui.label_FormatFlags->setStatusTip(format_hdr_options[UI::FileOption::FormatHdr::label_FormatFlags].desc);
        ui.label_FormatFlags->setToolTip(format_hdr_options[UI::FileOption::FormatHdr::label_FormatFlags].desc);

        if (loading_preset)
            PopulateComboBox(ui.comboBox_FormatFlags, format_hdr_compression);

        if (not loading_preset and last_selected_format != ".hdr" and last_selected_format != ".pic") {
            PopulateComboBox(ui.comboBox_FormatFlags, format_hdr_compression);
            ui.comboBox_FormatFlags->setCurrentIndex(format_hdr_options[UI::FileOption::FormatHdr::label_FormatFlags].data);
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
    qDebug() << "SetupFileTreeContextMenu";

    action_add = new QAction(file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_add).name, this);
    action_delete = new QAction(file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_delete).name, this);
    action_clear = new QAction(file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_clear).name, this);
    action_select = new QAction(file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_select).name, this);
    action_select_all = new QAction(file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_select_all).name, this);
    action_select_none = new QAction(file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_select_none).name, this);
    action_submenu_filter = new QAction(file_tree_sub_menu_formats->at(
        FileTree::ActionMenu::SubMenu::FilterImageFormats::action_submenu_filter).name, this);
    action_view = new QAction(file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_view).name, this);
    action_preview = new QAction(file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_preview).name, this);

    auto action_line_1 = new QAction(this);
    action_line_1->setSeparator(true);
    auto action_line_2 = new QAction(this);
    action_line_2->setSeparator(true);
    auto action_line_3 = new QAction(this);
    action_line_3->setSeparator(true);
    
    // Sub-Menu: Filters
    submenu_format_filter = new QMenu();
    auto action_submenu_undock = new QAction(file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_submenu_undock).name, this);
    auto action_filter_jpeg = new QAction(file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_jpeg).name, this);
    auto action_filter_jp2 = new QAction(file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_jp2).name, this);
    auto action_filter_png = new QAction(file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_png).name, this);
    auto action_filter_webp = new QAction(file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_webp).name, this);
    auto action_filter_bmp = new QAction(file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_bmp).name, this);
    auto action_filter_avif = new QAction(file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_avif).name, this);
    auto action_filter_pbm = new QAction(file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_pbm).name, this);
    auto action_filter_sr = new QAction(file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_sr).name, this);
    auto action_filter_tiff = new QAction(file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_tiff).name, this);
    auto action_filter_exr = new QAction(file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_exr).name, this);
    auto action_filter_hdr = new QAction(file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_hdr).name, this);
    action_filter_jpeg->setCheckable(true);
    action_filter_jp2->setCheckable(true);
    action_filter_png->setCheckable(true);
    action_filter_webp->setCheckable(true);
    action_filter_bmp->setCheckable(true);
    action_filter_avif->setCheckable(true);
    action_filter_pbm->setCheckable(true);
    action_filter_sr->setCheckable(true);
    action_filter_tiff->setCheckable(true);
    action_filter_exr->setCheckable(true);
    action_filter_hdr->setCheckable(true);

    // TODO: No tristate? Remove check if any filtered item unchecked? FileSelectionChange()

    action_add->setStatusTip(file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_add).desc);
    action_add->setToolTip(file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_add).desc);
    action_delete->setStatusTip(file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_delete).desc);
    action_delete->setToolTip(file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_delete).desc);
    action_clear->setStatusTip(file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_clear).desc);
    action_clear->setToolTip(file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_clear).desc);
    action_select->setStatusTip(file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_select).desc);
    action_select->setToolTip(file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_select).desc);
    action_select_all->setStatusTip(file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_select_all).desc);
    action_select_all->setToolTip(file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_select_all).desc);
    action_select_none->setStatusTip(file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_select_none).desc);
    action_select_none->setToolTip(file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_select_none).desc);
    action_submenu_filter->setStatusTip(file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_submenu_filter).desc);
    action_submenu_filter->setToolTip(file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_submenu_filter).desc);
    action_submenu_undock->setStatusTip(file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_submenu_undock).desc);
    action_submenu_undock->setToolTip(file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_submenu_undock).desc);
    action_filter_jpeg->setStatusTip(file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_jpeg).desc);
    action_filter_jpeg->setToolTip(file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_jpeg).desc);
    action_filter_jp2->setStatusTip(file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_jp2).desc);
    action_filter_jp2->setToolTip(file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_jp2).desc);
    action_filter_png->setStatusTip(file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_png).desc);
    action_filter_png->setToolTip(file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_png).desc);
    action_filter_webp->setStatusTip(file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_webp).desc);
    action_filter_webp->setToolTip(file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_webp).desc);
    action_filter_bmp->setStatusTip(file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_bmp).desc);
    action_filter_bmp->setToolTip(file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_bmp).desc);
    action_filter_avif->setStatusTip(file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_avif).desc);
    action_filter_avif->setToolTip(file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_avif).desc);
    action_filter_pbm->setStatusTip(file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_pbm).desc);
    action_filter_pbm->setToolTip(file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_pbm).desc);
    action_filter_sr->setStatusTip(file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_sr).desc);
    action_filter_sr->setToolTip(file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_sr).desc);
    action_filter_tiff->setStatusTip(file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_tiff).desc);
    action_filter_tiff->setToolTip(file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_tiff).desc);
    action_filter_exr->setStatusTip(file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_exr).desc);
    action_filter_exr->setToolTip(file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_exr).desc);
    action_filter_hdr->setStatusTip(file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_hdr).desc);
    action_filter_hdr->setToolTip(file_tree_sub_menu_formats->at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_hdr).desc);
    action_view->setStatusTip(file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_view).desc);
    action_view->setToolTip(file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_view).desc);
    action_preview->setStatusTip(file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_preview).desc);
    action_preview->setToolTip(file_tree_menu_items->at(FileTree::ActionMenu::MainMenu::action_preview).desc);

    // Main Menu Actions
    Q_ASSERT(connect(action_add, SIGNAL(triggered()), this, SLOT(LoadImageFiles())));
    Q_ASSERT(connect(action_delete, SIGNAL(triggered()), this, SLOT(DeleteConfirmationPopup())));
    Q_ASSERT(connect(action_clear, &QAction::triggered, [this] { DeleteConfirmationPopup(true); }));
    Q_ASSERT(connect(action_select, &QAction::triggered, this, [this] { FileSelectionToggle(GetCurrentFileTreeRow()); }));
    Q_ASSERT(connect(action_select_all, &QAction::triggered, this, [this] { FileSelectionToggleAll(true); }));
    Q_ASSERT(connect(action_select_none, &QAction::triggered, this, [this] { FileSelectionToggleAll(false); }));
    Q_ASSERT(connect(action_view, SIGNAL(triggered()), this, SLOT(Test()))); // TODO
    Q_ASSERT(connect(action_preview, SIGNAL(triggered()), this, SLOT(Test()))); // TODO
    
    // Format Filter Submenu Actions
    Q_ASSERT(connect(action_filter_jpeg, &QAction::changed, this, [=] { FileSelectionFilter(submenu_format_filter->actions()); }));
    Q_ASSERT(connect(action_filter_jp2, &QAction::changed, this, [=] { FileSelectionFilter(submenu_format_filter->actions()); }));
    Q_ASSERT(connect(action_filter_png, &QAction::changed, this, [=] { FileSelectionFilter(submenu_format_filter->actions()); }));
    Q_ASSERT(connect(action_filter_webp, &QAction::changed, this, [=] { FileSelectionFilter(submenu_format_filter->actions()); }));
    Q_ASSERT(connect(action_filter_bmp, &QAction::changed, this, [=] { FileSelectionFilter(submenu_format_filter->actions()); }));
    Q_ASSERT(connect(action_filter_avif, &QAction::changed, this, [=] { FileSelectionFilter(submenu_format_filter->actions()); }));
    Q_ASSERT(connect(action_filter_pbm, &QAction::changed, this, [=] { FileSelectionFilter(submenu_format_filter->actions()); }));
    Q_ASSERT(connect(action_filter_sr, &QAction::changed, this, [=] { FileSelectionFilter(submenu_format_filter->actions()); }));
    Q_ASSERT(connect(action_filter_tiff, &QAction::changed, this, [=] { FileSelectionFilter(submenu_format_filter->actions()); }));
    Q_ASSERT(connect(action_filter_exr, &QAction::changed, this, [=] { FileSelectionFilter(submenu_format_filter->actions()); }));
    Q_ASSERT(connect(action_filter_hdr, &QAction::changed, this, [=] { FileSelectionFilter(submenu_format_filter->actions()); }));

    // Undockable Format Filter Menu
    Q_ASSERT(connect(action_submenu_undock, &QAction::triggered, this,
        [=] {
            submenu_format_filter->showTearOffMenu();
            action_submenu_undock->setVisible(false);
        }));
    Q_ASSERT(connect(submenu_format_filter, &QMenu::aboutToShow, this,
        [=] {
            if (submenu_format_filter->isTearOffMenuVisible())
                action_submenu_undock->setVisible(false);
            else
                action_submenu_undock->setVisible(true);
        }));

    // Create the Context Menu
    ui.treeWidget_FileInfo->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui.treeWidget_FileInfo->addActions({ 
        action_add, action_delete, action_clear, action_line_1,
        action_select, action_select_all, action_select_none, action_submenu_filter, action_line_2,
        action_view, action_preview, 
        });
    submenu_format_filter->addActions({
        action_submenu_undock, action_line_3, action_filter_jpeg, action_filter_jp2, action_filter_png, action_filter_webp,
        action_filter_bmp, action_filter_avif, action_filter_pbm, action_filter_sr, action_filter_tiff, action_filter_exr, action_filter_hdr
        });
    action_submenu_filter->setMenu(submenu_format_filter);

    // Disabled most actions until files are loaded into tree.
    ToggleFileTreeContextMenuItems(false);

#ifdef _DEBUG
    auto action_line_D = new QAction(this);
    action_line_D->setSeparator(true);

    QAction* action_debug_quick_load = new QAction("Debug Quick Load", this);
    QStringList testing_file_list;
    //testing_file_list.append(qdefault_path + R"(/test_images/01.jpg)"); // large file
    testing_file_list.append(qdefault_path + R"(/test_images/79.jpg)");
    testing_file_list.append(qdefault_path + R"(/test_images/evil_monkey.png)");
    testing_file_list.append(qdefault_path + R"(/test_images/AC01.png)");
    testing_file_list.append(qdefault_path + R"(/test_images/AC02.png)");
    connect(action_debug_quick_load, &QAction::triggered, [this, testing_file_list] { AddNewFiles(testing_file_list); });
    
    QAction* action_debug_large_load = new QAction("Debug Large Load", this);
    QStringList testing_folder_list;
    testing_folder_list.append(qdefault_path + R"(/test_images)");
    connect(action_debug_large_load, &QAction::triggered, [this, testing_folder_list] { AddNewFiles(testing_folder_list); });
    
    ui.treeWidget_FileInfo->addActions({ action_line_D, action_debug_quick_load, action_debug_large_load });
#endif // _DEBUG
}

void BatchItImage::ToggleFileTreeContextMenuItems(bool enable)
{
    action_delete->setEnabled(enable);
    action_clear->setEnabled(enable);
    action_select->setEnabled(enable);
    action_select_all->setEnabled(enable);
    action_select_none->setEnabled(enable);
    action_submenu_filter->setEnabled(enable);
    action_view->setEnabled(enable);
    action_preview->setEnabled(enable);
    if (enable) {
        action_delete->setFont(*font_default);
        action_clear->setFont(*font_default);
        action_select->setFont(*font_default);
        action_select_all->setFont(*font_default);
        action_select_none->setFont(*font_default);
        action_submenu_filter->setFont(*font_default);
        action_view->setFont(*font_default);
        action_preview->setFont(*font_default);
    }
    else {
        action_delete->setFont(*font_default_light);
        action_clear->setFont(*font_default_light);
        action_select->setFont(*font_default_light);
        action_select_all->setFont(*font_default_light);
        action_select_none->setFont(*font_default_light);
        action_submenu_filter->setFont(*font_default_light);
        action_view->setFont(*font_default_light);
        action_preview->setFont(*font_default_light);
    }
}

void BatchItImage::SetColorPreviewStyleSheet()
{
    // Show lighter or darker text based on background color in label_ColorPreview.
    QString text_color;
    if (background_color.alpha() < 64) {
        text_color = "color: rgba(0, 0, 0, 64)}";
    }
    else if (background_color.lightness() > 96 or background_color.alpha() < 128) {
        text_color = "color: rgba(0, 0, 0, 255)}";
    }
    else {
        text_color = "color: rgba(255, 255, 255, 255)}";
    }
    QString style_sheet = "QLabel {background-color: rgba(" + \
        QVariant(background_color.red()).toString() + ", " + \
        QVariant(background_color.green()).toString() + ", " + \
        QVariant(background_color.blue()).toString() + ", " + \
        QVariant(background_color.alpha()).toString() + \
        "); " + text_color;
    //qDebug() << style_sheet;
    qDebug() << background_color.lightness() << "-" << background_color.hue();
    ui.label_ColorPreview->setStyleSheet(style_sheet);
}

void BatchItImage::ChangePreset(int index)
{
    qDebug() << "ChangePreset:" << index;
    SavePresetDialog();
    
    ui.comboBox_Preset_1->blockSignals(true);
    ui.comboBox_Preset_2->blockSignals(true);
    ui.comboBox_Preset_3->blockSignals(true);

    current_selected_preset = index;
    ui.comboBox_Preset_1->setCurrentIndex(current_selected_preset);
    ui.comboBox_Preset_2->setCurrentIndex(current_selected_preset);
    ui.comboBox_Preset_3->setCurrentIndex(current_selected_preset);
    LoadPreset(preset_list.at(current_selected_preset));

    ui.comboBox_Preset_1->blockSignals(false);
    ui.comboBox_Preset_2->blockSignals(false);
    ui.comboBox_Preset_3->blockSignals(false);

    RemoveOptionsChanged();
}

void BatchItImage::SavePreset(bool save_all)
{
    if (save_all) qDebug() << "SavePreset:" << current_selected_preset;
    else qDebug() << "SavePreset: All";

    int save_option;
    if (ui.radioButton_Overwrite->isChecked()) {
        save_option = ImageSaver::SaveOptionFlag::OVERWRITE;
    }
    else if (ui.radioButton_RenameOriginal->isChecked()) {
        save_option = ImageSaver::SaveOptionFlag::RENAME_ORIGINAL;
    }
    else {
        save_option = ImageSaver::SaveOptionFlag::NEW_NAME;
    }
    bool save_path_relative = ui.radioButton_RelativePath->isChecked();

    // Update current preset in list then save it in settings
    preset_list.at(current_selected_preset).setPresetIndex(current_selected_preset);
    //preset_list.at(current_selected_preset).setPresetDescription(ui.comboBox_Preset_1->itemText(current_selected_preset)); // Updated in ChangePresetDescription()
    preset_list.at(current_selected_preset).setWidthModifierIndex(ui.comboBox_WidthMod->currentIndex());
    preset_list.at(current_selected_preset).setWidthNumber(ui.spinBox_WidthNumber->value());
    preset_list.at(current_selected_preset).setHeightModifierIndex(ui.comboBox_HeightMod->currentIndex());
    preset_list.at(current_selected_preset).setHeightNumber(ui.spinBox_HeightNumber->value());
    preset_list.at(current_selected_preset).setKeepAspectRatio(ui.checkBox_KeepAspectRatio->isChecked());
    preset_list.at(current_selected_preset).setResamplingFilterIndex(ui.comboBox_Resample->currentIndex());
    preset_list.at(current_selected_preset).setBorderTypeIndex(ui.comboBox_BorderType->currentIndex());
    preset_list.at(current_selected_preset).setBackgroundColor(background_color);
    preset_list.at(current_selected_preset).setBlurFilterIndex(ui.comboBox_BlurFilter->currentIndex());
    preset_list.at(current_selected_preset).setBlurNormalize(ui.checkBox_BlurNormalize->isChecked());
    preset_list.at(current_selected_preset).setBlurX(ui.verticalSlider_BlurX1->value());
    preset_list.at(current_selected_preset).setBlurY(ui.verticalSlider_BlurY1->value());
    preset_list.at(current_selected_preset).setBlurSX(ui.verticalSlider_BlurX2->value());
    preset_list.at(current_selected_preset).setBlurSY(ui.verticalSlider_BlurY2->value());
    preset_list.at(current_selected_preset).setBlurDepth(ui.verticalSlider_BlurD->value());
    preset_list.at(current_selected_preset).setRotationDegrees(ui.dial_Rotation->value());
    preset_list.at(current_selected_preset).setIncreaseBoundaries(ui.checkBox_IncreaseBounds->isChecked());
    preset_list.at(current_selected_preset).setFlipImage(ui.checkBox_FlipImage->isChecked());
    preset_list.at(current_selected_preset).setWatermarkAdded(ui.groupBox_Watermark->isChecked());
    preset_list.at(current_selected_preset).setWatermarkPath(ui.lineEdit_WatermarkPath->text());
    preset_list.at(current_selected_preset).setWatermarkLocationIndex(ui.comboBox_WatermarkLocation->currentIndex());
    preset_list.at(current_selected_preset).setWatermarkTransparency(ui.spinBox_WatermarkTransparency->value());
    preset_list.at(current_selected_preset).setWatermarkOffsetX(ui.spinBox_WatermarkOffsetX->value());
    preset_list.at(current_selected_preset).setWatermarkOffsetY(ui.spinBox_WatermarkOffsetY->value());
    preset_list.at(current_selected_preset).setFormatChanged(ui.groupBox_ChangeFormat->isChecked());
    preset_list.at(current_selected_preset).setFormatExtensionIndex(ui.comboBox_ImageFormat->currentIndex());
    preset_list.at(current_selected_preset).setFormatFormatFlagIndex(ui.comboBox_FormatFlags->currentIndex());
    preset_list.at(current_selected_preset).setFormatQuality(ui.horizontalSlider_Quality->value());
    preset_list.at(current_selected_preset).setFormatOptimize(ui.checkBox_Optimize->isChecked());
    preset_list.at(current_selected_preset).setFormatProgressive(ui.checkBox_Progressive->isChecked());
    preset_list.at(current_selected_preset).setFormatCompression(ui.spinBox_Compression->value());
    preset_list.at(current_selected_preset).setFormatExtra1(ui.spinBox_ExtraSetting1->value());
    preset_list.at(current_selected_preset).setFormatExtra2(ui.spinBox_ExtraSetting2->value());
    preset_list.at(current_selected_preset).setSaveFileProcedureIndex(save_option);
    preset_list.at(current_selected_preset).setSaveFileNameChange(ui.lineEdit_FileName->text().toStdString());
    preset_list.at(current_selected_preset).setSavePathRelative(save_path_relative);
    if (save_path_relative)
        preset_list.at(current_selected_preset).setSaveFilePathChange(ui.lineEdit_RelativePath->text());
    else
        preset_list.at(current_selected_preset).setSaveFilePathChange(ui.lineEdit_AbsolutePath->text());

    QSettings settings(preset_settings_file, QSettings::IniFormat);
    settings.beginGroup("Settings");
    settings.setValue("current_selected_preset", ui.comboBox_Preset_1->currentIndex());
    settings.endGroup();

    if (save_all) {
        qDebug() << "Saving All Presets to Settings File";
        for (int i = 0; i < preset_list.size(); i++) {
            SavePresetToSettingsFile(i);
        }
        ui.statusbar->showMessage(
            status_bar_messages.at(UI::StatusBar::SavePreset).name,
            status_bar_messages.at(UI::StatusBar::SavePreset).data
        );
    }
    else {
        qDebug() << "Saving Preset #" << current_selected_preset << "to Settings File";
        SavePresetToSettingsFile(current_selected_preset);
    }
    RemoveOptionsChanged();
}

void BatchItImage::SavePresetToSettingsFile(int index)
{
    QSettings settings(preset_settings_file, QSettings::IniFormat);

    settings.beginGroup("Preset" + std::to_string(index));
    settings.setValue("presetDescription", preset_list.at(index).presetDescription());
    settings.setValue("widthModifierIndex", preset_list.at(index).widthModifierIndex());
    settings.setValue("widthNumber", preset_list.at(index).widthNumber());
    settings.setValue("heightModifierIndex", preset_list.at(index).heightModifierIndex());
    settings.setValue("heightModifierIndex", preset_list.at(index).heightModifierIndex());
    settings.setValue("keepAspectRatio", preset_list.at(index).keepAspectRatio());
    settings.setValue("resamplingFilterIndex", preset_list.at(index).resamplingFilterIndex());
    settings.setValue("borderTypeIndex", preset_list.at(index).borderTypeIndex());
    settings.setValue("background_color_blue", preset_list.at(index).backgroundColor().blue());
    settings.setValue("background_color_green", preset_list.at(index).backgroundColor().green());
    settings.setValue("background_color_red", preset_list.at(index).backgroundColor().red());
    settings.setValue("background_color_alpha", preset_list.at(index).backgroundColor().alpha());
    settings.setValue("blurFilterIndex", preset_list.at(index).blurFilterIndex());
    settings.setValue("blurNormalize", preset_list.at(index).blurNormalize());
    settings.setValue("blurX", preset_list.at(index).blurX());
    settings.setValue("blurY", preset_list.at(index).blurY());
    settings.setValue("blurSX", preset_list.at(index).blurSX());
    settings.setValue("blurSY", preset_list.at(index).blurSY());
    settings.setValue("blurDepth", preset_list.at(index).blurDepth());
    settings.setValue("rotationDegrees", preset_list.at(index).rotationDegrees());
    settings.setValue("increaseBoundaries", preset_list.at(index).increaseBoundaries());
    settings.setValue("flipImage", preset_list.at(index).flipImage());
    settings.setValue("watermarkAdded", preset_list.at(index).watermarkAdded());
    settings.setValue("watermarkPath", preset_list.at(index).watermarkPath());
    settings.setValue("watermarkLocationIndex", preset_list.at(index).watermarkLocationIndex());
    settings.setValue("watermarkTransparency", preset_list.at(index).watermarkTransparency());
    settings.setValue("watermarkOffsetX", preset_list.at(index).watermarkOffsetX());
    settings.setValue("watermarkOffsetY", preset_list.at(index).watermarkOffsetY());
    settings.setValue("formatChanged", preset_list.at(index).formatChanged());
    settings.setValue("formatExtension", preset_list.at(index).formatExtensionIndex());
    settings.setValue("formatFormatFlagIndex", preset_list.at(index).formatFormatFlagIndex());
    settings.setValue("formatQuality", preset_list.at(index).formatQuality());
    settings.setValue("formatOptimize", preset_list.at(index).formatOptimize());
    settings.setValue("formatProgressive", preset_list.at(index).formatProgressive());
    settings.setValue("formatCompression", preset_list.at(index).formatCompression());
    settings.setValue("formatExtra1", preset_list.at(index).formatExtra1());
    settings.setValue("formatExtra2", preset_list.at(index).formatExtra2());
    settings.setValue("saveFileProcedureIndex", preset_list.at(index).saveFileProcedureIndex());
    settings.setValue("saveFileNameChange", QString::fromStdString(preset_list.at(index).saveFileNameChange()));
    settings.setValue("savePathRelative", preset_list.at(index).savePathRelative());
    settings.setValue("saveFilePathChange", preset_list.at(index).saveFilePathChange());
    settings.endGroup();
}

void BatchItImage::SaveRecentFiles()
{
    QSettings settings(preset_settings_file, QSettings::IniFormat);

    settings.beginGroup("Recent");
    for (qsizetype i = 0; i < recent_file_paths_loaded.size(); i++) {
        QString num = QVariant::fromValue(i).toString();
        qDebug() << num + ".)" << recent_file_paths_loaded.at(i);
        settings.setValue(num, recent_file_paths_loaded.at(i));
    }
    settings.endGroup();
    BuildRecentFilesMenu();
}

void BatchItImage::LoadRecentFiles()
{
    qDebug() << "LoadRecentFiles";
    recent_file_paths_loaded.clear();
    QSettings settings(preset_settings_file, QSettings::IniFormat);

    if (settings.childGroups().indexOf("Recent") > -1) {
        settings.beginGroup("Recent");
        qDebug() << "[Recent]";
        for (qsizetype i = 0; i < recent_file_paths_loaded_max; i++) {
            QString num = QVariant::fromValue(i).toString();
            QString recent_file = settings.value(num, "").toString();
            if (recent_file.length()) {
                recent_file_paths_loaded.append(settings.value(num, "").toString());
                qDebug() << num + ".)" << recent_file_paths_loaded.at(i).toUtf8();
            }
            else
                break;
        }
        settings.endGroup();
    }
    BuildRecentFilesMenu();
}

void BatchItImage::BuildRecentFilesMenu()
{
    ui.menu_RecentImageFiles->clear(); // Actions owned by the menu and not shown in any other widget are "deleted".
    ui.menu_RecentImageFiles->addActions({ action_load_all_files, action_line_recent_top });
    for (auto& recent_file_path : recent_file_paths_loaded) {
        auto image_file_link = new QAction(recent_file_path, this);
        Q_ASSERT(connect(image_file_link, &QAction::triggered, this, [=] { AddNewFile(recent_file_path); }));
        ui.menu_RecentImageFiles->addAction(image_file_link);
    }
    ui.menu_RecentImageFiles->addActions({ action_line_recent_bottom, action_clear_all_files });
}


void BatchItImage::LoadPreset(Preset preset)
{
    ui.comboBox_WidthMod->setCurrentIndex(preset.widthModifierIndex());
    ui.spinBox_WidthNumber->setValue(preset.widthNumber());
    ui.comboBox_HeightMod->setCurrentIndex(preset.heightModifierIndex());
    ui.spinBox_HeightNumber->setValue(preset.heightNumber());
    ui.comboBox_Resample->setCurrentIndex(preset.resamplingFilterIndex());
    ui.checkBox_KeepAspectRatio->setChecked(preset.keepAspectRatio());
    ui.comboBox_BorderType->setCurrentIndex(preset.borderTypeIndex());
    background_color = preset.backgroundColor();
    ui.comboBox_BlurFilter->setCurrentIndex(preset.blurFilterIndex());
    ui.checkBox_BlurNormalize->setChecked(preset.blurNormalize());
    ui.verticalSlider_BlurX1->setValue(preset.blurX());
    ui.verticalSlider_BlurY1->setValue(preset.blurY());
    ui.verticalSlider_BlurX2->setValue(preset.blurSX());
    ui.verticalSlider_BlurY2->setValue(preset.blurSY());
    ui.verticalSlider_BlurD->setValue(preset.blurDepth());
    ui.dial_Rotation->setValue(preset.rotationDegrees());
    ui.lcdNumber_Rotation->display(preset.rotationDegrees());
    ui.checkBox_IncreaseBounds->setChecked(preset.increaseBoundaries());
    ui.checkBox_FlipImage->setChecked(preset.flipImage());
    ui.groupBox_Watermark->setChecked(preset.watermarkAdded());
    ui.lineEdit_WatermarkPath->setText(preset.watermarkPath());
    ui.comboBox_WatermarkLocation->setCurrentIndex(preset.watermarkLocationIndex());
    ui.spinBox_WatermarkTransparency->setValue(preset.watermarkTransparency());
    ui.spinBox_WatermarkOffsetX->setValue(preset.watermarkOffsetX());
    ui.spinBox_WatermarkOffsetY->setValue(preset.watermarkOffsetY());
    ui.groupBox_ChangeFormat->setChecked(preset.formatChanged());
    ui.comboBox_ImageFormat->setCurrentIndex(preset.formatExtensionIndex());
    ui.comboBox_FormatFlags->setCurrentIndex(preset.formatFormatFlagIndex());
    ui.horizontalSlider_Quality->setValue(preset.formatQuality());
    ui.checkBox_Optimize->setChecked(preset.formatOptimize());
    ui.checkBox_Progressive->setChecked(preset.formatProgressive());
    ui.spinBox_Compression->setValue(preset.formatCompression());
    ui.spinBox_ExtraSetting1->setValue(preset.formatExtra1());
    ui.spinBox_ExtraSetting2->setValue(preset.formatExtra2());
    ui.lineEdit_FileName->setText(QString::fromStdString(preset.saveFileNameChange()));
    int save_option = preset.saveFileProcedureIndex();
    if (save_option == ImageSaver::SaveOptionFlag::OVERWRITE) {
        ui.radioButton_Overwrite->setChecked(true);
    }
    else if (save_option == ImageSaver::SaveOptionFlag::RENAME_ORIGINAL) {
        ui.radioButton_RenameOriginal->setChecked(true);
    }
    else { // save_option == NEW_NAME
        ui.radioButton_NewFileName->setChecked(true);
    }
    bool save_path_relative = preset.savePathRelative();
    ui.radioButton_RelativePath->setChecked(save_path_relative);
    ui.radioButton_AbsolutePath->setChecked(not save_path_relative);
    if (save_path_relative)
        ui.lineEdit_RelativePath->setText(preset.saveFilePathChange());
    else
        ui.lineEdit_AbsolutePath->setText(preset.saveFilePathChange());

    SetColorPreviewStyleSheet();
    UpdateLineEditTextTips(ui.lineEdit_FileName);
    UpdateLineEditTextTips(ui.lineEdit_RelativePath);
    UpdateLineEditTextTips(ui.lineEdit_AbsolutePath);
    EnableOptionGroup(ui.groupBox_Watermark->children(), preset.watermarkAdded());
    EnableSpecificBlurOptions(true);
    EnableSpecificFormatOptions(true);
}

void BatchItImage::LoadPresets()
{
    QSettings settings(preset_settings_file, QSettings::IniFormat);
    preset_list.clear();
    int cspi = 0;

    // Default Presets (will be loaded when none are found in the settings file.)
    Preset preset1;
    preset1.setPresetIndex(0);
    preset1.setPresetDescription("(Default) Create New 600x600 Image.");
    preset1.setWidthModifierIndex(ImageEditor::SizeModifier::CHANGE_TO);
    preset1.setWidthNumber(600);
    preset1.setHeightModifierIndex(ImageEditor::SizeModifier::CHANGE_TO);
    preset1.setHeightNumber(600);
    preset1.setResamplingFilterIndex(cv::InterpolationFlags::INTER_CUBIC);

    Preset preset2;
    preset2.setPresetIndex(1);
    preset2.setPresetDescription("(Default) Resize Image 200x200 and Rename Original.");
    preset2.setWidthModifierIndex(ImageEditor::SizeModifier::CHANGE_TO);
    preset2.setWidthNumber(200);
    preset2.setHeightModifierIndex(ImageEditor::SizeModifier::CHANGE_TO);
    preset2.setHeightNumber(200);
    preset2.setKeepAspectRatio(false);
    preset2.setResamplingFilterIndex(cv::InterpolationFlags::INTER_CUBIC);
    preset2.setSaveFileProcedureIndex(ImageSaver::SaveOptionFlag::RENAME_ORIGINAL);
    preset2.setSaveFileNameChange(ImageSaver::MetadataIdentifiers.at(ImageSaver::MetadataFlags::FILE_NAME) + "__org");

    if (settings.childGroups().indexOf("Preset0") > -1) {
        int i = 0;
        do {
            qDebug() << "Found Preset#" + std::to_string(i) + " in Settings";
            settings.beginGroup("Preset" + std::to_string(i));
            Preset preset;
            preset.setPresetIndex(i);
            preset.setPresetDescription(settings.value("description").toString());
            preset.setWidthModifierIndex(settings.value("width_modifier").toInt());
            preset.setWidthNumber(settings.value("width_number").toInt());
            preset.setHeightModifierIndex(settings.value("height_modifier").toInt());
            preset.setHeightNumber(settings.value("height_number").toInt());
            preset.setResamplingFilterIndex(settings.value("resamplingFilter").toInt());
            preset.setKeepAspectRatio(settings.value("keepAspectRatio").toBool());
            preset.setBorderTypeIndex(settings.value("borderTypeIndex").toInt());
            preset.setBackgroundColor(
                settings.value("background_color_red").toInt(),
                settings.value("background_color_green").toInt(),
                settings.value("background_color_blue").toInt(),
                settings.value("background_color_alpha").toInt());
            preset.setBlurFilterIndex(settings.value("blurFilterIndex").toInt());
            preset.setBlurNormalize(settings.value("blurNormalize").toBool());
            preset.setBlurX(settings.value("blurX").toInt());
            preset.setBlurY(settings.value("blurY").toInt());
            preset.setBlurSX(settings.value("blurSX").toInt());
            preset.setBlurSX(settings.value("blurSX").toInt());
            preset.setBlurDepth(settings.value("blurDepth").toInt());
            preset.setRotationDegrees(settings.value("rotationDegrees").toInt());
            preset.setIncreaseBoundaries(settings.value("increaseBoundaries").toBool());
            preset.setFlipImage(settings.value("flipImage").toBool());
            preset.setWatermarkAdded(settings.value("watermarkAdded").toBool());
            preset.setWatermarkPath(settings.value("watermarkPath").toString());
            preset.setWatermarkLocationIndex(settings.value("watermarkLocationIndex").toInt());
            preset.setWatermarkTransparency(settings.value("watermarkTransparency").toInt());
            preset.setWatermarkOffsetX(settings.value("watermarkOffsetX").toInt());
            preset.setWatermarkOffsetY(settings.value("watermarkOffsetY").toInt());
            preset.setFormatChanged(settings.value("formatChanged").toBool());
            preset.setFormatExtensionIndex(settings.value("formatExtension").toInt());
            preset.setFormatFormatFlagIndex(settings.value("formatFormatFlagIndex").toInt());
            preset.setFormatQuality(settings.value("formatQuality").toInt());
            preset.setFormatOptimize(settings.value("formatOptimize").toBool());
            preset.setFormatProgressive(settings.value("formatProgressive").toBool());
            preset.setFormatCompression(settings.value("formatCompression").toInt());
            preset.setFormatExtra1(settings.value("formatExtra1").toInt());
            preset.setFormatExtra2(settings.value("formatExtra2").toInt());
            preset.setSaveFileProcedureIndex(settings.value("saveFileProcedure").toInt());
            preset.setSaveFileNameChange(settings.value("saveFileNameChange").toString().toStdString());
            preset.setSavePathRelative(settings.value("savePathRelative").toBool());
            preset.setSaveFilePathChange(settings.value("saveFilePathChange").toString());
            //preset_list.push_back({ preset });
            settings.endGroup();

        } while (settings.childGroups().indexOf("Preset" + i) > -1);

        settings.beginGroup("Settings");
        cspi = settings.value("current_selected_preset").toInt();
        settings.endGroup();

        //qDebug() << settings.allKeys();

        // TEMP: load only defaults, keep presets as defaults between sessions.
        preset_list.push_back({ preset1 });
        preset_list.push_back({ preset2 });
    }
    else {
        qDebug() << "Loading Default Presets";
        preset_list.push_back({ preset1 });
        preset_list.push_back({ preset2 });
        //SavePreset(true);
    }

    // Insert preset titles into all preset combo boxes.
    AddPresetsToComboBox(&preset_list, std::vector<QComboBox*>{
        ui.comboBox_Preset_1, ui.comboBox_Preset_2, ui.comboBox_Preset_3 });
    ChangePreset(cspi);

    // Load selected preset data into ui.
    LoadPreset(preset_list.at(current_selected_preset));
}

void BatchItImage::CreateNewPreset()
{
    SavePresetDialog();
    RemoveOptionsChanged();
    int new_preset_index = preset_list.size();
    qDebug() << "CreateNewPreset:" << new_preset_index;
    
    Preset new_preset;
    new_preset.setPresetIndex(new_preset_index);
    new_preset.setPresetDescription("New Preset");
    preset_list.push_back({ new_preset });

    ChangePresetDescription(
        new_preset_index,
        dialog_messages.at(Dialog::Messages::CreateNewPreset).name,
        dialog_messages.at(Dialog::Messages::CreateNewPreset).desc
        //ReplaceAll(dialog_messages.at(Dialog::Messages::CreateNewPreset).name, "##", preset_number),
        //ReplaceAll(dialog_messages.at(Dialog::Messages::CreateNewPreset).desc, "##", preset_number)
    );
}

void BatchItImage::RemoveCurrentPreset()
{
    QString preset_number = "#" + QVariant(CurrentSelectedPreset() + 1).toString();
    QString title;
    QString message;
    QDialogButtonBox::StandardButtons buttons = QDialogButtonBox::NoButton;
    uint custom_buttons = DialogMessage::CustomButton::NoCustomButton;
    bool use_bold_text = false;

    if (preset_list.size() > 1) {
        title = ReplaceAll(dialog_messages.at(Dialog::Messages::remove_preset_dialog).name, "##", preset_number);
        message = ReplaceAll(dialog_messages.at(Dialog::Messages::remove_preset_dialog).desc, "##", preset_number);
        buttons = QDialogButtonBox::Cancel;
        custom_buttons = DialogMessage::CustomButton::Delete;
        use_bold_text = dialog_messages.at(Dialog::Messages::remove_preset_dialog).data;
    }
    else {
        title = ReplaceAll(dialog_messages.at(Dialog::Messages::remove_preset_dialog_halted).name, "##", preset_number);
        message = ReplaceAll(dialog_messages.at(Dialog::Messages::remove_preset_dialog_halted).desc, "##", preset_number);
        buttons = QDialogButtonBox::Close;
        custom_buttons = DialogMessage::CustomButton::NoCustomButton;
        use_bold_text = dialog_messages.at(Dialog::Messages::remove_preset_dialog_halted).data;
    }
    auto* remove_preset_dialog = new DialogMessage(title, message, buttons, custom_buttons, this, use_bold_text);

    if (remove_preset_dialog->exec()) {
        qDebug() << "Deleting Preset";
        preset_list.erase(preset_list.begin() + current_selected_preset);
        current_selected_preset = (current_selected_preset) ? current_selected_preset - 1 : 0;
        
        AddPresetsToComboBox(&preset_list, std::vector<QComboBox*>{
            ui.comboBox_Preset_1, ui.comboBox_Preset_2, ui.comboBox_Preset_3
        });
        ChangePreset(current_selected_preset);
        SavePreset(true);
    }
    else {
        qDebug() << "Cancel Deleting Preset";
    }
    delete remove_preset_dialog;
}

bool BatchItImage::SavePresetDialog(bool include_cancel_buttons, bool closing)
{
    if (edit_options_change_tracker or save_options_change_tracker) {
        QString preset_number = "#" + QVariant(CurrentSelectedPreset() + 1).toString();
        QString title;
        QString message;
        QDialogButtonBox::StandardButtons buttons = QDialogButtonBox::NoButton;
        DialogMessage::CustomButtons custom_buttons = DialogMessage::CustomButton::NoCustomButton;

        if (closing) {
            title = ReplaceAll(dialog_messages.at(Dialog::Messages::save_preset_dialog_closing).name, "##", preset_number);
            message = ReplaceAll(dialog_messages.at(Dialog::Messages::save_preset_dialog_closing).desc, "##", preset_number);
            buttons = QDialogButtonBox::Cancel;
            custom_buttons = DialogMessage::CustomButton::SaveClose | DialogMessage::CustomButton::Close;
        }
        else {
            title = ReplaceAll(dialog_messages.at(Dialog::Messages::save_preset_dialog).name, "##", preset_number);
            message = ReplaceAll(dialog_messages.at(Dialog::Messages::save_preset_dialog).desc, "##", preset_number);
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
                else if (QDialogButtonBox::RejectRole == button_role_clicked
                    or QDialogButtonBox::DestructiveRole == button_role_clicked) { // Don't Save and Don't Continue
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
        [=](int index) {
            *preset_index = index;
            change_preset_desc_dialog->deleteLater();
        }));

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
                preset_text + preset_list->at(i).presetDescription(),
                QString::fromStdString("Preset" + std::to_string(i))
            );
            preset_cb.at(x)->setItemData(i,
                preset_list->at(i).presetDescription(),
                Qt::ToolTipRole
            );
            preset_cb.at(x)->setItemData(i,
                preset_list->at(i).presetDescription(),
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

void BatchItImage::EditAndSave(bool selected_only)
{
    int file_count = current_file_metadata_list.size();
    qDebug() << "Edit And Save" << file_count << "Images";
    
    // If current preset settings are not saved, ask to save them now before editing images.
    if (SavePresetDialog(true) or file_count == 0) {
        qDebug() << "Edit And Save... Aborted";
        return;
    }
    ui.enhancedProgressBar->restartProgressBar(file_count, 3.0f, true, function_PrintBatchImageLog);
    image_edit_start_time = std::chrono::system_clock::now();
    ie_pointer_list.clear();
    StartBatchImageLog();

    for (int i = 0; i < file_count; i++) {
        //qDebug().noquote() << current_file_metadata_list.at(i).to_string();

        if (selected_only and not current_file_metadata_list.at(i).selected) {
            emit progressMade(3.0f);
            continue;
        }

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
            background_color.blue(),
            background_color.green(),
            background_color.red(),
            background_color.alpha(),
            ui.comboBox_BlurFilter->currentData().toInt(),
            ui.checkBox_BlurNormalize->isChecked(),
            ui.verticalSlider_BlurX1->value(),
            ui.verticalSlider_BlurY1->value(),
            ui.verticalSlider_BlurX2->value(),
            ui.verticalSlider_BlurY2->value(),
            ui.verticalSlider_BlurD->value(),
            (ui.groupBox_Watermark->isChecked()) ? ui.lineEdit_WatermarkPath->text().toStdString() : "",
            ui.comboBox_WatermarkLocation->currentData().toInt(),
            ui.spinBox_WatermarkTransparency->value(),
            ui.spinBox_WatermarkOffsetX->value(),
            ui.spinBox_WatermarkOffsetY->value()
        );

        // Add Callback function when edit finishes, send data to SaveImageFile()
        //new_ie->addFinishedCallback(std::bind(&BatchItImage::SaveImageFile, this, std::placeholders::_1, std::placeholders::_2, i, new_ie));
        new_ie->addFinishedCallback(std::bind(&BatchItImage::SaveImageFile, this, i, new_ie));

        // Start the image edit process on another thread
        //std::future<uint> worker_thread = std::async(&ImageEditor::startEditProcess, new_ie);
        auto worker_thread = std::thread(&ImageEditor::startEditProcess, new_ie);

        //int image_edits_made = worker_thread.get();
        //qDebug() << "Done:" <<  image_edits_made;

        ie_pointer_list.push_back(new_ie);

        emit progressMade();

        worker_thread.detach();
    }
}

void BatchItImage::CancelAllImageEditing()
{
    qDebug().noquote() << "CancelAllImageEditing";
    for (auto& ie : ie_pointer_list) {
        if (ie) {
            ie->cancelEditProcess();
        }
    }
    ie_pointer_list.clear();
}

void BatchItImage::SaveImageFile(int image_index, ImageEditor* image_editor)
{
    qDebug() << "SaveImageFile:" << image_index << "  Edit-Code:" << image_editor->imageEditsMade();
    
    emit progressMade();

    int save_file_option_flag = ImageSaver::SaveOptionFlag::OVERWRITE;
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
        save_file_option_flag = ImageSaver::SaveOptionFlag::RENAME_ORIGINAL;
    }
    else if (ui.radioButton_NewFileName->isChecked()) {
        save_file_option_flag = ImageSaver::SaveOptionFlag::NEW_NAME;
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
        image_editor->editedImage(),
        current_file_metadata_list.at(image_index).path,
        save_file_option_flag,
        use_relative_save_path,
        save_file_path_change,
        ui.lineEdit_FileName->text().toStdString(),
        extension,
        image_index + 1
    );
    image_saver->pushFormatParameters(format_flag, quality, optimize, progressive, compression, extra1, extra2);

    // If edit process canceled or errors made, do not save, jump to updating log.
    //if (image_editor->editedImage()->empty() or image_editor->errorMessages()->size() > 0) {
    if (image_editor->isEditProcessCanceled()) {
        image_saver->cancelImageSave();
    }

    // Add Callback function when saving finishes, send data to UpdateLog().
    image_saver->addFinishedCallback(std::bind(&BatchItImage::UpdateLog, this, image_editor, image_saver));

    // Start the image saving process on another thread
    //std::future<bool> worker_thread = std::async(&ImageSaver::saveImageFile, image_saver);
    auto worker_thread = std::thread(&ImageSaver::saveImageFile, image_saver);
    
    worker_thread.detach();
}

void BatchItImage::StartBatchImageLog()
{
    qDebug() << "StartBatchImageLog";

    // Create Header, if not already created.
    if (log_lines.empty()) {
        std::chrono::zoned_time session_start_time_zt{ std::chrono::current_zone(), session_start_time };
        log_lines.push_back(log_text.at(LogFile::Line::ThickDivider));
        log_lines.push_back(log_text.at(LogFile::Line::Title));
        log_lines.push_back(log_text.at(LogFile::Line::ThickDivider));
        log_lines.push_back(
            ReplaceAll(log_text.at(LogFile::Line::SessionStart), "##", std::format("{0:%Y.%m.%d %H:%M:%OS}", session_start_time_zt))
        );
        log_lines.push_back(log_text.at(LogFile::Line::SessionEnd));
        log_lines.push_back("");
    }

    // Add Batch Summary
    log_lines.push_back(log_text.at(LogFile::Line::ThinDivider));
    log_lines.push_back(
        ReplaceAll(log_text.at(LogFile::Line::Batch), "##", "#" + std::to_string(++log_batch_number))
    );
    log_batch_summary = log_lines.size();
    log_lines.push_back(log_text.at(LogFile::Line::ThinDivider));
    log_lines.push_back(log_text.at(LogFile::Line::SummarySuccesses));
    log_lines.push_back(log_text.at(LogFile::Line::SummaryErrors));
    log_lines.push_back(log_text.at(LogFile::Line::SummaryTime));
    log_lines.push_back(log_text.at(LogFile::Line::ThinDivider));

    // Add Settings Used
    if (edit_options_change_tracker or save_options_change_tracker) {
        log_lines.push_back(log_text.at(LogFile::Line::UnsavedSettings));
    }
    else {
        log_lines.push_back(" " + ui.comboBox_Preset_1->currentText().toStdString());
    }
    log_lines.push_back("");

    // Width / Height
    if (ui.comboBox_WidthMod->currentIndex() > ImageEditor::SizeModifier::NO_CHANGE
        or ui.comboBox_HeightMod->currentIndex() > ImageEditor::SizeModifier::NO_CHANGE)
    {
        log_lines.push_back(
            " " + ui.comboBox_WidthMod->currentText().toStdString() + " " + std::to_string(ui.spinBox_WidthNumber->value())
        );
        log_lines.push_back(
            " " + ui.comboBox_HeightMod->currentText().toStdString() + " " + std::to_string(ui.spinBox_HeightNumber->value())
        );
    }

    // Resample
    if (ui.comboBox_WidthMod->currentIndex() > ImageEditor::SizeModifier::NO_CHANGE
        or ui.comboBox_HeightMod->currentIndex() > ImageEditor::SizeModifier::NO_CHANGE
        or ui.dial_Rotation->value() > 0)
    {
        log_lines.push_back(
            " " + ui.comboBox_Resample->currentText().toStdString()
        );
    }

    // Keep Aspect Ratio
    if (ui.checkBox_KeepAspectRatio->isChecked()) {
        log_lines.push_back(
            " " + ui.checkBox_KeepAspectRatio->text().toStdString()
        );
    }

    // Border / Background Color
    if (ui.dial_Rotation->value() > 0) {
        log_lines.push_back(
            " " + ui.comboBox_BorderType->currentText().toStdString()
        );
        log_lines.push_back(
            " " + ui.pushButton_ColorDialog->text().toStdString() + ": RGBA(" + \
            std::to_string(background_color.red()) + ", " + \
            std::to_string(background_color.green()) + ", " + \
            std::to_string(background_color.blue()) + ", " + \
            std::to_string(background_color.alpha()) + ")"
        );
    }

    // Blur
    if (ui.comboBox_BlurFilter->currentIndex() > ImageEditor::BlurFilter::NO_FILTER) {
        log_lines.push_back(
            " " + ui.comboBox_BlurFilter->currentText().toStdString()
        );
        if (ui.comboBox_BlurFilter->currentIndex() == ImageEditor::BlurFilter::BOX_FILTER
            and ui.checkBox_BlurNormalize->isChecked())
        {
            log_lines.push_back(
                " " + ui.checkBox_BlurNormalize->text().toStdString()
            );
        }
        if (ui.comboBox_BlurFilter->currentIndex() == ImageEditor::BlurFilter::BOX_FILTER
            or ui.comboBox_BlurFilter->currentIndex() == ImageEditor::BlurFilter::BILATERAL_FILTER
            or ui.comboBox_BlurFilter->currentIndex() == ImageEditor::BlurFilter::GAUSSIAN_BLUR
            or ui.comboBox_BlurFilter->currentIndex() == ImageEditor::BlurFilter::MEDIAN_BLUR)
        {
            log_lines.push_back(
                " " + ui.label_BlurX1->text().toStdString() + ": " + std::to_string(ui.verticalSlider_BlurX1->value())
            );
        }
        if (ui.comboBox_BlurFilter->currentIndex() == ImageEditor::BlurFilter::BOX_FILTER
            or ui.comboBox_BlurFilter->currentIndex() == ImageEditor::BlurFilter::GAUSSIAN_BLUR)
        {
            log_lines.push_back(
                " " + ui.label_BlurY1->text().toStdString() + ": " + std::to_string(ui.verticalSlider_BlurY1->value())
            );
        }
        if (ui.comboBox_BlurFilter->currentIndex() == ImageEditor::BlurFilter::BOX_FILTER
            or ui.comboBox_BlurFilter->currentIndex() == ImageEditor::BlurFilter::BILATERAL_FILTER
            or ui.comboBox_BlurFilter->currentIndex() == ImageEditor::BlurFilter::GAUSSIAN_BLUR)
        {
            log_lines.push_back(
                " " + ui.label_BlurX2->text().toStdString() + ": " + std::to_string(ui.verticalSlider_BlurX2->value())
            );
        }
        if (ui.comboBox_BlurFilter->currentIndex() == ImageEditor::BlurFilter::BOX_FILTER
            or ui.comboBox_BlurFilter->currentIndex() == ImageEditor::BlurFilter::BILATERAL_FILTER
            or ui.comboBox_BlurFilter->currentIndex() == ImageEditor::BlurFilter::GAUSSIAN_BLUR)
        {
            log_lines.push_back(
                " " + ui.label_BlurY2->text().toStdString() + ": " + std::to_string(ui.verticalSlider_BlurY2->value())
            );
        }
        if (ui.comboBox_BlurFilter->currentIndex() == ImageEditor::BlurFilter::BOX_FILTER) {
            log_lines.push_back(
                " " + ui.label_BlurD->text().toStdString() + ": " + std::to_string(ui.verticalSlider_BlurD->value())
            );
        }
    }

    // Rotation
    if (ui.dial_Rotation->value() > 0) {
        log_lines.push_back(
            " " + ui.groupBox_Rotation->title().toStdString() + " " + std::to_string(ui.dial_Rotation->value())
        );
        log_lines.push_back(
            " " + ui.checkBox_IncreaseBounds->text().toStdString()
        );
    }
    if (ui.checkBox_FlipImage->isChecked()) {
        log_lines.push_back(
            " " + ui.checkBox_FlipImage->text().toStdString()
        );
    }

    // Watermark
    if (ui.groupBox_Watermark->isChecked() and ui.lineEdit_WatermarkPath->text().length() > 0) {
        log_lines.push_back(
            " " + ui.groupBox_Watermark->title().toStdString() + " " + ui.lineEdit_WatermarkPath->text().toStdString()
        );
        log_lines.push_back(
            " " + ui.label_WatermarkLocation->text().toStdString() + " " + ui.comboBox_WatermarkLocation->currentText().toStdString()
        );
        log_lines.push_back(
            " " + ui.label_WatermarkTransparency->text().toStdString() + " " + std::to_string(ui.spinBox_WatermarkTransparency->value())
        );
        log_lines.push_back(
            " " + ui.label_WatermarkOffset->text().toStdString() + " " + \
            std::to_string(ui.spinBox_WatermarkOffsetX->value()) + ", " + \
            std::to_string(ui.spinBox_WatermarkOffsetX->value())
        );
    }

    // File Name
    if (ui.checkBox_Overwrite->isChecked()) {
        log_lines.push_back(
            " " + ui.checkBox_Overwrite->text().toStdString()
        );
    }
    else if (ui.radioButton_RenameOriginal->isChecked()) {
        log_lines.push_back(
            " " + ui.radioButton_RenameOriginal->text().toStdString() + ": " + ui.lineEdit_FileName->text().toStdString()
        );
    }
    else if (ui.radioButton_NewFileName->isChecked()) {
        log_lines.push_back(
            " " + ui.radioButton_NewFileName->text().toStdString() + ": " + ui.lineEdit_FileName->text().toStdString()
        );
    }

    // File Path
    if (ui.radioButton_RelativePath->isChecked()) {
        log_lines.push_back(
            " " + ui.radioButton_RelativePath->text().toStdString() + " " + ui.lineEdit_RelativePath->text().toStdString()
        );
    }
    else if (ui.radioButton_AbsolutePath->isChecked()) {
        log_lines.push_back(
            " " + ui.radioButton_AbsolutePath->text().toStdString() + " " + ui.lineEdit_AbsolutePath->text().toStdString()
        );
    }

    // File Format/Extension
    if (ui.groupBox_ChangeFormat->isChecked()) {
        log_lines.push_back(
            " " + ui.label_ImageFormat->text().toStdString() + " " + ui.comboBox_ImageFormat->currentText().toStdString()
        );
        if (ui.comboBox_ImageFormat->currentIndex() == ImageSaver::ImageExtension::jpeg
            or ui.comboBox_ImageFormat->currentIndex() == ImageSaver::ImageExtension::jpg
            or ui.comboBox_ImageFormat->currentIndex() == ImageSaver::ImageExtension::jpe
            or ui.comboBox_ImageFormat->currentIndex() == ImageSaver::ImageExtension::png
            or ui.comboBox_ImageFormat->currentIndex() == ImageSaver::ImageExtension::pam
            or ui.comboBox_ImageFormat->currentIndex() == ImageSaver::ImageExtension::tiff
            or ui.comboBox_ImageFormat->currentIndex() == ImageSaver::ImageExtension::tif
            or ui.comboBox_ImageFormat->currentIndex() == ImageSaver::ImageExtension::exr
            or ui.comboBox_ImageFormat->currentIndex() == ImageSaver::ImageExtension::hdr)
        {
            log_lines.push_back(
                " " + ui.label_FormatFlags->text().toStdString() + " " + ui.comboBox_FormatFlags->currentText().toStdString()
            );
        }
        if (ui.comboBox_ImageFormat->currentIndex() == ImageSaver::ImageExtension::jpeg
            or ui.comboBox_ImageFormat->currentIndex() == ImageSaver::ImageExtension::jpg
            or ui.comboBox_ImageFormat->currentIndex() == ImageSaver::ImageExtension::jpe
            or ui.comboBox_ImageFormat->currentIndex() == ImageSaver::ImageExtension::webp
            or ui.comboBox_ImageFormat->currentIndex() == ImageSaver::ImageExtension::avif
            or ui.comboBox_ImageFormat->currentIndex() == ImageSaver::ImageExtension::tiff
            or ui.comboBox_ImageFormat->currentIndex() == ImageSaver::ImageExtension::tif)
        {
            log_lines.push_back(
                " " + ui.label_Quality->text().toStdString() + " " + std::to_string(ui.horizontalSlider_Quality->value())
            );
        }

        if (ui.comboBox_ImageFormat->currentIndex() == ImageSaver::ImageExtension::jpeg
            or ui.comboBox_ImageFormat->currentIndex() == ImageSaver::ImageExtension::jpg
            or ui.comboBox_ImageFormat->currentIndex() == ImageSaver::ImageExtension::jpe
            or ui.comboBox_ImageFormat->currentIndex() == ImageSaver::ImageExtension::png
            or ui.comboBox_ImageFormat->currentIndex() == ImageSaver::ImageExtension::pbm
            or ui.comboBox_ImageFormat->currentIndex() == ImageSaver::ImageExtension::pgm
            or ui.comboBox_ImageFormat->currentIndex() == ImageSaver::ImageExtension::ppm
            or ui.comboBox_ImageFormat->currentIndex() == ImageSaver::ImageExtension::pxm
            or ui.comboBox_ImageFormat->currentIndex() == ImageSaver::ImageExtension::exr)
        {
            if (ui.checkBox_Optimize->isChecked()) {
                log_lines.push_back(
                    " " + ui.checkBox_Optimize->text().toStdString()
                );
            }
        }
        if (ui.comboBox_ImageFormat->currentIndex() == ImageSaver::ImageExtension::jpeg
            or ui.comboBox_ImageFormat->currentIndex() == ImageSaver::ImageExtension::jpg
            or ui.comboBox_ImageFormat->currentIndex() == ImageSaver::ImageExtension::jpe
            or ui.comboBox_ImageFormat->currentIndex() == ImageSaver::ImageExtension::exr)
        {
            if (ui.checkBox_Progressive->isChecked()) {
                log_lines.push_back(
                    " " + ui.checkBox_Progressive->text().toStdString()
                );
            }
        }
        if (ui.comboBox_ImageFormat->currentIndex() == ImageSaver::ImageExtension::jpeg
            or ui.comboBox_ImageFormat->currentIndex() == ImageSaver::ImageExtension::jpg
            or ui.comboBox_ImageFormat->currentIndex() == ImageSaver::ImageExtension::jpe
            or ui.comboBox_ImageFormat->currentIndex() == ImageSaver::ImageExtension::jp2
            or ui.comboBox_ImageFormat->currentIndex() == ImageSaver::ImageExtension::png
            or ui.comboBox_ImageFormat->currentIndex() == ImageSaver::ImageExtension::avif
            or ui.comboBox_ImageFormat->currentIndex() == ImageSaver::ImageExtension::exr)
        {
            log_lines.push_back(
                " " + ui.label_Compression->text().toStdString() + " " + std::to_string(ui.spinBox_Compression->value())
            );
        }
        // ExtraSetting1
        if (ui.comboBox_ImageFormat->currentIndex() == ImageSaver::ImageExtension::jpeg
            or ui.comboBox_ImageFormat->currentIndex() == ImageSaver::ImageExtension::jpg
            or ui.comboBox_ImageFormat->currentIndex() == ImageSaver::ImageExtension::jpe)
        {
            if (ui.spinBox_ExtraSetting1->value() > -1) {
                log_lines.push_back(
                    " " + ui.label_ExtraSetting1->text().toStdString() + " " + std::to_string(ui.spinBox_ExtraSetting1->value())
                );
            }
        }
        else if (ui.comboBox_ImageFormat->currentIndex() == ImageSaver::ImageExtension::tiff
            or ui.comboBox_ImageFormat->currentIndex() == ImageSaver::ImageExtension::tif)
        {
            log_lines.push_back(
                " " + ui.label_ExtraSetting1->text().toStdString() + " " + std::to_string(ui.spinBox_ExtraSetting1->value())
            );
        }
        // ExtraSetting2
        if (ui.comboBox_ImageFormat->currentIndex() == ImageSaver::ImageExtension::jpeg
            or ui.comboBox_ImageFormat->currentIndex() == ImageSaver::ImageExtension::jpg
            or ui.comboBox_ImageFormat->currentIndex() == ImageSaver::ImageExtension::jpe)
        {
            if (ui.spinBox_ExtraSetting2->value() > -1) {
                log_lines.push_back(
                    " " + ui.label_ExtraSetting2->text().toStdString() + " " + std::to_string(ui.spinBox_ExtraSetting2->value())
                );
            }
        }
        else if (ui.comboBox_ImageFormat->currentIndex() == ImageSaver::ImageExtension::avif
            or ui.comboBox_ImageFormat->currentIndex() == ImageSaver::ImageExtension::tiff
            or ui.comboBox_ImageFormat->currentIndex() == ImageSaver::ImageExtension::tif)
        {
            log_lines.push_back(
                " " + ui.label_ExtraSetting2->text().toStdString() + " " + std::to_string(ui.spinBox_ExtraSetting2->value())
            );
        }
    }
    log_lines.push_back(log_text.at(LogFile::Line::ThinDivider));
    log_lines.push_back("");
}

void BatchItImage::UpdateLog(ImageEditor* edited_image, ImageSaver* saved_image)
{
    qDebug() << "UpdateLog";

    std::vector<std::string>* edit_errors = edited_image->errorMessages();
    std::string* save_error = saved_image->errorMessage();

    log_lines.push_back(ReplaceAll(log_text.at(LogFile::Line::ImageNumber), "##", "#" + std::to_string(saved_image->imageCount())));
    log_lines.push_back(saved_image->originalImagePath());
    log_lines.push_back(saved_image->imageSavePath());

    if (saved_image->isImageSaveCanceled()) {
        log_lines.push_back(log_text.at(LogFile::Line::SaveSuccess));
    }
    else {
        if (edit_errors->empty()) {
            successful_image_edits++;
        }
        else {
            for (auto& error : *edit_errors) {
                log_lines.push_back(ReplaceAll(log_text.at(LogFile::Line::EditError), "##", error));
            }
        }
        if (save_error->empty()) {
            successful_image_saves++;
            log_lines.push_back(log_text.at(LogFile::Line::SaveCanceled));
        }
        else {
            image_save_errors++;
            log_lines.push_back(ReplaceAll(log_text.at(LogFile::Line::SaveError), "##", *save_error));
        }
    }
    log_lines.push_back("");

    emit progressMade();
    delete edited_image;
    delete saved_image;
}

void BatchItImage::PrintBatchImageLog()
{
    // If no attempt to edit any images was made (none selected or canceled immediately).
    // Note: This will be called as soon as any progress (bar) is made and finished.
    if (successful_image_saves == 0 and image_save_errors == 0) {
        log_lines.erase(log_lines.begin() + log_end_line, log_lines.end());
        log_batch_number--;
        successful_image_edits = 0;
        successful_image_saves = 0;
        image_save_errors = 0;
        return; 
    }

    // Finish entering the last pieces of data to log.
    auto image_edit_end_time = std::chrono::system_clock::now();
    auto session_end_time = std::chrono::system_clock::now();
    std::chrono::zoned_time session_start_time_zt{ std::chrono::current_zone(), session_start_time };
    std::chrono::zoned_time session_end_time_zt{ std::chrono::current_zone(), session_end_time };
    auto time_elapsed = image_edit_end_time - image_edit_start_time;
    auto session_time_elapsed = session_end_time - session_start_time;

    qDebug() << "PrintBatchImageLog (Time Elapsed):" << time_elapsed.count();
    qDebug() << session_time_elapsed.count();

    auto time_elapsed_formatted = std::chrono::hh_mm_ss{
        std::chrono::duration_cast<std::chrono::milliseconds>(time_elapsed)
    };
    std::string completion_time = std::to_string(time_elapsed_formatted.hours().count()) + ":" + \
        std::to_string(time_elapsed_formatted.minutes().count()) + ":" + \
        std::to_string(time_elapsed_formatted.seconds().count()) + "." + \
        std::to_string(time_elapsed_formatted.subseconds().count());

    log_lines.at(4) = ReplaceAll( // Session End
        log_lines.at(4), "##", std::format("{0:%Y.%m.%d %H:%M:%OS}", session_end_time_zt)
    );
    log_lines.at(log_batch_summary + 1) = ReplaceAll(
        log_lines.at(log_batch_summary + 1), "##", std::to_string(successful_image_saves)
    );
    log_lines.at(log_batch_summary + 2) = ReplaceAll(
        log_lines.at(log_batch_summary + 2), "##", std::to_string(image_save_errors)
    );
    log_lines.at(log_batch_summary + 3) = ReplaceAll(
        log_lines.at(log_batch_summary + 3), "##", completion_time
    );

    // Create Log File Name and Path
    const std::string log_file_name = "log_" + std::format("{0:%Y%m%d%H%M%OS}", session_start_time_zt) + ".txt";
    bool log_error = CreateDirectories(log_directory_path);

    // Write To Log File
    if (not log_error) {
        std::ofstream log_file(log_directory_path / log_file_name, std::ios::out | std::ios::app);
        if (log_file.is_open()) {
            for (uint i = log_end_line; i < log_lines.size(); i++) {
                log_file << log_lines.at(i) << "\n";
            }
            log_file.close();
        }
        else {
            qWarning() << "Error: Unable to open and write to log file.";
            log_error = true;
        }
    }

    // Delete Old Log Files (past the maximum allowed)
    std::array<QString, 2> log_extensions = { ".txt", ".log" };
    QStringList all_log_files = IterateDirectory(std::filesystem::directory_iterator(log_directory_path), log_extensions);
    all_log_files.sort();
    if (all_log_files.size() > max_log_files) {
        all_log_files.erase(all_log_files.end() - max_log_files, all_log_files.end());
        for (const auto& file : all_log_files) {
            qDebug() << "Deleted:" << file;
            std::filesystem::remove(file.toStdString());
        }
    }

    // Show dialog informing user that editing and saving is finished and a log file was (or wasn't) created.
    QString title;
    QString message;
    QDialogButtonBox::StandardButtons buttons = QDialogButtonBox::Ok;
    DialogMessage::CustomButtons custom_buttons = DialogMessage::CustomButton::OpenLog | DialogMessage::CustomButton::SaveLogAs;

    if (log_error) {
        title = dialog_messages.at(Dialog::Messages::log_created_dialog_error).name;
        message = dialog_messages.at(Dialog::Messages::log_created_dialog_error).desc;
        buttons = QDialogButtonBox::Ok;
        custom_buttons = DialogMessage::CustomButton::NoCustomButton;
    }
    else if (log_end_line) {
        title = dialog_messages.at(Dialog::Messages::log_created_dialog_updated).name;
        message = dialog_messages.at(Dialog::Messages::log_created_dialog_updated).desc;
    }
    else {
        title = dialog_messages.at(Dialog::Messages::log_created_dialog).name;
        message = dialog_messages.at(Dialog::Messages::log_created_dialog).desc;
    }
    auto* log_created_dialog = new DialogMessage(title, message, buttons, custom_buttons, this);
    Q_ASSERT(connect(log_created_dialog, &DialogMessage::buttonRoleClicked, this,
        [=](QDialogButtonBox::ButtonRole button_role_clicked) {
            if (QDialogButtonBox::ButtonRole::ActionRole == button_role_clicked) { // Open Log
                // Windows Only (Mac: open, Win: notepad)
                const std::string open_log_file_path = "start notepad \"" + (log_directory_path / log_file_name).string() + "\"";
                qDebug() << open_log_file_path;
                std::system(open_log_file_path.c_str());
            }
            else if (QDialogButtonBox::ButtonRole::ApplyRole == button_role_clicked) { // Save Log File
                std::string log_file_new_save_path = QFileDialog::getSaveFileName( this,
                    file_dialog_titles.at(Dialog::FileSearch::log_file_new_save_path),
                    QString::fromStdString((log_directory_path / log_file_name).string()),
                    file_dialog_titles.at(Dialog::FileSearch::log_file_new_save_path_extensions) + \
                    " (*" + log_extensions[0] + " *" + log_extensions[1] + ")"
                ).toStdString();
                qDebug() << "Save:" << log_file_new_save_path;
                if (std::filesystem::exists(std::filesystem::path(log_file_new_save_path).parent_path()))
                    std::filesystem::copy_file(log_directory_path / log_file_name, log_file_new_save_path);
            }
        }
    ));
    log_created_dialog->exec();
    log_created_dialog->deleteLater();

    log_end_line = log_lines.size();
    successful_image_edits = 0;
    successful_image_saves = 0;
    image_save_errors = 0;
}

void BatchItImage::LoadImageFiles()
{
    QStringList files = QFileDialog::getOpenFileNames(
        this,
        file_dialog_titles.at(Dialog::FileSearch::LoadImageFiles),
        qdefault_path, // TODO: Default config setting, "recent"
        supported_image_extensions_dialog_str
    );
    if (not files.empty()) {
        AddNewFiles(files);
    }
}

QString BatchItImage::GetImageFile(QString default_image_path)
{
    QString file = QFileDialog::getOpenFileName(
        this,
        file_dialog_titles.at(Dialog::FileSearch::GetImageFile),
        default_image_path,
        supported_image_extensions_dialog_str
    );
    if (file.length() > 0)
        return file;
    else
        return default_image_path;
}

void BatchItImage::AddNewFile(QString file)
{
    QStringList file_list;
    file_list.append(file);
    BuildFileMetadataList(file_list);
}

void BatchItImage::AddNewFiles(QStringList file_list)
{
    qDebug() << "AddNewFiles:" << file_list.count();

    // Create new updated file list from files found in directories.
    QStringList updated_file_list;
    for (const auto& file : file_list)
    {
        std::filesystem::path file_path = std::filesystem::u8path(file.toStdString()).make_preferred();
        // TODO: handle windows shortcut files. https://stackoverflow.com/questions/22986845/windows-read-the-target-of-shortcut-file-in-c
        if (std::filesystem::is_symlink(file_path)) {
            file_path = std::filesystem::read_symlink(file_path);
            qDebug() << "symlink target:" <<  file_path;
        }
        if (std::filesystem::is_directory(file_path)) {
            QStringList files_from_dir;
            if (ui.checkBox_SearchSubDirs->isChecked())
                files_from_dir = IterateDirectory(std::filesystem::recursive_directory_iterator(file_path), extension_list);
            else
                files_from_dir = IterateDirectory(std::filesystem::directory_iterator(file_path), extension_list);
            updated_file_list.append(files_from_dir);
            //qDebug() << "File:" << std::filesystem::directory_iterator(file_path)->path().string();
        }
        else {
            updated_file_list.append(QString::fromStdString(file_path.string()));
        }
    }
    if (updated_file_list.size())
        BuildFileMetadataList(updated_file_list);
    //DebugPrintList(current_file_metadata_list, "current_file_metadata_list");
}

template<typename DirectoryIter, std::size_t array_size>
QStringList BatchItImage::IterateDirectory(DirectoryIter iterator, std::array<QString, array_size>& matching_extension_list)
{
    QStringList updated_file_list;
    for (auto const& dir_entry : iterator) {
        if (std::filesystem::is_regular_file(dir_entry)) {
            qDebug() << "File Found:" <<  dir_entry.path().string();
            std::string file_ext = dir_entry.path().extension().string();
            for (const auto& matching_extension : matching_extension_list) {
                std::string ext = matching_extension.toStdString();
                if (ext == file_ext) {
                    QString file = QString::fromStdString(dir_entry.path().string());
                    updated_file_list.append(file);
                    break;
                }
            }
        }
        else {
            qDebug() << "Sub-Directory or Non File Found:" <<  dir_entry.path().string();
        }
    }
    return updated_file_list;
}

void BatchItImage::BuildFileMetadataList(const QStringList file_list)
{
    std::string file_path_str;
    int load_order = last_load_count; // Get highest load_order in all file lists. 
    int file_count = file_list.size();

    ui.enhancedProgressBar->restartProgressBar(file_count, 3.0f, function_FileLoadingFinished);

    //qDebug() << "file_list (count):" <<  file_list.count();

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
                qDebug() << "- QThread Worker Created -";
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
    qDebug().noquote() << "HandleFileMetadata" <<  file_metadata->to_string();

    if (file_metadata->load_order) {
        emit progressMade();
        
        int item_count = ui.treeWidget_FileInfo->topLevelItemCount();
        //qDebug() << item_count;

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
        qDebug() << "----> Bad/Non Image File";

        // TODO: return bad file path? then add to vector of bad files, show non_image_file_dialog for each or ignore all
        if (not non_image_file_dialog_shown) {
            auto* non_image_file_dialog = new DialogMessage(
                dialog_messages.at(Dialog::Messages::non_image_file_dialog).name,
                dialog_messages.at(Dialog::Messages::non_image_file_dialog).desc,
                QDialogButtonBox::Ignore,
                DialogMessage::CustomButton::NoCustomButton,
                this
            );
            Q_ASSERT(connect(non_image_file_dialog, &DialogMessage::buttonClicked,
                [=](QDialogButtonBox::StandardButton button) {
                    //qDebug() << "Button:" <<  button;
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
    //qDebug().noquote() << current_file_metadata_list.back().to_string();
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
    std::filesystem::path file_path = std::filesystem::u8path(file.path).make_preferred();
    QString file_path_str = QString::fromStdString(file_path.string());

    // Add path to recently loaded list
    auto existing_recent_index = recent_file_paths_loaded.indexOf(file_path_str);
    if (existing_recent_index > -1) {
        recent_file_paths_loaded.removeAt(existing_recent_index);
    }
    recent_file_paths_loaded.prepend(file_path_str);
    if (recent_file_paths_loaded.size() > recent_file_paths_loaded_max) {
        recent_file_paths_loaded.erase(recent_file_paths_loaded.begin() + 10, recent_file_paths_loaded.end());
    }

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
    file_selected_check_box->setStatusTip(file_tree_other_text[FileTree::Column::FILE_LOAD_ORDER] + QVariant(file.load_order).toString());
    
    connect(file_selected_check_box, &QCheckBox::toggled, this,
        [=] (bool toggle) { FileSelectionChange(file_index, toggle); });

    ui.treeWidget_FileInfo->setItemWidget(new_item, FileTree::Column::FILE_SELECTED, file_selected_check_box);
    new_item->setToolTip(FileTree::Column::FILE_SELECTED, file_tree_other_text[FileTree::Column::FILE_LOAD_ORDER] + QVariant(file.load_order).toString());
    //new_item->setData(FileTree::Column::FILE_SELECTED, Qt::ToolTipRole, QVariant(file.load_order));
    //new_item->setData(FileTree::Column::FILE_SELECTED, Qt::StatusTipRole, QVariant(file.load_order)); // Won't show for widget column
    new_item->setText(FileTree::Column::FILE_NAME, QString::fromStdString(file_path.filename().string()));
    new_item->setStatusTip(FileTree::Column::FILE_NAME, file_tree_other_text[FileTree::Column::FILE_NAME] + QString::fromStdString(file.path));
    new_item->setToolTip(FileTree::Column::FILE_NAME, file_tree_other_text[FileTree::Column::FILE_NAME] + QString::fromStdString(file.path));
    new_item->setText(FileTree::Column::IMAGE_DIMENSIONS, QString::fromStdString(std::to_string(file.width) + " x " + std::to_string(file.height)));
    new_item->setTextAlignment(FileTree::Column::IMAGE_DIMENSIONS, Qt::AlignCenter);
    new_item->setStatusTip(FileTree::Column::IMAGE_DIMENSIONS, file_tree_other_text[FileTree::Column::IMAGE_DIMENSIONS] + QVariant(file.width).toString() + " x " + QVariant(file.height).toString());
    //new_item->setToolTip(FileTree::Column::IMAGE_DIMENSIONS, QString::fromStdString("Image Width: " + std::to_string(file.width) + ",   Image Height: " + std::to_string(file.height)));
    new_item->setToolTip(FileTree::Column::IMAGE_DIMENSIONS, file_tree_other_text[FileTree::Column::IMAGE_DIMENSIONS] + QVariant(file.width).toString() + " x " + QVariant(file.height).toString());
    new_item->setText(FileTree::Column::FILE_SIZE, QString::fromStdString(file_size_str));
    new_item->setTextAlignment(FileTree::Column::FILE_SIZE, Qt::AlignRight);
    new_item->setStatusTip(FileTree::Column::FILE_SIZE, file_tree_other_text[FileTree::Column::FILE_SIZE] + QString::fromStdString(file_size_str));
    new_item->setToolTip(FileTree::Column::FILE_SIZE, file_tree_other_text[FileTree::Column::FILE_SIZE] + QString::fromStdString(file_size_str));
    new_item->setText(FileTree::Column::DATE_CREATED, QString::fromStdString(date_created));
    new_item->setStatusTip(FileTree::Column::DATE_CREATED, file_tree_other_text[FileTree::Column::DATE_CREATED] + QString(date_created));
    new_item->setToolTip(FileTree::Column::DATE_CREATED, file_tree_other_text[FileTree::Column::DATE_CREATED] + QString(date_created));
    new_item->setText(FileTree::Column::DATE_MODIFIED, QString::fromStdString(date_modified));
    new_item->setStatusTip(FileTree::Column::DATE_MODIFIED, file_tree_other_text[FileTree::Column::DATE_MODIFIED] + QString(date_modified));
    new_item->setToolTip(FileTree::Column::DATE_MODIFIED, file_tree_other_text[FileTree::Column::DATE_MODIFIED] + QString(date_modified));
    
    new_item->setFont(FileTree::Column::FILE_SELECTED, *font_mono);
    new_item->setFont(FileTree::Column::FILE_NAME, *font_mono);
    new_item->setFont(FileTree::Column::IMAGE_DIMENSIONS, *font_mono);
    new_item->setFont(FileTree::Column::FILE_SIZE, *font_mono);
    new_item->setFont(FileTree::Column::DATE_CREATED, *font_mono);
    new_item->setFont(FileTree::Column::DATE_MODIFIED, *font_mono);
    if (sorted_column > -1 and sorted_column < FileTree::Column::COUNT)
        new_item->setFont(sorted_column, *font_mono_bold);

    QTreeWidgetItem* item_file_path = new QTreeWidgetItem(new_item);
    item_file_path->setText(0, file_path_str);
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

void BatchItImage::FileSelectionChange(uint index, bool toggle)
{
    //int current_file_tree_row = ui.treeWidget_FileInfo->currentIndex().row();
    qDebug() << "Row:" << index << "  Checked:" << toggle;
    current_file_metadata_list.at(index).selected = toggle;
}

void BatchItImage::FileSelectionToggle(int index)
{
    qDebug() << "FileSelectionToggle:" << index + 1 << "of" << current_file_metadata_list.size();
    if (index > -1 and index < current_file_metadata_list.size()) {
        Qt::CheckState toggle = (current_file_metadata_list.at(index).selected)
            ? Qt::Unchecked : Qt::Checked;
        qDebug() << "FileSelectionToggle:" << toggle;
        qobject_cast<QCheckBox*>(ui.treeWidget_FileInfo->itemWidget(
            ui.treeWidget_FileInfo->topLevelItem(index), 0))->setCheckState(toggle);
    }
}

void BatchItImage::FileSelectionToggleAll(bool toggle)
{
    qDebug() << "FileSelectionToggleAll:" << toggle;
    Qt::CheckState checked_toggle = (toggle) ? Qt::Checked : Qt::Unchecked;
    for (int i = 0; i < ui.treeWidget_FileInfo->topLevelItemCount(); i++) {
        qobject_cast<QCheckBox*>(ui.treeWidget_FileInfo->itemWidget(
            ui.treeWidget_FileInfo->topLevelItem(i), 0))->setCheckState(checked_toggle);
    }
}

void BatchItImage::FileSelectionFilter(QList<QAction*> actions)
{
    for (size_t i = 0; i < current_file_metadata_list.size(); i++) {

        std::string ext = std::filesystem::path(current_file_metadata_list.at(i).path).extension().string();

        auto file_selection_cb = qobject_cast<QCheckBox*>(ui.treeWidget_FileInfo->
            itemWidget(ui.treeWidget_FileInfo->topLevelItem(i), FileTree::Column::FILE_SELECTED));

        if (ImageSaver::ExtensionList.at(ImageSaver::ImageExtension::jpeg) == ext
            or ImageSaver::ExtensionList.at(ImageSaver::ImageExtension::jpg) == ext
            or ImageSaver::ExtensionList.at(ImageSaver::ImageExtension::jpe) == ext)
        {
            file_selection_cb->setChecked(actions.at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_jpeg)->isChecked());
        }
        else if (ImageSaver::ExtensionList.at(ImageSaver::ImageExtension::jp2) == ext) {
            file_selection_cb->setChecked(actions.at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_jp2)->isChecked());
        }
        else if (ImageSaver::ExtensionList.at(ImageSaver::ImageExtension::png) == ext) {
            file_selection_cb->setChecked(actions.at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_png)->isChecked());
        }
        else if (ImageSaver::ExtensionList.at(ImageSaver::ImageExtension::webp) == ext) {
            file_selection_cb->setChecked(actions.at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_webp)->isChecked());
        }
        else if (ImageSaver::ExtensionList.at(ImageSaver::ImageExtension::bmp) == ext
            or ImageSaver::ExtensionList.at(ImageSaver::ImageExtension::dib) == ext)
        {
            file_selection_cb->setChecked(actions.at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_bmp)->isChecked());
        }
        else if (ImageSaver::ExtensionList.at(ImageSaver::ImageExtension::avif) == ext) {
            file_selection_cb->setChecked(actions.at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_avif)->isChecked());
        }
        else if (ImageSaver::ExtensionList.at(ImageSaver::ImageExtension::pbm) == ext
            or ImageSaver::ExtensionList.at(ImageSaver::ImageExtension::pgm) == ext
            or ImageSaver::ExtensionList.at(ImageSaver::ImageExtension::ppm) == ext
            or ImageSaver::ExtensionList.at(ImageSaver::ImageExtension::pxm) == ext
            or ImageSaver::ExtensionList.at(ImageSaver::ImageExtension::pnm) == ext
            or ImageSaver::ExtensionList.at(ImageSaver::ImageExtension::pfm) == ext
            or ImageSaver::ExtensionList.at(ImageSaver::ImageExtension::pam) == ext)
        {
            file_selection_cb->setChecked(actions.at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_pbm)->isChecked());
        }
        else if (ImageSaver::ExtensionList.at(ImageSaver::ImageExtension::sr) == ext
            or ImageSaver::ExtensionList.at(ImageSaver::ImageExtension::ras) == ext)
        {
            file_selection_cb->setChecked(actions.at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_sr)->isChecked());
        }
        else if (ImageSaver::ExtensionList.at(ImageSaver::ImageExtension::tiff) == ext
            or ImageSaver::ExtensionList.at(ImageSaver::ImageExtension::tif) == ext)
        {
            file_selection_cb->setChecked(actions.at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_tiff)->isChecked());
        }
        else if (ImageSaver::ExtensionList.at(ImageSaver::ImageExtension::exr) == ext){
            file_selection_cb->setChecked(actions.at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_exr)->isChecked());
        }
        else if (ImageSaver::ExtensionList.at(ImageSaver::ImageExtension::hdr) == ext
            or ImageSaver::ExtensionList.at(ImageSaver::ImageExtension::pic) == ext)
        {
            file_selection_cb->setChecked(actions.at(FileTree::ActionMenu::SubMenu::FilterImageFormats::action_filter_hdr)->isChecked());
        }
    }
}

void BatchItImage::FileLoadingFinished()
{
    qDebug() << "FileLoadingFinished";
    FileSortingFinished();
    SaveRecentFiles();
}

void BatchItImage::FileSortingFinished()
{
    for (int i = 0; i < FileTree::Column::COUNT; i++) {
        ui.treeWidget_FileInfo->resizeColumnToContents(i);
        //ui.treeWidget_FileInfo->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    }
}

void BatchItImage::SortFileTreeByColumn(int index)
{
    qDebug() << "Column Clicked:" << index << "  Sort Order:" << current_file_sort_order;
    ui.enhancedProgressBar->restartProgressBar(ui.treeWidget_FileInfo->topLevelItemCount(), 2.0f, function_FileSortingFinished);

    Qt::SortOrder qsort_indicator = Qt::SortOrder::AscendingOrder;
    
    // Get sort order
    if (current_file_column_sorted != index) {
        current_file_column_sorted = index;
        current_file_sort_order = FileTree::SortOrder::ASCENDING1;
    }
    else {
        current_file_sort_order = (current_file_sort_order < FileTree::SortOrder::DESCENDING2) ? ++current_file_sort_order : FileTree::SortOrder::ASCENDING1;
        qsort_indicator = (fmod(current_file_sort_order, 2) == 0) ? Qt::SortOrder::AscendingOrder : Qt::SortOrder::DescendingOrder;
    }
    
    // Sort list of file metadata based on column clicked.
    if (current_file_column_sorted == FileTree::Column::FILE_LOAD_ORDER and
        (current_file_sort_order == FileTree::SortOrder::ASCENDING1 or current_file_sort_order == FileTree::SortOrder::ASCENDING2)) {
        std::sort(begin(current_file_metadata_list), end(current_file_metadata_list), [](auto const& left, auto const& right) {
                return left.load_order < right.load_order;
            });
    }
    else if (current_file_column_sorted == FileTree::Column::FILE_LOAD_ORDER and
        (current_file_sort_order == FileTree::SortOrder::DESCENDING1 or current_file_sort_order == FileTree::SortOrder::DESCENDING2)) {
        std::sort(begin(current_file_metadata_list), end(current_file_metadata_list), [](auto const& left, auto const& right) {
                return left.load_order > right.load_order;
            });
    }
    else if (current_file_column_sorted == FileTree::Column::FILE_NAME and
        (current_file_sort_order == FileTree::SortOrder::ASCENDING1 or current_file_sort_order == FileTree::SortOrder::ASCENDING2)) {
        std::sort(begin(current_file_metadata_list), end(current_file_metadata_list), [](auto const& left, auto const& right) {
                return left.path < right.path;
            });
    }
    else if (current_file_column_sorted == FileTree::Column::FILE_NAME and
        (current_file_sort_order == FileTree::SortOrder::DESCENDING1 or current_file_sort_order == FileTree::SortOrder::DESCENDING2)) {
        std::sort(begin(current_file_metadata_list), end(current_file_metadata_list), [](auto const& left, auto const& right) {
                return left.path > right.path;
            });
    }
    else if (current_file_column_sorted == FileTree::Column::IMAGE_DIMENSIONS and (current_file_sort_order == FileTree::SortOrder::ASCENDING1)) {
        std::sort(begin(current_file_metadata_list), end(current_file_metadata_list), [](auto const& left, auto const& right) {
                return left.width < right.width;
            });
    }
    else if (current_file_column_sorted == FileTree::Column::IMAGE_DIMENSIONS and (current_file_sort_order == FileTree::SortOrder::DESCENDING1)) {
        std::sort(begin(current_file_metadata_list), end(current_file_metadata_list), [](auto const& left, auto const& right) {
                return left.width > right.width;
            });
    }
    else if (current_file_column_sorted == FileTree::Column::IMAGE_DIMENSIONS and
        (current_file_sort_order == FileTree::SortOrder::ASCENDING2)) {
        std::sort(begin(current_file_metadata_list), end(current_file_metadata_list), [](auto const& left, auto const& right) {
                return left.height < right.height;
            });
    }
    else if (current_file_column_sorted == FileTree::Column::IMAGE_DIMENSIONS and
        (current_file_sort_order == FileTree::SortOrder::DESCENDING2)) {
        std::sort(begin(current_file_metadata_list), end(current_file_metadata_list), [](auto const& left, auto const& right) {
                return left.height > right.height;
            });
    }
    else if (current_file_column_sorted == FileTree::Column::FILE_SIZE and
        (current_file_sort_order == FileTree::SortOrder::ASCENDING1 or current_file_sort_order == FileTree::SortOrder::ASCENDING2)) {
        std::sort(begin(current_file_metadata_list), end(current_file_metadata_list), [](auto const& left, auto const& right) {
                return left.size < right.size;
            });
    }
    else if (current_file_column_sorted == FileTree::Column::FILE_SIZE and
        (current_file_sort_order == FileTree::SortOrder::DESCENDING1 or current_file_sort_order == FileTree::SortOrder::DESCENDING2)) {
        std::sort(begin(current_file_metadata_list), end(current_file_metadata_list), [](auto const& left, auto const& right) {
                return left.size > right.size;
            });
    }
    else if (current_file_column_sorted == FileTree::Column::DATE_CREATED and
        (current_file_sort_order == FileTree::SortOrder::ASCENDING1 or current_file_sort_order == FileTree::SortOrder::ASCENDING2)) {
        std::sort(begin(current_file_metadata_list), end(current_file_metadata_list), [](auto const& left, auto const& right) {
                return left.date_created < right.date_created;
            });
    }
    else if (current_file_column_sorted == FileTree::Column::DATE_CREATED and
        (current_file_sort_order == FileTree::SortOrder::DESCENDING1 or current_file_sort_order == FileTree::SortOrder::DESCENDING2)) {
        std::sort(begin(current_file_metadata_list), end(current_file_metadata_list), [](auto const& left, auto const& right) {
                return left.date_created > right.date_created;
            });
    }
    else if (current_file_column_sorted == FileTree::Column::DATE_MODIFIED and
        (current_file_sort_order == FileTree::SortOrder::ASCENDING1 or current_file_sort_order == FileTree::SortOrder::ASCENDING2)) {
        std::sort(begin(current_file_metadata_list), end(current_file_metadata_list), [](auto const& left, auto const& right) {
                return left.date_modified < right.date_modified;
            });
    }
    else if (current_file_column_sorted == FileTree::Column::DATE_MODIFIED and
        (current_file_sort_order == FileTree::SortOrder::DESCENDING1 or current_file_sort_order == FileTree::SortOrder::DESCENDING2)) {
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
    //qDebug() << "Object Watched:" << watched->objectName().toStdString() << "  eventFiltered:" << event->type();
    if (event->type() == QEvent::ContextMenu) {
        qDebug() << "ContextMenu Called";
    }
    if (event->type() == QEvent::ChildPolished) { // Used in-place of "no ActionsContextMenu event"
        qDebug() << "ActionsContextMenu Called?";
        int current_file_tree_row = GetCurrentFileTreeRow();
        if (current_file_tree_row > -1) {
            if (current_file_metadata_list.at(current_file_tree_row).selected)
                action_select->setText(unselect_text);
            else
                action_select->setText(select_text);
        }
    }
    // Pass on the event.
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
        qDebug() << "Delete Key Pressed";
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
                    dialog_messages.at(Dialog::Messages::delete_dialog_clear).name,
                    dialog_messages.at(Dialog::Messages::delete_dialog_clear).desc,
                    QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                    DialogMessage::CustomButton::NoCustomButton,
                    this
                );
            }
            else {
                delete_dialog = new DialogMessage(
                    dialog_messages.at(Dialog::Messages::delete_dialog).name,
                    dialog_messages.at(Dialog::Messages::delete_dialog).desc,
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
            //qDebug() << delete_dialog->exec(); //QDialog::Accepted ?
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
    qDebug() << "RemoveFileFromTree At Index:" << current_file_tree_row << "  Button:" << button_clicked;
    int dupe_index = -1;
    std::vector<size_t> d_indexes;

    if (QDialogButtonBox::Yes & button_clicked) { // Delete Highlighted Row
        qDebug() << "Yes Button";
        //dupe_index = IsFileInList(current_file_metadata_list.at(current_file_tree_row).path, deleted_file_metadata_list);
        //if (dupe_index == -1) {
            deleted_file_metadata_list.push_back(current_file_metadata_list.at(current_file_tree_row));
        //}
        current_file_metadata_list.erase(current_file_metadata_list.begin() + current_file_tree_row);
        DeleteTreeItemObjects(ui.treeWidget_FileInfo, current_file_tree_row);
    }
    else if (QDialogButtonBox::YesToAll & button_clicked) { // Delete All Selected
        qDebug() << "Yes To All Button";
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
        qDebug() << "Ok Button";

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
        qDebug() << "No Button";
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
        qDebug() << "Deleting all object in row:" <<  row_index;
        QList<QTreeWidgetItem*> child_widget_list = tree->topLevelItem(row_index)->takeChildren();
        qDeleteAll(child_widget_list);
        delete tree->takeTopLevelItem(row_index);
    } 
    else { // Clear/Delete All
        while (auto item = tree->takeTopLevelItem(0)) {
            qDebug() << "Deleting all objects in first row: current-count->" <<  tree->topLevelItemCount();
            QList<QTreeWidgetItem*> child_widget_list = item->takeChildren();
            qDeleteAll(child_widget_list);
            /*for (auto& child_widget : child_widget_list) {
                qDebug() << "Deleting a Widget in TopLevelItem: current-count->" << child_widget_list.count();
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


bool BatchItImage::CreateDirectories(std::filesystem::path file_path)
{
    bool error = false;
    try { // Create missing directories
        if (not std::filesystem::exists(file_path))
            std::filesystem::create_directories(file_path);
    }
    catch (const std::exception& err) {
        qWarning() << "Error: Directory Creation Failed - " << err.what();
        error = true;
    }
    return error;
}


void BatchItImage::AddTextToFileName()
{
    // Get Text
    QString cur_qtext = ui.lineEdit_FileName->text();

    // Move cursor to an index right after <> if it is in-between <>
    qDebug() << "Cursor Position:" <<  ui.lineEdit_FileName->cursorPosition();
    int rb_found = cur_qtext.indexOf(">", ui.lineEdit_FileName->cursorPosition());
    if (rb_found > -1) {
        int lb_found = cur_qtext.indexOf("<", ui.lineEdit_FileName->cursorPosition());
        if (lb_found == -1 or lb_found > rb_found) {
            ui.lineEdit_FileName->setCursorPosition(rb_found + 1);
            qDebug() << "Cursor Position Moved:" <<  ui.lineEdit_FileName->cursorPosition();
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

void BatchItImage::CheckWatermarkPath()
{
    std::filesystem::path path = std::filesystem::path(
        ui.lineEdit_WatermarkPath->text().toStdString()
    );
    bool watermark_path_verified = false;
    if (std::filesystem::exists(path)) {
        QString file_ext = QString::fromStdString(path.extension().string());
        for (const auto& img_ext : extension_list) {
            if (img_ext == file_ext) {
                watermark_path_verified = true;
                break;
            }
        }
    }
    if (watermark_path_verified) {
        last_existing_wm_path = ui.lineEdit_WatermarkPath->text();
    }
    else {
        auto* check_wm_path_dialog = new DialogMessage(
            dialog_messages.at(Dialog::Messages::check_wm_path_dialog).name,
            dialog_messages.at(Dialog::Messages::check_wm_path_dialog).desc,
            QDialogButtonBox::Open | QDialogButtonBox::Cancel,
            DialogMessage::CustomButton::NoCustomButton,
            this
        );
        QString last_verified_watermark_path = (last_existing_wm_path.length() > 0)
            ? last_existing_wm_path : preset_list.at(CurrentSelectedPreset()).watermarkPath();
        
        if (check_wm_path_dialog->exec()) {
            ui.lineEdit_WatermarkPath->setText(GetImageFile(last_verified_watermark_path));
            last_existing_wm_path = ui.lineEdit_WatermarkPath->text();
        }
        else {
            ui.lineEdit_WatermarkPath->setText(last_verified_watermark_path);
        }
        delete check_wm_path_dialog;
    }
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
            dialog_messages.at(Dialog::Messages::check_path_dialog).name,
            dialog_messages.at(Dialog::Messages::check_path_dialog).desc,
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
        file_dialog_titles.at(Dialog::FileSearch::GetSaveDirectoryPath),
        qpath // TODO: Default config setting, "recent"
    );
    qDebug() << directory.toStdString();
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

const QString BatchItImage::ReplaceAll(QString str, const QString& from, const QString& to,
    const Qt::CaseSensitivity case_sensitivity, bool no_really_all) { // Static
    size_t start_pos = 0;
    while ((start_pos = str.indexOf(from, start_pos, case_sensitivity)) != -1) {
        str.replace(start_pos, from.length(), to);
        if (not no_really_all)
            start_pos += to.length();
    }
    return str;
}
