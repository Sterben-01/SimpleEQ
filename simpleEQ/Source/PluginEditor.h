/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"




struct LookAndFeel : juce::LookAndFeel_V4
{
    void drawRotarySlider(juce::Graphics&, int x, int y, int width, int height,
        float sliderPosProportional, float rotaryStartAngle,
        float rotaryEndAngle, juce::Slider&) override;
};



//struct BackgroundImage : juce::Component
//{
//    BackgroundImage();
//    void paint(juce::Graphics& g) override;
//    juce::Image backgroundImage;
//};


struct RotarySliderWithLables : juce::Slider
{
    RotarySliderWithLables(juce::RangedAudioParameter& rap, const juce::String& unitSuffix) : 
        juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox), 
        param(&rap), 
        suffix(unitSuffix)
    {
        setLookAndFeel(&LNF);
    }


    ~RotarySliderWithLables()
    {
        setLookAndFeel(nullptr);
    }

    void paint(juce::Graphics& g) override;
    juce::Rectangle<int> getSliderBounds() const;
    int getTextHeight() const { return 14; }
    juce::String getDisplayString() const;


private:
    LookAndFeel LNF;
    juce::RangedAudioParameter* param;
    juce::String suffix;
};






struct ResopnceCurveComponent : juce::Component, juce::AudioProcessorParameter::Listener, juce::Timer
{
    ResopnceCurveComponent(SimpleEQAudioProcessor&);
    ~ResopnceCurveComponent();



    void parameterValueChanged(int parameterIndex, float newValue) override; // listener class 
    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override {}; // listener class

    void timerCallback() override; //timer class callback decide whether param changed -> need to be repainted
    void paint(juce::Graphics& g) override;

private:
    SimpleEQAudioProcessor& audioProcessor;
    juce::Atomic<bool> parametersChanged{ false };
    MonoChain monoChain;


};




















//==============================================================================
/**
*/
class SimpleEQAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    SimpleEQAudioProcessorEditor(SimpleEQAudioProcessor&);
    ~SimpleEQAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;




private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SimpleEQAudioProcessor& audioProcessor;

 
    //BackgroundImage backgroundImage;

    RotarySliderWithLables peakFreqSlider, peakGainSlider, peakQualitySlider, lowCutFreqSlider, highCutFreqSlider, lowCutSlopeSlider, highCutSlopeSlider;

    using APVTS = juce::AudioProcessorValueTreeState;
    using APVTS_Attachment = APVTS::SliderAttachment;

    APVTS_Attachment peakFreqSliderAttachment, peakGainSliderAttachment, peakQualitySliderAttachment, lowCutFreqSliderAttachment, highCutFreqSliderAttachment, lowCutSlopeSliderAttachment, highCutSlopeSliderAttachment;


    ResopnceCurveComponent responseCurveComponent;



    std::vector<juce::Component*> getComps();

    //juce::ImageComponent imageComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleEQAudioProcessorEditor)
};
