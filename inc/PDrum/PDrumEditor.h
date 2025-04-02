#ifndef P_DRUM_EDITOR_H
#define P_DRUM_EDITOR_H

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <Components/Membrane/VibratingMembrane.h>

#include "PDrum.h"

/**
 * @brief Editor class for the PDrum processor.
 */
class PDrumEditor final : public juce::AudioProcessorEditor, juce::Timer {
public:
    /**
     * @brief Constructor for the PDrumEditor.
     */
    explicit PDrumEditor(PDrum &);

    /**
     * @brief Destructor for the PDrumEditor.
     */
    ~PDrumEditor() override {
        midiKeyboardState.removeListener(&processor.getMidiMessageCollector());
    }

    /**
     * @brief Paint the editor's background.
     */
    void paint(juce::Graphics &) override;

    /**
     * @brief Resize the editor.
     */
    void resized() override;

private:
    /** Reference to the PDrum processor */
    PDrum &processor;

    /** 2D vibrating membrane simulation to represent the drum head */
    VibratingMembrane membrane;

    /** Knob to adjust the size of the membrane */
    KnobComponent membraneSizeKnob;

    /** Knob to adjust the tension of the membrane */
    KnobComponent membraneTensionKnob;

    /** Depth of the drum */
    KnobComponent depthKnob;

    /** MIDI keyboard state */
    juce::MidiKeyboardState midiKeyboardState;

    /** MIDI keyboard component for user interaction */
    juce::MidiKeyboardComponent midiKeyboardComponent{
        midiKeyboardState, juce::MidiKeyboardComponent::horizontalKeyboard
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PDrumEditor)

    /**
     * @brief Timer callback function to update the editor.
     */
    void timerCallback() override;
};

#endif // P_DRUM_EDITOR_H
