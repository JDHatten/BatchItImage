#include "BatchItImage.h"

/*
TODO: 
    Log Actions
    Holding right click scrolls left to right
    Track changes to undo file list deletes or image edit ui changes
    Image File Filter... show/edit only PNG files, files > 1 MB, etc.

*/


MessageWindow::MessageWindow(QString title, QString message, QFlags<QDialogButtonBox::StandardButton> buttons, QWidget* parent) : QDialog(parent)//, m_ui(new Ui::Dialog_MessageWindow)
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
    //delete m_ui;
}
void MessageWindow::changeEvent(QEvent* e)
{
    //DEBUG("MessageWindow changeEvent");
    QDialog::changeEvent(e);
}
void MessageWindow::ButtonBoxClicked(QAbstractButton* button)
{
    DEBUG("MessageWindow ButtonBoxClicked");
    QDialogButtonBox::StandardButton std_button = ui.buttonBox->standardButton(button);
    DEBUG(std_button);
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


/// <summary>
/// A QObject that will take a file path and build a struct with metadata.
/// Intended to be ran in another thread as a "Worker".
/// Slot -> GetFileMetadata() -> Signal -> FileMetadataReady(FileMetadata*)
/// </summary>
/// <param name="file_path">--A file path string.</param>
/// <param name="load_order">--This file metadata should be added now on initilzation.</param>
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
        file_metadata->path = file_path; // File that could not be opened. Bad image / not an image / etc.
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

    // comboBox_WidthMod Item Selections
    width_selections[0].data = ImageEditor::NO_CHANGE;
    width_selections[0].name = "No Change";
    width_selections[0].desc = "Image widths may still be modified if 'keep aspect ratio' is checked.";
    width_selections[1].data = ImageEditor::CHANGE_TO;
    width_selections[1].name = "Change Width To:";
    width_selections[1].desc = "All images widths will be modified to a specfic number.";
    width_selections[2].data = ImageEditor::MODIFY_BY;
    width_selections[2].name = "Modify Width By:";
    width_selections[2].desc = "This adds to or subtracts from an image's current width. Ex. 1080 + '220' = 1300";
    width_selections[3].data = ImageEditor::MODIFY_BY_PCT;
    width_selections[3].name = "Modify Width By (%):";
    width_selections[3].desc = "This modifies an image's current width by percetage. Ex. 720 x '200%' = 1440";
    width_selections[4].data = ImageEditor::DOWNSCALE;
    width_selections[4].name = "Downscale Width To:";
    width_selections[4].desc = "All images above entered width will be modified to that specfic number. All images already at or below that number will not be modified.";
    width_selections[5].data = ImageEditor::UPSCALE;
    width_selections[5].name = "Upscale Width To:";
    width_selections[5].desc = "All images below entered width will be modified to that specfic number. All images already at or above that number will not be modified.";
    PopulateComboBoxes(ui.comboBox_WidthMod, width_selections, 6);

    // comboBox_HeightMod Item Selections
    height_selections[0].data = ImageEditor::NO_CHANGE;
    height_selections[0].name = "No Change";
    height_selections[0].desc = "Image heights may still be modified if 'keep aspect ratio' is checked.";
    height_selections[1].data = ImageEditor::CHANGE_TO;
    height_selections[1].name = "Change Height To:";
    height_selections[1].desc = "All images heights will be modified to a specfic number.";
    height_selections[2].data = ImageEditor::MODIFY_BY;
    height_selections[2].name = "Modify Height By:";
    height_selections[2].desc = "This adds to or subtracts from an image's current height. Ex. 1080 + '220' = 1300";
    height_selections[3].data = ImageEditor::MODIFY_BY_PCT;
    height_selections[3].name = "Modify Height By (%):";
    height_selections[3].desc = "This modifies an image's current height by percetage. Ex. 720 x '200%' = 1440";
    height_selections[4].data = ImageEditor::DOWNSCALE;
    height_selections[4].name = "Downscale Height To:";
    height_selections[4].desc = "All images above entered height will be modified to that specfic number. All images already at or below that number will not be modified.";
    height_selections[5].data = ImageEditor::UPSCALE;
    height_selections[5].name = "Upscale Height To:";
    height_selections[5].desc = "All images below entered height will be modified to that specfic number. All images already at or above that number will not be modified.";
    PopulateComboBoxes(ui.comboBox_HeightMod, height_selections, 6);

    ui.checkBox_KeepAspectRatio->setText("Keep Aspect Ratio");
    ui.checkBox_KeepAspectRatio->setToolTip("In order to keep aspect ratio, either width or height must be set to \"No Change\" or \"0\"");

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
    PopulateComboBoxes(ui.comboBox_Resample, resampling_selections, 3);

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
    PopulateComboBoxes(ui.comboBox_AddText, file_name_creation, 4);

    // comboBox_ImageFormat
    image_formats[0].data = ".jpeg";
    image_formats[0].name = "JPEG Files - *.jpeg";
    image_formats[0].desc = "JPEG (Joint Photographic Experts Group) is a commonly used method of lossy compression for digital images, particularly for those images produced by digital photography. The degree of compression can be adjusted, allowing a selectable tradeoff between storage size and image quality. JPEG typically achieves 10:1 compression with little perceptible loss in image quality.";
    image_formats[1].data = ".jpg";
    image_formats[1].name = "JPEG Files - *.jpg";
    image_formats[1].desc = image_formats[2].desc;
    image_formats[2].data = ".jpe";
    image_formats[2].name = "JPEG Files - *.jpe";
    image_formats[2].desc = image_formats[2].desc;

    image_formats[3].data = ".jp2";
    image_formats[3].name = "JPEG 2000 Files - *.jp2";
    image_formats[3].desc = "JPEG 2000 (Joint Photographic Experts Group) is an image compression standard based on a discrete wavelet transform (DWT).  Note that it is still not widely supported in web browsers (other than Safari) and hence is not generally used on the World Wide Web.";

    image_formats[4].data = ".png";
    image_formats[4].name = "Portable Network Graphics - *.png";
    image_formats[4].desc = "Portable Network Graphics (PNG) is a raster-graphics file format that supports lossless data compression. PNG supports palette-based images (with palettes of 24-bit RGB or 32-bit RGBA colors), grayscale images (with or without an alpha channel for transparency), and full-color non-palette-based RGB or RGBA images.";

    image_formats[5].data = ".webp";
    image_formats[5].name = "WebP - *.webp";
    image_formats[5].desc = "WebP is a raster graphics file format developed by Google intended as a replacement for JPEG, PNG, and GIF file formats. It supports both lossy and lossless compression, as well as animation and alpha transparency.";

    image_formats[6].data = ".bmp";
    image_formats[6].name = "Windows Bitmaps - *.bmp";
    image_formats[6].desc = "The BMP file format or bitmap, is a raster graphics image file format used to store bitmap digital images, independently of the display device (such as a graphics adapter), especially on Microsoft Windows and OS/2 operating systems.";
    image_formats[7].data = ".dib";
    image_formats[7].name = "Windows Bitmaps - *.dib";
    image_formats[7].desc = image_formats[0].desc;

    image_formats[8].data = ".avif";
    image_formats[8].name = "AVIF - *.avif";
    image_formats[8].desc = "AV1 Image File Format (AVIF) is an open, royalty-free image file format specification for storing images or image sequences compressed with AV1 in the HEIF container format.  AV1 Supports: * Multiple color spaces (HDR, SDR, color space signaling via CICP or ICC) * Lossless and lossy compression * 8-, 10-, and 12-bit color depths * Monochrome (alpha/depth) or multi-components * 4:2:0, 4:2:2, 4:4:4 chroma subsampling and RGB * Film grain synthesis * Image sequences/animation";

    image_formats[9].data = ".pbm";
    image_formats[9].name = "Netpbm Formats - *.pbm";
    image_formats[9].desc = "Netpbm (formerly Pbmplus) is an open-source package of graphics programs and a programming library. It is used mainly in the Unix world, but also works on Microsoft Windows, macOS, and other operating systems.  Graphics formats used and defined by the Netpbm project: portable pixmap format (PPM), portable graymap format (PGM), and portable bitmap format (PBM). They are also sometimes referred to collectively as the portable anymap format (PNM).";
    image_formats[10].data = ".pgm";
    image_formats[10].name = "Netpbm Formats - *.pgm";
    image_formats[10].desc = image_formats[9].desc;
    image_formats[11].data = ".ppm";
    image_formats[11].name = "Netpbm Formats - *.ppm";
    image_formats[11].desc = image_formats[9].desc;
    image_formats[12].data = ".pxm";
    image_formats[12].name = "Netpbm Formats - *.pxm";
    image_formats[12].desc = image_formats[9].desc;
    image_formats[13].data = ".pnm";
    image_formats[13].name = "Netpbm Formats - *.pnm";
    image_formats[13].desc = image_formats[9].desc;

    image_formats[14].data = ".pfm";
    image_formats[14].name = "Netpbm Formats - *.pfm";
    image_formats[14].desc = "The PFM (Portable Floatmap) is supported by the de facto reference implementation Netpbm and is the unofficial four byte IEEE 754 single precision floating point extension. PFM is supported by the programs Photoshop, GIMP, and ImageMagick.";

    image_formats[15].data = ".sr";
    image_formats[15].name = "Sun Rasters - *.sr";
    image_formats[15].desc = "Sun Raster was a raster graphics file format used on SunOS by Sun Microsystems. ACDSee, FFmpeg, GIMP, ImageMagick, IrfanView, LibreOffice, Netpbm, PaintShop Pro, PMView, and XnView among others support Sun Raster image files. The format does not support transparency.";
    image_formats[16].data = ".ras";
    image_formats[16].name = "Sun Rasters - *.ras";
    image_formats[16].desc = image_formats[15].desc;

    image_formats[17].data = ".tiff";
    image_formats[17].name = "TIFF Files - *.tiff";
    image_formats[17].desc = "Tag Image File Format (TIFF or TIF), is an image file format for storing raster graphics images, popular among graphic artists, the publishing industry, and photographers. TIFF is widely supported by scanning, faxing, word processing, optical character recognition, image manipulation, desktop publishing, and page-layout applications.";
    image_formats[18].data = ".tif";
    image_formats[18].name = "TIFF Files - *.tif";
    image_formats[18].desc = image_formats[17].desc;

    image_formats[19].data = ".exr";
    image_formats[19].name = "OpenEXR Image Files - *.exr";
    image_formats[19].desc = "OpenEXR is a high-dynamic range, multi-channel raster file format, created under a free software license similar to the BSD license.  It supports multiple channels of potentially different pixel sizes, including 32-bit unsigned integer, 32-bit and 16-bit floating point values, as well as various compression techniques which include lossless and lossy compression algorithms. It also has arbitrary channels and encodes multiple points of view such as left- and right-camera images.";

    image_formats[20].data = ".hdr";
    image_formats[20].name = "Radiance HDR - *.hdr";
    image_formats[20].desc = "RGBE or Radiance HDR is an image format that stores pixels as one byte each for RGB (red, green, and blue) values with a one byte shared exponent. Thus it stores four bytes per pixel. RGBE allows pixels to have the dynamic range and precision of floating-point values in a relatively compact data structure (32 bits per pixel).";
    image_formats[21].data = ".pic";
    image_formats[21].name = "Radiance HDR - *.pic";
    image_formats[21].desc = image_formats[20].desc;

    PopulateComboBoxes(ui.comboBox_ImageFormat, image_formats, 22);

    /* https://docs.opencv.org/4.x/d4/da8/group__imgcodecs.html#ga288b8b3da0892bd651fce07b3bbd3a56
    *.bmp, *.dib, *.jpeg, *.jpg, *.jpe, *.jp2, *.png, *.webp, *.avif, *.pbm, *.pgm, *.ppm, *.pxm, *.pnm, *.pfm, *.sr, *.ras, *.tiff, *.tif, *.exr, *.hdr, *.pic"; */

    // comboBox_Subsampling
    format_subsamplings[0].data = cv::IMWRITE_JPEG_SAMPLING_FACTOR_411;
    format_subsamplings[0].name = "4x1, 1x1, 1x1";
    format_subsamplings[0].desc = "JPEG Sampling 4:1:1.";
    format_subsamplings[1].data = cv::IMWRITE_JPEG_SAMPLING_FACTOR_420;
    format_subsamplings[1].name = "2x2, 1x1, 1x1 (Default)";
    format_subsamplings[1].desc = "JPEG Sampling 4:2:0. In most cases this is the best option.";
    format_subsamplings[2].data = cv::IMWRITE_JPEG_SAMPLING_FACTOR_422;
    format_subsamplings[2].name = "2x1, 1x1, 1x1";
    format_subsamplings[2].desc = "JPEG Sampling 4:2:2.";
    format_subsamplings[3].data = cv::IMWRITE_JPEG_SAMPLING_FACTOR_440;
    format_subsamplings[3].name = "1x2, 1x1, 1x1";
    format_subsamplings[3].desc = "JPEG Sampling 4:4:0.";
    format_subsamplings[4].data = cv::IMWRITE_JPEG_SAMPLING_FACTOR_444;
    format_subsamplings[4].name = "1x1, 1x1, 1x1 (No Subsampling)";
    format_subsamplings[4].desc = "JPEG Sampling 4:4:4. It can help to turn off subsampling when using more than one quality setting (Luma/Chroma).";
    PopulateComboBoxes(ui.comboBox_Subsampling, format_subsamplings, 5);



    /***************************
        Prep UI Widgets, Etc
    ****************************/

    ui.progressBar->setVisible(false);
    ui.treeWidget_FileInfo->clear();
    ui.treeWidget_FileInfo->headerItem()->setText(FILE_SELECTED, "");
    ui.treeWidget_FileInfo->setColumnWidth(FILE_SELECTED, ui.treeWidget_FileInfo->minimumWidth());
    //ui.treeWidget_FileInfo->setSortingEnabled(true);
    ui.treeWidget_FileInfo->header()->setSectionsClickable(true);
    ui.treeWidget_FileInfo->header()->sortIndicatorOrder();

    ui.treeWidget_FileInfo->installEventFilter(this); // Keep watch of all events happening in file tree. -> eventFilter()

    preset_list.reserve(10);
    current_file_metadata_list.reserve(30);
    deleted_file_metadata_list.reserve(10);

    // TODO: get settings for search sub dirs, recent images loaded (last 10?), others?
    ui.checkBox_SearchSubDirs->setChecked(search_subdirs);

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
    //for (const auto& ext : supported_images) {
    for (const auto& ext : image_formats) {
        supported_image_extensions_dialog_str.append("*" + std::get<std::string>(ext.data) + " ");
    }
    supported_image_extensions_dialog_str.insert(supported_image_extensions_dialog_str.size() - 1, ")");
    supported_image_extensions_dialog_str.append("\nAll Files (*)");
    DEBUG(supported_image_extensions_dialog_str.toStdString());



    /***************************
        UI Events
    ****************************/

    // Menu
    connect(ui.action_AddImages, SIGNAL(triggered(bool)), this, SLOT(LoadImageFiles()));
    connect(ui.action_About, SIGNAL(triggered(bool)), this, SLOT(Test()));
    //connect(ui.action_About, &QAbstractButton::pressed, this, &QApplication::aboutQt);
    connect(aboutQtAct, &QAction::triggered, [this] { QApplication::aboutQt(); });

    // Image File Tree
    connect(ui.treeWidget_FileInfo->header(), SIGNAL(sectionClicked(int)), this, SLOT(SortFileTreeByColumn(int)));
    connect(ui.checkBox_SearchSubDirs, &QCheckBox::stateChanged, [this] { search_subdirs = ui.checkBox_SearchSubDirs->isChecked(); });
    SetupFileTreeContextMenu();

    // Image Edit Widgets
    connect(ui.comboBox_Preset_1, SIGNAL(currentIndexChanged(int)), this, SLOT(ChangePreset(int)));
    //connect(ui.comboBox_Preset_1, SIGNAL(currentIndexChanged(int)), ui.comboBox_Preset_2, SLOT(ChangePresets(int))); // Done in the ui xml + 9 Others
    
    // Image Save Widgets
    connect(ui.pushButton_EditAndSave, SIGNAL(clicked(bool)), this, SLOT(EditAndSave()));
    connect(ui.comboBox_AddText, SIGNAL(activated(int)), this, SLOT(AddTextToFileName()));
    connect(ui.lineEdit_RelativePath, SIGNAL(editingFinished()), this, SLOT(CheckRelativePath()));
    connect(ui.pushButton_AddBackOneDir, &QAbstractButton::pressed,
        [this] {
            ui.lineEdit_RelativePath->setText(ui.lineEdit_RelativePath->text().prepend("../"));
            CheckRelativePath();
        });
    connect(ui.lineEdit_AbsolutePath, SIGNAL(editingFinished()), this, SLOT(CheckAbsolutePath()));
    connect(ui.pushButton_FindAbsolutePath, &QAbstractButton::pressed,
        [this] { ui.lineEdit_AbsolutePath->setText(GetSaveDirectoryPath()); });



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
    QString status_meesage = "BatchItImage";
    ui.statusbar->showMessage(status_meesage, -1);
}

BatchItImage::~BatchItImage() {}


// Just for quick testing
void BatchItImage::Test()
{
    DEBUG("--TEST--");
    //_comboBox_Preset_1->addItem("New Item");
    //image_label->setText("Something Else");
    //DEBUG(ui.comboBox_AddText->currentData().toString().toStdString());
    //DEBUG(ui.comboBox_AddText->currentData().toChar());
    //qDebug() << ui.comboBox_AddText->currentData();
    //DEBUG(ui.comboBox_AddText->currentData().typeName());

    /*
    if (strcmp(ui.comboBox_AddText->currentData().typeName(), "int") == 0)
    {
        DEBUG("INT");
        //int cur_data = ui.comboBox_AddText->currentData().toInt();
        DEBUG(ui.comboBox_AddText->currentData().toInt());
    }
    else
    {
        DEBUG("STRING");
        //std::string cur_data = ui.comboBox_AddText->currentData().toString().toStdString();
        DEBUG(ui.comboBox_AddText->currentData().toString().toStdString());
    }
    */

    //ui.progressBar->setValue(current_load_number);
}


/// <summary>
/// Add items to various combo boxes which include titles, tooltip descriptions, and other data.
/// </summary>
/// <param name="cb">--Pointer to a QComboBox.</param>
/// <param name="items">--Data array to enter into a combo box.</param>
/// <param name="items_size">--Size/length of item data array.</param>
void BatchItImage::PopulateComboBoxes(QComboBox* cb, UIData items[], int items_size)
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
    }
}

/// <summary>
/// Build a "right click" context menu for the file tree.
/// </summary>
void BatchItImage::SetupFileTreeContextMenu()
{
    action_add = new QAction("Add Images", this);
    action_delete = new QAction("Delete Images", this);
    action_clear = new QAction("Clear List", this);
    action_select = new QAction("Select Image", this);
    action_view = new QAction("View Image", this);
    action_preview = new QAction("Preview Modified Image", this);

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
        //emit valueChanged(index);
        
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
    //DEBUG("SavePresets");

    //QStringList recent_image_files;
    //recent_image_files.resize(20);
    // TODO: record time file added, sort descending, when adding to "recent_image_files" create new QStringList and join with old.


    QSettings settings(preset_settings_file, QSettings::IniFormat);
    settings.beginGroup("Settings");
    settings.setValue("current_selected_preset", ui.comboBox_Preset_1->currentIndex());
    settings.endGroup();

    DEBUG2("SP->current_selected_preset: ", current_selected_preset);

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
            settings.setValue("rotation_angle", preset_list.at(i).rotation_angle);
            settings.setValue("format_change", preset_list.at(i).format_change);
            settings.setValue("format", preset_list.at(i).format_extension);
            settings.setValue("format_subsampling", preset_list.at(i).format_subsampling);
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
            save_option = OVERWRITE;
        }
        else if (ui.radioButton_RenameOriginal->isChecked()) {
            save_option = RENAME_ORG;
        }
        else {
            save_option = NEW_NAME;
        }
        bool relative_save_path = ui.radioButton_RelativePath->isChecked();

        // Update current preset in list then save it in settings
        preset_list.at(current_selected_preset).index = current_selected_preset;
        preset_list.at(current_selected_preset).description = ui.comboBox_Preset_1->currentText();
        preset_list.at(current_selected_preset).width_change_selection = ui.comboBox_WidthMod->currentIndex();
        preset_list.at(current_selected_preset).width_number = ui.spinBox_WidthNumber->value();
        preset_list.at(current_selected_preset).height_change_selection = ui.comboBox_HeightMod->currentIndex();
        preset_list.at(current_selected_preset).height_number = ui.spinBox_HeightNumber->value();
        preset_list.at(current_selected_preset).keep_aspect_ratio = ui.checkBox_KeepAspectRatio->isChecked();
        preset_list.at(current_selected_preset).resampling_filter = ui.comboBox_Resample->currentIndex();
        preset_list.at(current_selected_preset).rotation_angle = ui.dial_Rotation->value();
        preset_list.at(current_selected_preset).format_change = ui.groupBox_ChangeFormat->isChecked();
        preset_list.at(current_selected_preset).format_extension = ui.comboBox_ImageFormat->currentIndex();
        preset_list.at(current_selected_preset).format_subsampling = ui.comboBox_Subsampling->currentIndex();
        preset_list.at(current_selected_preset).format_optimize = ui.checkBox_Optimize->isChecked();
        preset_list.at(current_selected_preset).format_progressive = ui.checkBox_Progressive->isChecked();
        preset_list.at(current_selected_preset).format_quality = ui.horizontalSlider_Quality->value();
        preset_list.at(current_selected_preset).format_compression = ui.horizontalSlider_Compression->value();
        preset_list.at(current_selected_preset).format_extra1 = ui.spinBox_ExtraSetting1->value();
        preset_list.at(current_selected_preset).format_extra2 = ui.spinBox_ExtraSetting2->value();
        preset_list.at(current_selected_preset).save_file_policy_option = save_option;
        preset_list.at(current_selected_preset).save_file_name_change = ui.lineEdit_FileName->text().toStdString();
        preset_list.at(current_selected_preset).relative_save_path = relative_save_path;
        if (relative_save_path)
            preset_list.at(current_selected_preset).save_file_path_change = ui.lineEdit_RelativePath->text().toStdString();
        else
            preset_list.at(current_selected_preset).save_file_path_change = ui.lineEdit_AbsolutePath->text().toStdString();


        /*settings.beginGroup("Preset" + std::to_string(current_selected_preset));
        settings.setValue("description", ui.comboBox_Preset_1->currentText());
        settings.setValue("width_change_selection", ui.comboBox_WidthMod->currentIndex());
        settings.setValue("width_number", ui.spinBox_WidthNumber->value());
        settings.setValue("height_change_selection", ui.comboBox_HeightMod->currentIndex());
        settings.setValue("height_number", ui.spinBox_HeightNumber->value());
        settings.setValue("keep_aspect_ratio", ui.checkBox_KeepAspectRatio->isChecked());
        settings.setValue("resampling_filter", ui.comboBox_Resample->currentIndex());
        settings.setValue("rotation_angle", ui.dial_Rotation->value());
        settings.setValue("save_file_policy_option", save_option);
        settings.setValue("save_file_name_change", ui.lineEdit_FileName->text());
        settings.endGroup();*/

        settings.beginGroup("Preset" + std::to_string(current_selected_preset));
        settings.setValue("description", preset_list.at(current_selected_preset).description);
        settings.setValue("width_change_selection", preset_list.at(current_selected_preset).width_change_selection);
        settings.setValue("width_number", preset_list.at(current_selected_preset).width_number);
        settings.setValue("height_change_selection", preset_list.at(current_selected_preset).height_change_selection);
        settings.setValue("height_number", preset_list.at(current_selected_preset).height_number);
        settings.setValue("keep_aspect_ratio", preset_list.at(current_selected_preset).keep_aspect_ratio);
        settings.setValue("resampling_filter", preset_list.at(current_selected_preset).resampling_filter);
        settings.setValue("rotation_angle", preset_list.at(current_selected_preset).rotation_angle);
        settings.setValue("format_change", preset_list.at(current_selected_preset).format_change);
        settings.setValue("format_extension", preset_list.at(current_selected_preset).format_extension);
        settings.setValue("format_subsampling", preset_list.at(current_selected_preset).format_subsampling);
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
    ui.dial_Rotation->setValue(preset.rotation_angle);
    ui.lcdNumber_Rotation->display(preset.rotation_angle);
    ui.groupBox_ChangeFormat->setChecked(preset.format_change);
    ui.comboBox_ImageFormat->setCurrentIndex(preset.format_extension);
    ui.comboBox_Subsampling->setCurrentIndex(preset.format_subsampling);
    ui.checkBox_Optimize->setChecked(preset.format_optimize);
    ui.checkBox_Progressive->setChecked(preset.format_progressive);
    ui.horizontalSlider_Quality->setValue(preset.format_quality);
    ui.horizontalSlider_Compression->setValue(preset.format_compression);
    ui.spinBox_ExtraSetting1->setValue(preset.format_extra1);
    ui.spinBox_ExtraSetting2->setValue(preset.format_extra2);
    ui.lineEdit_FileName->setText(QString::fromStdString(preset.save_file_name_change));
    int save_option = preset.save_file_policy_option;
    if (save_option == OVERWRITE) {
        ui.radioButton_Overwrite->setChecked(true);
    }
    else if (save_option == RENAME_ORG) {
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
    preset1.description = "[Preset #" + QString::fromStdString(std::to_string(preset1.index + 1)) + "] (Default) Create New 600x600 Image.";
    preset1.width_change_selection = ImageEditor::CHANGE_TO;
    preset1.width_number = 600;
    preset1.height_change_selection = ImageEditor::CHANGE_TO;
    preset1.height_number = 600;
    //preset1.keep_aspect_ratio = true;
    preset1.resampling_filter = cv::InterpolationFlags::INTER_CUBIC;
    //preset1.rotation_angle = 0;
    //preset1.format_change = false;
    //preset1.format_extension = 0;
    //preset1.format_subsampling = 1;
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
    preset2.description = "[Preset #" + QString::fromStdString(std::to_string(preset2.index + 1)) + "] (Default) Resize Image 200x200 and Rename Original.";
    preset2.width_change_selection = ImageEditor::CHANGE_TO;
    preset2.width_number = 200;
    preset2.height_change_selection = ImageEditor::CHANGE_TO;
    preset2.height_number = 200;
    preset2.keep_aspect_ratio = false;
    preset2.resampling_filter = cv::InterpolationFlags::INTER_CUBIC;
    //preset2.rotation_angle = 0;
    //preset2.format_change = false;
    //preset2.format_extension = 0;
    //preset2.format_subsampling = 1;
    //preset2.format_optimize = false;
    //preset2.format_progressive = false;
    //preset2.format_quality = 95;
    //preset2.format_compression = 0;
    //preset2.format_extra1 = -1;
    //preset2.format_extra2 = -1;
    preset2.save_file_policy_option = RENAME_ORG;
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
            preset.rotation_angle = settings.value("rotation_angle").toInt();
            preset.format_change = settings.value("format_change").toBool();
            preset.format_extension = settings.value("format_extension").toInt();
            preset.format_subsampling = settings.value("format_subsampling").toInt();
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

        // TEMP: load only defaults, keep presets as defualts between sesions.
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
    ui.comboBox_Preset_1->clear();
    ui.comboBox_Preset_2->clear();
    ui.comboBox_Preset_3->clear();
    for (int i = 0; i < preset_list.size(); i++) {
        ui.comboBox_Preset_1->insertItem( i,
            preset_list.at(i).description,
            QString::fromStdString("Preset" + std::to_string(i))
        );
        ui.comboBox_Preset_2->insertItem( i,
            preset_list.at(i).description,
            QString::fromStdString("Preset" + std::to_string(i))
        );
        ui.comboBox_Preset_3->insertItem( i,
            preset_list.at(i).description,
            QString::fromStdString("Preset" + std::to_string(i))
        );
    }
    ChangePreset(cspi);

    // Load selected preset data into ui.
    LoadPreset(preset_list.at(current_selected_preset));
}

/// <summary>
/// Start editing and saving images in file tree (in another thread).
/// </summary>
void BatchItImage::EditAndSave()
{
    DEBUG("Edit And Save...");

    // TODO: if current preset settings are not saved, ask to save them now before editing images
    // Save Current, Save New, Cancel... all preset ui elements would need to trigger a "changed and not saved" flag
    SavePreset();

    for (int i = 0; i < current_file_metadata_list.size(); i++) {
        DEBUG(current_file_metadata_list.at(i).to_string());

        // Create pointer to what will be the new edited image
        cv::Mat* img_p = new cv::Mat();

        // Setup the image editor with a file path and all the edits to be done.
        ImageEditor* new_ie = new ImageEditor(current_file_metadata_list.at(i).path, img_p);
        new_ie->width_modifier = preset_list.at(current_selected_preset).width_change_selection;
        new_ie->width = preset_list.at(current_selected_preset).width_number;
        new_ie->height_modifier = preset_list.at(current_selected_preset).height_change_selection;
        new_ie->height = preset_list.at(current_selected_preset).height_number;
        new_ie->keep_aspect_ratio = preset_list.at(current_selected_preset).keep_aspect_ratio;
        new_ie->interpolation = preset_list.at(current_selected_preset).resampling_filter;
        new_ie->rotate_degrees = preset_list.at(current_selected_preset).rotation_angle;

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

        if (save_option == RENAME_ORG or save_option == NEW_NAME) {
            std::string file_name_changes = preset_list.at(current_selected_preset).save_file_name_change;

            std::string change_data[4] = {
                org_file_path.stem().string(), // File Name
                std::to_string(image_index + 1), // Counter
                std::to_string(image_edited.cols), // Width
                std::to_string(image_edited.rows) // Height
            };
            file_name_changes = CreateNewFileName(file_name_changes, change_data);

            // TODO: if file already exists ask to overwrite?
            // TODO: how to handle permissions?
            //if (std::filesystem::exists(file_path_rename))

            if (save_option == RENAME_ORG) {
                std::filesystem::path org_file_path_rename = org_file_path.parent_path() / file_name_changes;
                std::filesystem::rename(org_file_path, org_file_path_rename);
                // TODO: If RENAME_ORG and an absolute or a relative path that isn't empty or just "\",
                // Inform user that they are renaming original but not saving the new file with the same name/path, is this what they intended to do?
                // Inform user before starting the edit process. (No need to rename org if not intending to overwrite it, or is there?)
                new_file_path = std::filesystem::path(new_root_save_path / org_file_path.stem());// .generic_string();
            }
            else if (save_option == NEW_NAME) {
                new_file_path = std::filesystem::path(new_root_save_path / file_name_changes);
                //std::filesystem::path new_file_path_rename = new_root_save_path / file_name_changes;
                //new_file_path = new_file_path_rename.generic_string();
            }
        }
        else { // save_option == OVERWRITE
            new_file_path = org_file_path.parent_path() / org_file_path.stem();
        }

        //DEBUG2("Save File Path: ", new_file_path.generic_string());

        // Add Extension and Get/Apply Format Specific Paramaters
        std::vector<int> params;
        DEBUG2("Change Format: ", preset_list.at(current_selected_preset).format_change);
        if (preset_list.at(current_selected_preset).format_change) {

            std::string extension = std::get<std::string>(
                image_formats[preset_list.at(current_selected_preset).format_extension].data
            );
            new_file_path.replace_extension(extension);
            
            // TODO: Format Specific Paramaters
            int subsampling = std::get<int>(
                format_subsamplings[preset_list.at(current_selected_preset).format_subsampling].data
            );
            DEBUG(subsampling);
            bool optimize = preset_list.at(current_selected_preset).format_optimize;
            bool progressive = preset_list.at(current_selected_preset).format_progressive;
            int quality = preset_list.at(current_selected_preset).format_quality;
            int compression = preset_list.at(current_selected_preset).format_compression;
            int extra1 = preset_list.at(current_selected_preset).format_extra1;
            int extra2 = preset_list.at(current_selected_preset).format_extra2;
            
            ie->GetFormatParams(&params, extension, subsampling, optimize, progressive, quality, compression, extra1, extra2);
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
            DEBUG(err.what());
            // TODO: log
        }
        if (image_saved) {
            DEBUG2("New edited image is succusfully saved at: ", new_file_path.generic_string());
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
/// <param name="change_data">--The file metadata to insert into new file name.</param>
/// <returns>--The new file name string.</returns>
std::string BatchItImage::CreateNewFileName(std::string file_name_changes, std::string change_data[4])
{
    std::string change_selections[4] = { ADD_FILE_NAME, ADD_COUNTER, ADD_WIDTH, ADD_HEIGHT };
    for (int i = 0; i < 4; i++) {
        int fn_index = file_name_changes.find(change_selections[i]);
        if (fn_index != std::string::npos) {
            file_name_changes.replace(fn_index, change_selections[i].length(), change_data[i]);
        }
    }

    // TODO: clean up any illegal characters that got through, like extra <>.

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
            if (search_subdirs)
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
            DEBUG("File Already In List");
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
                [=](QDialogButtonBox::StandardButton button) {
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
void BatchItImage::LoadFileIntoTree(int file_index)
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

    // Formated File Size:
    size_t file_size = t_stat.st_size;
    std::string file_size_str = BytesToFileSizeString(file_size);

    // Formated File Creation and Last Modified Times: https://en.cppreference.com/w/c/chrono
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
    
    //connect(file_selected_check_box, &QAbstractButton::toggled, this,
    //    [this, file_selected_check_box] { // Update event: file selected in file metadata list.
    //        DEBUG4("Row: ", ui.treeWidget_FileInfo->currentIndex().row(), " Checked: ", file_selected_check_box->isChecked());
    //        current_file_metadata_list.at(ui.treeWidget_FileInfo->currentIndex().row()).selected = file_selected_check_box->isChecked();
    //    });
    connect(file_selected_check_box, SIGNAL(toggled(bool)), this, SLOT(FileSelectionChange(bool)));

    ui.treeWidget_FileInfo->setItemWidget(new_item, FILE_SELECTED, file_selected_check_box);
    new_item->setData(FILE_SELECTED, Qt::ToolTipRole, QVariant(file.load_order));
    //new_item->setData(FILE_SELECTED, Qt::StatusTipRole, QVariant(file.load_order)); // TODO: Setup ui.statusbar https://doc.qt.io/qt-6/qt.html#ItemDataRole-enum
    new_item->setText(FILE_NAME, QString::fromStdString(file_path.filename().string()));
    new_item->setText(IMAGE_DIMENSIONS, QString::fromStdString(std::to_string(file.width) + " x " + std::to_string(file.height)));
    new_item->setTextAlignment(FILE_SIZE, Qt::AlignCenter);
    new_item->setText(FILE_SIZE, QString::fromStdString(file_size_str));
    new_item->setTextAlignment(FILE_SIZE, Qt::AlignRight);
    //new_item->setSizeHint(FILE_SIZE, QSize(10, 10));
    new_item->setText(DATE_CREATED, QString::fromStdString(date_created));
    new_item->setText(DATE_MODIFIED, QString::fromStdString(date_modified));
    new_item->setFont(FILE_SELECTED, *font_mono);
    new_item->setFont(FILE_NAME, *font_mono_bold);
    new_item->setFont(IMAGE_DIMENSIONS, *font_mono);
    new_item->setFont(FILE_SIZE, *font_mono);
    new_item->setFont(DATE_CREATED, *font_mono);
    new_item->setFont(DATE_MODIFIED, *font_mono);

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
    for (int i = 0; i < FILE_COLUMN_COUNT; i++) {
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
        current_file_sort_order = ASCENDING1;
    }
    else {
        current_file_sort_order = (current_file_sort_order < DESCENDING2) ? ++current_file_sort_order : ASCENDING1;
        qsort_indicator = (fmod(current_file_sort_order, 2) == 0) ? Qt::SortOrder::AscendingOrder : Qt::SortOrder::DescendingOrder;
    }
    
    // Sort list of file metadata based on column clicked.
    if (current_file_column_sorted == FILE_LOAD_ORDER and (current_file_sort_order == ASCENDING1 or current_file_sort_order == ASCENDING2)) {
        std::sort(begin(current_file_metadata_list), end(current_file_metadata_list), [](auto const& left, auto const& right) {
                return left.load_order < right.load_order;
            });
    }
    else if (current_file_column_sorted == FILE_LOAD_ORDER and (current_file_sort_order == DESCENDING1 or current_file_sort_order == DESCENDING2)) {
        std::sort(begin(current_file_metadata_list), end(current_file_metadata_list), [](auto const& left, auto const& right) {
                return left.load_order > right.load_order;
            });
    }
    else if (current_file_column_sorted == FILE_NAME and (current_file_sort_order == ASCENDING1 or current_file_sort_order == ASCENDING2)) {
        std::sort(begin(current_file_metadata_list), end(current_file_metadata_list), [](auto const& left, auto const& right) {
                return left.path < right.path;
            });
    }
    else if (current_file_column_sorted == FILE_NAME and (current_file_sort_order == DESCENDING1 or current_file_sort_order == DESCENDING2)) {
        std::sort(begin(current_file_metadata_list), end(current_file_metadata_list), [](auto const& left, auto const& right) {
                return left.path > right.path;
            });
    }
    else if (current_file_column_sorted == IMAGE_DIMENSIONS and (current_file_sort_order == ASCENDING1)) {
        std::sort(begin(current_file_metadata_list), end(current_file_metadata_list), [](auto const& left, auto const& right) {
                return left.width < right.width;
            });
    }
    else if (current_file_column_sorted == IMAGE_DIMENSIONS and (current_file_sort_order == DESCENDING1)) {
        std::sort(begin(current_file_metadata_list), end(current_file_metadata_list), [](auto const& left, auto const& right) {
                return left.width > right.width;
            });
    }
    else if (current_file_column_sorted == IMAGE_DIMENSIONS and (current_file_sort_order == ASCENDING2)) {
        std::sort(begin(current_file_metadata_list), end(current_file_metadata_list), [](auto const& left, auto const& right) {
                return left.height < right.height;
            });
    }
    else if (current_file_column_sorted == IMAGE_DIMENSIONS and (current_file_sort_order == DESCENDING2)) {
        std::sort(begin(current_file_metadata_list), end(current_file_metadata_list), [](auto const& left, auto const& right) {
                return left.height > right.height;
            });
    }
    else if (current_file_column_sorted == FILE_SIZE and (current_file_sort_order == ASCENDING1 or current_file_sort_order == ASCENDING2)) {
        std::sort(begin(current_file_metadata_list), end(current_file_metadata_list), [](auto const& left, auto const& right) {
                return left.size < right.size;
            });
    }
    else if (current_file_column_sorted == FILE_SIZE and (current_file_sort_order == DESCENDING1 or current_file_sort_order == DESCENDING2)) {
        std::sort(begin(current_file_metadata_list), end(current_file_metadata_list), [](auto const& left, auto const& right) {
                return left.size > right.size;
            });
    }
    else if (current_file_column_sorted == DATE_CREATED and (current_file_sort_order == ASCENDING1 or current_file_sort_order == ASCENDING2)) {
        std::sort(begin(current_file_metadata_list), end(current_file_metadata_list), [](auto const& left, auto const& right) {
                return left.date_created < right.date_created;
            });
    }
    else if (current_file_column_sorted == DATE_CREATED and (current_file_sort_order == DESCENDING1 or current_file_sort_order == DESCENDING2)) {
        std::sort(begin(current_file_metadata_list), end(current_file_metadata_list), [](auto const& left, auto const& right) {
                return left.date_created > right.date_created;
            });
    }
    else if (current_file_column_sorted == DATE_MODIFIED and (current_file_sort_order == ASCENDING1 or current_file_sort_order == ASCENDING2)) {
        std::sort(begin(current_file_metadata_list), end(current_file_metadata_list), [](auto const& left, auto const& right) {
                return left.date_modified < right.date_modified;
            });
    }
    else if (current_file_column_sorted == DATE_MODIFIED and (current_file_sort_order == DESCENDING1 or current_file_sort_order == DESCENDING2)) {
        std::sort(begin(current_file_metadata_list), end(current_file_metadata_list), [](auto const& left, auto const& right) {
                return left.date_modified > right.date_modified;
            });
    }

    // Clear file tree and add back sorted files
    ui.treeWidget_FileInfo->clear();
    for (int i = 0; i < current_file_metadata_list.size(); i++) {
        LoadFileIntoTree(i);
    }

    ui.treeWidget_FileInfo->header()->setSortIndicator(current_file_column_sorted, qsort_indicator);
    ui.treeWidget_FileInfo->header()->setSortIndicatorShown(true);
}

/// <summary>
/// Convert bytes into a formated file size string -> KB -> MB -> GB -> TB.
/// </summary>
/// <param name="bytes">--File size in bytes.</param>
/// <returns>A Formated String - Example: " 1.37 MB "</returns>
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
    if (existing_text.front().toLatin1() == QString::fromStdString("/") or
        existing_text.front().toLatin1() == QString::fromStdString("\\")) {
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
/// <param name="no_really_all">--If set will include cases where "to" is a substring of "from".</param>
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
