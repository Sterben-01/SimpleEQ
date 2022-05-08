/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

enum Slope {
    Slope_12,
    Slope_24,
    Slope_36,
    Slope_48

};

struct ChainSettings { //default values
    float peakFreq{ 0 }, peakGainInDecibels{ 0 }, peakQuality{ 0 };
    float lowCutFreq{ 0 }, highCutFreq{ 0 };
    //int lowCutSlope{ 0 }, highCutSlope{ 0 }; change to slope type

    Slope lowCutSlope{ Slope::Slope_12 }, highCutSlope{ Slope::Slope_12 };

};

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts);




//==============================================================================
/**
*/
class SimpleEQAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    SimpleEQAudioProcessor();
    ~SimpleEQAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout(); //核心

    juce::AudioProcessorValueTreeState apvts{*this, nullptr, "Parameters", createParameterLayout() }; //核心









private:

    using Filter = juce::dsp::IIR::Filter<float>;

    using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>; //effect chain. passing in context can let it passing it automatically

    using MonoChain = juce::dsp::ProcessorChain<CutFilter, Filter, CutFilter>;

    MonoChain leftChain, rightChain; //DSP only can process mono. So we need two.
    


    enum ChainPositions 
    {
        LowCut,
        Peak,
        HighCut
    };


    void updatePeakFilter(const ChainSettings& chainSettings);


    using Coefficients = Filter::CoefficientsPtr;
    static void updateCoefficients(Coefficients& old, const Coefficients& replacements);

    template<typename ChainType, typename CoefficientType>
    void updateCutFilter(ChainType& LowCut, const CoefficientType& cutCoefficients, const Slope& lowCutSlope) 
    {
        //auto cutCoefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(chainSettings.lowCutFreq,
        //    getSampleRate(),
        //    2 * (chainSettings.lowCutSlope + 1));

        //auto& leftLowCut = leftChain.get<ChainPositions::LowCut>();

        LowCut.setBypassed<0>(true);
        LowCut.setBypassed<1>(true);
        LowCut.setBypassed<2>(true);
        LowCut.setBypassed<3>(true);


        //switch (chainSettings.lowCutSlope)
        switch(lowCutSlope)
        {
        case Slope_12:
        {
            *LowCut.get<0>().coefficients = *cutCoefficients[0];
            LowCut.setBypassed<0>(false);
            break;
        }
        case Slope_24:
        {
            *LowCut.get<0>().coefficients = *cutCoefficients[0];
            LowCut.setBypassed<0>(false);
            *LowCut.get<1>().coefficients = *cutCoefficients[1];
            LowCut.setBypassed<1>(false);
            break;

        }
        case Slope_36:
        {
            *LowCut.get<0>().coefficients = *cutCoefficients[0];
            LowCut.setBypassed<0>(false);
            *LowCut.get<1>().coefficients = *cutCoefficients[1];
            LowCut.setBypassed<1>(false);
            *LowCut.get<2>().coefficients = *cutCoefficients[2];
            LowCut.setBypassed<2>(false);
            break;
        }

        case Slope_48:
        {
            *LowCut.get<0>().coefficients = *cutCoefficients[0];
            LowCut.setBypassed<0>(false);
            *LowCut.get<1>().coefficients = *cutCoefficients[1];
            LowCut.setBypassed<1>(false);
            *LowCut.get<2>().coefficients = *cutCoefficients[2];
            LowCut.setBypassed<2>(false);
            *LowCut.get<3>().coefficients = *cutCoefficients[3];
            LowCut.setBypassed<3>(false);
            break;
        }
        }

    }



    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleEQAudioProcessor)
};
