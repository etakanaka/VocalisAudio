/*
 * Vocalis Audio — Vocalis Voice
 *
 * Created and maintained by Enos Takanaka
 * Copyright (C) 2026 Enos Takanaka
 *
 * Developed with AI assistance using OpenAI Codex.
 */
#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
// A host owns an active JUCE editor and must notify its processor before deletion.
// Merely deleting the pointer leaves getActiveEditor() dangling in JUCE 9.
struct EditorDeleter
{
    juce::AudioProcessor *processor;
    void operator()(juce::AudioProcessorEditor *editor) const
    {
        if (editor)
        {
            processor->editorBeingDeleted(editor);
            delete editor;
        }
    }
};
using EditorOwner = std::unique_ptr<juce::AudioProcessorEditor, EditorDeleter>;
inline EditorOwner ownEditor(juce::AudioProcessor &processor)
{
    return EditorOwner(processor.createEditorAndMakeActive(), EditorDeleter{&processor});
}
