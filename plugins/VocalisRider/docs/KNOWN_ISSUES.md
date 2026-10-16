# Known issues and accepted limitations

Date: 2026-10-16. See FINAL_DEBUG_REPORT.md for fixed bugs and evidence.

| Item | Classification | Consequence / action |
|---|---|---|
| REAPER acceptance outstanding | UNVERIFIED | Discovery, embedding, real routing, automation recording, old-project/session reopening and offline render need manual checks. This blocks production approval. |
| ThreadSanitizer runtime | BLOCKED | Minimal probe exits 66, unexpected memory mapping. No race-free certification. Concurrent stress passed but is not a TSan substitute. |
| LeakSanitizer runtime | BLOCKED | Leak-enabled tests abort because ptrace prevents scanning. ASan/UBSan pass with leak detection disabled; absence of leaks is not proven. |
| Tightening Range | VERIFIED PASS for retained smoothing; accepted limitation | Gain can briefly remain beyond newly narrowed limits while smoothing. User explicitly chose this existing behaviour during the audit. No hard clamp added. |
| Noise discrimination | UNVERIFIED perceptually | Detector is RMS/activity based, not a speech classifier. Loud spill/noise can trigger riding. Configure Spill and test real microphone material. |
| Headroom / sudden peaks | VERIFIED PASS for intended scalar-gain architecture | No limiter/lookahead; Output and Rider can produce levels above 0 dBFS. A loud onset can pass before gain settles. Preserve headroom. |
| WRITE host policy | UNVERIFIED in REAPER | Correct message-thread notifications do not prove actual envelope recording. Confirm points in a temporary project. |
| Offline WRITE / blocked message loop | VERIFIED FAIL for unlimited/lossless capture expectation | Notification delivery depends on message loop; 8191-slot FIFO can overflow. Dropped points are indicated; no blocking or fake recording. One-million-point persistent cap also applies. |
| Capture persistence / seeks | Accepted existing limitation | Capture is volatile, not serialized; loops/seeks are not separated into takes. Export before closing and record uninterrupted passes. |
| READ replay precision | Accepted existing limitation | 0.1 dB parameter resolution plus 12 ms smoothing; exact automatic-render null is not promised for automation playback. |
| Display semantics | Accepted existing limitation | Rider is internal riding gain before Output/Bypass. Idle is bounded internally by Range. Crossed host-automated endpoints are sorted in DSP but raw named values remain displayed. |
| Preset label / user presets | Accepted existing limitation | Editor/host programs load factory configurations. Automating the preset-label parameter alone does not apply them. User presets rely on host state/preset facilities; no new save dialog. |
| GUI/native chooser/focus | UNVERIFIED on desktop | Direct component mouse/key/scaling/lifecycle tests pass; actual REAPER focus, high-DPI embedding and native export chooser need manual testing. |
| Recorded-vocal listening / long live session | UNVERIFIED | Synthetic analytical/stress signals are not a substitute for listening to real whisper/dialogue/singing/noisy microphones. |

No remaining crash, buffer fault, sanitizer diagnostic or failed registered test was observed in the final executed suite. This statement is limited to the tested configurations. Historical reports describe earlier defects and are superseded by the final reports.
