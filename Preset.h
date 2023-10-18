#pragma once

#ifndef   PRESET_H
#define   PRESET_H

#include <QColor>
#include <QString>
#include <string>

#include "ImageEditor.h"
#include "ImageSaver.h"

class Preset
{
private:
    int index; // Use for sorting?
    QString description;
    uint width_modifier;
    int width_number;
    uint height_modifier;
    int height_number;
    bool keep_aspect_ratio;
    uint resampling_filter;
    int rotation_degrees;
    bool increase_boundaries;
    bool flip_image;
    uint border_type;
    QColor background_color;
    uint blur_filter;
    bool blur_normalize;
    int blur_x;
    int blur_y;
    int blur_sx;
    int blur_sy;
    int blur_depth;
    bool watermark_added;
    QString watermark_path;
    uint watermark_location;
    int watermark_transparency;
    int watermark_offset_x;
    int watermark_offset_y;
    bool format_changed;
    uint format_extension;
    uint format_format_flag;
    bool format_optimize;
    bool format_progressive;
    int format_quality;
    int format_compression;
    int format_extra1;
    int format_extra2;
    uint save_file_procedure;
    std::string save_file_name_change;
    bool save_path_relative;
    QString save_file_path_change;

public:
    Preset() : // Initialize Defaults
        index(0),
        description(""),
        width_modifier(ImageEditor::SizeModifier::NO_CHANGE),
        width_number(0),
        height_modifier(ImageEditor::SizeModifier::NO_CHANGE),
        height_number(0),
        keep_aspect_ratio(true),
        resampling_filter(0),
        rotation_degrees(0),
        increase_boundaries(true),
        flip_image(false),
        border_type(cv::BORDER_CONSTANT),
        background_color(0, 0, 0, 255),
        blur_filter(ImageEditor::BlurFilter::NO_FILTER),
        blur_normalize(true),
        blur_x(0),
        blur_y(0),
        blur_sx(0),
        blur_sy(0),
        blur_depth(-1),
        watermark_added(false),
        watermark_path(""),
        watermark_location(ImageEditor::WatermarkLocation::BottomRight),
        watermark_transparency(100),
        watermark_offset_x(0),
        watermark_offset_y(0),
        format_changed(false),
        format_extension(ImageSaver::SupportedImageFormats::jpeg), // Below format defaults specific to Jpeg
        format_format_flag(1),
        format_optimize(false),
        format_progressive(false),
        format_quality(95),
        format_compression(0),
        format_extra1(-1),
        format_extra2(-1),
        save_file_procedure(ImageSaver::SaveOptionFlag::NEW_NAME),
        save_file_name_change(ImageSaver::MetadataIdentifiers.at(ImageSaver::MetadataFlags::FILE_NAME) + "__new"),
        save_path_relative(true),
        save_file_path_change("") {}

    int presetIndex();
    void setPresetIndex(int index);

    QString presetDescription();
    void setPresetDescription(QString text);

    uint widthModifierIndex();
    void setWidthModifierIndex(uint index);

    int widthNumber();
    void setWidthNumber(int value);

    uint heightModifierIndex();
    void setHeightModifierIndex(uint index);

    int heightNumber();
    void setHeightNumber(int value);

    bool keepAspectRatio();
    void setKeepAspectRatio(bool checked);

    uint resamplingFilterIndex();
    void setResamplingFilterIndex(uint index);

    int rotationDegrees();
    void setRotationDegrees(int value);

    bool increaseBoundaries();
    void setIncreaseBoundaries(bool checked);

    bool flipImage();
    void setFlipImage(bool checked);

    uint borderTypeIndex();
    void setBorderTypeIndex(uint index);

    QColor backgroundColor();
    void setBackgroundColor(QColor color);
    void setBackgroundColor(int red, int green, int blue, int alpha);

    uint blurFilterIndex();
    void setBlurFilterIndex(uint index);

    bool blurNormalize();
    void setBlurNormalize(bool checked);

    int blurX();
    void setBlurX(int value);

    int blurY();
    void setBlurY(int value);

    int blurSX();
    void setBlurSX(int value);

    int blurSY();
    void setBlurSY(int value);

    int blurDepth();
    void setBlurDepth(int value);

    bool watermarkAdded();
    void setWatermarkAdded(bool checked);

    QString watermarkPath();
    void setWatermarkPath(QString text_path);

    uint watermarkLocationIndex();
    void setWatermarkLocationIndex(uint index);

    int watermarkTransparency();
    void setWatermarkTransparency(int value);

    int watermarkOffsetX();
    void setWatermarkOffsetX(int value);

    int watermarkOffsetY();
    void setWatermarkOffsetY(int value);

    bool formatChanged();
    void setFormatChanged(bool checked);

    uint formatExtensionIndex();
    void setFormatExtensionIndex(uint index);

    uint formatFormatFlagIndex();
    void setFormatFormatFlagIndex(uint index);

    bool formatOptimize();
    void setFormatOptimize(bool checked);

    bool formatProgressive();
    void setFormatProgressive(bool checked);

    int formatQuality();
    void setFormatQuality(int value);

    int formatCompression();
    void setFormatCompression(int value);

    int formatExtra1();
    void setFormatExtra1(int value);

    int formatExtra2();
    void setFormatExtra2(int value);

    uint saveFileProcedureIndex();
    void setSaveFileProcedureIndex(uint index);

    std::string saveFileNameChange();
    void setSaveFileNameChange(std::string text_path);

    bool savePathRelative();
    void setSavePathRelative(bool checked);

    QString saveFilePathChange();
    void setSaveFilePathChange(QString text_path);

    std::string to_string() const;
};

#endif // PRESET_H