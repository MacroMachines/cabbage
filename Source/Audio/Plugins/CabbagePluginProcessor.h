/*
  Copyright (C) 2016 Rory Walsh

  Cabbage is free software; you can redistribute it
  and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  Cabbage is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU General Public
  License along with Csound; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
  02111-1307 USA
*/

#ifndef CABBAGEPLUGINPROCESSOR_H_INCLUDED
#define CABBAGEPLUGINPROCESSOR_H_INCLUDED

#include "CsoundPluginProcessor.h"
#include "../../Widgets/CabbageWidgetData.h"
#include "../../CabbageIds.h"
//#include "CabbageAudioParameter.h"
#include "../../Widgets/CabbageXYPad.h"

class CabbageAudioParameter;

class CabbagePluginProcessor
    : public CsoundPluginProcessor
{
public:

    class CabbageJavaClass  : public DynamicObject
    {

        CabbagePluginProcessor* owner;
    public:

        CabbageJavaClass (CabbagePluginProcessor* owner): owner (owner)
        {
            setMethod ("print", print);
        }

        static Identifier getClassName()   { return "Cabbage"; }

        static var print (const var::NativeFunctionArgs& args)
        {
            if (args.numArguments > 0)
                if (CabbageJavaClass* thisObject = dynamic_cast<CabbageJavaClass*> (args.thisObject.getObject()))
                    thisObject->owner->cabbageScriptGeneratedCode.add (args.arguments[0].toString());

            return var::undefined();
        }



        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageJavaClass)
    };

    struct PlantImportStruct
    {
        String nsp, name, csoundCode;
        StringArray cabbageCode;
    };


    CabbagePluginProcessor (File inputFile = File(), const int ins=2, const int outs=2);
    ~CabbagePluginProcessor();

    ValueTree cabbageWidgets;
    void createCsound(File inputFile, bool shouldCreateParameters = true);
    void getChannelDataFromCsound() override;
    void triggerCsoundEvents() override;
    void addImportFiles (StringArray& lineFromCsd);
    void parseCsdFile (StringArray& linesFromCsd);
    void setCabbageParameter(String channel, float value);
    void createParameters();
    void updateWidgets (String csdText);
    void handleXmlImport (XmlElement* xml, StringArray& linesFromCsd);
    void getMacros (StringArray& csdText);
    void generateCabbageCodeFromJS (PlantImportStruct& importData, String text);
    void insertUDOCode (PlantImportStruct importData, StringArray& linesFromCsd);
    void insertPlantCode (StringArray& linesFromCsd);
    bool isWidgetPlantParent (StringArray linesFromCsd, int lineNumber);
    bool shouldClosePlant (StringArray linesFromCsd, int lineNumber);
    void setPluginName (String name) {    pluginName = name;  }
    String getPluginName() { return pluginName;  }
    void expandMacroText (String &line, ValueTree wData);


    CabbageAudioParameter* getParameterForXYPad (String name);
    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    File getCsdFile()
    {
        return csdFile;
    }

    StringArray getCurrentCsdFileAsStringArray()
    {
        StringArray csdArray;
        csdArray.addLines (csdFile.loadFileAsString());
        return csdArray;
    }

    //===== XYPad methods =========
    void addXYAutomator (CabbageXYPad* xyPad, ValueTree wData);
    void enableXYAutomator (String name, bool enable, Line<float> dragLine);
    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void setParametersFromXml (XmlElement* e);
    XmlElement savePluginState (String tag, File xmlFile = File());
    void restorePluginState (XmlElement* xmlElement);
    //==============================================================================
    StringArray cabbageScriptGeneratedCode;
    Array<PlantImportStruct> plantStructs;
private:
    controlChannelInfo_s* csoundChanList;
    int samplingRate = 44100;
    int numberOfLinesInPlantCode = 0;
    String pluginName;
    File csdFile;
    int linesToSkip = 0;
    NamedValueSet macroText;
    var macroNames;
    var macroStrings;
    bool xyAutosCreated = false;
    OwnedArray<XYPadAutomator> xyAutomators;

};

class CabbageAudioParameter : public AudioParameterFloat
{
    
public:
    CabbageAudioParameter (CabbagePluginProcessor* owner, ValueTree wData, Csound& csound, String channel, String name, float minValue, float maxValue, float def, float incr, float skew)
    : AudioParameterFloat (name, channel, NormalisableRange<float> (minValue, maxValue, incr, skew), def),  currentValue (def), widgetName (name), channel (channel), owner(owner)
    {
        // widgetType = CabbageWidgetData::getStringProp (widgetData, CabbageIdentifierIds::type);
    }
    ~CabbageAudioParameter() {}
    
    float getValue() const override
    {
        return range.convertTo0to1 (currentValue);
    }
    
    void setValue (float newValue) override
    {
        //csound.SetChannel (channel.toUTF8(), range.convertFrom0to1 (newValue));
        currentValue = range.convertFrom0to1 (newValue);
        owner->setCabbageParameter(channel, currentValue);
        
    }
    
    const String getWidgetName() {   return widgetName;  }
    
    String channel;
    String widgetName;
    float currentValue;
    
    CabbagePluginProcessor* owner;
};



#endif  // CABBAGEPLUGINPROCESSOR_H_INCLUDED
