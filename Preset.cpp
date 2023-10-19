#include "Preset.h"

int Preset::presetIndex() { return index; }
void Preset::setPresetIndex(int index) { Preset::index = index; }

QString Preset::presetDescription() { return description; }
void Preset::setPresetDescription(QString text) { description = text; }

uint Preset::widthModifierIndex() { return width_modifier; }
void Preset::setWidthModifierIndex(uint index) { width_modifier = index; }

int Preset::widthNumber() { return width_number; }
void Preset::setWidthNumber(int value) { width_number = value; }

uint Preset::heightModifierIndex() { return height_modifier; }
void Preset::setHeightModifierIndex(uint index) { height_modifier = index; }

int Preset::heightNumber() { return height_number; }
void Preset::setHeightNumber(int value) { height_number = value; }

bool Preset::keepAspectRatio() { return keep_aspect_ratio; }
void Preset::setKeepAspectRatio(bool checked) { keep_aspect_ratio = checked; }

uint Preset::resamplingFilterIndex() { return resampling_filter; }
void Preset::setResamplingFilterIndex(uint index) { resampling_filter = index; }

int Preset::rotationDegrees() { return rotation_degrees; }
void Preset::setRotationDegrees(int value) { rotation_degrees = value; }

bool Preset::increaseBoundaries() { return increase_boundaries; }
void Preset::setIncreaseBoundaries(bool checked) { increase_boundaries = checked; }

bool Preset::flipImage() { return flip_image; }
void Preset::setFlipImage(bool checked) { flip_image = checked; }

uint Preset::borderTypeIndex() { return border_type; }
void Preset::setBorderTypeIndex(uint index) { border_type = index; }

QColor Preset::backgroundColor() { return background_color; }
void Preset::setBackgroundColor(QColor color) { background_color = color; }
void Preset::setBackgroundColor(int red, int green, int blue, int alpha) { background_color.setRgb(red, green, blue, alpha); }

uint Preset::blurFilterIndex() { return blur_filter; }
void Preset::setBlurFilterIndex(uint index) { blur_filter = index; }

bool Preset::blurNormalize() { return blur_normalize; }
void Preset::setBlurNormalize(bool checked) { blur_normalize = checked; }

int Preset::blurX() { return blur_x; }
void Preset::setBlurX(int value) { blur_x = value; }

int Preset::blurY() { return blur_y; }
void Preset::setBlurY(int value) { blur_y = value; }

int Preset::blurSX() { return blur_sx; }
void Preset::setBlurSX(int value) { blur_sx = value; }

int Preset::blurSY() { return blur_sy; }
void Preset::setBlurSY(int value) { blur_sy = value; }

int Preset::blurDepth() { return blur_depth; }
void Preset::setBlurDepth(int value) { blur_depth = value; }

bool Preset::watermarkAdded() { return watermark_added; }
void Preset::setWatermarkAdded(bool checked) { watermark_added = checked; }

QString Preset::watermarkPath() { return watermark_path; }
void Preset::setWatermarkPath(QString text_path) { watermark_path = text_path; }

uint Preset::watermarkLocationIndex() { return watermark_location; }
void Preset::setWatermarkLocationIndex(uint index) { watermark_location = index; }

int Preset::watermarkTransparency() { return watermark_transparency; }
void Preset::setWatermarkTransparency(int value) { watermark_transparency = value; }

int Preset::watermarkOffsetX() { return watermark_offset_x; }
void Preset::setWatermarkOffsetX(int value) { watermark_offset_x = value; }

int Preset::watermarkOffsetY() { return watermark_offset_y; }
void Preset::setWatermarkOffsetY(int value) { watermark_offset_y = value; }

bool Preset::formatChanged() { return format_changed; }
void Preset::setFormatChanged(bool checked) { format_changed = checked; }

uint Preset::formatExtensionIndex() { return format_extension; }
void Preset::setFormatExtensionIndex(uint index) { format_extension = index; }

uint Preset::formatFormatFlagIndex() { return format_format_flag; }
void Preset::setFormatFormatFlagIndex(uint index) { format_format_flag = index; }

bool Preset::formatOptimize() { return format_optimize; }
void Preset::setFormatOptimize(bool checked) { format_optimize = checked; }

bool Preset::formatProgressive() { return format_progressive; }
void Preset::setFormatProgressive(bool checked) { format_progressive = checked; }

int Preset::formatQuality() { return format_quality; }
void Preset::setFormatQuality(int value) { format_quality = value; }

int Preset::formatCompression() { return format_compression; }
void Preset::setFormatCompression(int value) { format_compression = value; }

int Preset::formatExtra1() { return format_extra1; }
void Preset::setFormatExtra1(int value) { format_extra1 = value; }

int Preset::formatExtra2() { return format_extra2; }
void Preset::setFormatExtra2(int value) { format_extra2 = value; }

uint Preset::saveFileProcedureIndex() { return save_file_procedure; }
void Preset::setSaveFileProcedureIndex(uint index) { save_file_procedure = index; }

std::string Preset::saveFileNameChange() { return save_file_name_change; }
void Preset::setSaveFileNameChange(std::string text_path) { save_file_name_change = text_path; }

bool Preset::savePathRelative() { return save_path_relative; }
void Preset::setSavePathRelative(bool checked) { save_path_relative = checked; }

QString Preset::saveFilePathChange() { return save_file_path_change; }
void Preset::setSaveFilePathChange(QString text_path) { save_file_path_change = text_path; }

std::string Preset::to_string() const {
    return
        "{\n  Preset Index:              " + std::to_string(index) +
        "\n  Preset Description:        " + description.toStdString() +
        "\n  Width Modifier Index:      " + std::to_string(width_modifier) +
        "\n  Width Number:              " + std::to_string(width_number) +
        "\n  Height Modifier Index:     " + std::to_string(height_modifier) +
        "\n  Height Number:             " + std::to_string(height_number) +
        "\n  Keep Aspect Ratio:         " + (keep_aspect_ratio ? "True" : "False") +
        "\n  Rotation:                  " + std::to_string(rotation_degrees) +
        "\n  Format Extension:          " + std::to_string(format_extension) +
        "\n  Save File Procedure Index: " + std::to_string(save_file_procedure) +
        "\n  Save File Name Change:     " + save_file_name_change +
        "\n  Relative Save Path:        " + (save_path_relative ? "True" : "False") +
        "\n  Save File Path:            " + save_file_path_change.toStdString() +
        "\n}";
}