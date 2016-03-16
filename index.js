'use strict'

// Setup
const JsAudio = require('./lib/jsaudio')
const formats = {
  paFloat32: 1,
  paInt32: 2,
  paInt24: 4,
  paInt16: 8,
  paInt8: 16,
  paUInt8: 32,
  paCustomFormat: 65536,
  paNonInterleaved: 2147483648
}
const streamOpts = {
  input: {
    device: 1,
    channelCount: 2,
    sampleFormat: formats.paFloat32,
    suggestedLatency: 0.09
  },
  output: {
    device: 3,
    channelCount: 2,
    sampleFormat: formats.paFloat32,
    suggestedLatency: 0.09
  },
  sampleRate: 44100,
  framesPerBuffer: 64,
  streamFlags: 0
}

// Exports
module.exports = JsAudio

// Test stuff, for now, needs to go into actual test assertions later
let jsAudio = JsAudio.new()
jsAudio
  .on('error', (e) => { console.log(e.stack) })
  .on('initialize-done', () => { console.log('initialized successfully!') })
  .on('get-version-done', (version) => { console.log(version) })
  .on('test-params-done', console.log)
  .on('get-host-api-count-done', console.log)
  .on('get-default-host-api-done', console.log)
  .on('get-host-api-info-done', console.log)
  .on('get-device-count-done', console.log)
  .on('get-default-input-device-done', console.log)
  .on('get-default-output-device-done', console.log)
  .on('get-device-info-done', console.log)
  .on('open-stream-done', console.log)

jsAudio.initialize()
jsAudio.getVersion()
jsAudio.getHostApiCount()
jsAudio.getDefaultHostApi()
jsAudio.getHostApiInfo()
jsAudio.getDeviceCount()
jsAudio.getDefaultInputDevice()
jsAudio.getDefaultOutputDevice()
jsAudio.getDeviceInfo(0)
jsAudio.getDeviceInfo(3)
setTimeout(() => console.log('hep2'), 5000)
jsAudio.openStream(streamOpts)
setTimeout(() => console.log('hep2'), 10000)
