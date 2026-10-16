-- REAPER ReaScript: select a track with Vocalis Rider Live, then run this action.
-- Appends points to this instance's Rider envelope. Existing points are preserved.
local track = reaper.GetSelectedTrack(0, 0)
if not track then reaper.MB('Select the vocal track first.', 'Vocalis Rider', 0) return end
local candidates = {}
for fx = 0, reaper.TrackFX_GetCount(track)-1 do
  local _, name = reaper.TrackFX_GetFXName(track, fx, '')
  if name:find('Vocalis Rider Live', 1, true) then candidates[#candidates+1] = fx end
end
if #candidates ~= 1 then reaper.MB('The selected track must contain exactly one Vocalis Rider Live instance.', 'Vocalis Rider', 0) return end
local fx, rider, mode = candidates[1], nil, nil
for param = 0, reaper.TrackFX_GetNumParams(track, fx)-1 do
  local _, name = reaper.TrackFX_GetParamName(track, fx, param, '')
  if name == 'Rider' then rider = param end
  if name == 'Automation' then mode = param end
end
if not rider or not mode then reaper.MB('Cannot identify Rider parameters.', 'Vocalis Rider', 0) return end
local ok, path = reaper.GetUserFileNameForRead('', 'Import Vocalis CSV capture', '.csv')
if not ok then return end
local file = io.open(path, 'r')
if not file then reaper.MB('Cannot open CSV.', 'Vocalis Rider', 0) return end
local points = {}
for line in file:lines() do
  local t, db = line:match('^([%d%.%-]+),([%d%.%-]+)')
  t, db = tonumber(t), tonumber(db)
  if t and db and t >= 0 and db >= -12 and db <= 12 then points[#points+1]={t,db} end
end
file:close()
if #points == 0 then reaper.MB('No valid points in CSV.', 'Vocalis Rider', 0) return end
reaper.Undo_BeginBlock()
local env = reaper.GetFXEnvelope(track, fx, rider, true)
local scaling = reaper.GetEnvelopeScalingMode(env)
for _, point in ipairs(points) do
  local value = reaper.ScaleToEnvelopeMode(scaling, (point[2]+12)/24)
  reaper.InsertEnvelopePoint(env, point[1], value, 0, 0, false, true)
end
reaper.Envelope_SortPoints(env)
reaper.TrackFX_SetParamNormalized(track, fx, mode, 1) -- READ is third of three choices
reaper.SetTrackAutomationMode(track, 1) -- Read
reaper.UpdateArrange()
reaper.Undo_EndBlock('Import Vocalis Rider CSV automation', -1)
