#pragma once
#ifndef ENHANCEDPROGRESSBAR_H
#define ENHANCEDPROGRESSBAR_H

#include <QProgressBar>

class EnhancedProgressBar : public QProgressBar
{
    Q_OBJECT

public:
    /// <summary>
    /// Sub class of QProgressBar that once configured, will automatically calculate ticks in every Slot updateProgressBar() called with
    /// an option to use a multiplier (to skip/speed up progress) and an option to add a callback function once maximum progress reached.
    /// </summary>
    /// <param name="parent">--Parent QWidget</param>
    explicit EnhancedProgressBar(QWidget* parent = 0);
    /// <summary>
    /// Sub class of QProgressBar that once configured, will automatically calculate ticks in every Slot updateProgressBar() called with
    /// an option to use a multiplier (to skip/speed up progress) and an option to add a callback function once all progress completed.
    /// </summary>
    /// <param name="orientation">--Orientation::Horizontal or Orientation::Vertical.</param>
    /// <param name="parent">--Parent QWidget</param>
    explicit EnhancedProgressBar(Qt::Orientation orientation, QWidget* parent = 0);
    /// <summary>
    /// Show (visible) and configure this enhanced progress bar, setting the amount of ticks to maximum progress. Note in order to update progress,
    /// every use of this enhanced progress bar needs to be configured first, which resets every time maximum progress reached.
    /// </summary>
    /// <param name="max_tick_count">--The amount of ticks to maximum progress. Must be called before starting update progress.</param>
    /// <param name="multiplier">--Set how many times updateProgressBar() must be called to complete one tick. [Default: 1.0]</param>
    /// <param name="finished_callback">--A function to call when maximum progress/ticks reached; example: std::bind(class::function, this). [Default: Null]</param>
    void configure(float max_ticks, float multiplier = 1.0f, std::function<void()> finished_callback = NULL);
    /// <summary>
    /// Return enhanced progress bar's minimum float value.
    /// </summary>
    /// <returns></returns>
    float enhancedMinimum();
    /// <summary>
    /// Return enhanced progress bar's maximum float value.
    /// </summary>
    /// <returns></returns>
    float enhancedMaximum();

public slots:
    /// <summary>
    /// Set the enhanced progress bar's minimum float value.
    /// </summary>
    /// <param name="minimum">--Float</param>
    void setMinimum(float minimum);
    /// <summary>
    /// Set the enhanced progress bar's maximum float value.
    /// </summary>
    /// <param name="maximum">--Float</param>
    void setMaximum(float maximum);
    /// <summary>
    /// Set the enhanced progress bar's minimum and maximum float values.
    /// </summary>
    /// <param name="minimum">--Float</param>
    /// <param name="maximum">--Float</param>
    void setRange(float min, float max);
    /// <summary>
    /// Update enhanced progress bar, hiding it when maximum progress reached. Note configure() must have been previously called before updating can proceed.
    /// </summary>
    /// <param name="multiplier">--Updates to make per one tick, effectively updating "X" amount of times. [Default: 1.0]</param>
    void updateProgressBar(float multiplier = 1.0f);

private:
    float minimum_f = 0.0f;
    float maximum_f = 100.0f;
    float current_progress = 0.0f;
    float progress_interval = 0.0f;
    const float margin_of_error = 0.05f;
    std::function<void()> finished_callback_function = NULL;
};

#endif // ENHANCEDPROGRESSBAR_H