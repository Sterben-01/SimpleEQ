/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

void LookAndFeel::drawRotarySlider(juce::Graphics& g,
    int x,
    int y,
    int width,
    int height,
    float sliderPosProportional,
    float rotaryStartAnegle,
    float rotaryEndAngle,
    juce::Slider& slider)


{
    using namespace juce;

    auto bounds = Rectangle<float>(x, y, width, height);
    //auto ccc = Colour(97u, 18u, 167u);
    g.setColour(Colour::fromFloatRGBA(1.f, 1.f, 1.f, 0.3f)); //circle color
    g.fillEllipse(bounds);
    g.setColour(Colour::fromFloatRGBA(1.f, 0.53f, 1.f, 0.7f)); //circle edge color
    g.drawEllipse(bounds, 3.f);

    if (auto* rswl = dynamic_cast<RotarySliderWithLables*>(&slider)) 
    {
        auto center = bounds.getCentre();
        Path p;
        Rectangle<float> r;
        r.setLeft(center.getX() - 2);
        r.setRight(center.getX() + 2);
        r.setTop(bounds.getY());
        r.setBottom(center.getY() - rswl->getTextHeight()*1.5);

        p.addRoundedRectangle(r, 2.f);
        jassert(rotaryStartAnegle < rotaryEndAngle);

        auto sliderAngRad = jmap(sliderPosProportional, 0.f, 1.f, rotaryStartAnegle, rotaryEndAngle);

        p.applyTransform(AffineTransform().rotated(sliderAngRad, center.getX(), center.getY()));

        g.fillPath(p);

        g.setFont(rswl->getTextHeight());
        auto text = rswl->getDisplayString();
        auto strWidth = g.getCurrentFont().getStringWidth(text);

        r.setSize(strWidth + 40, rswl->getTextBoxHeight() + 2);

        r.setCentre(bounds.getCentre());

        Font samplefont;
        samplefont.setTypefaceName("Meiryo UI");

        g.setColour(Colours::black); //number color
        //g.drawRect(r);
        g.setFont(samplefont);
        g.setFont(22.0f); //number font
        g.drawFittedText(text, r.toNearestInt(), juce::Justification::centred, 1);

    }



    //auto center = bounds.getCentre();
    //Path p;
    //Rectangle<float> r;
    //r.setLeft(center.getX() - 2);
    //r.setRight(center.getX() + 2);
    //r.setTop(bounds.getY());
    //r.setBottom(center.getY());

    //p.addRectangle(r);

    //jassert(rotaryStartAnegle < rotaryEndAngle);

    //auto sliderAngRad = jmap(sliderPosProportional, 0.f, 1.f, rotaryStartAnegle, rotaryEndAngle);

    //p.applyTransform(AffineTransform().rotated(sliderAngRad, center.getX(), center.getY()));

    //g.fillPath(p);

}





void RotarySliderWithLables::paint(juce::Graphics& g)
{
    using namespace juce;

    auto startAngle = degreesToRadians(180.f + 45.f);
    auto endEngle = degreesToRadians(180.f - 45.f) + MathConstants<float>::twoPi;

    auto range = getRange();

    auto sliderBounds = getSliderBounds();

    //g.setColour(Colours::red); //test bound region
    //g.drawRect(getLocalBounds());//test bound region
    //g.setColour(Colours::green);//test bound region
    //g.drawRect(sliderBounds);//test bound region

    


    getLookAndFeel().drawRotarySlider(g, 
                                        sliderBounds.getX(), 
                                        sliderBounds.getY(),
                                        sliderBounds.getWidth(),
                                        sliderBounds.getHeight(), 
                                        jmap(getValue(), range.getStart(), range.getEnd(), 0.0, 1.0),
                                        startAngle, 
                                        endEngle, 
                                        *this);

    auto center = sliderBounds.toFloat().getCentre();
    auto radius = sliderBounds.getWidth() * 0.5f;
    //g.setColour(Colour(0u, 100u, 100u)); //knob label color
    g.setFont(22.0f); //knob label size
    auto numChoices = labels.size();

    for (int i = 0; i < numChoices; i++)
    {
        auto pos = labels[i].pos;
        jassert(0.f <= pos);
        jassert(pos <= 1.f);

        auto ang = jmap(pos, 0.f, 1.f, startAngle, endEngle);

        auto  c = center.getPointOnCircumference(radius + getTextHeight() * 0.5f + 1, ang);

        Rectangle<float> r;
        auto str = labels[i].label;
        r.setSize(g.getCurrentFont().getStringWidth(str) +5, getTextHeight() +5); //area size
        r.setCentre(c);
        r.setY(r.getY() + getTextHeight()+10); //distance to knob
        //g.drawRect(r); 
        g.setColour(Colour::fromFloatRGBA(1.f, 1.f, 1.f, 0.5f)); //circle color
        g.fillRoundedRectangle(r,4.f);
        g.setColour(Colour(0u, 100u, 100u)); //knob label color
        g.drawFittedText(str, r.toNearestInt(), juce::Justification::centred, 1);
    }


}

juce::Rectangle<int>RotarySliderWithLables::getSliderBounds() const
{

    auto bounds = getLocalBounds();
    auto size = juce::jmin(bounds.getWidth(), bounds.getHeight());

    size -= getTextHeight() * 2;
    juce::Rectangle<int>r;
    r.setSize(size, size);
    r.setCentre(bounds.getCentreX(), 0);
    r.setY(5); //distance between knobs and edge.1


    return r;
}


juce::String RotarySliderWithLables::getDisplayString() const
{
    if (auto* choiceParam = dynamic_cast<juce::AudioParameterChoice*>(param))
    {
        return choiceParam->getCurrentChoiceName();
    }
    juce::String str;

    bool addK = false;
    if (auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param))
    {
        float val = getValue();
        if (val > 999.f)
        {
            val /= 1000.f;
            addK = true;
        }

        str = juce::String(val, (addK ? 2 : 0));


    }   
    else
    {
        jassertfalse;
    }

    if (suffix.isNotEmpty())
    {
        str << " ";
        if (addK)
        {
            str << "k";
        }
        str << suffix;
    }
    return str;



}

ResopnceCurveComponent::ResopnceCurveComponent(SimpleEQAudioProcessor& p) : audioProcessor(p)
{
    const auto& params = audioProcessor.getParameters();
    for (auto param : params) {
        param->addListener(this);
    }
    updateChain();
    startTimerHz(144); // timer callback initializer
}





ResopnceCurveComponent::~ResopnceCurveComponent()
{
    const auto& params = audioProcessor.getParameters();
    for (auto param : params) {
        param->removeListener(this);
    }
}




void ResopnceCurveComponent::parameterValueChanged(int parameterIndex, float newValue) {
    parametersChanged.set(true);
}



void ResopnceCurveComponent::timerCallback() {
    if (parametersChanged.compareAndSetBool(false, true)) {
        //waiting for update monochain and signal a repaint
        //auto chainSettings = getChainSettings(audioProcessor.apvts);
        //auto peakCoefficients = makePeakFilter(chainSettings, audioProcessor.getSampleRate());
        //updateCoefficients(monoChain.get<ChainPositions::Peak>().coefficients, peakCoefficients);


        //auto lowCutCoefficients = makeLowCutFilter(chainSettings, audioProcessor.getSampleRate());
        //auto highCutCoefficients = makeHighCutFilter(chainSettings, audioProcessor.getSampleRate());

        //updateCutFilter(monoChain.get<ChainPositions::LowCut>(), lowCutCoefficients, chainSettings.lowCutSlope);

        //updateCutFilter(monoChain.get<ChainPositions::HighCut>(), highCutCoefficients, chainSettings.highCutSlope);
        updateChain();
        repaint();
    }
}


void ResopnceCurveComponent::updateChain()
{
    auto chainSettings = getChainSettings(audioProcessor.apvts);
    auto peakCoefficients = makePeakFilter(chainSettings, audioProcessor.getSampleRate());
    updateCoefficients(monoChain.get<ChainPositions::Peak>().coefficients, peakCoefficients);


    auto lowCutCoefficients = makeLowCutFilter(chainSettings, audioProcessor.getSampleRate());
    auto highCutCoefficients = makeHighCutFilter(chainSettings, audioProcessor.getSampleRate());

    updateCutFilter(monoChain.get<ChainPositions::LowCut>(), lowCutCoefficients, chainSettings.lowCutSlope);

    updateCutFilter(monoChain.get<ChainPositions::HighCut>(), highCutCoefficients, chainSettings.highCutSlope);
}




void ResopnceCurveComponent::paint(juce::Graphics& g)
{
    using namespace juce;
    //g.fillAll(Colours::green); what the fuck is this shit?
    // 
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    //g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    //g.setColour (juce::Colours::white);
    //g.setFont (15.0f);
    //g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centredTop, 1);
    g.drawImage(background, getLocalBounds().toFloat(), true);
    //auto responseArea = getLocalBounds();
    //auto responseArea = getRenderArea();
    auto responseArea = getAnalysisArea();
    auto _width = responseArea.getWidth();

    auto& lowcut = monoChain.get<ChainPositions::LowCut>();
    auto& highcut = monoChain.get<ChainPositions::HighCut>();
    auto& peak = monoChain.get<ChainPositions::Peak>();
    auto sampleRate = audioProcessor.getSampleRate();

    std::vector<double> mags;
    mags.resize(_width);
    for (int i = 0; i < _width; i++) {
        double mag = 1.f;
        auto freq = mapToLog10(double(i) / double(_width), 20.0, 20000.0);

        if (!monoChain.isBypassed <ChainPositions::Peak>()) {
            mag *= peak.coefficients->getMagnitudeForFrequency(freq, sampleRate);
        }


        if (!lowcut.isBypassed<0>()) {
            mag *= lowcut.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        }
        if (!lowcut.isBypassed<1>()) {
            mag *= lowcut.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        }
        if (!lowcut.isBypassed<2>()) {
            mag *= lowcut.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        }
        if (!lowcut.isBypassed<3>()) {
            mag *= lowcut.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        }


        if (!highcut.isBypassed<0>()) {
            mag *= highcut.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        }
        if (!highcut.isBypassed<1>()) {
            mag *= highcut.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        }
        if (!highcut.isBypassed<2>()) {
            mag *= highcut.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        }
        if (!highcut.isBypassed<3>()) {
            mag *= highcut.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        }


        mags[i] = Decibels::gainToDecibels(mag);

    }
    Path responseCurve;

    const double outputMin = responseArea.getBottom();
    const double outputMax = responseArea.getY();
    auto map = [outputMin, outputMax](double input) {return jmap(input, -24.0, 24.0, outputMin, outputMax); };
    responseCurve.startNewSubPath(responseArea.getX(), map(mags.front()));

    for (int i = 0; i < mags.size(); i++) {

        responseCurve.lineTo(responseArea.getX() + i, map(mags[i]));

    }

    g.setColour(Colours::orange);
    g.drawRoundedRectangle(getRenderArea().toFloat(), 4.f, 1.f);

    g.setColour(Colours::white);
    g.strokePath(responseCurve, PathStrokeType(2.f));

    

}


void ResopnceCurveComponent::resized()
{
    using namespace juce;
    background = Image(Image::PixelFormat::ARGB, getWidth(), getHeight(), true); //set here to ARGB to let it transparent

    Graphics g(background);

    Array<float> freqs
    {
        20,/*30,40,*/50,100,
        200,/*300,400,*/500,1000,
        2000,/*3000,4000,*/5000,10000,
        20000
    };


    auto renderArea = getAnalysisArea();
    auto left = renderArea.getX();
    auto right = renderArea.getRight();
    auto top = renderArea.getY();
    auto bottom = renderArea.getBottom();
    auto width = renderArea.getWidth();

    Array<float> renderAreaArray;

    for (auto f : freqs)
    {
        auto normX = mapFromLog10(f, 20.f, 20000.f);

        //g.drawVerticalLine(getWidth() * normX, 0.f, getHeight());
        renderAreaArray.add(left + width * normX);

    }

    g.setColour(Colours::dimgrey);

    for (auto item : renderAreaArray)
    {
        g.drawVerticalLine(item, top, bottom);
    }


    Array<float> gain
    {
        -24, -12, 0, 12, 24

    };

    for (auto gDB : gain)
    {
        auto y = jmap(gDB, -24.f, 24.f, float(bottom), float(top));

        g.setColour(gDB == 0.f ? Colour(0u, 172u, 1u) : Colours::darkgrey);

        g.drawHorizontalLine(y, left, right);
    }

    g.drawRect(getAnalysisArea());

    g.setColour(Colours::lightblue);
    const int chartlabelFontsize = 18.f;
    g.setFont(chartlabelFontsize);

    for (int i = 0; i < freqs.size(); i++)  //draw freq chart label.
    {
        auto f = freqs[i];
        auto x = renderAreaArray[i];

        bool addK = false;
        String str;

        if (f > 2000.f) { //let 2k 5k 10k labels change to black color to be more clear.
            g.setColour(Colours::black);
        }

        if (f > 999.f) 
        {
            addK = true;
            f /= 1000.f;

        }

        str << f;


        if (addK) 
        {
            str << "k";
            
        }
        str << "Hz";

        auto textWidth = g.getCurrentFont().getStringWidth(str);
        Rectangle<int> r;
        r.setSize(textWidth, chartlabelFontsize);
        r.setCentre(x, 0);
        r.setY(1);
        g.drawFittedText(str, r, juce::Justification::centred, 1);
    }

    for (auto gDB : gain) //draw gain chart label.
    {
        auto y = jmap(gDB, -24.f, 24.f, float(bottom), float(top));

        String str;

        if (gDB > 0) 
        {
            str << "+";
        }
        str << gDB;
        auto textWidth = g.getCurrentFont().getStringWidth(str);
        Rectangle<int> r;
        r.setSize(textWidth, chartlabelFontsize);
        r.setX(getWidth() - textWidth);
        r.setCentre(r.getCentreX(), y);
        g.setColour(gDB == 0.f ? Colour(0u, 172u, 1u) : Colours::black);
        g.drawFittedText(str, r, juce::Justification::centred, 1); //draw gain (right side)




        str.clear();
        str << (gDB - 24.f);
        r.setX(1);
        textWidth = g.getCurrentFont().getStringWidth(str);
        r.setSize(textWidth, chartlabelFontsize);
        g.setColour(gDB == 24.f ? Colour(0u, 172u, 1u) : Colours::white);
        g.drawFittedText(str, r, juce::Justification::centred, 1); //draw dBFS (left side)



    }

}

juce::Rectangle<int> ResopnceCurveComponent::getRenderArea()
{
    auto bounds = getLocalBounds();
    /*bounds.reduce(JUCE_LIVE_CONSTANT(5), 
        JUCE_LIVE_CONSTANT(5));*/
    //bounds.reduce(25, 50);

    bounds.removeFromTop(20); //this part control render area residule space. indicate to the space for chart label.
    bounds.removeFromBottom(2);
    bounds.removeFromLeft(35);
    bounds.removeFromRight(35);

    return bounds;

}

juce::Rectangle<int> ResopnceCurveComponent::getAnalysisArea()
{
    auto bounds = getRenderArea();
    bounds.removeFromTop(5);
    bounds.removeFromBottom(5);
    return bounds;

}



//==============================================================================
SimpleEQAudioProcessorEditor::SimpleEQAudioProcessorEditor (SimpleEQAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), 
    peakFreqSlider(*audioProcessor.apvts.getParameter("Peak Freq"), "Hz"),
    peakGainSlider(*audioProcessor.apvts.getParameter("Peak Gain"), "dB"),
    peakQualitySlider(*audioProcessor.apvts.getParameter("Peak Quality"), ""),
    lowCutFreqSlider(*audioProcessor.apvts.getParameter("LowCut Freq"), "Hz"),
    lowCutSlopeSlider(*audioProcessor.apvts.getParameter("LowCut Slope"), "dB/Oct"),
    highCutFreqSlider(*audioProcessor.apvts.getParameter("HighCut Freq"), "Hz"),
    highCutSlopeSlider(*audioProcessor.apvts.getParameter("HighCut Slope"), "dB/Oct"),


    responseCurveComponent(audioProcessor),
    peakFreqSliderAttachment(audioProcessor.apvts, "Peak Freq", peakFreqSlider),
    peakGainSliderAttachment(audioProcessor.apvts, "Peak Gain", peakGainSlider),
    peakQualitySliderAttachment(audioProcessor.apvts, "Peak Quality", peakQualitySlider),
    lowCutFreqSliderAttachment(audioProcessor.apvts, "LowCut Freq", lowCutFreqSlider),
    lowCutSlopeSliderAttachment(audioProcessor.apvts, "LowCut Slope", lowCutSlopeSlider),
    highCutFreqSliderAttachment(audioProcessor.apvts, "HighCut Freq", highCutFreqSlider),
    highCutSlopeSliderAttachment(audioProcessor.apvts, "HighCut Slope", highCutSlopeSlider)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    peakFreqSlider.labels.add({ 0.f, "20 Hz" });
    peakFreqSlider.labels.add({ 1.f, "20 kHz" });

    peakGainSlider.labels.add({ 0.f, "-24 dB" });
    peakGainSlider.labels.add({ 1.f, "24 dB" });

    peakQualitySlider.labels.add({ 0.f, "0.1" });
    peakQualitySlider.labels.add({ 1.f, "10.0" });

    lowCutFreqSlider.labels.add({ 0.f, "20 Hz" });
    lowCutFreqSlider.labels.add({ 1.f, "20 kHz" });

    highCutFreqSlider.labels.add({ 0.f, "20 Hz" });
    highCutFreqSlider.labels.add({ 1.f, "20 kHz" });

    lowCutSlopeSlider.labels.add({ 0.f, "12" });
    lowCutSlopeSlider.labels.add({ 1.f, "48" });

    highCutSlopeSlider.labels.add({ 0.f, "12" });
    highCutSlopeSlider.labels.add({ 1.f, "48" });

    for (auto comp : getComps())
    {
        addAndMakeVisible(*comp);
    }

    //auto testimg = juce::ImageCache::getFromMemory(BinaryData::_74FEACB9673C6E78044BFF6B863A94BBmin_jpg, BinaryData::_74FEACB9673C6E78044BFF6B863A94BBmin_jpgSize);

    //if (!testimg.isNull())
    //{
    //    imageComponent.setImage(testimg, juce::RectanglePlacement::stretchToFit);
    //}
    //else 
    //{
    //    jassert(!testimg.isNull());
    //}
    
    //juce::Component::addAndMakeVisible(imageComponent);

    setSize (1000, 800);
}








std::vector<juce::Component*> SimpleEQAudioProcessorEditor::getComps()
{

    return{ &peakFreqSlider, &peakGainSlider, &peakQualitySlider, &lowCutFreqSlider, &highCutFreqSlider, &lowCutSlopeSlider, &highCutSlopeSlider, &responseCurveComponent };

    //return std::vector<juce::Component*>{ &peakFreqSlider, &peakGainSlider, &peakQualitySlider, &lowCutFreqSlider, &highCutFreqSlider };

}

SimpleEQAudioProcessorEditor::~SimpleEQAudioProcessorEditor()
{

}

//==============================================================================
void SimpleEQAudioProcessorEditor::paint (juce::Graphics& g)
{
    using namespace juce;
    g.fillAll(Colours::mediumslateblue); //color for main bkg
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    //g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    //g.setColour (juce::Colours::white);
    //g.setFont (15.0f);
    //g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centredTop, 1);
    //imageComponent.setBoundsRelative(0.0f, 0.0f, 1.0f, 1.0f);
    auto imagea = juce::ImageCache::getFromMemory(BinaryData::_74FEACB9673C6E78044BFF6B863A94BBmin_jpg, BinaryData::_74FEACB9673C6E78044BFF6B863A94BBmin_jpgSize);
    if (imagea.isNull())
    {
        jassert(!imagea.isNull());
    }
    else 
    {
        g.drawImage(imagea, getLocalBounds().toFloat());
    }
}

void SimpleEQAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    //auto backgroundimagearea = bounds.removeFromTop(bounds.getHeight() * 0.15);

    //backgroundImage.setBounds(backgroundimagearea);

    auto responseArea = bounds.removeFromTop(bounds.getHeight() * 0.33);

    responseCurveComponent.setBounds(responseArea);


    auto LowCutArea = bounds.removeFromLeft(bounds.getWidth() * 0.33);
    auto HighCutArea = bounds.removeFromRight(bounds.getWidth() * 0.5);

    lowCutFreqSlider.setBounds(LowCutArea.removeFromTop(LowCutArea.getHeight() * 0.5));
    lowCutSlopeSlider.setBounds(LowCutArea);

    highCutFreqSlider.setBounds(HighCutArea.removeFromTop(HighCutArea.getHeight() * 0.5));
    highCutSlopeSlider.setBounds(HighCutArea);

    peakFreqSlider.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.33));
    peakGainSlider.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.5));
    peakQualitySlider.setBounds(bounds);


    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}


//BackgroundImage::BackgroundImage()
//{
//    backgroundImage = juce::ImageCache::getFromMemory(BinaryData::_74FEACB9673C6E78044BFF6B863A94BBmin_jpg, BinaryData::_74FEACB9673C6E78044BFF6B863A94BBmin_jpgSize);
//    if (backgroundImage.isNull())
//    {
//        jassert(!backgroundImage.isNull());
//    }
//}
//
//void BackgroundImage::paint(juce::Graphics& g)
//{
//    using namespace juce;
//    g.fillAll(Colours::green);
//    g.drawText("Hello World!", getLocalBounds(), Justification::centred, true);
//    g.drawImage(backgroundImage, getLocalBounds().toFloat());
//}