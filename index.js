'use strict'

// Setup
const JsAudio = require('./lib/jsaudio')
const streamOpts = {
  input: {
    device: 1
  },
  output: {
    device: 3
  }
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
jsAudio.getDeviceInfo()
jsAudio.openStream(streamOpts)
