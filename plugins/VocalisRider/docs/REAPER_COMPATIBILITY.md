# REAPER compatibility — final audit

Date: 2026-10-16. Supported host: **REAPER for Linux only**. Supported plugin: **native Linux x86_64 VST3 only**.

## Evidence and status

| Area | Classification | Evidence / limit |
|---|---|---|
| REAPER installation | VERIFIED PASS | `/home/enos/opt/REAPER/reaper`, executable ELF64; adjacent release notes identify 7.82; linked dependencies resolve. Running version not queried. |
| Plugin installation/identity | VERIFIED PASS | `~/.vst3/Vocalis Rider Live.vst3`, native Release binary identical to tested output; original CIDs/parameters/state format preserved. |
| Native module load/audio/buses/READ | VERIFIED PASS | Independent VST3 smoke host, including 20 complete load/unload cycles; this host is not REAPER. |
| Discovery and embedded GUI | UNVERIFIED | Desktop display `:0` is inaccessible (`xwininfo` fails). No REAPER session was opened. |
| Audio / sidechain routing in REAPER | UNVERIFIED | Processor and module buses/processing pass; REAPER pin routing not executed. |
| READ playback | UNVERIFIED in REAPER | Actual VST3 input parameter changes apply +3 dB in the module test. Host envelope playback still needs verification. |
| WRITE recording | UNVERIFIED in REAPER | Supported JUCE/VST3 edit API implemented and message-thread notification/reentrancy tests pass. No recorded REAPER envelope is claimed. |
| Project save/reopen, old projects | UNVERIFIED in REAPER | Pre-debugging plugin state fixtures restore all values and IDs match; this does not prove REAPER project reopening. |
| Offline rendering / multiple host instances | UNVERIFIED in REAPER | Processor-instance independence and concurrent stress pass; actual host rendering and instance embedding not run. |
| CSV importer | UNVERIFIED | Script retained; not executed in REAPER, no Lua interpreter on PATH. |

No user REAPER projects or configuration were modified. The original GUI/baseline plugin is recoverable from the checkpoint described in FINAL_DEBUG_REPORT.md.

## Automation implementation

OFF rides internally and sends no intentional moving Rider edits. WRITE opens a Rider gesture and publishes calculated gain from a processor-owned 50 Hz message timer. READ uses the externally supplied Rider parameter and smoothing, without automatic gain override. Host notifications are not made in the audio callback. Timer now rechecks mode after begin-gesture callbacks, preventing the reproduced reentrant READ overwrite. Buttons now emit balanced user gestures.

Local JUCE VST3 wrapper `beginGesture`, `paramChanged`, `endGesture` invokes `beginEdit`, `setParamNormalized`, `performEdit`, `endEdit` on the message thread. A host must still choose to record these edits. Neither visible motion nor processor parameter publication proves recording. Offline rendering without a serviced message loop cannot reliably deliver WRITE notifications. CSV retains real points while capacity permits; it is an explicit export mechanism, not a fabricated automation lane.

The capture FIFO has 8191 usable slots (~163.82 seconds at 50 Hz). Its overflow is counted and nonblocking. Persistent message-thread capture caps at one million points. Export before closing. Capture is not part of saved plugin state. Host loops/seeks can create duplicate times. READ's 12 ms smoothing and 0.1 dB Rider resolution prevent sample-identical envelope replay guarantees.

## Manual acceptance checklist

Use a disposable project or a copy of an old project, never overwrite the original.

1. Start `/home/enos/opt/REAPER/reaper`. Preferences → Plug-ins → VST: verify `/home/enos/.vst3` is searched, then Re-scan if necessary. Insert **VST3: Vocalis Rider Live**.
2. Play known speech/singing; check quiet/loud passages, pauses, Spill, sensitivities, Target, Fast/Slow, Range, Idle, Output and bypass. Compare the fader with audible gain before Output/Bypass. Check headroom: no limiter is present.
3. Verify mono/stereo; open/close and resize editor repeatedly. Test all controls, editable readouts, fine drags, keyboard focus, presets and several independent instances.
4. Send music 1/2 to vocal-track channels 3/4. Pin main Vocal to 1/2, Music to 3/4, output to 1/2; use VST3 bus-count options if required. Confirm music influence with no music leaking into output. Change routing with normal host controls.
5. Arm only Rider; select plugin WRITE and host Write/Touch. Play, stop and inspect actual envelope points. Select plugin READ and host Read, replay with editor open/closed. Check OFF creates no moving Rider edits. Test mode transitions and host automation on buttons/knobs.
6. Save/reopen the test project and a copy of an old project. Compare all parameters, preset label, pins and automation. Verify offline rendering and multiple instances. Test CSV export/import in a fresh envelope if native recording fails.

Production approval requires recorded results for these steps. Current decision: **READY FOR CONTROLLED TESTING**, not production-certified REAPER support.
