'use strict'

// Setup
const JsAudio = require('./lib/jsaudio')
const JsAudioNative = require('./lib/jsaudio').JsAudioNative
const JsPaStream = JsAudio.JsPaStream
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
var streamOpts = {
  input: {
    device: 57,
    channelCount: 2,
    sampleFormat: formats.paFloat32,
    suggestedLatency: 0.003
  },
  output: {
    device: 43,
    channelCount: 2,
    sampleFormat: formats.paFloat32,
    suggestedLatency: 0.003
  },
  sampleRate: 48000,
  framesPerBuffer: 0,
  streamFlags: 0
}

var stream = new JsPaStream()
streamOpts.stream = stream
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
  .on('start-stream-done', console.log)
  .on('get-stream-write-available-done', console.log)

jsAudio.initialize()
jsAudio.getVersion()
jsAudio.getHostApiCount()
jsAudio.getDefaultHostApi()
jsAudio.getHostApiInfo(2)
jsAudio.getDeviceCount()
jsAudio.getDefaultInputDevice()
jsAudio.getDefaultOutputDevice()
jsAudio.getDeviceInfo(1)
jsAudio.getDeviceInfo(10)
jsAudio.openStream(streamOpts)
jsAudio.startStream(stream)

var buffer
setTimeout(() => {
  var framesToWrite = 0,
    framesToRead = 0,
    phase = 0,
    phaseIncrement = (2 * Math.PI * 80) / 48000.0,
    readBuffer = new Float32Array();
  while(1) {
    framesToRead = JsAudioNative.getStreamReadAvailable(stream);
    if(framesToRead) {
      var tempBuffer = new Float32Array(framesToRead * 2)
      JsAudioNative.readStream(stream, tempBuffer)
      
      var copyBuffer = new Float32Array(readBuffer.length + tempBuffer.length)
      copyBuffer.set(readBuffer)
      copyBuffer.set(tempBuffer, readBuffer.length)
      readBuffer = new Float32Array(copyBuffer)
    }
    framesToWrite = JsAudioNative.getStreamWriteAvailable(stream)
    if(framesToWrite && readBuffer.length > 0) {
      buffer = new Float32Array(readBuffer.buffer, 0, framesToWrite * 2)

      JsAudioNative.writeStream(stream, buffer)
      
      readBuffer = new Float32Array(readBuffer.buffer, framesToWrite * 2 * Float32Array.BYTES_PER_ELEMENT)
    }
  }
}, 1000)

